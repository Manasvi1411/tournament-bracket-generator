# tournament-bracket-generator

This is a solo project that implements a tournament bracket generator in C using a Binary Tree data structure.
Each node represents a match, with child nodes representing the preceding matches or participants.
The winner of each match propagates upward until the final match at the root.

## Data Structure Used
- Binary Tree
  - Leaf nodes represent players/teams
  - Internal nodes represent matches
  - Root node represents the final match

## Features
- Knockout-style tournament bracket generation
- Binary tree–based match organization
- Round-by-round progression of winners

## Technologies
- C Programming Language
- Binary Trees

## Author
- Solo project
