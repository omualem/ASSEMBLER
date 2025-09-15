
# Imaginary language assembler

A brief description of what this project does and who it's for


## Objective
This project implements such an assembler in C for a simulated (imaginary) computer. The assembler reads assembly source files (.as), validates them, translates them into binary machine code, and outputs result files (.ob). If the source file contains errors, it generates clear error messages instead of machine code.
## The Imaginary Computer

The project is based on a simplified CPU model with the following characteristics:

- Registers: 8 general-purpose registers (r0 – r7), each 12 bits wide.

- Memory: 1024 words of 12 bits each. Part of memory is used as a stack.

- Instruction Word Format:

  - Opcode (operation code)

  - Addressing modes for source and destination operands

  - Extra bits for relocatable/absolute/external addressing

- Supported Operations: A set of 16 basic instructions (e.g., move, add, subtract, stop).

This machine model is only theoretical but sufficient to demonstrate how a real assembler works.
## How the Assembler Works

The assembler operates in several stages:

1. **Input Reading**

- Load an assembly source file (.as).

- Each line contains either an instruction, a directive, or a label.

2. **Parsing**

- Break down the source into tokens (mnemonics, registers, constants, labels).

- Identify instructions, operands, and addressing modes.

3. **Validation**

- Ensure syntax correctness.

- Check that registers and labels exist.

- Verify operand count and types.

4. **Translation**

- Convert mnemonics into opcodes.

- Encode operands into binary format based on addressing modes.

- Generate binary machine words.

5. **Output Generation**

- If valid: create an object file (.ob) containing binary machine code.

- If invalid: print error messages describing the problems.
## Input and Output

**Input**
- Valid assembly source files (.as) demonstrating correct usage of the language.

- Invalid assembly source files that trigger syntax and semantic errors (to demonstrate error handling).

**Output**

- Machine code files (.ob) containing the translated binary instructions.

- Entry file (.ent) containing the symbols in the as file.

- Extern file (.ext) containing the external symbols in the as file.

- Error messages for faulty input.


## Examples

**Example 1 – Valid Assembly**

Input (example1.as):
```
; file ps.as
.entry LENGTH
.extern W
MAIN: mov @r3 ,LENGTH
LOOP: jmp L1
prn -5
bne W
sub @r1, @r4
bne L3
L1: inc K
.entry LOOP
jmp W
END: stop
STR: .string "abcdef"
LENGTH: .data 6,-9,15
K: .data 22
.extern L3

```

Output (example1.ob):

```
18 11
oM
GA
H2
Es
HG
GE
/s
FM
AB
p0
CQ
FM
AB
Ds
IC
Es
AB
Hg
Bh
Bi
Bj
Bk
Bl
Bm
AA
AG
/3
AP
AW
```

example1.ent:

```
LENGTH	125
LOOP	103
```

example1.ext:

```
W	108
W	116
L3	112
```


**Example 2 – Invalid Assembly:**

Input (example2.as):

```
add r8, r2
```

Output:

```
Error: Undefined register 'r8'
```
## How to Run

1. Compile the assembler

```
make
```

This uses the provided Makefile to compile all .c files into the assembler executable.

2. Run the assembler on a source file

```
./assembler example1.as
```

3. Check the results
- If the input is valid: an .ob file with machine code will be created.

- If the input is invalid: error messages will be printed.


## Requirements

- C source code organized across multiple .c and .h files.

- Makefile for compilation.

- Must compile on Ubuntu with strict flags:

```
gcc -Wall -ansi -pedantic -o assembler *.c
```
