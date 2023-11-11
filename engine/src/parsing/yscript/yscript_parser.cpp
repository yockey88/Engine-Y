#include "parsing/yscript/yscript_parser.hpp"

#include <spdlog/fmt/fmt.h>

#include "log.hpp"

namespace YE {

    bool YScriptParser::Match(const std::vector<YScriptTokenType>& types , bool advance) {
        for (auto& type : types) {
            if (Check(type)) {
                if (advance)
                    Advance();
                return true;
            }
        }
        return false;
    }

    bool YScriptParser::VarType() {
        return false;
    }
    
    bool YScriptParser::NodeType() {
        return Match({ 
            YScriptTokenType::PROJECT , YScriptTokenType::SCENE ,
            YScriptTokenType::ENTITY ,
            YScriptTokenType::TRANSFORM  ,
            YScriptTokenType::RENDERABLE , YScriptTokenType::TEXTURED_RENDERABLE , YScriptTokenType::RENDERABLE_MODEL ,
            YScriptTokenType::POINT_LIGHT ,
            YScriptTokenType::CAMERA ,
            YScriptTokenType::SCRIPT , YScriptTokenType::NATIVE_SCRIPT ,
            YScriptTokenType::PHYSICS_BODY ,
            YScriptTokenType::BOX_COLLIDER , YScriptTokenType::SPHERE_COLLIDER , YScriptTokenType::CAPSULE_COLLIDER , YScriptTokenType::MODEL_COLLIDER
        });
    }
    
    bool YScriptParser::PropertyType() {
        return Match({
            YScriptTokenType::NAME , YScriptTokenType::AUTHOR , 
            YScriptTokenType::VERSION , YScriptTokenType::DESCRIPTION , YScriptTokenType::RESOURCES ,
            YScriptTokenType::PATH ,
            YScriptTokenType::POSITION , YScriptTokenType::ROTATION , YScriptTokenType::SCALE ,
            YScriptTokenType::MESH , YScriptTokenType::SHADER , YScriptTokenType::TEXTURE , YScriptTokenType::MODEL ,
            YScriptTokenType::COLOR , YScriptTokenType::AMBIENT , YScriptTokenType::DIFFUSE , YScriptTokenType::SPECULAR ,
            YScriptTokenType::CONSTANT , YScriptTokenType::LINEAR , YScriptTokenType::QUADRATIC ,
            YScriptTokenType::FRONT , YScriptTokenType::UP , YScriptTokenType::RIGHT , YScriptTokenType::WORLD_UP , YScriptTokenType::EULER_ANGLES , 
                YScriptTokenType::VIEWPORT , YScriptTokenType::CLIP , YScriptTokenType::SPEED , YScriptTokenType::SENSITIVITY , 
                YScriptTokenType::FOV , YScriptTokenType::ZOOM ,
            YScriptTokenType::OBJECT ,
            YScriptTokenType::PHYSICS_BODY_TYPE ,
            YScriptTokenType::RADIUS , YScriptTokenType::HEIGHT , 
        });
    }
    
    bool YScriptParser::VerifyNamelessNode(YScriptTokenType type) {
        return (type == YScriptTokenType::TRANSFORM           || type == YScriptTokenType::RENDERABLE   || 
                type == YScriptTokenType::TEXTURED_RENDERABLE || type == YScriptTokenType::POINT_LIGHT  ||
                type == YScriptTokenType::RENDERABLE_MODEL    || type == YScriptTokenType::SCRIPT       || 
                type == YScriptTokenType::PHYSICS_BODY        || type == YScriptTokenType::BOX_COLLIDER ||
                type == YScriptTokenType::SPHERE_COLLIDER     || type == YScriptTokenType::CAPSULE_COLLIDER);
    }

    void YScriptParser::Synchronize() {
        Advance();
        while (!IsAtEnd()) {
            if (Previous().type == YScriptTokenType::SEMICOLON) return;
            switch (Peek().type) {
                default:
                    Advance();
            }
        }
    }

    std::unique_ptr<ASTStmnt> YScriptParser::ParseDeclaration() {
        try {
            if (Match({ YScriptTokenType::PROJECT })) return ParseProjectMetadata();
            if (Match({ YScriptTokenType::NODE })) return ParseNodeDeclaration();
            if (Match({ YScriptTokenType::EXCLAMATION })) return ParseFunctionDeclaration("node-method");
            if (Match({ YScriptTokenType::FN })) return ParseFunctionDeclaration("function");
            if (Match({ YScriptTokenType::MAIN_FN })) return ParseEntryPointDeclaration();
            if (VarType()) return ParseVarDeclaration();
            return ParseStatement();
        } catch (const yscript_parser_error& e) {
            Synchronize();
            aborted = true;
            ENGINE_ERROR("{}" , e.what());
            return nullptr;
        }
    }
    
    std::unique_ptr<ASTStmnt> YScriptParser::ParseProjectMetadata() {
        YScriptToken identifier;
        if (Match({ YScriptTokenType::IDENTIFIER }))
            identifier = Previous();
        else if (Match({ YScriptTokenType::UNDERSCORE }))
            identifier = YScriptToken(YScriptTokenType::EMPTY_VALUE , Previous().line , Previous().col , "");
        else
            throw GetError("Expected project identifier or '$<>' to leave project anonymous");
        
        std::vector<std::unique_ptr<ASTExpr>> metadata{};
        if (Match({ YScriptTokenType::OPEN_BRACE })) {
            

            while (!Check(YScriptTokenType::CLOSE_BRACE) && !IsAtEnd()) {
                std::unique_ptr<ASTExpr> md = ParseExpression();
                if (md != nullptr) metadata.push_back(std::move(md));
                if (aborted) break;
            }

            Consume(YScriptTokenType::CLOSE_BRACE , "Expected '}' to close project metadata block");

        } else if (!Match({ YScriptTokenType::SEMICOLON })) {
            throw GetError("Expected project metadata block or ';' to close project metadata declaration");
        }

        return std::make_unique<ProjectMetadataStmnt>(identifier , metadata);
    }
    
    std::unique_ptr<ASTStmnt> YScriptParser::ParseNodeDeclaration() {
        Consume(YScriptTokenType::LESS_THAN , "Expected '<' to define node type");
        
        if (!NodeType()) 
            throw GetError("Expected node type keyword found '" + Peek().value + "'");
        
        YScriptToken type = Previous();

        Consume(YScriptTokenType::GREATER_THAN , "Expected '>' to close node type definition");
    
        YScriptToken identifier;
        if (!Match({ YScriptTokenType::IDENTIFIER })) {
            if (!VerifyNamelessNode(type.type))
                throw GetError(fmt::format("Node type {} requires identifier" , type.value));
            Consume(YScriptTokenType::UNDERSCORE , "To leave nodes anonymouse use _ in place of identifier if node type allows it");
            identifier = YScriptToken(YScriptTokenType::EMPTY_VALUE , Previous().line , Previous().col , "");
        } else {
            identifier = Previous();
        }

        std::unique_ptr<ASTStmnt> description = nullptr;
        if (Match({ YScriptTokenType::OPEN_BRACE })) {
            description = ParseNodeBlock();
        } else if (!Match({ YScriptTokenType::SEMICOLON })) {
            throw GetError("Expected nody description or ';' to close node declaration");
        }
        
        return std::make_unique<NodeDeclStmnt>(type , identifier , description);
    }
    
    std::unique_ptr<ASTStmnt> YScriptParser::ParseEntryPointDeclaration() {
        return FinishFunction(
            "entry-point" , 
            YScriptToken(YScriptTokenType::MAIN_FN , Previous().line , Previous().col , "main") , 
            YScriptToken(YScriptTokenType::EMPTY_VALUE , Previous().line , Previous().col , "<>")
        );
    }

    std::unique_ptr<ASTStmnt> YScriptParser::ParseFunctionDeclaration(const std::string& kind) {
        YScriptToken node = Consume(YScriptTokenType::IDENTIFIER , "Expected function identifier after '!'");
        YScriptToken identifier;

        if (Match({ YScriptTokenType::LESS_THAN })) {
            identifier = Consume(YScriptTokenType::IDENTIFIER , "Expected function identifier after node identifier");
            Consume(YScriptTokenType::GREATER_THAN , "Expected '>' to close attached node identifier");
        }

        return FinishFunction(kind , identifier , node);
    }
    
    std::unique_ptr<ASTStmnt> YScriptParser::FinishFunction(const std::string& kind , YScriptToken id , YScriptToken node) {
        Consume(YScriptTokenType::OPEN_PAREN , "Expected '(' to open function argument list");

        std::vector<YScriptToken> arguments{};
        if (!Check(YScriptTokenType::CLOSE_PAREN)) {
            do {
                if (arguments.size() >= 255)
                    throw GetError("Cannot have more than 255 arguments");
                    
                if (Match({ YScriptTokenType::IDENTIFIER  , YScriptTokenType::FLOAT ,
                             YScriptTokenType::STRING , YScriptTokenType::CHARACTER })) {
                    arguments.push_back(Previous());
                } else {
                    throw GetError("Expected argument to function");
                }
            } while (Match({ YScriptTokenType::COMMA }));
        }
        Consume(YScriptTokenType::CLOSE_PAREN , "Expected ')' to close function argument list");

        Consume(YScriptTokenType::OPEN_BRACE , "Expected '{' to open function body");
        std::unique_ptr<ASTStmnt> body = ParseBlock();

        if (body == nullptr)
            throw GetError(fmt::format("Expected {} body" , kind));

        return std::make_unique<FunctionStmnt>(node , id , arguments , body);   
    }

    std::unique_ptr<ASTStmnt> YScriptParser::ParseVarDeclaration() {
        YScriptToken type = Previous();
        YScriptToken name = Consume(YScriptTokenType::IDENTIFIER , "Expected variable name after type");
        std::unique_ptr<ASTExpr> initializer = nullptr;

        if (Match({ YScriptTokenType::EQUALS })) {
            in_var_decl = true;
            initializer = ParseExpression();
            in_var_decl = false;
        }
        Consume(YScriptTokenType::SEMICOLON , "Expected ';' to close variable declaration");

        return std::make_unique<VarDeclStmnt>(type , name , initializer);
    }
    
    std::unique_ptr<ASTStmnt> YScriptParser::ParseStatement() {
        // more to come
        return ParseExpressionStatement();
    }
    
    std::unique_ptr<ASTStmnt> YScriptParser::ParseExpressionStatement() {
        std::unique_ptr<ASTExpr> expr = ParseExpression();
        Consume(YScriptTokenType::SEMICOLON , "Expected ';' to close expression statement");
        return std::make_unique<ExprStmnt>(expr);
    }

    std::unique_ptr<ASTStmnt> YScriptParser::ParseNodeBlock() {
        std::vector<std::unique_ptr<ASTStmnt>> nodes;

        while (!Check(YScriptTokenType::CLOSE_BRACE) && !IsAtEnd()) {
            std::unique_ptr<ASTStmnt> node = ParseDeclaration();
            if (node != nullptr) nodes.push_back(std::move(node));
            if (aborted) break;
        }

        Consume(YScriptTokenType::CLOSE_BRACE , "Expected '}' to close node block");

        return std::make_unique<NodeBodyStmnt>(nodes);
    }
    
    std::unique_ptr<ASTStmnt> YScriptParser::ParseBlock() {
        std::vector<std::unique_ptr<ASTStmnt>> statements;

        while (!Check(YScriptTokenType::CLOSE_BRACE) && !IsAtEnd()) {
            std::unique_ptr<ASTStmnt> stmnt = ParseDeclaration();
            if (stmnt != nullptr) statements.push_back(std::move(stmnt));
            if (aborted) break;
        }

        Consume(YScriptTokenType::CLOSE_BRACE , "Expected '}' to close block");

        return std::make_unique<BlockStmnt>(statements);
    }

    std::unique_ptr<ASTExpr> YScriptParser::ParseAssignment() {
        std::unique_ptr<ASTExpr> expr = ParseOperatorAssignment();
        if (Match({ YScriptTokenType::EQUALS })) {
            std::unique_ptr<ASTExpr> value = ParseAssignment();

            if (expr->PrimaryToken().type == YScriptTokenType::IDENTIFIER) {
                std::unique_ptr<VarExpr> expr_ptr(static_cast<VarExpr*>(expr.release()));
                YScriptToken identifier = expr_ptr->identifier;
                // if (in_node) {
                //     if (node_stack.empty())
                //         throw GetError("UNDEFINED BEHAVIOR | UNKNOWN STATE");
                //     identifier.value = node_stack.top().value + ":" + identifier.value;
                // }
                return std::make_unique<AssignExpr>(identifier , value);
            }
            throw GetError("Invalid assignment target");
        }
        return expr;
    }
    
    std::unique_ptr<ASTExpr> YScriptParser::ParseOperatorAssignment() {
        // std::unique_ptr<ASTExpr> expr = ParseLogicalOr();
        // if (Match({ YScriptTokenType::PLUS_EQUALS , YScriptTokenType::MINUS_EQUALS ,
        //             YScriptTokenType::ASTERISK_EQUALS , YScriptTokenType::FORWARD_SLASH_EQUALS ,
        //             YScriptTokenType::PERCENT_EQUALS})) {
        //     YScriptToken op = Previous();

        //     switch (op.type) {
        //         case YScriptTokenType::PLUS_EQUALS:
        //             op.type = YScriptTokenType::PLUS;
        //             op.value = "+";
        //         break;
        //         case YScriptTokenType::MINUS_EQUALS:
        //             op.type = YScriptTokenType::MINUS;
        //             op.value = "-";
        //         break;
        //         case YScriptTokenType::ASTERISK_EQUALS:
        //             op.type = YScriptTokenType::ASTERISK;
        //             op.value = "*";
        //         break;
        //         case YScriptTokenType::FORWARD_SLASH_EQUALS:
        //             op.type = YScriptTokenType::FORWARD_SLASH;
        //             op.value = "/";
        //         break;
        //         case YScriptTokenType::PERCENT_EQUALS:
        //             op.type = YScriptTokenType::PERCENT;
        //             op.value = "%";
        //         break;
        //         default:
        //             throw GetError("Invalid operator for assignment");
        //     }

        //     std::unique_ptr<ASTExpr> value = ParseOperatorAssignment();
            
        //     if (expr->PrimaryToken().type == YScriptTokenType::IDENTIFIER || PropertyType(expr->PrimaryToken().type)) {
        //         YScriptToken identifier = expr->PrimaryToken();
        //         std::unique_ptr<ASTExpr> access_expr = nullptr;
        //         // if (in_node) {
        //         //     if (node_stack.empty())
        //         //         throw GetError("UNDEFINED BEHAVIOR | UNKNOWN STATE");
        //         //     identifier.value = node_stack.top().value + ":" + identifier.value;
        //         //     expr->PrimaryToken().value = identifier.value;
        //         // }
                
        //         // if (expand_access_assignment) {
        //         //     std::unique_ptr<AccessExpr> access = nullptr;
        //         //     try {
        //         //         access = std::unique_ptr<AccessExpr>(dynamic_cast<AccessExpr*>(expr.release()));
        //         //     } catch (const std::bad_cast& e) {
        //         //         throw GetError("UNDEFINED BEHAVIOR | UNKNOWN STATE");
        //         //     }

        //         //     identifier.value += ":" + access->identifier.value;
        //         //     access_expr = std::unique_ptr<ASTExpr>(access.release()); 

        //         //     expand_access_assignment = false;
        //         // }
        //         std::unique_ptr<ASTExpr> expanded_value = nullptr;
        //         if (access_expr != nullptr)
        //             expanded_value = std::make_unique<BinaryExpr>(access_expr , op , value);
        //         else
        //             expanded_value = std::make_unique<BinaryExpr>(expr , op , value);

        //         std::unique_ptr<ASTStmnt> expanded_stmnt = std::make_unique<ExprStmnt>(expanded_value);
        //         return std::make_unique<VarAssignExpr>(identifier , expanded_stmnt);
        //     }
        //     throw GetError("Invalid assignment target");
        // }
        // return expr;
        return ParseLogicalOr();
    }

    std::unique_ptr<ASTExpr> YScriptParser::ParseLogicalOr() {
        std::unique_ptr<ASTExpr> expr = ParseLogicalAnd();
        while (Match({ YScriptTokenType::LOGICAL_OR })) {
            YScriptToken op = Previous();
            std::unique_ptr<ASTExpr> right = ParseLogicalAnd();
            expr = std::make_unique<LogicalExpr>(expr , op , right);
        }
        return expr;
    }

    std::unique_ptr<ASTExpr> YScriptParser::ParseLogicalAnd() {
        std::unique_ptr<ASTExpr> expr = ParseEquality();
        while (Match({ YScriptTokenType::LOGICAL_AND })) {
            YScriptToken op = Previous();
            std::unique_ptr<ASTExpr> right = ParseEquality();
            expr = std::make_unique<LogicalExpr>(expr , op , right);
        }
        return expr;
    }

    std::unique_ptr<ASTExpr> YScriptParser::ParseEquality() {
        std::unique_ptr<ASTExpr> expr = ParseComparison();
        while (Match({ YScriptTokenType::EQUALS_EQUALS , YScriptTokenType::EXCLAMATION_EQUALS })) {
            YScriptToken op = Previous();
            std::unique_ptr<ASTExpr> right = ParseComparison();
            expr = std::make_unique<BinaryExpr>(expr , op , right);
        }
        return expr;
    }

    std::unique_ptr<ASTExpr> YScriptParser::ParseComparison() {
        std::unique_ptr<ASTExpr> expr = ParseTerm();
        while (Match({
            YScriptTokenType::LESS_THAN , YScriptTokenType::GREATER_THAN ,
            YScriptTokenType::LESS_THAN_EQUALS , YScriptTokenType::GREATER_THAN_EQUALS
        })) {
            YScriptToken op = Previous();
            std::unique_ptr<ASTExpr> right = ParseTerm();
            expr = std::make_unique<BinaryExpr>(expr , op , right);
        }
        return expr;
    }

    std::unique_ptr<ASTExpr> YScriptParser::ParseTerm() {
        std::unique_ptr<ASTExpr> expr = ParseFactor();
        while (Match({ YScriptTokenType::PLUS , YScriptTokenType::MINUS })) {
            YScriptToken op = Previous();
            std::unique_ptr<ASTExpr> right = ParseFactor();
            expr = std::make_unique<BinaryExpr>(expr , op , right);
        }
        return expr;
    }
    
    std::unique_ptr<ASTExpr> YScriptParser::ParseFactor() {
        std::unique_ptr<ASTExpr> expr = ParseUnary();
        while (Match({ YScriptTokenType::ASTERISK , YScriptTokenType::FORWARD_SLASH , YScriptTokenType::PERCENT })) {
            YScriptToken op = Previous();
            std::unique_ptr<ASTExpr> right = ParseUnary();
            expr = std::make_unique<BinaryExpr>(expr , op , right);
        }
        return expr;
    }

    std::unique_ptr<ASTExpr> YScriptParser::ParseUnary() {
        if (Match({ YScriptTokenType::MINUS , YScriptTokenType::EXCLAMATION })) {
            YScriptToken op = Previous();
            std::unique_ptr<ASTExpr> right = ParseUnary();
            return std::make_unique<UnaryExpr>(op , right);
        }
        return ParseCall();
    }

    std::unique_ptr<ASTExpr> YScriptParser::ParseCall() {
        std::unique_ptr<ASTExpr> expr = ParsePrimary();
        while (true) {
            if (Match({ YScriptTokenType::OPEN_PAREN })) {
                expr = FinishCall(expr);
            } else if (Match({ YScriptTokenType::COLON_COLON })) {
                YScriptToken identifier;
                if (Match({ YScriptTokenType::IDENTIFIER }) || PropertyType())
                    identifier = Previous();
                else
                    throw GetError("Expected property or variable name after ':'");
                expr = std::make_unique<AccessExpr>(expr , identifier);
            } else {
                break;
            }
        }
        return expr;
    }
    
    std::unique_ptr<ASTExpr> YScriptParser::FinishCall(std::unique_ptr<ASTExpr>& callee) {
        std::vector<std::unique_ptr<ASTExpr>> arguments;
        if (!Check(YScriptTokenType::CLOSE_PAREN)) {
            do {
                if (arguments.size() >= 255)
                    throw GetError("Cannot have more than 255 arguments");
                arguments.push_back(ParseExpression());
            } while (Match({ YScriptTokenType::COMMA }));
        }

        Consume(YScriptTokenType::CLOSE_PAREN , "Expected ')' to close function call");

        return std::make_unique<CallExpr>(callee , arguments);
    }

    std::unique_ptr<ASTExpr> YScriptParser::ParsePrimary() {
        uint32_t line = Peek().line;
        uint32_t col = Peek().col;

        if (PropertyType())
            return ParseProperty();

        if (Match({ 
            YScriptTokenType::TRUE_TKN , YScriptTokenType::FALSE_TKN , 
            YScriptTokenType::FLOAT ,
            YScriptTokenType::STRING , YScriptTokenType::CHARACTER 
        })) return std::make_unique<LiteralExpr>(Previous());

        if (Match({ YScriptTokenType::IDENTIFIER }))
            return std::make_unique<VarExpr>(Previous());

        if (Match({ YScriptTokenType::OPEN_PAREN })) {
            std::unique_ptr<ASTExpr> expr = ParseExpression();
            Consume(YScriptTokenType::CLOSE_PAREN , "Expected ')' to close expression");
            return std::make_unique<GroupingExpr>(expr);
        }

        throw GetError("Expected primary expression");
    }

    std::unique_ptr<ASTExpr> YScriptParser::ParseProperty() {
        std::vector<std::unique_ptr<ASTExpr>> values;
        
        YScriptToken type = Previous();
        
        Consume(YScriptTokenType::COLON , "Expected ':' after property type , cannot leave property uninitialized");

        if (Match({ 
            YScriptTokenType::STRING , YScriptTokenType::CHARACTER , 
            YScriptTokenType::FLOAT
        })) {
            values.emplace_back(std::make_unique<LiteralExpr>(Previous()));
        } else if (Match({ YScriptTokenType::IDENTIFIER })) {
            values.emplace_back(std::make_unique<VarExpr>(Previous()));
        } else if (Match({ YScriptTokenType::OPEN_BRACE })) {
            
            while (!Check(YScriptTokenType::CLOSE_BRACE) && !IsAtEnd()) {
                std::unique_ptr<ASTExpr> expr = ParseUnary();
                if (expr != nullptr) values.push_back(std::move(expr));
                if (aborted) break;
                if (Check(YScriptTokenType::COMMA))
                    Advance();
            }

            Consume(YScriptTokenType::CLOSE_BRACE , "Expected '}' to close property value list");

        } else {
            throw GetError("Expected property value");
        }

        if (Check(YScriptTokenType::COMMA))
            Advance();

        if (values.size() < 1)
            throw GetError("Expected at least one property value");

        return std::make_unique<PropertyExpr>(type , values);
    }

    ProjectAst YScriptParser::Parse() {
        if (IsAtEnd()) return {};
        if (Peek().type != YScriptTokenType::START_OF_FILE) 
            throw GetError("Expected start of file");

        index = 1;

        try {
            while (!IsAtEnd() && Peek().type != YScriptTokenType::END_OF_FILE && !aborted) {
                std::unique_ptr<ASTStmnt> node = ParseDeclaration();
                if (node != nullptr) nodes.push_back(std::move(node));
            }
        } catch (yscript_parser_error& e) {
            aborted = true;
            ENGINE_ERROR("Project File Parse Failed :: {}" , e.what());
            return {};
        }

        return std::move(nodes);
    }

}