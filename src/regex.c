#include "regex.h"
#include <string.h>
#include <stdlib.h>

regex parse_regex(const char *str)
{
    // TODO
}

// this function checks if the character is a char like a, b, c, etc,
// not ., |, *, +, ?, (, ), etc. So this function
// returns 1 if its a normal character and 0 if its one of the special characters.
int is_normal_char(char t){
   const char *symbols = ".|*+?()";
   for (int i = 0; symbols[i] != '\0'; i++) {
        if (t == symbols[i]) {
            return 0;  
        }
    }
    return 1;
}

// this function checks if we need to add a concatenation operator between 
// two characters. Its simple, we need to add a concatenation operator, when the 
// first character is a normal character, a closing parenthesis, a star, a + or a ?, 
// for example, a(b*) its going to be a·(b*). So we return 1 if the first 
// character is one of the first cases and the second character is one of 
// the second cases. 
int needs_concatenation(char c1, char c2){
    int c1_ends = is_normal_char(c1) || c1 == ')' || c1 == '*' || c1 == '+'
    || c1 == '?';
    int c2_starts = is_normal_char(c2) || c2 == '(';
    if (c1 == ' ' || c2 == ' ') return 0; 
    // if one of the characters is a space,, 
    //we don't need to concatenate, if we do not check this, we will 
    //add a concatenation operator at the end of the string, i know this
    // because it happened to me xd. Something like 
    //aab a -> a.a.b. .a, we don't want that.
  return c1_ends && c2_starts;
}

// this function takes a regex string and returns a new string with the 
// concatenation operator added where needed.
char* insert_concatenation(const char *str){
    int length = strlen(str);
    char *new_str = malloc(2*length + 1);
    int j = 0;
    for (int i = 0; i < length; i++) {
        new_str[j++] = str[i];
        if (i < length - 1 && needs_concatenation(str[i], str[i + 1])) {
            new_str[j++] = '.'; 
        }   
    }
    new_str[j] = '\0';
    return new_str;
}

// This is just an aux function that returns the precedence of the 
// operators, we need this for the infix to postfix conversion, the 
// because the higher the precedence, the earlier it is evaluated,
// as we saw in class, the precedence is |,.,?,+,*
int precedence(char c) {
    switch (c) {
        case '*':
        case '+':
        case '?':
            return 3;
        case '.':
            return 2;
        case '|':
            return 1;
        default:
            return 0;
    }
}

// auxiliar functions for the stack
int is_empty(stack *s) {
    return s->top == -1;
}

char peek(stack *s) {
    return s->data[s->top];
}

void push(stack *s, char c) {
    if (s->top == s->capacity - 1) {
        s->capacity *= 2;
        s->data = realloc(s->data, s->capacity);
    }
    s->data[++s->top] = c;
}

char pop(stack *s) {
    return s->data[s->top--];
}

void free_stack(stack *s) {
    free(s->data);
}

// this function takes a regex string in infix notation and returns a new string 
// in postfix notation, we are going to use the shunting yard algorithm, 
// first we create an empty string for the postfix notation (because we don't
// want to modify the original) and a stack.
// for the operators, we pop from the stack to the output string until we find an 
// operator with less precedence or a left parenthesis, then we push the current 
// operator to the stack.
// It's important to handle the parentheses, when we find a left parenthesis, 
// we push it to the stack, when we find a right parenthesis, we pop from the stack 
// to the output string until we find a left parenthesis, then we discard the left 
// parenthesis from the stack, we don't put it in the output, just discard it. 
char* infix_to_postfix(const char *str) {
    int length = strlen(str);
    char *postfix = malloc(length + 1);
    postfix[0] = '\0';
    stack symbols;
    symbols.data = malloc(strlen(str));
    symbols.top = -1;
    symbols.capacity = length;
    for (int i = 0; i < length; i++) {
       if (is_normal_char(str[i])) {
            strncat(postfix, &str[i], 1);
        } else if (str[i] == '(') {
            push(&symbols, str[i]);
        } else if (str[i] == ')') {
            while (symbols.top >= 0 && peek(&symbols) != '(') {
                strncat(postfix, &symbols.data[symbols.top--], 1);
            }
            if (symbols.top >= 0 && peek(&symbols) == '(')
            {
                symbols.top--; // we discard the '(' from the stack
            }
            
        } else {
            while (symbols.top >= 0 && precedence(symbols.data[symbols.top]) 
            >= precedence(str[i])) {
                strncat(postfix, &symbols.data[symbols.top--], 1);
            }
            push(&symbols, str[i]);
        }
    }
    while (symbols.top >= 0) {
        strncat(postfix, &symbols.data[symbols.top--], 1);
    }
    free_stack(&symbols);
    return postfix;
}


#ifdef TEST
#include <stdio.h>

int main(void) {
    char input[64];

    printf("Test mode:\n");
    printf("1 <char>       - test is_normal_char\n");
    printf("2 <char1><char2> - test needs_concatenation\n");
    printf("3 <regex>      - test insert_concatenation\n");
    printf("4 <regex>      - test infix_to_postfix\n");
    printf("q              - quit\n\n");

    while (fgets(input, sizeof(input), stdin)) {
        if (input[0] == 'q') break;

        if (input[0] == '1') {
            char c = input[2];
            printf("is_normal_char('%c') = %d\n", c, is_normal_char(c));
        } else if (input[0] == '2') {
            char c1 = input[2];
            char c2 = input[3];
            printf("needs_concatenation('%c', '%c') = %d\n", c1, c2, needs_concatenation(c1, c2));
        } else if (input[0] == '3') {
            input[strcspn(input, "\n")] = '\0';
            char *regex = insert_concatenation(input + 2);
            printf("insert_concatenation(\"%s\") = \"%s\"\n", input + 2, regex);
            free(regex);
        } else if (input[0] == '4') {
            input[strcspn(input, "\n")] = '\0';
            char *postfix = infix_to_postfix(input + 2);
            printf("infix_to_postfix(\"%s\") = \"%s\"\n", input + 2, postfix);
            free(postfix);
        } else {
            printf("Invalid command\n");
        }
    }

    return 0;
}
#endif
