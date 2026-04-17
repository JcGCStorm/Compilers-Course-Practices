# Compiler Lab – Support Base

This repository is intended as **optional support material** for the Compiler course/lab.

The goal is for students to:

- Have a minimal reference base to start each assignment.
- Review a **proposed solution** to compare approaches.
- Quickly test input/output cases.

> Objective: serve as a guide, not to replace each student's own implementation.

## Current Scope

At the moment, this repo includes the base implementation for:

1. Converting a regular expression (regex) into an NFA.
2. Validating strings using the constructed NFA.

Main implementation in C:

- `src/regex.c`, `src/regex.h`: regex parsing (infix -> postfix).
- `src/nfa.c`, `src/nfa.h`: NFA construction and simulation.
- `src/main.c`: command-line interface.

## Supported Regex Operators

- `|` alternation
- implicit concatenation (e.g., `ab`)
- `*` Kleene star
- `+` positive closure
- `?` optional
- `(` `)` grouping
- `\` escaping special characters

## Requirements

- C compiler with C11 support
- CMake >= 3.16
- `make` (or an equivalent generator in your environment)

## Build

### Linux/macOS (or WSL)

```bash
cmake -S . -B build
cmake --build build
```

Resulting executable: `build/regex_to_nfa`

### Windows (PowerShell)

```powershell
cmake -S . -B build
cmake --build build
```

Resulting executable (typical): `build\regex_to_nfa.exe`

## Usage

The program has two modes:

- `-r`: prints the regex in postfix notation.
- `-t`: tests strings against the regex and returns accept/reject results.

### 1) Convert regex to postfix

Linux:

```bash
echo "(ab)*" | ./build/regex_to_nfa -r
```

Windows PowerShell:

```powershell
echo "(ab)*" | .\build\regex_to_nfa.exe -r
```

### 2) Validate strings with the NFA

`stdin` input format for `-t`:

1. First line: the regex.
2. Following lines: one input string per line.

Linux:

```bash
printf '%s\n' "(ab)*" "ab" "aba" "abab" | ./build/regex_to_nfa -t
```

Windows PowerShell:

```powershell
@(
	"(ab)*"
	"ab"
	"aba"
	"abab"
) -join "`n" | .\build\regex_to_nfa.exe -t
```

Output:

- `1` if the string is accepted.
- `0` if the string is rejected.

> Note: in the current implementation, results are printed consecutively (for example, `101`) and end with a trailing newline.

## General Structure

- `src/`: source code for the base project.
- `build/`: build artifacts (generated).
- `validator`: validation executable included in the repo.
- `Dockerfile`: minimal environment to build and run validation.

## Upcoming Assignments

This repository is set up to grow with new lab assignments. The suggested convention is to keep:

- an initial base per assignment,
- a proposed solution,
- and reference test cases.

## License

See `LICENCE`.
