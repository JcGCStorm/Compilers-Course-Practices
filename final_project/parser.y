%{
#include <iostream>
#include <string>
#include "ast.h"

extern int yylex();
extern int yylineno;
extern char* yytext;
void yyerror(const char* s) {
    std::cerr << "Error sintáctico: " << s << " en línea " << yylineno << std::endl;
}

Node* root = nullptr;
%}

%union {
    char* sval;
    Node* node;
}

%token <sval> ID INT_LIT STRING_LIT
%token KW_INT KW_IF KW_ELSE KW_WHILE KW_FUNC KW_RETURN KW_PRINT KW_INPUT
%token ASSIGN PLUS MINUS STAR SLASH EQ NEQ LT GT
%token LBRACE RBRACE LPAREN RPAREN COMMA SEMI

%type <node> program decl_list decl var_decl func_decl param_list params param block
%type <node> stmt_list stmt expr_stmt if_stmt while_stmt return_stmt print_stmt input_stmt assign_stmt call_stmt
%type <node> expr logic_expr rel_expr add_expr mul_expr primary args arg_list

%%

program:
    decl_list { root = new Node("Program"); root->add_child($1); }
    ;

decl_list:
    decl_list decl { $$ = $1; $$->add_child($2); }
    | decl { $$ = new Node("Declarations"); $$->add_child($1); }
    ;

decl:
    var_decl { $$ = $1; }
    | func_decl { $$ = $1; }
    ;

var_decl:
    KW_INT ID SEMI { 
        $$ = new Node("VarDecl", $2); 
        free($2); 
    }
    | KW_INT ID ASSIGN expr SEMI {
        $$ = new Node("VarDeclInit", $2);
        $$->add_child($4);
        free($2);
    }
    ;

func_decl:
    KW_FUNC KW_INT ID LPAREN param_list RPAREN block {
        $$ = new Node("FuncDecl", $3);
        $$->add_child($5);
        $$->add_child($7);
        free($3);
    }
    | KW_FUNC KW_INT ID LPAREN RPAREN block {
        $$ = new Node("FuncDecl", $3);
        $$->add_child(new Node("Params"));
        $$->add_child($6);
        free($3);
    }
    ;

param_list:
    params { $$ = new Node("Params"); $$->children = $1->children; delete $1; }
    ;

params:
    params COMMA param { $$ = $1; $$->add_child($3); }
    | param { $$ = new Node("TempParams"); $$->add_child($1); }
    ;

param:
    KW_INT ID { $$ = new Node("Param", $2); free($2); }
    ;

block:
    LBRACE stmt_list RBRACE { $$ = $2; }
    | LBRACE RBRACE { $$ = new Node("Block"); }
    ;

stmt_list:
    stmt_list stmt { $$ = $1; $$->add_child($2); }
    | stmt { $$ = new Node("Block"); $$->add_child($1); }
    ;

stmt:
    var_decl { $$ = $1; }
    | expr_stmt { $$ = $1; }
    | if_stmt { $$ = $1; }
    | while_stmt { $$ = $1; }
    | return_stmt { $$ = $1; }
    | print_stmt { $$ = $1; }
    | input_stmt { $$ = $1; }
    | block { $$ = $1; }
    ;

expr_stmt:
    expr SEMI { $$ = $1; }
    | assign_stmt SEMI { $$ = $1; }
    ;

assign_stmt:
    ID ASSIGN expr {
        $$ = new Node("Assign", $1);
        $$->add_child($3);
        free($1);
    }
    ;

if_stmt:
    KW_IF LPAREN expr RPAREN stmt KW_ELSE stmt {
        $$ = new Node("IfElse");
        $$->add_child($3);
        $$->add_child($5);
        $$->add_child($7);
    }
    | KW_IF LPAREN expr RPAREN stmt {
        $$ = new Node("If");
        $$->add_child($3);
        $$->add_child($5);
    }
    ;

while_stmt:
    KW_WHILE LPAREN expr RPAREN stmt {
        $$ = new Node("While");
        $$->add_child($3);
        $$->add_child($5);
    }
    ;

return_stmt:
    KW_RETURN expr SEMI {
        $$ = new Node("Return");
        $$->add_child($2);
    }
    | KW_RETURN SEMI {
        $$ = new Node("Return");
    }
    ;

print_stmt:
    KW_PRINT expr SEMI {
        $$ = new Node("Print");
        $$->add_child($2);
    }
    ;

input_stmt:
    KW_INPUT ID SEMI {
        $$ = new Node("Input", $2);
        free($2);
    }
    ;

expr:
    logic_expr { $$ = $1; }
    ;

logic_expr:
    rel_expr { $$ = $1; }
    ;

rel_expr:
    rel_expr EQ add_expr { $$ = new Node("Op_Eq"); $$->add_child($1); $$->add_child($3); }
    | rel_expr NEQ add_expr { $$ = new Node("Op_Neq"); $$->add_child($1); $$->add_child($3); }
    | rel_expr LT add_expr { $$ = new Node("Op_Lt"); $$->add_child($1); $$->add_child($3); }
    | rel_expr GT add_expr { $$ = new Node("Op_Gt"); $$->add_child($1); $$->add_child($3); }
    | add_expr { $$ = $1; }
    ;

add_expr:
    add_expr PLUS mul_expr { $$ = new Node("Op_Add"); $$->add_child($1); $$->add_child($3); }
    | add_expr MINUS mul_expr { $$ = new Node("Op_Sub"); $$->add_child($1); $$->add_child($3); }
    | mul_expr { $$ = $1; }
    ;

mul_expr:
    mul_expr STAR primary { $$ = new Node("Op_Mul"); $$->add_child($1); $$->add_child($3); }
    | mul_expr SLASH primary { $$ = new Node("Op_Div"); $$->add_child($1); $$->add_child($3); }
    | primary { $$ = $1; }
    ;

primary:
    INT_LIT { $$ = new Node("IntLit", $1); free($1); }
    | STRING_LIT { $$ = new Node("StringLit", $1); free($1); }
    | ID { $$ = new Node("Id", $1); free($1); }
    | LPAREN expr RPAREN { $$ = $2; }
    | call_stmt { $$ = $1; }
    ;

call_stmt:
    ID LPAREN args RPAREN {
        $$ = new Node("Call", $1);
        $$->add_child($3);
        free($1);
    }
    | ID LPAREN RPAREN {
        $$ = new Node("Call", $1);
        $$->add_child(new Node("Args"));
        free($1);
    }
    ;

args:
    arg_list { $$ = new Node("Args"); $$->children = $1->children; delete $1; }
    ;

arg_list:
    arg_list COMMA expr { $$ = $1; $$->add_child($3); }
    | expr { $$ = new Node("TempArgs"); $$->add_child($1); }
    ;

%%
