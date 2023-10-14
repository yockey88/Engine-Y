#ifndef YE_GLSL_PARSER_HPP
#define YE_GLSL_PARSER_HPP

#include <string>
#include <vector>
#include <memory>
#include <utility>

#include "shader_lexer.hpp"
#include "shader_preprocessor.hpp"
#include "shader_ast.hpp"

namespace YE {

    enum ShaderPathIndex {
        VERTEX   = 0 ,
        FRAGMENT = 1 ,
        GEOMETRY = 2 ,
    };

    struct ProgramInfo {
        std::string shader_name;
        std::string paths[3]; // vertex , fragment , geometry
        std::string source[3]; // vertex , fragment , geometry
        uint32_t version = 0;
        std::string profile = "";
    };

    class glsl_parser_error : public std::exception {
        std::string msg;

        public:
            glsl_parser_error(const std::string& msg) : msg("[Unknown Position] " + msg) {}
            glsl_parser_error(uint32_t line , uint32_t col , const std::string& name) 
                : msg("[" + std::to_string(line) + ":" + std::to_string(col) + "] " + name) {}
            ~glsl_parser_error() {}

            const char* what() const noexcept override { return msg.c_str(); }
    };

    class GlslParser {
        GlslLexer* lexer = nullptr;
        GlslPreprocessor* preprocessor = nullptr;
        std::string path = "";
        std::string original_src = "";
        std::stringstream compiled_src;

        ProgramInfo info;
        PrintTokenHelper printer;
        GlslTokenType curr_token_type = GlslTokenType::INVALID;
        uint32_t index = 0;

        bool in_var_declaration = false;
        bool abort = false;

        std::vector<GlslToken> tokens;
        std::vector<std::shared_ptr<GlslStmnt>> statements;

        inline glsl_parser_error GetError(const std::string& msg) const {
            return glsl_parser_error(tokens[index].line , tokens[index].column , msg);
        }

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

        bool Match(const std::vector<GlslTokenType>& types);
        bool IsValidReturnType();

        // void Synchronize();

        std::shared_ptr<GlslStmnt> ParseTU();
        std::shared_ptr<GlslStmnt> ParseGlobalDecl();
        
        public:
            GlslParser(const std::vector<GlslToken>& tokens , const std::string& src)
                : tokens(tokens)  , path(path) {}
            ~GlslParser() {}

            std::pair<std::string , std::vector<std::shared_ptr<GlslStmnt>>> Parse();

            inline std::vector<std::shared_ptr<GlslStmnt>> GetStatements() const { return statements; }
            inline std::string GetSource() const { return original_src; }
    };

}

        // std::shared_ptr<GlslStmnt> ParseVarDecl();
        // std::shared_ptr<GlslStmnt> ParseStmnt();
        // std::shared_ptr<GlslStmnt> ParsePrintStmnt();
        // std::shared_ptr<GlslStmnt> ParseExprStmnt();
        // std::shared_ptr<GlslStmnt> ParseBlockStmnt();
        // std::shared_ptr<GlslStmnt> ParseIfStmnt();
        // std::shared_ptr<GlslStmnt> ParseWhileStmnt();
        // std::shared_ptr<GlslStmnt> ParseForStmnt();
        // std::shared_ptr<GlslStmnt> ParseFuncDecl(const std::string& kind);
        // std::shared_ptr<GlslStmnt> ParseReturnStmnt();

        // std::shared_ptr<GlslExpr> ParseExpr() { return ParseAssignment(); }
        // std::shared_ptr<GlslExpr> ParseAssignment();
        // std::shared_ptr<GlslExpr> ParseLogicalOr();
        // std::shared_ptr<GlslExpr> ParseLogicalAnd();
        // std::shared_ptr<GlslExpr> ParseEquality();
        // std::shared_ptr<GlslExpr> ParseComparison();
        // std::shared_ptr<GlslExpr> ParseTerm();
        // std::shared_ptr<GlslExpr> ParseFactor();
        // std::shared_ptr<GlslExpr> ParseUnary();
        // std::shared_ptr<GlslExpr> ParseCall();
        // std::shared_ptr<GlslExpr> FinishCall(std::shared_ptr<GlslExpr> callee);
        // std::shared_ptr<GlslExpr> ParsePrimary();

#endif // !YE_GLSL_PARSER_HPP