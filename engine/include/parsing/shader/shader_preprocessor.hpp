#ifndef ENGINEY_SHADER_PREPROCESSOR_HPP
#define ENGINEY_SHADER_PREPROCESSOR_HPP

#include <string>
#include <vector>
#include <utility>
#include <sstream>

#include "shader_lexer.hpp"

namespace EngineY {

    class glsl_preprocessor_error : public std::exception {
        std::string msg;

        public:
            glsl_preprocessor_error(const std::string& msg) : msg("[Unknown Position] " + msg) {}
            glsl_preprocessor_error(uint32_t line , uint32_t col , const std::string& name) 
                : msg("[" + std::to_string(line) + ":" + std::to_string(col) + "] " + name) {}
            ~glsl_preprocessor_error() {}

            const char* what() const noexcept override { return msg.c_str(); }
    };
    
    struct FinalTokenInfo {
        uint32_t glsl_version;
        std::string profile;
        std::vector<GlslToken> tokens;
    };

    class GlslPreprocessor {

        uint32_t index = 0;
        std::string source = "";
        std::stringstream modified_source;

        std::vector<GlslToken> tokens;
        std::vector<GlslToken> processed_tokens;

        FinalTokenInfo info;

        glsl_preprocessor_error GetError(const std::string& msg) const;
        
        inline GlslToken Peek() const { return tokens[index]; }
        inline GlslToken Previous() const { return tokens[index - 1]; }

        inline GlslToken Advance() {
            if (!IsEOF())
                ++index;
            return Previous();
        }

        inline GlslToken Consume(GlslTokenType type , const std::string& msg) {
            if (Peek().type == type)
                return Advance();
            throw GetError(msg);
        }

        inline bool IsEOF() const { return (index >= tokens.size()); }

        inline bool Check(GlslTokenType type) const {
            if (IsEOF())
                return false;
            return (Peek().type == type);
        }
        
        /// \note Modifies path to the full path of the file or returns false if not found
        bool AttemptFindFile(std::string& path);
        void ProcessDirective();
        void ProcessVersion();
        void ExpandMacro();
        void ExpandInclude(uint32_t macro_start_index);
        void EmbedSource();

        bool Match(GlslTokenType types);

        // void WriteToken(GlslToken token);
        void PushToken(GlslToken token);
        void PushToken();

        public:
            GlslPreprocessor(const std::vector<GlslToken>& tokens , const std::string& source)
                : tokens(tokens) , source(source) {}
            ~GlslPreprocessor() {}

            std::pair<std::string , FinalTokenInfo> Process();
    };

}

#endif // !YE_SHADER_PREPROCESSOR_HPP