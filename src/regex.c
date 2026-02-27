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
