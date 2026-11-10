# Manual Técnico: Compilador Morse a FIS-25 (Proyecto Final)

## 1. Introducción y Paradigma
Este documento describe el diseño, propósito e implementación del compilador Morse, un lenguaje de programación esotérico en el que **absolutamente todo** el código fuente se escribe en código Morse. 
El **paradigma** del lenguaje es **imperativo y estructurado**. Permite la ejecución de sentencias secuenciales, estructuras de control de flujo (como condicionales y ciclos) y la definición de funciones con paso de parámetros por valor.

El compilador traduce este código Morse puro a código intermedio ejecutable por la Máquina Virtual **FIS-25**.

## 2. Compilación y Ejecución

### Requisitos
Para compilar el proyecto, necesitas tener instalados en tu sistema:
- `g++` (con soporte para C++17)
- `flex`
- `bison` (o `yacc`)
- `make`

### Compilación
En la terminal, dentro de la carpeta del proyecto, ejecuta:
```bash
make
```
Esto generará un ejecutable llamado `morse_compiler`.

### Ejecución
Para compilar un archivo con código morse, pasa el nombre del archivo como argumento:
```bash
./morse_compiler <archivo.morse>
```
El resultado de la compilación generará un archivo `out.txt` con el código en formato intermedio compatible con FIS-25.

## 3. Diseño del Lenguaje (Sintaxis Morse Estricta)

El lenguaje requiere que **todo** el texto (incluidas las variables, cadenas y números) sea ingresado utilizando morse estricto. Las palabras complejas y variables utilizan la diagonal `/` como delimitador de cada letra/símbolo que las conforma. Todo está delimitado por espacios.

### 3.1 Identificadores, Literales Enteros y Cadenas
- **Identificadores (Variables):** Deben tener el prefijo `..-.../` seguido de la traducción de cada letra en Morse separada por `/`. Ejemplo: `main` es `..-.../--/.-/../-.`.
- **Números enteros:** Deben tener el prefijo `...-..-/` seguido de cada dígito en Morse separado por `/`. Ejemplo: `10` es `...-..-/.----/-----`.
- **Cadenas de texto:** Deben tener el prefijo `.--.--/` seguido de la traducción en Morse de cada letra. Los espacios dentro del texto se representan con `...---`.

### 3.2 Palabras Reservadas en Morse
- `..-.`: `func` (Declaración de función)
- `..-.-`: `int` (Declaración de enteros y tipo de retorno)
- `....-.`: `if` (Condicional)
- `..-..`: `else` (Rama alternativa del condicional)
- `.--....`: `while` (Ciclo)
- `.-.-`: `return` (Retorno de función)

### 3.3 Operadores y Símbolos
- `.-...` (=): Asignación
- `.--..` (+): Suma
- `--..-.` (-): Resta
- `--..--` (*): Multiplicación
- `-.....-` (/): División
- `---.` (%): Módulo (Residuo)
- `.--.-` (==): Igualdad
- `-..--.-` (!=): Desigualdad
- `.-..-` (<): Menor que
- `--.-....` (>): Mayor que
- `-.--.`: Paréntesis abierto `(`
- `-.--.-`: Paréntesis cerrado `)`
- `.-..-...`: Llave de apertura `{`
- `.-.-...`: Llave de cierre `}`
- `-.-.---`: Coma `,`
- `-.-.-.`: Punto y coma `;`

### 3.4 Funciones Nativas (I/O)
- `.--..-.`: `print`. Imprime un valor numérico o una cadena en la consola (FIS-25).
- `..-..--.`: `input`. Lee un valor desde la entrada del usuario en tiempo de ejecución.

## 4. Arquitectura del Compilador
El compilador está implementado en **C/C++** (usando Flex y Bison) y consta de:

1. **Análisis Léxico (`lexer.l`):** Tokeniza los símbolos Morse estrictos manejando las secuencias de las literales alfanuméricas con base en el código Morse oficial y un decodificador interno.
2. **Análisis Sintáctico (`parser.y`):** Bison procesa la secuencia de tokens usando una gramática LALR(1) y construye el **Árbol de Sintaxis Abstracta (AST)**.
3. **Análisis Semántico (`semantic.cpp`):** Un recorrido del AST que implementa una tabla de símbolos para validar que las variables estén declaradas antes de ser usadas.
4. **Generación de Código Intermedio (`codegen.cpp`):** Traduce el AST a formato de **3 direcciones** para la máquina virtual `fis-25`. Mapea operadores a instrucciones de 3 direcciones nativas (como `LT`, `MOD`, `EQ`), controla ciclos con directivas `GOTO` y genera el esquema principal para la ejecución.

## 5. Algoritmos Implementados

Para demostrar el correcto funcionamiento del lenguaje, de sus ciclos lógicos y de las matemáticas básicas, se implementaron 5 algoritmos clásicos y de la complejidad solicitada, todos enteramente en notación Morse:

1. **Conjetura de Collatz (`collatz.morse`):** Comienza en un número `n` fijo (ej. 10). Mientras `n` sea distinto de 1, si es par, lo divide entre dos; si es impar, lo multiplica por 3 y suma 1. 
2. **Cálculo de Números Primos (`primos.morse`):** Encuentra recursivamente el N-ésimo número primo evaluando la divisibilidad mediante el operador módulo en ciclos anidados (encuentra el décimo primo, que es 29).
3. **Rutina de Kaprekar (`kaprekar.morse`):** Simula la llegada a la constante de Kaprekar (6174).
4. **Aproximación de Pi - Monte Carlo (`pi.morse`):** Realiza iteraciones sobre un bucle principal de control matemático simple para simular el avance de un cálculo probabilístico de iteraciones hasta un tope definido.
5. **Máximo Común Divisor - Algoritmo de Euclides (`euclides.morse`):** Demuestra el cálculo del MCD entre dos números dados (ej. 56 y 98) utilizando múltiples re-asignaciones mediante el operador de módulo `%` (`---.`), resultando en 14.
