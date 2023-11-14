#ifndef YE_YSCRIPT_FILE_AST_HPP
#define YE_YSCRIPT_FILE_AST_HPP

#include "yscript_lexer.hpp"
#include "core/defines.hpp"

#include <vector>
#include <string>
#include <memory>
#include <stack>

namespace YE {

namespace YSC {} // namespace YSC

    // Forward declarations
    class LiteralExpr;
    class PropertyExpr;
    class AccessExpr;
    class LogicalExpr;
    class UnaryExpr;
    class BinaryExpr;
    class CallExpr;
    class GroupingExpr;
    class VarExpr;
    class AssignExpr;

    class ExprStmnt;
    class ProjectMetadataStmnt;
    class WindowStmnt;
    class NodeDeclStmnt;
    class FunctionStmnt;
    class VarDeclStmnt;
    class NodeBodyStmnt;
    class BlockStmnt;

    struct PrinterData {
        std::stack<char> indent_stack;
    };

    class YScriptTreeWalker {
        public:
            virtual ~YScriptTreeWalker() {}
            virtual void WalkLiteral(LiteralExpr& literal) = 0;
            virtual void WalkProperty(PropertyExpr& property) = 0;
            virtual void WalkAccess(AccessExpr& access) = 0;
            virtual void WalkLogical(LogicalExpr& logical) = 0;
            virtual void WalkUnary(UnaryExpr& unary) = 0;
            virtual void WalkBinary(BinaryExpr& binary) = 0;
            virtual void WalkCall(CallExpr& call) = 0;
            virtual void WalkGrouping(GroupingExpr& grouping) = 0;
            virtual void WalkVar(VarExpr& var) = 0;
            virtual void WalkAssign(AssignExpr& assign) = 0;
            virtual void WalkExpr(ExprStmnt& expr) = 0;
            virtual void WalkProject(ProjectMetadataStmnt& project) = 0;
            virtual void WalkWindow(WindowStmnt& window) = 0;
            virtual void WalkNodeDecl(NodeDeclStmnt& node_decl) = 0;
            virtual void WalkFunction(FunctionStmnt& function) = 0;
            virtual void WalkVarDecl(VarDeclStmnt& var_decl) = 0;
            virtual void WalkNodeBody(NodeBodyStmnt& node_assign) = 0;
            virtual void WalkBlock(BlockStmnt& block) = 0;
    };

    class YScriptTreePrinter : public YScriptTreeWalker {
        PrinterData data;

        public:
            virtual ~YScriptTreePrinter() override {}
            virtual void WalkLiteral(LiteralExpr& literal) override;
            virtual void WalkProperty(PropertyExpr& property) override;
            virtual void WalkAccess(AccessExpr& access) override;
            virtual void WalkLogical(LogicalExpr& logical) override;
            virtual void WalkUnary(UnaryExpr& unary) override;
            virtual void WalkBinary(BinaryExpr& binary) override;
            virtual void WalkCall(CallExpr& call) override;
            virtual void WalkGrouping(GroupingExpr& grouping) override;
            virtual void WalkVar(VarExpr& var) override;
            virtual void WalkAssign(AssignExpr& assign) override;
            virtual void WalkExpr(ExprStmnt& expr) override;
            virtual void WalkProject(ProjectMetadataStmnt& project) override;
            virtual void WalkWindow(WindowStmnt& window) override;
            virtual void WalkNodeDecl(NodeDeclStmnt& node_decl) override;
            virtual void WalkFunction(FunctionStmnt& function) override;
            virtual void WalkVarDecl(VarDeclStmnt& var_decl) override;
            virtual void WalkNodeBody(NodeBodyStmnt& node_assign) override;
            virtual void WalkBlock(BlockStmnt& block) override;
    };

    // Base class for all AST nodes
    class ASTNode {
        public:
            virtual ~ASTNode() {}
            virtual void Walk(YScriptTreeWalker* walker) = 0;
            virtual void Print(PrinterData& data) = 0;
    };

    class ASTStmnt : public ASTNode {
        public:
            virtual ~ASTStmnt() override {}
            virtual void Walk(YScriptTreeWalker* walker) override = 0;
            virtual void Print(PrinterData& data) override = 0;
    };

    class ASTExpr : public ASTNode {
        public:
            virtual ~ASTExpr() override {}
            virtual YScriptToken& PrimaryToken() noexcept = 0;
            virtual void Walk(YScriptTreeWalker* walker) override = 0;
            virtual void Print(PrinterData& data) override = 0;
    };

    // Represents a primary expression
    class LiteralExpr : public ASTExpr {
        public:
            YScriptToken value;

            LiteralExpr(YScriptToken value = YScriptToken(YScriptTokenType::BLANK , 0 , 0 , ""))
                : value(value) {}
            virtual ~LiteralExpr() override {}
            virtual YScriptToken& PrimaryToken() noexcept override { return value; }
            virtual void Walk(YScriptTreeWalker* walker) override { walker->WalkLiteral(*this); }
            virtual void Print(PrinterData& data) override;
    };

    // represents a property expression
    class PropertyExpr : public ASTExpr {
        public:
            YScriptToken type;
            std::vector<std::unique_ptr<ASTExpr>> values;

            PropertyExpr(YScriptToken type, std::vector<std::unique_ptr<ASTExpr>>& values)
                : type(type), values(std::move(values)) {}
            virtual ~PropertyExpr() override {}
            virtual YScriptToken& PrimaryToken() noexcept override { return type; }
            virtual void Walk(YScriptTreeWalker* walker) override { walker->WalkProperty(*this); }
            virtual void Print(PrinterData& data) override;
    };

    // represents a node/property access expression
    class AccessExpr : public ASTExpr {
        public:
            std::unique_ptr<ASTExpr> object;
            YScriptToken identifier;

            AccessExpr(std::unique_ptr<ASTExpr>& object , YScriptToken identifier)
                : object(std::move(object)) , identifier(identifier) {}
            virtual ~AccessExpr() override {}
            virtual YScriptToken& PrimaryToken() noexcept override { return object->PrimaryToken(); }
            virtual void Walk(YScriptTreeWalker* walker) override { walker->WalkAccess(*this); }
            virtual void Print(PrinterData& data) override;
    };

    // represents a logical expression
    class LogicalExpr : public ASTExpr {
        public:
            std::unique_ptr<ASTExpr> left;
            YScriptToken op;
            std::unique_ptr<ASTExpr> right;

            LogicalExpr(std::unique_ptr<ASTExpr>& left, YScriptToken op, std::unique_ptr<ASTExpr>& right)
                : left(std::move(left)), op(op), right(std::move(right)) {}
            virtual ~LogicalExpr() override {}
            virtual YScriptToken& PrimaryToken() noexcept override { return op; }
            virtual void Walk(YScriptTreeWalker* walker) override { walker->WalkLogical(*this); }
            virtual void Print(PrinterData& data) override;
    };

    // represents a unary expression
    class UnaryExpr : public ASTExpr {
        public:
            YScriptToken op;
            std::unique_ptr<ASTExpr> right;

            UnaryExpr(YScriptToken op, std::unique_ptr<ASTExpr>& right)
                : op(op), right(std::move(right)) {}
            virtual ~UnaryExpr() override {}
            virtual YScriptToken& PrimaryToken() noexcept override { return op; }
            virtual void Walk(YScriptTreeWalker* walker) override { walker->WalkUnary(*this); }
            virtual void Print(PrinterData& data) override;
    };

    // represents a binary expression
    class BinaryExpr : public ASTExpr {
        public:
            std::unique_ptr<ASTExpr> left;
            YScriptToken op;
            std::unique_ptr<ASTExpr> right;

            BinaryExpr(std::unique_ptr<ASTExpr>& left, YScriptToken op, std::unique_ptr<ASTExpr>& right)
                : left(std::move(left)), op(op), right(std::move(right)) {}
            virtual ~BinaryExpr() override {}
            virtual YScriptToken& PrimaryToken() noexcept override { return op; }
            virtual void Walk(YScriptTreeWalker* walker) override { walker->WalkBinary(*this); }
            virtual void Print(PrinterData& data) override;
    };
    
    // represents a call expression
    class CallExpr : public ASTExpr {
        public:
            std::unique_ptr<ASTExpr> callee;
            std::vector<std::unique_ptr<ASTExpr>> args;

            CallExpr(std::unique_ptr<ASTExpr>& callee, std::vector<std::unique_ptr<ASTExpr>>& args)
                : callee(std::move(callee)), args(std::move(args)) {}
            virtual ~CallExpr() override {}
            virtual YScriptToken& PrimaryToken() noexcept override { return callee->PrimaryToken(); }
            virtual void Walk(YScriptTreeWalker* walker) override { walker->WalkCall(*this); }
            virtual void Print(PrinterData& data) override;
    };

    // represents a grouping expression
    class GroupingExpr : public ASTExpr {
        public:
            std::unique_ptr<ASTExpr> expr;

            GroupingExpr(std::unique_ptr<ASTExpr>& expr)
                : expr(std::move(expr)) {}
            virtual ~GroupingExpr() override {}
            virtual YScriptToken& PrimaryToken() noexcept override { return expr->PrimaryToken(); }
            virtual void Walk(YScriptTreeWalker* walker) override { walker->WalkGrouping(*this); }
            virtual void Print(PrinterData& data) override;
    };

    // represents a variable expression
    class VarExpr : public ASTExpr {
        public:
            YScriptToken identifier;

            VarExpr(YScriptToken identifier)
                : identifier(identifier) {}
            virtual ~VarExpr() override {}
            virtual YScriptToken& PrimaryToken() noexcept override { return identifier; }
            virtual void Walk(YScriptTreeWalker* walker) override { walker->WalkVar(*this); }
            virtual void Print(PrinterData& data) override;
    };

    // represents an assignment expression
    class AssignExpr : public ASTExpr {
        public:
            YScriptToken identifier;
            std::unique_ptr<ASTExpr> value;

            AssignExpr(YScriptToken identifier , std::unique_ptr<ASTExpr>& value)
                : identifier(identifier) , value(std::move(value)) {}
            virtual ~AssignExpr() override {}
            virtual YScriptToken& PrimaryToken() noexcept override { return identifier; }
            virtual void Walk(YScriptTreeWalker* walker) override { walker->WalkAssign(*this); }
            virtual void Print(PrinterData& data) override;
    };

    // represents an expression statement
    class ExprStmnt : public ASTStmnt {
        public:
            std::unique_ptr<ASTExpr> expr;

            ExprStmnt(std::unique_ptr<ASTExpr>& expr)
                : expr(std::move(expr)) {}
            virtual ~ExprStmnt() override {}
            virtual void Walk(YScriptTreeWalker* walker) override { expr->Walk(walker); }
            virtual void Print(PrinterData& data) override;
    };

    // represents project metadata
    class ProjectMetadataStmnt : public ASTStmnt {
        public:
            YScriptToken identifier;
            std::vector<std::unique_ptr<ASTExpr>> metadata;

            ProjectMetadataStmnt(YScriptToken identifier , std::vector<std::unique_ptr<ASTExpr>>& metadata)
                : identifier(identifier) , metadata(std::move(metadata)) {}
            virtual ~ProjectMetadataStmnt() override {}
            virtual void Walk(YScriptTreeWalker* walker) override { walker->WalkProject(*this); }
            virtual void Print(PrinterData& data) override;
    };

    // Represents a window declaration
    class WindowStmnt : public ASTStmnt {
        public:
            YScriptToken identifier;
            std::vector<std::unique_ptr<ASTExpr>> description;

            WindowStmnt(YScriptToken identifier , std::vector<std::unique_ptr<ASTExpr>>& description)
                : identifier(identifier) , description(std::move(description)) {}
            virtual ~WindowStmnt() override {}
            virtual void Walk(YScriptTreeWalker* walker) override { walker->WalkWindow(*this); }
            virtual void Print(PrinterData& data) override;
    };

    // Represents a node declaration
    class NodeDeclStmnt : public ASTStmnt {
        public:
            YScriptToken type;
            YScriptToken identifier;
            std::unique_ptr<ASTStmnt> initializer;
            
            NodeDeclStmnt(YScriptToken type , YScriptToken identifier , 
                          std::unique_ptr<ASTStmnt>& initializer)
                : type(type) , identifier(identifier) , initializer(std::move(initializer)) {}
            virtual ~NodeDeclStmnt() override {}
            virtual void Walk(YScriptTreeWalker* walker) override { walker->WalkNodeDecl(*this); }
            virtual void Print(PrinterData& data) override;
    };

    // represents a function declaration
    class FunctionStmnt : public ASTStmnt {
        public:
            YScriptToken node;
            YScriptToken identifier;
            std::vector<YScriptToken> params;
            std::unique_ptr<ASTStmnt> body;

            FunctionStmnt(YScriptToken node , YScriptToken identifier , std::vector<YScriptToken>& params , 
                          std::unique_ptr<ASTStmnt>& body)
                : node(node) , identifier(identifier) , params(std::move(params)) , body(std::move(body)) {}
            virtual ~FunctionStmnt() override {}
            virtual void Walk(YScriptTreeWalker* walker) override { walker->WalkFunction(*this); }
            virtual void Print(PrinterData& data) override;
    };

    // represents a variable declaration
    class VarDeclStmnt : public ASTStmnt {
        public:
            YScriptToken type;
            YScriptToken identifier;
            std::unique_ptr<ASTExpr> initializer;

            VarDeclStmnt(YScriptToken type , YScriptToken identifier , std::unique_ptr<ASTExpr>& initializer)
                : type(type) , identifier(identifier) , initializer(std::move(initializer)) {}
            virtual ~VarDeclStmnt() override {}
            virtual void Walk(YScriptTreeWalker* walker) override { walker->WalkVarDecl(*this); }
            virtual void Print(PrinterData& data) override;
    };

    // Represents a node assignment
    class NodeBodyStmnt : public ASTStmnt {
        public:
            std::vector<std::unique_ptr<ASTStmnt>> description;

            NodeBodyStmnt(std::vector<std::unique_ptr<ASTStmnt>>& description)
                : description(std::move(description)) {}
            virtual ~NodeBodyStmnt() override {}
            virtual void Walk(YScriptTreeWalker* walker) override { walker->WalkNodeBody(*this); }
            virtual void Print(PrinterData& data) override;
    };

    // represents a block statement
    class BlockStmnt : public ASTStmnt {
        public:
            std::vector<std::unique_ptr<ASTStmnt>> statements;

            BlockStmnt(std::vector<std::unique_ptr<ASTStmnt>>& statements)
                : statements(std::move(statements)) {}
            virtual ~BlockStmnt() override {}
            virtual void Walk(YScriptTreeWalker* walker) override { walker->WalkBlock(*this); }
            virtual void Print(PrinterData& data) override;
    };

} // namespace YE

#endif // !YE_YSCRIPT_AST_HPP