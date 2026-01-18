# tournament-bracket-generator

This is a solo project that implements a tournament bracket generator in C using a Binary Tree data structure.  
The program models a knockout-style tournament where each match is represented as a node in the binary tree, and winners propagate upward until the final match at the root.

---

## Data Structure Used

### Binary Tree
- Leaf nodes represent the initial players or teams
- Internal nodes represent matches between participants
- The root node represents the final match of the tournament

This structure naturally fits tournament brackets and allows clear round-by-round progression.

---

## Features

- Knockout-style tournament bracket generation
- Binary tree–based match organization
- Automatic progression of winners through rounds
- Clear representation of tournament hierarchy

---

## Technologies Used

- C Programming Language
- Binary Trees (Data Structures)

---

## Usage

Compile and run the program using a C compiler such as GCC:

```bash
gcc tournament_bracket_generator.c -o tournament
./tournament

