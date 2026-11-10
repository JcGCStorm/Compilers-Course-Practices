import re

with open('lexer.l', 'r') as f:
    code = f.read()

# Remove YYLTYPE declaration
code = re.sub(r'extern YYLTYPE yylloc;', '', code)

# Remove YY_USER_ACTION macro which uses yylloc
code = re.sub(r'#define YY_USER_ACTION.*?\n%\}', '%}', code, flags=re.DOTALL)

# Replace INVALID_TOKEN with 256
code = code.replace('INVALID_TOKEN', '256')

with open('lexer.l', 'w') as f:
    f.write(code)
