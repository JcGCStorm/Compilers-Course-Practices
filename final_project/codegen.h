#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include <fstream>
#include <string>
#include <vector>

class CodeGenerator {
private:
    std::ofstream out;
    int temp_count;
    int label_count;

    std::string new_temp();
    std::string new_label(const std::string& prefix = "L");

    std::string generate_expr(Node* node);
    void generate_stmt(Node* node);

public:
    CodeGenerator();
    bool generate(Node* root, const std::string& filename);
};

#endif
