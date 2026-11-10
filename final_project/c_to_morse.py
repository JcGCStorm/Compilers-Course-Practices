import sys
import re

morse_letters = {"a": ".-", "b": "-...", "c": "-.-.", "d": "-..", "e": ".", "f": "..-.", "g": "--.", "h": "....", "i": "..", "j": ".---", "k": "-.-", "l": ".-..", "m": "--", "n": "-.", "o": "---", "p": ".--.", "q": "--.-", "r": ".-.", "s": "...", "t": "-", "u": "..-", "v": "...-", "w": ".--", "x": "-..-", "y": "-.--", "z": "--.."}
morse_digits = {"0": "-----", "1": ".----", "2": "..---", "3": "...--", "4": "....-", "5": ".....", "6": "-....", "7": "--...", "8": "---..", "9": "----."}

def to_id(s): return "..-..." + "".join("/" + morse_letters[c] for c in s.lower() if c in morse_letters)
def to_int(s): return "...-..-" + "".join("/" + morse_digits[c] for c in s if c in morse_digits)
def to_str(s): return ".--.--" + "".join("/...---" if c == ' ' else "/" + morse_letters[c] for c in s.lower() if c in morse_letters or c == ' ')

tokens = {
    "INT": "..-.-", "IF": "....-.", "ELSE": "..-..", "WHILE": ".--....",
    "FUNC": "..-.", "RETURN": ".-.-", "PRINT": ".--..-.", "INPUT": "..-..--.",
    "=": ".-...", "+": ".--..", "-": "--..-.", "*": "--..--", "/": "-.....-", "%": "---.",
    "==": ".--.-", "!=": "-..--.-", "<": ".-..-", ">": "--.-....",
    "{": ".-..-...", "}": ".-.-...", "(": "-.--.", ")": "-.--.-", ",": "-.-.---", ";": "-.-.-."
}

def translate_file(input_file, output_file):
    try:
        with open(input_file, 'r') as f:
            code = f.read()
    except FileNotFoundError:
        print(f"Error: No se encontró el archivo '{input_file}'")
        return

    out_lines = []
    for line in code.split('\n'):
        out_line = []
        for word in re.findall(r'"[^"]*"|\S+', line):
            if word in tokens: out_line.append(tokens[word])
            elif word.isdigit(): out_line.append(to_int(word))
            elif word.startswith('"'): out_line.append(to_str(word.strip('"')))
            else: out_line.append(to_id(word))
        out_lines.append(" ".join(out_line))
    
    with open(output_file, 'w') as f:
        f.write("\n".join(out_lines) + "\n")
    print(f"Traducción exitosa: '{input_file}' -> '{output_file}'")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Uso: python3 c_to_morse.py <archivo.c> [archivo_salida.morse]")
        sys.exit(1)
        
    input_file = sys.argv[1]
    if len(sys.argv) >= 3:
        output_file = sys.argv[2]
    else:
        output_file = input_file.rsplit('.', 1)[0] + ".morse"
        
    translate_file(input_file, output_file)
