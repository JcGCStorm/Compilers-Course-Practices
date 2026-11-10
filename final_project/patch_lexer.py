import re

with open('lexer.l', 'r') as f:
    code = f.read()

replacements = {
    'KW_FLOAT': 'INVALID_TOKEN',
    'KW_BOOL': 'INVALID_TOKEN',
    'KW_STRING': 'INVALID_TOKEN',
    'TRUE': 'INVALID_TOKEN',
    'FALSE': 'INVALID_TOKEN',
    'NOT_EQUAL': 'NEQ',
    'LESS_THAN': 'LT',
    'GREATER_THAN': 'GT',
    'LESS_EQ': 'INVALID_TOKEN',
    'GREATER_EQ': 'INVALID_TOKEN',
    'AND': 'INVALID_TOKEN',
    'OR': 'INVALID_TOKEN',
    'NOT': 'INVALID_TOKEN',
    'MULT': 'STAR',
    'DIV': 'SLASH',
    'KW_ELSEIF': 'INVALID_TOKEN',
    'KW_FOR': 'INVALID_TOKEN',
    'FUNCTION': 'KW_FUNC',
    'LBRACKET': 'INVALID_TOKEN',
    'RBRACKET': 'INVALID_TOKEN',
    'RETURN': 'KW_RETURN',
    'PRINT': 'KW_PRINT',
    'INP': 'KW_INPUT',
    'IDENTIFIER': 'ID',
    'INT_LITERAL': 'INT_LIT',
    'FLOAT_LITERAL': 'INVALID_TOKEN',
    'STRING_LITERAL': 'STRING_LIT'
}

for old, new in replacements.items():
    code = re.sub(r'\b' + old + r'\b', new, code)

# Add MOD support
code = code.replace('if (text == "-.....-") return SLASH;', 'if (text == "-.....-") return SLASH;\n    if (text == "---.") return MOD;')

with open('lexer.l', 'w') as f:
    f.write(code)

