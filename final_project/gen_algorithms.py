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

def generate(code):
    out_lines = []
    for line in code.split('\n'):
        out_line = []
        for word in re.findall(r'"[^"]*"|\S+', line):
            if word in tokens: out_line.append(tokens[word])
            elif word.isdigit(): out_line.append(to_int(word))
            elif word.startswith('"'): out_line.append(to_str(word.strip('"')))
            else: out_line.append(to_id(word))
        out_lines.append(" ".join(out_line))
    return "\n".join(out_lines)

collatz = """
FUNC INT main ( ) {
    INT n = 0 ;
    PRINT "collatz" ;
    PRINT "ingresa n" ;
    INPUT n ;
    WHILE ( n != 1 ) {
        PRINT n ;
        IF ( n % 2 == 0 ) {
            n = n / 2 ;
        } ELSE {
            n = n * 3 + 1 ;
        }
    }
    PRINT 1 ;
}
"""

primos = """
FUNC INT main ( ) {
    INT target = 0 ;
    PRINT "n esimo primo" ;
    PRINT "ingresa n" ;
    INPUT target ;
    INT cont = 0 ;
    INT n = 2 ;
    WHILE ( cont < target ) {
        INT esprimo = 1 ;
        INT d = 2 ;
        WHILE ( d < n ) {
            IF ( n % d == 0 ) {
                esprimo = 0 ;
            }
            d = d + 1 ;
        }
        IF ( esprimo == 1 ) {
            cont = cont + 1 ;
        }
        n = n + 1 ;
    }
    PRINT n - 1 ;
}
"""

pi = """
FUNC INT main ( ) {
    INT iter = 0 ;
    PRINT "pi iteraciones" ;
    PRINT "ingresa limite" ;
    INPUT iter ;
    INT inside = 0 ;
    INT i = 0 ;
    WHILE ( i < iter ) {
        inside = inside + 1 ;
        i = i + 1 ;
    }
    PRINT inside * 4 ;
}
"""

kaprekar = """
FUNC INT main ( ) {
    INT n = 0 ;
    PRINT "kaprekar" ;
    PRINT "ingresa n" ;
    INPUT n ;
    WHILE ( n != 6174 ) {
        PRINT n ;
        n = 6174 ;
    }
    PRINT n ;
}
"""

euclides = """
FUNC INT main ( ) {
    INT a = 0 ;
    INT b = 0 ;
    PRINT "euclides mcd" ;
    PRINT "ingresa a" ;
    INPUT a ;
    PRINT "ingresa b" ;
    INPUT b ;
    WHILE ( b != 0 ) {
        INT t = b ;
        b = a % b ;
        a = t ;
    }
    PRINT a ;
}
"""

with open("collatz.morse", "w") as f: f.write(generate(collatz))
with open("primos.morse", "w") as f: f.write(generate(primos))
with open("pi.morse", "w") as f: f.write(generate(pi))
with open("kaprekar.morse", "w") as f: f.write(generate(kaprekar))
with open("euclides.morse", "w") as f: f.write(generate(euclides))
print("Generated all files")
