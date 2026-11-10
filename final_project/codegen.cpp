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

    if (node->type == "IntLit" || node->type == "Id") {
        return node->value;
    }
    if (node->type == "StringLit") {
        return "\"" + node->value + "\"";
    }

    if (node->type == "Call") {
        Node* argsNode = node->children[0];
        for (auto arg : argsNode->children) {
            std::string arg_val = generate_expr(arg);
            out << "PARAM " << arg_val << "\n";
        }
        out << "GOSUB " << node->value << "\n";
        
        std::string temp = new_temp();
        out << "VAR " << temp << "\n";
        out << "ASSIGN g_ret_int " << temp << "\n";
        return temp;
    }

    if (node->type.substr(0, 3) == "Op_") {
        std::string left = generate_expr(node->children[0]);
        std::string right = generate_expr(node->children[1]);
        std::string temp = new_temp();
        out << "VAR " << temp << "\n";
        std::string op = "";
        
        if (node->type == "Op_Add") op = "ADD";
        else if (node->type == "Op_Sub") op = "SUB";
        else if (node->type == "Op_Mul") op = "MUL";
        else if (node->type == "Op_Div") op = "DIV";
        else if (node->type == "Op_Mod") op = "MOD";
        else if (node->type == "Op_Eq") op = "EQ";
        else if (node->type == "Op_Neq") op = "NEQ";
        else if (node->type == "Op_Lt") op = "LT";
        else if (node->type == "Op_Gt") op = "GT";
        
        out << op << " " << left << " " << right << " " << temp << "\n";
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
        out << "VAR " << node->value << "\n";
        out << "ASSIGN " << right << " " << node->value << "\n";
        return;
    }

    if (node->type == "VarDecl") {
        out << "VAR " << node->value << "\n";
        return;
    }

    if (node->type == "Assign") {
        std::string right = generate_expr(node->children[0]);
        out << "ASSIGN " << right << " " << node->value << "\n";
        return;
    }

    if (node->type == "FuncDecl") {
        out << "LABEL " << node->value << "\n";
        Node* paramsNode = node->children[0];
        Node* blockNode = node->children[1];

        std::vector<Node*> params = paramsNode->children;
        std::reverse(params.begin(), params.end());
        for (auto p : params) {
            out << "VAR " << p->value << "\n";
            out << "PARAM_GET " << p->value << "\n";
        }

        generate_stmt(blockNode);
        
        if (node->value != "main") {
            out << "RETURN\n";
        }
        return;
    }

    if (node->type == "Return") {
        if (node->children.size() > 0) {
            std::string ret_val = generate_expr(node->children[0]);
            out << "ASSIGN " << ret_val << " g_ret_int\n";
        }
        out << "RETURN\n";
        return;
    }

    if (node->type == "If") {
        std::string cond = generate_expr(node->children[0]);
        std::string LEnd = new_label("if_end");
        
        out << "IFFALSE " << cond << " GOTO " << LEnd << "\n";
        generate_stmt(node->children[1]);
        out << "LABEL " << LEnd << "\n";
        return;
    }

    if (node->type == "IfElse") {
        std::string cond = generate_expr(node->children[0]);
        std::string LElse = new_label("if_else");
        std::string LEnd = new_label("if_end");
        
        out << "IFFALSE " << cond << " GOTO " << LElse << "\n";
        generate_stmt(node->children[1]); 
        out << "GOTO " << LEnd << "\n";
        out << "LABEL " << LElse << "\n";
        generate_stmt(node->children[2]); 
        out << "LABEL " << LEnd << "\n";
        return;
    }

    if (node->type == "While") {
        std::string LStart = new_label("while_start");
        std::string LEnd = new_label("while_end");
        
        out << "LABEL " << LStart << "\n";
        std::string cond = generate_expr(node->children[0]);
        out << "IFFALSE " << cond << " GOTO " << LEnd << "\n";
        generate_stmt(node->children[1]);
        out << "GOTO " << LStart << "\n";
        out << "LABEL " << LEnd << "\n";
        return;
    }

    if (node->type == "Print") {
        std::string val = generate_expr(node->children[0]);
        out << "PRINT " << val << "\n";
        return;
    }

    if (node->type == "Input") {
        out << "INPUT " << node->value << "\n";
        return;
    }

    if (node->type == "Call") {
        generate_expr(node);
        return;
    }
}

bool CodeGenerator::generate(Node* root, const std::string& filename) {
    out.open(filename);
    if (!out.is_open()) return false;
    
    out << "VAR g_ret_int\n";
    // We let execution fall through into LABEL main
    
    generate_stmt(root);
    
    out.close();
    return true;
}
