#include <iostream>
#include <fstream>
#include "ast.h"
#include "semantic.h"
#include "codegen.h"

extern int yyparse();
extern FILE* yyin;
extern Node* root;

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <archivo.morse>" << std::endl;
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        std::cerr << "No se pudo abrir el archivo: " << argv[1] << std::endl;
        return 1;
    }

    yyin = file;
    
    std::cout << "--- Analizando " << argv[1] << " ---" << std::endl;
    if (yyparse() == 0 && root != nullptr) {
        std::cout << "Análisis sintáctico exitoso. AST:\n";
        root->print();

        std::cout << "\n--- Análisis Semántico ---" << std::endl;
        SemanticAnalyzer semantic;
        if (semantic.analyze(root)) {
            std::cout << "Análisis semántico exitoso.\n";

            std::cout << "\n--- Generación de Código ---" << std::endl;
            CodeGenerator codegen;
            std::string outfile = "out.txt";
            if (codegen.generate(root, outfile)) {
                std::cout << "Código intermedio generado en: " << outfile << std::endl;
            } else {
                std::cerr << "Error en la generación de código.\n";
            }
        } else {
            std::cerr << "Errores semánticos encontrados.\n";
        }
    } else {
        std::cerr << "Error en el análisis sintáctico." << std::endl;
    }

    fclose(file);
    if (root) delete root;
    return 0;
}
