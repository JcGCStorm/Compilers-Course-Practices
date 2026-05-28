#include "semantic.h"

SemanticAnalyzer::SemanticAnalyzer() : has_error(false) {
    enter_scope(); // Global scope
}

void SemanticAnalyzer::enter_scope() {
    symbol_table_stack.push_back(std::map<std::string, std::string>());
}

void SemanticAnalyzer::leave_scope() {
    if (!symbol_table_stack.empty()) {
        symbol_table_stack.pop_back();
    }
}

bool SemanticAnalyzer::declare_symbol(const std::string& name, const std::string& type) {
    if (symbol_table_stack.back().count(name) > 0) {
        std::cerr << "Error Semántico: Símbolo '" << name << "' ya declarado en este ámbito.\n";
        has_error = true;
        return false;
    }
    symbol_table_stack.back()[name] = type;
    return true;
}

bool SemanticAnalyzer::check_symbol(const std::string& name) {
    for (auto it = symbol_table_stack.rbegin(); it != symbol_table_stack.rend(); ++it) {
        if (it->count(name) > 0) {
            return true;
        }
    }
    std::cerr << "Error Semántico: Símbolo '" << name << "' no declarado.\n";
    has_error = true;
    return false;
}

void SemanticAnalyzer::analyze_node(Node* node) {
    if (!node) return;

    if (node->type == "Block") {
        enter_scope();
        for (auto child : node->children) analyze_node(child);
        leave_scope();
        return;
    }

    if (node->type == "FuncDecl") {
        declare_symbol(node->value, "function");
        enter_scope();
        for (auto child : node->children) analyze_node(child);
        leave_scope();
        return;
    }

    if (node->type == "VarDecl" || node->type == "VarDeclInit" || node->type == "Param") {
        declare_symbol(node->value, "int");
        if (node->type == "VarDeclInit") {
            analyze_node(node->children[0]);
        }
        return;
    }

    if (node->type == "Assign") {
        check_symbol(node->value);
        analyze_node(node->children[0]);
        return;
    }

    if (node->type == "Id") {
        check_symbol(node->value);
        return;
    }

    if (node->type == "Call") {
        check_symbol(node->value);
        for (auto child : node->children) analyze_node(child);
        return;
    }

    if (node->type == "Input") {
        check_symbol(node->value);
        return;
    }

    for (auto child : node->children) {
        analyze_node(child);
    }
}

bool SemanticAnalyzer::analyze(Node* root) {
    has_error = false;
    analyze_node(root);
    return !has_error;
}
