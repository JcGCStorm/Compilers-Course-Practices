#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include <map>
#include <vector>
#include <string>
#include <iostream>

class SemanticAnalyzer {
private:
    std::vector<std::map<std::string, std::string>> symbol_table_stack;
    bool has_error;

    void enter_scope();
    void leave_scope();
    bool declare_symbol(const std::string& name, const std::string& type);
    bool check_symbol(const std::string& name);

    void analyze_node(Node* node);

public:
    SemanticAnalyzer();
    bool analyze(Node* root);
};

#endif
