import os
import re
import glob

morse_letters = {
    "a": ".-", "b": "-...", "c": "-.-.", "d": "-..", "e": ".",
    "f": "..-.", "g": "--.", "h": "....", "i": "..", "j": ".---",
    "k": "-.-", "l": ".-..", "m": "--", "n": "-.", "o": "---",
    "p": ".--.", "q": "--.-", "r": ".-.", "s": "...", "t": "-",
    "u": "..-", "v": "...-", "w": ".--", "x": "-..-", "y": "-.--",
    "z": "--.."
}
morse_digits = {
    "0": "-----", "1": ".----", "2": "..---", "3": "...--", "4": "....-",
    "5": ".....", "6": "-....", "7": "--...", "8": "---..", "9": "----."
}

def to_morse_id(s):
    parts = ["..-..."]
    for c in s.lower():
        if c in morse_letters:
            parts.append(morse_letters[c])
        elif c in morse_digits:
            parts.append(morse_digits[c])
    return "/".join(parts)

def to_morse_int(s):
    parts = ["...-..-"]
    for c in s:
        parts.append(morse_digits[c])
    return "/".join(parts)

def to_morse_str(s):
    parts = [".--.--"]
    for c in s[1:-1].lower(): # remove quotes
        if c == ' ':
            parts.append("...---")
        elif c in morse_letters:
            parts.append(morse_letters[c])
        elif c in morse_digits:
            parts.append(morse_digits[c])
    return "/".join(parts)

old_to_new_symbols = {
    "--.": "..-.-",     # KW_INT
    ".-": "....-.",     # KW_IF
    "..": "..-..",      # KW_ELSE
    "-...": ".--....",  # KW_WHILE
    "-.-.": "..-.",     # KW_FUNC
    "-..": ".-.-",      # KW_RETURN
    ".": ".--..-.",     # KW_PRINT
    "..-": "..-..--.",  # KW_INPUT
    "-...-": ".-...",   # ASSIGN
    ".-.-.": ".--..",   # PLUS
    "-....-": "--..-.", # MINUS
    "--.--": "--..--",  # STAR
    "-..-.": "-.....-", # SLASH
    "---.": "---.",     # MOD
    "..--..": ".--.-",  # EQ
    "-.-.--": "-..--.-",# NEQ
    ".-.-": ".-..-",    # LT
    ".-...": "--.-....",# GT
    "-.--.": ".-..-...",# LBRACE
    "-.--.-": ".-.-...",# RBRACE
    ".--.": "-.--.",    # LPAREN
    ".--.-": "-.--.-",  # RPAREN
    "--..--": "-.-.---",# COMMA
    "-.-.-.": "-.-.-.", # SEMI
}

def translate_content(text):
    tokens = []
    pos = 0
    while pos < len(text):
        if text[pos] in ' \t\r\n':
            tokens.append(text[pos])
            pos += 1
            continue
            
        m = re.match(r'"[^"]*"', text[pos:])
        if m:
            tokens.append(to_morse_str(m.group(0)))
            pos += m.end()
            continue
            
        m = re.match(r'[a-zA-Z_][a-zA-Z0-9_]*', text[pos:])
        if m:
            tokens.append(to_morse_id(m.group(0)))
            pos += m.end()
            continue
            
        m = re.match(r'[0-9]+', text[pos:])
        if m:
            tokens.append(to_morse_int(m.group(0)))
            pos += m.end()
            continue
            
        m = re.match(r'[^\sa-zA-Z0-9_"]+', text[pos:])
        if m:
            sym = m.group(0)
            if sym in old_to_new_symbols:
                tokens.append(old_to_new_symbols[sym])
            else:
                tokens.append(sym)
            pos += m.end()
            continue
            
        tokens.append(text[pos])
        pos += 1
        
    return "".join(tokens)

for f in glob.glob("*.morse"):
    if f == "hola_mundo.morse":
        continue
    with open(f, 'r') as fp:
        old_data = fp.read()
    new_data = translate_content(old_data)
    with open(f, 'w') as fp:
        fp.write(new_data)
    print(f"Translated {f}")

