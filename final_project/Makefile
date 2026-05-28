CXX = g++
CXXFLAGS = -std=c++17 -Wall -g
FLEX = flex
BISON = bison

all: morse_compiler

parser.tab.cpp parser.tab.hpp: parser.y
	$(BISON) -d -o parser.tab.cpp parser.y

lex.yy.cpp: lexer.l parser.tab.hpp
	$(FLEX) -o lex.yy.cpp lexer.l

morse_compiler: lex.yy.cpp parser.tab.cpp main.cpp semantic.cpp codegen.cpp
	$(CXX) $(CXXFLAGS) -o morse_compiler lex.yy.cpp parser.tab.cpp main.cpp semantic.cpp codegen.cpp

clean:
	rm -f lex.yy.cpp parser.tab.cpp parser.tab.hpp morse_compiler
