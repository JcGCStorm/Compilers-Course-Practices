#include "regex.h"

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
  return c1_ends && c2_starts;
}

#ifdef TEST
#include <stdio.h>

int main(void) {
    char input[64];

    printf("Test mode:\n");
    printf("1 <char>       - test is_normal_char\n");
    printf("2 <char1><char2> - test needs_concatenation\n");
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
        }
    }

    return 0;
}
#endif
