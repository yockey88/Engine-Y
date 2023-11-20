#ifndef ENGINEY_YSCRIPT_PARSER_HPP
#define ENGINEY_YSCRIPT_PARSER_HPP

#include <vector>
#include <stack>
#include <unordered_map>

#include "yscript_ast.hpp"

template<>
struct std::hash< EngineY::YScriptToken> {
    std::size_t operator()(const  EngineY::YScriptToken& token) const noexcept {
        return std::hash<std::string>{}(token.value);
    }
};

namespace EngineY {
    
    using ProjectAst = std::vector<std::unique_ptr<ASTNode>>;

    class yscript_parser_error : public std::exception {
        std::string msg;

        public:
            yscript_parser_error(const std::string& msg , uint32_t line , uint32_t col)
                : msg("Y Script Parser Error at [" + std::to_string(line) + " , " + std::to_string(col) + "] :: " + msg) {}
            ~yscript_parser_error() {}

            const char* what() const noexcept override { return msg.c_str(); }
    };

    class YScriptParser {
        std::vector<YScriptToken> lexemes;
        std::vector<std::unique_ptr<ASTNode>> nodes;

        uint32_t index = 0;

        bool aborted = false;
        bool in_var_decl = false;

        inline yscript_parser_error GetError(const std::string& msg) const {
            return yscript_parser_error(msg , lexemes[index].line , lexemes[index].col);
        }
        
        inline YScriptToken Peek() const { return lexemes[index]; }
        inline YScriptToken Previous() const { return lexemes[index - 1]; }

        inline YScriptToken Advance() {
            if (!IsAtEnd()) ++index;
            return Previous();
        }

        inline YScriptToken Consume(YScriptTokenType t , const std::string& msg) {
            if (Peek().type == t) return Advance();
            throw GetError(msg);
        }
        
        inline bool IsAtEnd() const {
            return index >= lexemes.size();
        }

        inline bool Check(YScriptTokenType type) {
            if (IsAtEnd()) return false;
            return Peek().type == type;
        }

        bool Match(const std::vector<YScriptTokenType>& types , bool advance = true);
        bool VarType();
        bool NodeType();
        bool PropertyType();

        bool VerifyNamelessNode(YScriptTokenType type);

        void Synchronize();

        std::unique_ptr<ASTStmnt> ParseDeclaration();
        std::unique_ptr<ASTStmnt> ParseProjectMetadata();
        std::unique_ptr<ASTStmnt> ParseWindowDeclaration();
        std::unique_ptr<ASTStmnt> ParseResourcesDeclaration();
        std::unique_ptr<ASTStmnt> ParseNodeDeclaration();
        std::unique_ptr<ASTStmnt> ParseSceneList();
        std::unique_ptr<ASTStmnt> ParseEntryPointDeclaration();
        std::unique_ptr<ASTStmnt> ParseFunctionDeclaration(const std::string& kind);
        std::unique_ptr<ASTStmnt> FinishFunction(const std::string& kind , YScriptToken id , YScriptToken node);
        std::unique_ptr<ASTStmnt> ParseVarDeclaration();
        std::unique_ptr<ASTStmnt> ParseStatement();
        std::unique_ptr<ASTStmnt> ParseExpressionStatement();
        std::unique_ptr<ASTStmnt> ParseNodeBlock();
        std::unique_ptr<ASTStmnt> ParseBlock();

        inline std::unique_ptr<ASTExpr> ParseExpression() { return ParseAssignment(); }
        std::unique_ptr<ASTExpr> ParseAssignment();
        std::unique_ptr<ASTExpr> ParseOperatorAssignment();
        std::unique_ptr<ASTExpr> ParseLogicalOr();
        std::unique_ptr<ASTExpr> ParseLogicalAnd();
        std::unique_ptr<ASTExpr> ParseEquality();
        std::unique_ptr<ASTExpr> ParseComparison();
        std::unique_ptr<ASTExpr> ParseTerm();
        std::unique_ptr<ASTExpr> ParseFactor();
        std::unique_ptr<ASTExpr> ParseUnary();
        std::unique_ptr<ASTExpr> ParseCall();
        std::unique_ptr<ASTExpr> FinishCall(std::unique_ptr<ASTExpr>& callee);
        std::unique_ptr<ASTExpr> ParsePrimary();
        std::unique_ptr<ASTExpr> ParseProperty();

        public:
            YScriptParser(const std::vector<YScriptToken>& tokens) 
                : lexemes(std::move(tokens)) {}
            ~YScriptParser() {}

            ProjectAst Parse();

            inline const bool ParseFailed() const { return aborted; }
    };

}

#endif // !YE_YSCRIPT_PARSER_HPP
