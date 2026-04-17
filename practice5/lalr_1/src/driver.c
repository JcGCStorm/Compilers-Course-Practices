#include "grammar.h"
#include "automaton.h"
#include "parser.h"
#include "scanner.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

extern int yylex(void);
extern char *yytext;
extern FILE *yyin;
extern int yylineno; // provided by flex if %option yylineno is used

typedef struct token_stream
{
    int lexer_token;
    int terminal_id;
    const char *lexeme;
} token_stream;

typedef struct parser_stack
{
    int *states;
    int size;
    int capacity;
} parser_stack;

static char *read_file_all(const char *path)
{
    if (path == NULL) return NULL;
    FILE *file = fopen(path, "rb");
    if (file == NULL) return NULL;
    if (fseek(file, 0, SEEK_END) != 0) { fclose(file); return NULL; }
    long length = ftell(file);
    if (length < 0) { fclose(file); return NULL; }
    if (fseek(file, 0, SEEK_SET) != 0) { fclose(file); return NULL; }
    char *buffer = (char *)malloc((size_t)length + 1);
    if (buffer == NULL) { fclose(file); return NULL; }
    size_t read_count = fread(buffer, 1, (size_t)length, file);
    fclose(file);
    if (read_count != (size_t)length) { free(buffer); return NULL; }
    buffer[length] = '\0';
    return buffer;
}

static int find_terminal_id(const grammar *g, const char *symbol_name)
{
    if (g == NULL || symbol_name == NULL) return -1;
    for (int i = 0; i < g->num_terminals; i++) {
        if (strcmp(g->terminals[i].symbol, symbol_name) == 0) return i;
    }
    return -1;
}

static int map_lexer_token_to_terminal_id(const grammar *g, int lexer_token, const char *lexeme)
{
    if (g == NULL) return -1;
    if (lexer_token == TOK_EOF) return g->num_terminals;
    if (lexer_token == TOK_ERROR) return -1;

    int terminal_id = find_terminal_id(g, lexeme);
    if (terminal_id >= 0) return terminal_id;

    switch (lexer_token) {
    case TOK_IDENTIFIER:
        terminal_id = find_terminal_id(g, "IDENTIFIER");
        if (terminal_id < 0) terminal_id = find_terminal_id(g, "ID");
        if (terminal_id < 0) terminal_id = find_terminal_id(g, "id");
        return terminal_id;
    case TOK_INT_LITERAL:
        terminal_id = find_terminal_id(g, "INT_LITERAL");
        if (terminal_id < 0) terminal_id = find_terminal_id(g, "INT");
        if (terminal_id < 0) terminal_id = find_terminal_id(g, "num");
        return terminal_id;
    case TOK_FLOAT_LITERAL:
        terminal_id = find_terminal_id(g, "FLOAT_LITERAL");
        if (terminal_id < 0) terminal_id = find_terminal_id(g, "FLOAT");
        if (terminal_id < 0) terminal_id = find_terminal_id(g, "num");
        return terminal_id;
    case TOK_STRING_LITERAL:
        terminal_id = find_terminal_id(g, "STRING_LITERAL");
        if (terminal_id < 0) terminal_id = find_terminal_id(g, "STRING");
        if (terminal_id < 0) terminal_id = find_terminal_id(g, "str");
        return terminal_id;
    case TOK_CHAR_LITERAL:
        terminal_id = find_terminal_id(g, "CHAR_LITERAL");
        if (terminal_id < 0) terminal_id = find_terminal_id(g, "CHAR");
        if (terminal_id < 0) terminal_id = find_terminal_id(g, "char_lit");
        return terminal_id;
    default: break;
    }

    if (lexer_token > 0 && lexer_token <= 127) {
        char one_char_symbol[2] = {(char)lexer_token, '\0'};
        terminal_id = find_terminal_id(g, one_char_symbol);
        if (terminal_id >= 0) return terminal_id;
    }
    return -1;
}

static bool init_parser_stack(parser_stack *stack)
{
    if (stack == NULL) return false;
    stack->capacity = 64;
    stack->size = 0;
    stack->states = (int *)malloc((size_t)stack->capacity * sizeof(int));
    if (stack->states == NULL) { stack->capacity = 0; return false; }
    stack->states[stack->size++] = 0;
    return true;
}

static void free_parser_stack(parser_stack *stack)
{
    if (stack == NULL) return;
    free(stack->states);
    stack->states = NULL;
    stack->size = 0;
    stack->capacity = 0;
}

static bool push_state(parser_stack *stack, int state_id)
{
    if (stack == NULL) return false;
    if (stack->size >= stack->capacity) {
        int new_capacity = stack->capacity * 2;
        int *resized = (int *)realloc(stack->states, (size_t)new_capacity * sizeof(int));
        if (resized == NULL) return false;
        stack->states = resized;
        stack->capacity = new_capacity;
    }
    stack->states[stack->size++] = state_id;
    return true;
}

static bool pop_states(parser_stack *stack, int count)
{
    if (stack == NULL || count < 0 || stack->size - count <= 0) return false;
    stack->size -= count;
    return true;
}

static int top_state(const parser_stack *stack)
{
    if (stack == NULL || stack->size <= 0) return -1;
    return stack->states[stack->size - 1];
}

static int reduction_pop_count(const grammar *g, production p)
{
    int epsilon_id = find_terminal_id(g, "epsilon");
    int count = 0;
    for (int i = 0; i < p.production_length; i++) {
        int symbol_id = p.production_symbol_ids[i];
        if (symbol_id == epsilon_id) continue;
        count++;
    }
    return count;
}

static bool next_token(const grammar *g, token_stream *out_token)
{
    if (g == NULL || out_token == NULL) return false;
    int lexer_token = yylex();
    const char *lexeme = yytext != NULL ? yytext : "";
    int terminal_id = map_lexer_token_to_terminal_id(g, lexer_token, lexeme);
    out_token->lexer_token = lexer_token;
    out_token->terminal_id = terminal_id;
    out_token->lexeme = lexeme;
    return terminal_id >= 0;
}

static void print_expected_terminals(const grammar *g, const parser_table *table, int state_id)
{
    printf("Expected one of the following tokens: ");
    bool first = true;
    for (int t = 0; t < table->num_terminals_with_eof; t++) {
        parser_action action = get_parser_action(table, state_id, t);
        if (action.type != PARSER_ACTION_ERROR) {
            if (!first) printf(", ");
            if (t == g->num_terminals) {
                printf("'$' (EOF)");
            } else {
                printf("'%s'", g->terminals[t].symbol);
            }
            first = false;
        }
    }
    printf("\n");
}

static bool validate_token_stream(const grammar *g, const parser_table *table)
{
    if (g == NULL || table == NULL) return false;
    parser_stack stack;
    if (!init_parser_stack(&stack)) return false;

    token_stream lookahead;
    if (!next_token(g, &lookahead)) {
        fprintf(stderr, "Lexical error at line %d: Token '%s' is not recognized by the grammar.\n", yylineno, yytext != NULL ? yytext : "");
        free_parser_stack(&stack);
        return false;
    }

    while (true) {
        int state_id = top_state(&stack);
        parser_action action = get_parser_action(table, state_id, lookahead.terminal_id);

        if (action.type == PARSER_ACTION_SHIFT) {
            if (!push_state(&stack, action.value)) {
                fprintf(stderr, "Parser stack overflow while shifting.\n");
                free_parser_stack(&stack);
                return false;
            }
            if (!next_token(g, &lookahead)) {
                fprintf(stderr, "Lexical error at line %d: Token '%s' is not recognized by the grammar.\n", yylineno, yytext != NULL ? yytext : "");
                free_parser_stack(&stack);
                return false;
            }
            continue;
        }

        if (action.type == PARSER_ACTION_REDUCE) {
            if (action.value < 0 || action.value >= g->num_productions) {
                fprintf(stderr, "Invalid reduction production index: %d\n", action.value);
                free_parser_stack(&stack);
                return false;
            }
            production p = g->productions[action.value];
            int pop_count = reduction_pop_count(g, p);
            if (!pop_states(&stack, pop_count)) {
                fprintf(stderr, "Invalid parser stack pop for production %d\n", action.value);
                free_parser_stack(&stack);
                return false;
            }
            int goto_from = top_state(&stack);
            int goto_state = get_parser_goto(table, goto_from, p.non_terminal_id);
            if (goto_state < 0) {
                fprintf(stderr, "Missing GOTO[%d, %s] after reduction p%d\n", goto_from, g->non_terminals[p.non_terminal_id].symbol, action.value);
                free_parser_stack(&stack);
                return false;
            }
            if (!push_state(&stack, goto_state)) {
                fprintf(stderr, "Parser stack overflow after reduction.\n");
                free_parser_stack(&stack);
                return false;
            }
            continue;
        }

        if (action.type == PARSER_ACTION_ACCEPT) {
            free_parser_stack(&stack);
            return true;
        }

        fprintf(stderr, "\n=== SYNTAX ERROR ===\n");
        fprintf(stderr, "Line: %d\n", yylineno);
        fprintf(stderr, "Unexpected token: '%s'\n", lookahead.lexeme);
        print_expected_terminals(g, table, state_id);
        fprintf(stderr, "====================\n\n");
        free_parser_stack(&stack);
        return false;
    }
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <grammar_file> <parse_table.csv> [source_file]\n", argv[0]);
        return 1;
    }

    const char *grammar_path = argv[1];
    const char *table_path = argv[2];
    const char *source_path = argc > 3 ? argv[3] : NULL;

    char *grammar_content = read_file_all(grammar_path);
    if (!grammar_content) {
        fprintf(stderr, "Failed to read grammar file '%s'\n", grammar_path);
        return 1;
    }

    grammar *g = create_grammar(grammar_content);
    free(grammar_content);
    if (!g) {
        fprintf(stderr, "Failed to parse grammar '%s'\n", grammar_path);
        return 1;
    }

    parser_table *table = load_parser_table_csv(g, table_path);
    if (!table) {
        fprintf(stderr, "Failed to load/parse CSV from '%s'\n", table_path);
        return 1;
    }

    if (source_path) {
        yyin = fopen(source_path, "r");
        if (!yyin) {
            fprintf(stderr, "Failed to open source file '%s': %s\n", source_path, strerror(errno));
            free_parser_table(table);
            return 1;
        }
    }

    bool accepted = validate_token_stream(g, table);
    if (accepted) {
        printf("Input is VALID according to the LALR table.\n");
    } else {
        printf("Input is REJECTED by the driver.\n");
    }

    free_parser_table(table);
    if (source_path && yyin) fclose(yyin);

    return accepted ? 0 : 2;
}
