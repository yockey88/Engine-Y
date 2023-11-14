#include "parsing/yscript/yscript_ast.hpp"

namespace YE {

    void YScriptTreePrinter::WalkLiteral(LiteralExpr& literal) {
        std::cout << "(";
        literal.Print(data);
        std::cout << ")\n";
    }

    void YScriptTreePrinter::WalkProperty(PropertyExpr& property) {
        std::cout << "(";
        property.Print(data);
        std::cout << ")\n";
    }
    
    void YScriptTreePrinter::WalkAccess(AccessExpr& access) {
        std::cout << "(";
        access.Print(data);
        std::cout << ")\n";
    }

    void YScriptTreePrinter::WalkLogical(LogicalExpr& logical) {
        std::cout << "(";
        logical.Print(data);
        std::cout << ")\n";
    }

    void YScriptTreePrinter::WalkUnary(UnaryExpr& unary) {
        std::cout << "(";
        unary.Print(data);
        std::cout << ")\n";
    }

    void YScriptTreePrinter::WalkBinary(BinaryExpr& binary) {
        std::cout << "(";
        binary.Print(data);
        std::cout << ")\n";
    }

    void YScriptTreePrinter::WalkCall(CallExpr& call) {
        std::cout << "(";
        call.Print(data);
        std::cout << ")\n";
    }

    void YScriptTreePrinter::WalkGrouping(GroupingExpr& grouping) {
        std::cout << "(";
        grouping.Print(data);
        std::cout << ")\n";
    }

    void YScriptTreePrinter::WalkVar(VarExpr& var) {
        std::cout << "(";
        var.Print(data);
        std::cout << ")\n";
    }

    void YScriptTreePrinter::WalkAssign(AssignExpr& assign) {
        std::cout << "(";
        assign.Print(data);
        std::cout << ")\n";
    }

    void YScriptTreePrinter::WalkExpr(ExprStmnt& expr) {
        std::cout << "(";
        expr.Print(data);
        std::cout << ")\n";
    }
    
    void YScriptTreePrinter::WalkProject(ProjectMetadataStmnt& project) {
        std::cout << "(";
        project.Print(data);
        std::cout << ")\n";
    }

    void YScriptTreePrinter::WalkWindow(WindowStmnt& window) {
        std::cout << "(";
        window.Print(data);
        std::cout << ")\n";
    }

    void YScriptTreePrinter::WalkNodeDecl(NodeDeclStmnt& node_decl) {
        std::cout << "(";
        node_decl.Print(data);
        std::cout << ")\n";
    }
    
    void YScriptTreePrinter::WalkFunction(FunctionStmnt& function) {
        std::cout << "(";
        function.Print(data);
        std::cout << ")\n";
    }
    
    void YScriptTreePrinter::WalkVarDecl(VarDeclStmnt& var_decl) {
        std::cout << "(";
        var_decl.Print(data);
        std::cout << ")\n";
    }

    void YScriptTreePrinter::WalkNodeBody(NodeBodyStmnt& node_assign) {
        std::cout << "(";
        node_assign.Print(data);
        std::cout << ")\n";
    }
    
    void YScriptTreePrinter::WalkBlock(BlockStmnt& block) {
        std::cout << "(";
        block.Print(data);
        std::cout << ")\n";
    }
    
    void LiteralExpr::Print(PrinterData& data) {
        std::cout << value.value;
    }

    void PropertyExpr::Print(PrinterData& data) {
        std::cout << type.value << " : { ";
        for (uint32_t i = 0; i < values.size(); ++i) {
            values[i]->Print(data);
            if (i != values.size() - 1)
                std::cout << " , ";
        }
        std::cout << " }";
    }
    
    void AccessExpr::Print(PrinterData& data) {
        object->Print(data);
        std::cout << "::" << identifier.value;
    }

    void LogicalExpr::Print(PrinterData& data) {
        std::cout << "(";
        left->Print(data);
        std::cout << " " << op.value << " ";
        right->Print(data);
        std::cout << ")";
    }

    void UnaryExpr::Print(PrinterData& data) {
        std::cout << "(" << op.value;
        right->Print(data);
        std::cout << ")";
    }
    
    void BinaryExpr::Print(PrinterData& data) {
        std::cout << "(";
        left->Print(data);
        std::cout << " " << op.value << " ";
        right->Print(data);
        std::cout << ")";
    }

    void CallExpr::Print(PrinterData& data) {
        std::cout << "(";
        callee->Print(data);
        std::cout << "(";
        for (uint32_t i = 0; i < args.size(); ++i) {
            args[i]->Print(data);
            if (i != args.size() - 1)
                std::cout << " , ";
        }
        std::cout << "))";
    }

    void GroupingExpr::Print(PrinterData& data) {
        expr->Print(data);
    }
    
    void VarExpr::Print(PrinterData& data) {
        std::cout << identifier.value;
    }

    void AssignExpr::Print(PrinterData& data) {
        std::cout << "(" << identifier.value << " = ";
        value->Print(data);
        std::cout << ")";
    }

    void ExprStmnt::Print(PrinterData& data) {
        std::cout << "(";
        expr->Print(data);
        std::cout << ")";
    }
    
    void ProjectMetadataStmnt::Print(PrinterData& data) {
        std::cout << "project " << identifier.value << " {";
        data.indent_stack.push('\t');

        if (metadata.size() > 0) {
            std::cout << "\n";

            for (uint32_t i = 0; i < metadata.size(); ++i) {
                for (uint32_t i = 0; i < data.indent_stack.size(); ++i)
                    std::cout << "    ";
                metadata[i]->Print(data);
                
                if (i != metadata.size() - 1)
                    std::cout << "\n";
            }

            std::cout << "\n";
        }

        data.indent_stack.pop();
        std::cout << "}";
    }

    void WindowStmnt::Print(PrinterData& data) {
        std::cout << "window " << identifier.value << " {\n";
        data.indent_stack.push('\t');
        
        if (description.size() > 0) {
            std::cout << "\n";

            for (uint32_t i = 0; i < description.size(); ++i) {
                for (uint32_t i = 0; i < data.indent_stack.size(); ++i)
                    std::cout << "    ";
                description[i]->Print(data);
                
                if (i != description.size() - 1)
                    std::cout << "\n";
            }
        }

        data.indent_stack.pop();
        std::cout << "}";
    }
    
    void NodeDeclStmnt::Print(PrinterData& data) {
        std::cout << "node<" << type.value << "> " << identifier.value;
        if (initializer != nullptr)
            initializer->Print(data);
        else 
            std::cout << ";";
    }

    void FunctionStmnt::Print(PrinterData& data) {
        std::cout << identifier.value << "<" << node.value << ">(";
        for (uint32_t i = 0; i < params.size(); ++i) {
            std::cout << params[i].value;
            if (i != params.size() - 1)
                std::cout << " , ";
        }
        std::cout << ") ";
        body->Print(data);
    }

    void VarDeclStmnt::Print(PrinterData& data) {
        std::cout << "(" << type.value << " " << identifier.value;
        if (initializer != nullptr) {
            std::cout << " = ";
            initializer->Print(data);
        }
        std::cout << ")";
    }

    void NodeBodyStmnt::Print(PrinterData& data) {
        std::cout << " {";
        data.indent_stack.push('\t');
        
        if (description.size() > 0) {
            std::cout << "\n";
            
            for (uint32_t i = 0; i < description.size(); ++i) { 

                for (uint32_t i = 0; i < data.indent_stack.size(); ++i)
                    std::cout << "    ";
                description[i]->Print(data);

                if (i != description.size() - 1)
                    std::cout << "\n";
            }

            std::cout << "\n";
        }

        data.indent_stack.pop();
        for (uint32_t i = 0; i < data.indent_stack.size(); ++i)
            std::cout << "    ";
        std::cout << "}";
    }

    void BlockStmnt::Print(PrinterData& data) {
        std::cout << "{";
        data.indent_stack.push('\t');

        if (statements.size() > 0) std::cout << "\n";
        for (uint32_t i = 0; i < statements.size(); ++i) {
            for (uint32_t i = 0; i < data.indent_stack.size(); ++i)
                std::cout << "    ";
            statements[i]->Print(data);
            std::cout << "\n";
        }
        
        std::cout << "}";
        data.indent_stack.pop();
    }
   
}