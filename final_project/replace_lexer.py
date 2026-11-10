import re

with open('/Users/admin/Documents/personal/Escuela/Compiladores/practicas/compilers/lexer.l', 'r') as f:
    code = f.read()

# Strip YYLTYPE and yy_user_action logic
code = re.sub(r'extern YYLTYPE yylloc;', '', code)
code = re.sub(r'#define YY_USER_ACTION.*?\n%\}', '%}', code, flags=re.DOTALL)

# Change INVALID_TOKEN to 256
code = code.replace('INVALID_TOKEN', '256')

replacements = {
    'KW_FLOAT': '256',
    'KW_BOOL': '256',
    'KW_STRING': '256',
    'TRUE': '256',
    'FALSE': '256',
    'NOT_EQUAL': 'NEQ',
    'LESS_THAN': 'LT',
    'GREATER_THAN': 'GT',
    'LESS_EQ': '256',
    'GREATER_EQ': '256',
    'AND': '256',
    'OR': '256',
    'NOT': '256',
    'MULT': 'STAR',
    'DIV': 'SLASH',
    'KW_ELSEIF': '256',
    'KW_FOR': '256',
    'FUNCTION': 'KW_FUNC',
    'LBRACKET': '256',
    'RBRACKET': '256',
    'RETURN': 'KW_RETURN',
    'PRINT': 'KW_PRINT',
    'INP': 'KW_INPUT',
    'IDENTIFIER': 'ID',
    'INT_LITERAL': 'INT_LIT',
    'FLOAT_LITERAL': '256',
    'STRING_LITERAL': 'STRING_LIT'
}

for old, new in replacements.items():
    code = re.sub(r'\b' + old + r'\b', new, code)

# Add MOD support
code = code.replace('if (text == "-.....-") return SLASH;', 'if (text == "-.....-") return SLASH;\n    if (text == "---.") return MOD;')

with open('lexer.l', 'w') as f:
    f.write(code)

