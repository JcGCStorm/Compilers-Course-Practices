%option noyywrap nodefault yylineno

%{
#include "ast.h"
#include "parser.tab.hpp"

#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>



int yycolumn = 1;

static std::unordered_map<std::string, std::string> make_letter_map() {
    return {
        {".-", "a"},   {"-...", "b"}, {"-.-.", "c"}, {"-..", "d"},  {".", "e"},
        {"..-.", "f"}, {"--.", "g"},  {"....", "h"}, {"..", "i"},   {".---", "j"},
        {"-.-", "k"},  {".-..", "l"}, {"--", "m"},   {"-.", "n"},   {"---", "o"},
        {".--.", "p"}, {"--.-", "q"}, {".-.", "r"},  {"...", "s"},  {"-", "t"},
        {"..-", "u"},  {"...-", "v"}, {".--", "w"},  {"-..-", "x"}, {"-.--", "y"},
        {"--..", "z"}
    };
}

static std::unordered_map<std::string, std::string> make_digit_map() {
    return {
        {"-----", "0"}, {".----", "1"}, {"..---", "2"}, {"...--", "3"}, {"....-", "4"},
        {".....", "5"}, {"-....", "6"}, {"--...", "7"}, {"---..", "8"}, {"----.", "9"}
    };
}

static const std::unordered_map<std::string, std::string> kLetters = make_letter_map();
static const std::unordered_map<std::string, std::string> kDigits = make_digit_map();

static bool split_token(const std::string& text, std::vector<std::string>& parts) {
    std::size_t start = 0;
    while (start <= text.size()) {
        std::size_t slash = text.find('/', start);
        std::string part = slash == std::string::npos
            ? text.substr(start)
            : text.substr(start, slash - start);

        if (part.empty()) {
            return false;
        }

        parts.push_back(part);
        if (slash == std::string::npos) {
            break;
        }
        start = slash + 1;
    }
    return !parts.empty();
}

static bool decode_parts(
    const std::vector<std::string>& parts,
    const std::unordered_map<std::string, std::string>& table,
    std::string& out
) {
    out.clear();
    for (const auto& part : parts) {
        auto found = table.find(part);
        if (found == table.end()) {
            return false;
        }
        out += found->second;
    }
    return true;
}

static int classify_morse_token(const std::string& text) {
    std::vector<std::string> parts;
    if (!split_token(text, parts)) {
        return 256;
    }

    if (text == "..-.-") return KW_INT;
    if (text == "..-..-") return 256;
    if (text == "...-..") return 256;
    if (text == "-...---") return 256;

    if (text == "-.-...-") return 256;
    if (text == "..-....") return 256;

    if (text == ".--.-") return EQ;
    if (text == "-..--.-") return NEQ;
    if (text == ".-..-") return LT;
    if (text == "--.-....") return GT;
    if (text == ".-..-.") return 256;
    if (text == "--.-.") return 256;

    if (text == ".--.-..") return 256;
    if (text == "---.-.") return 256;
    if (text == "-.----") return 256;

    if (text == ".-...") return ASSIGN;
    if (text == ".--..") return PLUS;
    if (text == "--..-.") return MINUS;
    if (text == "--..--") return STAR;
    if (text == "-.....-") return SLASH;
    if (text == "---.") return MOD;

    if (text == "....-.") return KW_IF;
    if (text == "..-..") return KW_ELSE;
    if (text == "...-.") return 256;

    if (text == ".--....") return KW_WHILE;
    if (text == "..-..-.") return 256;

    if (text == "..-.") return KW_FUNC;
    if (text == "-.-.-.") return SEMI;
    if (text == ".-..-...") return LBRACE;
    if (text == ".-.-...") return RBRACE;
    if (text == "-.--.") return LPAREN;
    if (text == "-.--.-") return RPAREN;
    if (text == ".-..-.-") return 256;
    if (text == ".-..---") return 256;
    if (text == "-.-.---") return COMMA;
    if (text == ".-.-") return KW_RETURN;

    if (text == ".--..-.") return KW_PRINT;
    if (text == "..-..--.") return KW_INPUT;

    if (parts.front() == "..-..." && parts.size() > 1) {
        std::string decoded;
        std::vector<std::string> payload(parts.begin() + 1, parts.end());
        if (!decode_parts(payload, kLetters, decoded)) {
            return 256;
        }
        yylval.sval = strdup(decoded.c_str());
        return ID;
    }

    if (parts.front() == "...-..-" && parts.size() > 1) {
        std::string decoded;
        std::vector<std::string> payload(parts.begin() + 1, parts.end());
        if (!decode_parts(payload, kDigits, decoded)) {
            return 256;
        }
        yylval.sval = strdup(decoded.c_str());
        return INT_LIT;
    }

    if (parts.front() == "..-..-..-" && parts.size() > 3) {
        std::string decoded;
        bool seen_dot = false;
        for (std::size_t i = 1; i < parts.size(); ++i) {
            if (parts[i] == ".") {
                if (seen_dot) {
                    return 256;
                }
                decoded += '.';
                seen_dot = true;
                continue;
            }

            auto found = kDigits.find(parts[i]);
            if (found == kDigits.end()) {
                return 256;
            }
            decoded += found->second;
        }

        if (!seen_dot || decoded.front() == '.' || decoded.back() == '.') {
            return 256;
        }

        yylval.sval = strdup(decoded.c_str());
        return 256;
    }

    if (parts.front() == ".--.--" && parts.size() > 1) {

       std::string decoded;

       for (std::size_t i = 1; i < parts.size(); ++i) {

           if (parts[i] == "...---") {
               decoded += ' ';
               continue;
           }

           auto letter = kLetters.find(parts[i]);
           if (letter != kLetters.end()) {
               decoded += letter->second;
               continue;
           }

           auto digit = kDigits.find(parts[i]);
           if (digit != kDigits.end()) {
               decoded += digit->second;
               continue;
           }

           return 256;
       }

       yylval.sval = strdup(decoded.c_str());
       return STRING_LIT;
    }

    return 256;
}


%}

%%

[ \t\r]+                { /* skip */ }
\n                      { yycolumn = 1; }
[.\-/]+                 {
                            int token = classify_morse_token(yytext);
                            if (token == 256) {
                                return 256;
                            }
                            return token;
                        }
.                       { return 256; }

%%
