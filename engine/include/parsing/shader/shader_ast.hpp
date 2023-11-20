#ifndef ENGINEY_GLSL_AST_HPP
#define ENGINEY_GLSL_AST_HPP

#include <memory>
#include <vector>

#include "shader_lexer.hpp"

namespace EngineY {

    class GlslAstNode;
    class GlslExpr;
    class GlslStmnt;
    class GlslLiteral;
    class GlslLogicalExpr;
    class GlslUnaryExpr;
    class GlslBinaryExpr;
    class GlslCallExpr;
    class GlslGroupingExpr;
    class GlslVariableExpr;
    class GlslAssignmentExpr;
    class GlslExprStmnt;
    class GlslVarStmnt;
    class GlslBlockStmnt;
    class GlslIfStmnt;
    class GlslWhileStmnt;
    class GlslFunctionStmnt;
    class GlslReturnStmnt;

    class GlslTreeWalker {
        public:
            virtual ~GlslTreeWalker() {}
            virtual void VisitLiteral(GlslLiteral& expr) = 0;
            virtual void VisitLogicalExpr(GlslLogicalExpr& expr) = 0;
            virtual void VisitUnaryExpr(GlslUnaryExpr& expr) = 0;
            virtual void VisitBinaryExpr(GlslBinaryExpr& expr) = 0;
            virtual void VisitCallExpr(GlslCallExpr& expr) = 0;
            virtual void VisitGroupingExpr(GlslGroupingExpr& expr) = 0;
            virtual void VisitVariableExpr(GlslVariableExpr& expr) = 0;
            virtual void VisitAssignmentExpr(GlslAssignmentExpr& expr) = 0;
            virtual void VisitExprStmnt(GlslExprStmnt& expr) = 0;
            virtual void VisitVarStmnt(GlslVarStmnt& expr) = 0;
            virtual void VisitBlockStmnt(GlslBlockStmnt& expr) = 0;
            virtual void VisitIfStmnt(GlslIfStmnt& expr) = 0;
            virtual void VisitWhileStmnt(GlslWhileStmnt& expr) = 0;
            virtual void VisitFunctionStmnt(GlslFunctionStmnt& expr) = 0;
            virtual void VisitReturnStmnt(GlslReturnStmnt& expr) = 0;
    };

    class GlslTreePrinter : public GlslTreeWalker {
        public:
            virtual ~GlslTreePrinter() {}
            virtual void VisitLiteral(GlslLiteral& expr) override;
            virtual void VisitLogicalExpr(GlslLogicalExpr& expr) override;
            virtual void VisitUnaryExpr(GlslUnaryExpr& expr) override;
            virtual void VisitBinaryExpr(GlslBinaryExpr& expr) override;
            virtual void VisitCallExpr(GlslCallExpr& expr) override;
            virtual void VisitGroupingExpr(GlslGroupingExpr& expr) override;
            virtual void VisitVariableExpr(GlslVariableExpr& expr) override;
            virtual void VisitAssignmentExpr(GlslAssignmentExpr& expr) override;
            virtual void VisitExprStmnt(GlslExprStmnt& expr) override;
            virtual void VisitVarStmnt(GlslVarStmnt& expr) override;
            virtual void VisitBlockStmnt(GlslBlockStmnt& expr) override;
            virtual void VisitIfStmnt(GlslIfStmnt& expr) override;
            virtual void VisitWhileStmnt(GlslWhileStmnt& expr) override;
            virtual void VisitFunctionStmnt(GlslFunctionStmnt& expr) override;
            virtual void VisitReturnStmnt(GlslReturnStmnt& expr) override;
    };

    class Generator : public GlslTreeWalker {
        public:
            virtual ~Generator() {}
            virtual void VisitLiteral(GlslLiteral& expr) override;
            virtual void VisitLogicalExpr(GlslLogicalExpr& expr) override;
            virtual void VisitUnaryExpr(GlslUnaryExpr& expr) override;
            virtual void VisitBinaryExpr(GlslBinaryExpr& expr) override;
            virtual void VisitCallExpr(GlslCallExpr& expr) override;
            virtual void VisitGroupingExpr(GlslGroupingExpr& expr) override;
            virtual void VisitVariableExpr(GlslVariableExpr& expr) override;
            virtual void VisitAssignmentExpr(GlslAssignmentExpr& expr) override;
            virtual void VisitExprStmnt(GlslExprStmnt& expr) override;
            virtual void VisitVarStmnt(GlslVarStmnt& expr) override;
            virtual void VisitBlockStmnt(GlslBlockStmnt& expr) override;
            virtual void VisitIfStmnt(GlslIfStmnt& expr) override;
            virtual void VisitWhileStmnt(GlslWhileStmnt& expr) override;
            virtual void VisitFunctionStmnt(GlslFunctionStmnt& expr) override;
            virtual void VisitReturnStmnt(GlslReturnStmnt& expr) override;
    };

    class GlslAstNode {
        public:
            virtual ~GlslAstNode() {}
            virtual void Accept(GlslTreeWalker& walker) = 0;
    };
    
    class GlslExpr : public GlslAstNode {
        public:
            virtual ~GlslExpr() {}
            virtual void Accept(GlslTreeWalker& walker) = 0;
            virtual GlslTokenType GetType() = 0;
    };

    class GlslStmnt : public GlslAstNode {
        public:
            virtual ~GlslStmnt() {}
            virtual void Accept(GlslTreeWalker& walker) = 0;
    };
    
    class GlslLiteral : public GlslExpr {
        public:
            GlslTokenType type;
            GlslToken value;

            GlslLiteral(GlslTokenType type, GlslToken value)
                : type(type), value(value) {}
            virtual ~GlslLiteral() {}
            virtual void Accept(GlslTreeWalker& walker) override {
                walker.VisitLiteral(*this);
            }
            virtual GlslTokenType GetType() override { return type; }
    };

    class GlslLogicalExpr : public GlslExpr {
        public:
            std::shared_ptr<GlslExpr> left;
            GlslToken op;
            std::shared_ptr<GlslExpr> right;

            GlslLogicalExpr(std::shared_ptr<GlslExpr> left, GlslToken op, std::shared_ptr<GlslExpr> right)
                : left(left), op(op), right(right) {}
            virtual ~GlslLogicalExpr() {}
            virtual void Accept(GlslTreeWalker& walker) override {
                walker.VisitLogicalExpr(*this);
            }
            virtual GlslTokenType GetType() override { return op.type; }
    };

    class GlslUnaryExpr : public GlslExpr {
        public:
            GlslToken op;
            std::shared_ptr<GlslExpr> right;

            GlslUnaryExpr(GlslToken op, std::shared_ptr<GlslExpr> right)
                : op(op), right(right) {}
            virtual ~GlslUnaryExpr() {}
            virtual void Accept(GlslTreeWalker& walker) override {
                walker.VisitUnaryExpr(*this);
            }
            virtual GlslTokenType GetType() override { return op.type; }
    };

    class GlslBinaryExpr : public GlslExpr {
        public:
            std::shared_ptr<GlslExpr> left;
            GlslToken op;
            std::shared_ptr<GlslExpr> right;

            GlslBinaryExpr(std::shared_ptr<GlslExpr> left, GlslToken op, std::shared_ptr<GlslExpr> right)
                : left(left), op(op), right(right) {}
            virtual ~GlslBinaryExpr() {}
            virtual void Accept(GlslTreeWalker& walker) override {
                walker.VisitBinaryExpr(*this);
            }
            virtual GlslTokenType GetType() override { return op.type; }
    };

    class GlslCallExpr : public GlslExpr {
        public:
            std::shared_ptr<GlslExpr> callee;
            GlslToken paren;
            std::vector<std::shared_ptr<GlslExpr>> args;

            GlslCallExpr(std::shared_ptr<GlslExpr> callee, GlslToken paren, std::vector<std::shared_ptr<GlslExpr>> args)
                : callee(callee), paren(paren), args(args) {}
            virtual ~GlslCallExpr() {}
            virtual void Accept(GlslTreeWalker& walker) override {
                walker.VisitCallExpr(*this);
            }
            virtual GlslTokenType GetType() override { return callee->GetType(); }
    };

    class GlslGroupingExpr : public GlslExpr {
        public:
            std::shared_ptr<GlslExpr> expr;

            GlslGroupingExpr(std::shared_ptr<GlslExpr> expr)
                : expr(expr) {}
            virtual ~GlslGroupingExpr() {}
            virtual void Accept(GlslTreeWalker& walker) override {
                walker.VisitGroupingExpr(*this);
            }
            virtual GlslTokenType GetType() override { return expr->GetType(); }
    };

    class GlslVariableExpr : public GlslExpr {
        public:
            GlslToken name;

            GlslVariableExpr(GlslToken name)
                : name(name) {}
            virtual ~GlslVariableExpr() {}
            virtual void Accept(GlslTreeWalker& walker) override {
                walker.VisitVariableExpr(*this);
            }
            virtual GlslTokenType GetType() override { return name.type; }
    };

    class GlslAssignmentExpr : public GlslExpr {
        public:
            GlslToken name;
            std::shared_ptr<GlslExpr> right;

            GlslAssignmentExpr(GlslToken name , std::shared_ptr<GlslExpr> right)
                : name(name), right(right) {}
            virtual ~GlslAssignmentExpr() {}
            virtual void Accept(GlslTreeWalker& walker) override {
                walker.VisitAssignmentExpr(*this);
            }
            virtual GlslTokenType GetType() override { return name.type; }
    };

    class GlslExprStmnt : public GlslStmnt {
        public:
            std::shared_ptr<GlslExpr> expr;

            GlslExprStmnt(std::shared_ptr<GlslExpr> expr)
                : expr(expr) {}
            virtual ~GlslExprStmnt() {}
            virtual void Accept(GlslTreeWalker& walker) override {
                walker.VisitExprStmnt(*this);
            }
    };

    class GlslVarStmnt : public GlslStmnt {
        public:
            GlslToken type;
            GlslToken name;
            std::shared_ptr<GlslExpr> expr;

            GlslVarStmnt(GlslToken type , GlslToken name , std::shared_ptr<GlslExpr> expr)
                : type(type) , name(name) , expr(expr) {}
            virtual ~GlslVarStmnt() {}
            virtual void Accept(GlslTreeWalker& walker) override {
                walker.VisitVarStmnt(*this);
            }
    };

    class GlslBlockStmnt : public GlslStmnt {
        public:
            std::vector<std::shared_ptr<GlslStmnt>> statements;

            GlslBlockStmnt(std::vector<std::shared_ptr<GlslStmnt>> statements)
                : statements(statements) {}
            virtual ~GlslBlockStmnt() {}
            virtual void Accept(GlslTreeWalker& walker) override {
                walker.VisitBlockStmnt(*this);
            }
    };

    class GlslIfStmnt : public GlslStmnt {
        public:
            std::shared_ptr<GlslExpr> condition;
            std::shared_ptr<GlslStmnt> then_branch;
            std::shared_ptr<GlslStmnt> else_branch;

            GlslIfStmnt(std::shared_ptr<GlslExpr> condition ,
                        std::shared_ptr<GlslStmnt> then_branch ,
                        std::shared_ptr<GlslStmnt> else_branch)
                        : condition(condition) , then_branch(then_branch) , else_branch(else_branch) {}
            virtual ~GlslIfStmnt() {}
            virtual void Accept(GlslTreeWalker& walker) override {
                walker.VisitIfStmnt(*this);
            }
    };

    class GlslWhileStmnt : public GlslStmnt {
        public:
            std::shared_ptr<GlslExpr> condition;
            std::shared_ptr<GlslStmnt> body;

            GlslWhileStmnt(std::shared_ptr<GlslExpr> condition , std::shared_ptr<GlslStmnt> body)
                : condition(condition) , body(body) {}
            virtual ~GlslWhileStmnt() {}
            virtual void Accept(GlslTreeWalker& walker) override {
                walker.VisitWhileStmnt(*this);
            }
    };

    class GlslFunctionStmnt : public GlslStmnt {
        public:
            GlslToken name;
            std::vector<GlslToken> params;
            std::shared_ptr<GlslStmnt> body;

            GlslFunctionStmnt(GlslToken name , std::vector<GlslToken> params , std::shared_ptr<GlslStmnt> body)
                : name(name) , params(params) , body(body) {}
            virtual ~GlslFunctionStmnt() {}
            virtual void Accept(GlslTreeWalker& walker) override {
                walker.VisitFunctionStmnt(*this);
            }
    };

    class GlslReturnStmnt : public GlslStmnt {
        public:
            GlslToken keyword;
            std::shared_ptr<GlslExpr> value;

            GlslReturnStmnt(GlslToken keyword , std::shared_ptr<GlslExpr> value)
                : keyword(keyword) , value(value) {}
            virtual ~GlslReturnStmnt() {}
            virtual void Accept(GlslTreeWalker& walker) override {
                walker.VisitReturnStmnt(*this);
            }
    };
    
}

#endif // !YE_GLSL_AST_HPP