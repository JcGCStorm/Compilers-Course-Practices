#ifndef AST_H
#define AST_H

#include <iostream>
#include <vector>
#include <string>

class Node {
public:
    std::string type;
    std::string value;
    std::vector<Node*> children;

    Node(std::string t, std::string v = "") : type(t), value(v) {}
    virtual ~Node() {
        for (Node* child : children) {
            delete child;
        }
    }

    void add_child(Node* child) {
        if (child) children.push_back(child);
    }

    void print(int indent = 0) {
        for (int i = 0; i < indent; i++) std::cout << "  ";
        std::cout << "|-- " << type << (value.empty() ? "" : ": " + value) << std::endl;
        for (auto child : children) {
            child->print(indent + 1);
        }
    }
};

#endif
