// If we have not defined REGEX_H yet, we define it and include 
// the contents of this file. Either way, we prevent multiple inclusions 
// of this file.
#ifndef REGEX_H
#define REGEX_H

// With this we define all the possible token types that we can encounter
// in a regex.
typedef enum {
    TOKEN_CHAR,
    TOKEN_DOT,
    TOKEN_STAR,
    TOKEN_PLUS,
    TOKEN_QUESTION,
    TOKEN_PIPE,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_EOF
} token_type;

// With this we defined a token, which is just a charaacter and its type.
typedef struct {
    char value;
    token_type type;
} token;

// With this we define a regex, which is just an array of tokens.
// Its important to keep track of the size and capacity of the array, 
// so we can resize it when needed.
typedef struct {
    token *items;
    int size;
    int capacity;
} regex;

// With this we define a stack, which is just an array of characters.
typedef struct {
    char *data;
    int top;
    int capacity;
} stack;

regex parse_regex(const char *str);

#endif
