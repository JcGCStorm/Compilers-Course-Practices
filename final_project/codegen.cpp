#include "codegen.h"
#include <iostream>
#include <algorithm>

CodeGenerator::CodeGenerator() : temp_count(0), label_count(0) {}

std::string CodeGenerator::new_temp() {
    return "t" + std::to_string(temp_count++);
}

std::string CodeGenerator::new_label(const std::string& prefix) {
    return prefix + "_" + std::to_string(label_count++);
}

std::string CodeGenerator::generate_expr(Node* node) {
    if (!node) return "";

    if (node->type == "IntLit" || node->type == "Id" || node->type == "StringLit") {
        return node->value;
    }

    if (node->type == "Call") {
        Node* argsNode = node->children[0];
        // param
        for (auto arg : argsNode->children) {
            std::string arg_val = generate_expr(arg);
            out << "param " << arg_val << "\n";
        }
        out << "gtosub " << node->value << "\n";
        
        std::string temp = new_temp();
        out << temp << " = g_ret_int\n";
        return temp;
    }

    if (node->type.substr(0, 3) == "Op_") {
        std::string left = generate_expr(node->children[0]);
        std::string right = generate_expr(node->children[1]);
        std::string temp = new_temp();
        std::string op = "";
        
        if (node->type == "Op_Add") op = "+";
        else if (node->type == "Op_Sub") op = "-";
        else if (node->type == "Op_Mul") op = "*";
        else if (node->type == "Op_Div") op = "/";
        else if (node->type == "Op_Eq") op = "==";
        else if (node->type == "Op_Neq") op = "!=";
        else if (node->type == "Op_Lt") op = "<";
        else if (node->type == "Op_Gt") op = ">";
        
        out << temp << " = " << left << " " << op << " " << right << "\n";
        return temp;
    }

    return "";
}

void CodeGenerator::generate_stmt(Node* node) {
    if (!node) return;

    if (node->type == "Block" || node->type == "Program" || node->type == "Declarations") {
        for (auto child : node->children) {
            generate_stmt(child);
        }
        return;
    }

    if (node->type == "VarDeclInit") {
        std::string right = generate_expr(node->children[0]);
        out << node->value << " = " << right << "\n";
        return;
    }

    if (node->type == "VarDecl") {
        // Just declaration, no immediate code needed unless initializing to 0
        return;
    }

    if (node->type == "Assign") {
        std::string right = generate_expr(node->children[0]);
        out << node->value << " = " << right << "\n";
        return;
    }

    if (node->type == "FuncDecl") {
        out << node->value << ":\n"; // label for function
        Node* paramsNode = node->children[0];
        Node* blockNode = node->children[1];

        // paramGet in reverse order (LIFO)
        std::vector<Node*> params = paramsNode->children;
        std::reverse(params.begin(), params.end());
        for (auto p : params) {
            out << "paramGet " << p->value << "\n";
        }

        generate_stmt(blockNode);
        
        // implicit return at end if missing
        out << "return\n";
        return;
    }

    if (node->type == "Return") {
        if (node->children.size() > 0) {
            std::string ret_val = generate_expr(node->children[0]);
            out << "g_ret_int = " << ret_val << "\n";
        }
        out << "return\n";
        return;
    }

    if (node->type == "If") {
        std::string cond = generate_expr(node->children[0]);
        std::string LEnd = new_label("if_end");
        
        out << "if false " << cond << " goto " << LEnd << "\n";
        generate_stmt(node->children[1]);
        out << LEnd << ":\n";
        return;
    }

    if (node->type == "IfElse") {
        std::string cond = generate_expr(node->children[0]);
        std::string LElse = new_label("if_else");
        std::string LEnd = new_label("if_end");
        
        out << "if false " << cond << " goto " << LElse << "\n";
        generate_stmt(node->children[1]); // if true
        out << "goto " << LEnd << "\n";
        out << LElse << ":\n";
        generate_stmt(node->children[2]); // else
        out << LEnd << ":\n";
        return;
    }

    if (node->type == "While") {
        std::string LStart = new_label("while_start");
        std::string LEnd = new_label("while_end");
        
        out << LStart << ":\n";
        std::string cond = generate_expr(node->children[0]);
        out << "if false " << cond << " goto " << LEnd << "\n";
        generate_stmt(node->children[1]);
        out << "goto " << LStart << "\n";
        out << LEnd << ":\n";
        return;
    }

    if (node->type == "Print") {
        std::string val = generate_expr(node->children[0]);
        out << "print " << val << "\n";
        return;
    }

    if (node->type == "Input") {
        out << "input " << node->value << "\n";
        return;
    }

    // Call as a statement
    if (node->type == "Call") {
        generate_expr(node);
        return;
    }
}

bool CodeGenerator::generate(Node* root, const std::string& filename) {
    out.open(filename);
    if (!out.is_open()) return false;
    
    // Jump to main implicitly? Usually we just assume execution starts from the top.
    // If the virtual machine starts at top, we might need to jump over functions to main or require a main function.
    // For now we just traverse AST. We'll emit code in order.
    out << "goto main\n"; // Assuming a 'main' function is required

    generate_stmt(root);
    
    out.close();
    return true;
}
