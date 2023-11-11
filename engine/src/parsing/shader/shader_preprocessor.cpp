#include "parsing/shader/shader_preprocessor.hpp"

#include <filesystem>

#include "log.hpp"
#include "core/filesystem.hpp"

namespace YE {

    glsl_preprocessor_error GlslPreprocessor::GetError(const std::string& msg) const {
        return glsl_preprocessor_error(tokens[index].line , tokens[index].column , msg);
    }

    bool GlslPreprocessor::AttemptFindFile(std::string& path) {
        std::string proj_shader_path = Filesystem::GetShaderPath();
        std::string engine_shader_path = Filesystem::GetEngineShaderPath(); 

        proj_shader_path += '/' + path;
        if (Filesystem::FileExists(proj_shader_path)) {
            path = proj_shader_path;
            return true;
        } 
        
        engine_shader_path += '/' + path;

        if (Filesystem::FileExists(engine_shader_path)) {
            path = engine_shader_path;
            return true;
        }

        for (auto& entry : std::filesystem::recursive_directory_iterator(
            std::filesystem::current_path()
        )) {
            if (entry.is_directory()) continue;
            if (entry.path().filename().string() == path) {
                path = entry.path().string();
                return true;
            }
        }
        return false;
    }

    bool GlslPreprocessor::Match(GlslTokenType type) {
        if (Check(type)) {
            Advance();
            return true;
        }
        return false;
    }

    void GlslPreprocessor::ProcessDirective() {
        GlslToken hash = Advance();
        switch (Peek().type) {
            case GlslTokenType::VERSION: 
                PushToken(hash);
                PushToken();
                ProcessVersion();
            break;
            case GlslTokenType::INCLUDE: ExpandInclude(hash.start_index); break;
            default: 
                throw GetError("Invalid directive :: " + Peek().value);
        }
    }
    
    void GlslPreprocessor::ProcessVersion() {
        if (!Check(GlslTokenType::INT_LITERAL))
            throw GetError("Expected version number, found :: " + Previous().value);
        
        PushToken();
        info.glsl_version = std::stoi(Previous().value);

        if (!Check(GlslTokenType::PROFILE)) {
            info.profile = "core";
        } else if (Peek().value != "core" && Peek().value != "compatibility") {
            throw GetError("Invalid profile :: " + Peek().value);
        } else {
            PushToken();
            info.profile = Peek().value;
        }
        
    }

    void GlslPreprocessor::ExpandMacro() {
        
    }

    void GlslPreprocessor::ExpandInclude(uint32_t macro_start_index) {
        std::cout << source[macro_start_index] << std::endl;
        
        uint32_t return_index = index - 1; // erase tokens from # to end of include directive

        Consume(GlslTokenType::INCLUDE ,  "Expected include directive, found :: " + Peek().value);
        Consume(GlslTokenType::OPEN_BRACKET , "Expected open bracket after include directive, found :: " + Peek().value);

        std::string include_path = "";

        while (!Check(GlslTokenType::CLOSE_BRACKET))
            include_path += Advance().value;
        
        Consume(GlslTokenType::CLOSE_BRACKET , "Expected close bracket after include path, found :: " + Peek().value);
        uint32_t close_bracket_end_index = Previous().end_index;

        uint32_t include_directive_length = tokens[index].start_index - macro_start_index;

        if (!Filesystem::FileExists(include_path))
            if (!AttemptFindFile(include_path)) 
                throw GetError("Failed to find shader include file :: " + include_path);

        GlslLexer lexer(Filesystem::ReadFileAsStr(include_path));

        auto [include_src , include_tokens] = std::move(lexer.Lex());
        if (include_tokens.empty()) 
            return;

        tokens.erase(tokens.begin() + return_index , tokens.begin() + index);
        tokens.insert(
            tokens.begin() + return_index ,
            include_tokens.begin() + 1 , // [<SOF>]
            include_tokens.end() - 1 // [<EOF>]
        );

        index = return_index;

        PrintTokenHelper{}(Peek());

        modified_source.clear();
        for (uint32_t i = 0; i < macro_start_index; ++i)
            modified_source << source[i];
        
        modified_source << '\n' << include_src;

        for (uint32_t i = close_bracket_end_index; i < source.size(); ++i)
            modified_source << source[i];

        std::cout << modified_source.str() << std::endl;

        // include_src += '\n';
        // original_src.erase(src_start_index - 1 , include_directive_length);
        // original_src.insert(src_start_index - 1 , "\n");
        // original_src.insert(src_start_index - 1 , include_src);

        // tokens.erase(tokens.begin() + token_start_index , tokens.begin() + token_start_index + num_include_tokens);
        // tokens.insert(
        //     tokens.begin() + token_start_index ,
        //     include_tokens.begin() + 1 , // [<SOF>]
        //     include_tokens.end() - 1 // [<EOF>]
        // );

        // std::cout << "Modified source :: \n" << original_src << "\n\n";

    }
    
    void GlslPreprocessor::EmbedSource() {
        
    }

    // not sure if I want these because glsl generation should be moved to the parser
    // void GlslPreprocessor::WriteToken(GlslToken token) {
    //     switch (token.type) {
    //         case GlslTokenType::PREPROCESSOR:
    //             GlslToken hash = Advance();
    //         default:
    //             throw GetError("Attempting to write invalid token type to modified source :: " + token.value);
    //     }
    // }

    void GlslPreprocessor::PushToken(GlslToken token) {
        processed_tokens.push_back(token);
    }

    void GlslPreprocessor::PushToken() {
        processed_tokens.push_back(Peek());
        Advance();
    }
    
    std::pair<std::string , FinalTokenInfo> GlslPreprocessor::Process() {
        while (!IsEOF()) {
            try {
                Check(GlslTokenType::PREPROCESSOR) ? 
                    ProcessDirective() : PushToken();
            } catch (const glsl_preprocessor_error& e) {
                ENGINE_ERROR(e.what());
                return { "<n>" , {} };
            }
        }

        return { modified_source.str() , info };
    }

}