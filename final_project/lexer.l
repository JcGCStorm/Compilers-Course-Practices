%{
#include <iostream>
#include <string>
#include "ast.h"
#include "parser.tab.hpp"

extern int yylineno;
%}

%option noyywrap
%option yylineno

%%

[ \t\r]+                { /* ignorar espacios */ }
\n                      { /* newline */ }
"//".*                  { /* comentarios */ }

"--."                   { return KW_INT; }
".-"                    { return KW_IF; }
".."                    { return KW_ELSE; }
"-..."                  { return KW_WHILE; }
"-.-."                  { return KW_FUNC; }
"-.."                   { return KW_RETURN; }
"."                     { return KW_PRINT; }
"..-"                   { return KW_INPUT; }

"-...-"                 { return ASSIGN; }
".-.-."                 { return PLUS; }
"-....-"                { return MINUS; }
"--.--"                 { return STAR; }
"-..-."                 { return SLASH; }
"..--.."                { return EQ; }
"-.-.--"                { return NEQ; }
".-.-"                  { return LT; }
".-..."                 { return GT; }
"-.--."                 { return LBRACE; }
"-.--.-"                { return RBRACE; }
".--."                  { return LPAREN; }
".--.-"                 { return RPAREN; }
"--..--"                { return COMMA; }
"-.-.-."                { return SEMI; }

[0-9]+                  { yylval.sval = strdup(yytext); return INT_LIT; }
\"[^\"]*\"              { yylval.sval = strdup(yytext); return STRING_LIT; }
[a-zA-Z_][a-zA-Z0-9_]*  { yylval.sval = strdup(yytext); return ID; }

.                       { std::cerr << "Error léxico: " << yytext << " en línea " << yylineno << "\n"; }

%%
