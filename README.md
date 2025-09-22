# Sokoban Game

A command-line implementation of the classic Sokoban puzzle game in C.

Unlike traditional Sokoban implementations, this version does not require the player to specify the character’s path. Instead, the **program automatically computes a valid path** to the correct pushing position, focusing only on push commands.

## Notes
This project was implemented as part of an academic assignment in the course **Introduction to Programming**.

## Introduction

Sokoban is a single-player puzzle game played on a two-dimensional grid.
The goal is to move boxes onto designated target positions by pushing them, subject to the following rules:

- The player can move up, down, left, or right (but only implicitly, when preparing for a push).
- A box can only be pushed if there is a free cell behind it.
- The player cannot push more than one box at a time.
- Moves outside the grid or through walls are not allowed.
- The player may undo moves.

The task was to write a C program that:

- Reads the initial board state.
- Executes a sequence of commands (print, push, undo).
- Outputs the board state after each command.

## Input commands

The board description consists of non-empty rows, where each cell is represented by a single character:

- "-" a free cell that is not a target;
- "+" a free cell that is a target;
- "#" a wall;
- "@" a free cell (not a target) occupied by the player;
- "*" a target cell occupied by the player;
- [a .. z] (lowercase letter) a non-target cell occupied by a box labeled with the given letter;
- [A .. Z] (uppercase letter) a target cell occupied by a box, where the box’s label is the lowercase equivalent of the given uppercase letter.

Empty line (\n) → print current board.

<box><direction> → push a box in a direction:

2 = down

8 = up

4 = left

6 = right

0 → undo the last push.

. → terminate input.

## Project Structure
```bash
├── sokoban.c          # Main source code
├── example1.in        # Example input and output files are provided externally
├── example1.out
├── example2.in
├── example2.out
├── example3.in
├── example3.out
└── README.md
```

## Compilation

The project is compiled with strict flags for maximum safety:

```bash
gcc -std=c17 -pedantic -Wall -Wextra -Werror \
    -Wformat-security -Wduplicated-cond -Wfloat-equal \
    -Wshadow -Wconversion -Wjump-misses-init \
    -Wlogical-not-parentheses -Wnull-dereference \
    -Wvla -fstack-protector-strong \
    -fsanitize=undefined -fno-sanitize-recover \
    -g -fno-omit-frame-pointer -O1 \
    sokoban.c -o sokoban
```
## Running

Example:
```bash
< examples/example1.in ./sokoban | diff - examples/example1.out
```

## Memory Safety

All tests are run with Valgrind to ensure no memory leaks or invalid access:
```bash
valgrind --leak-check=full -q --error-exitcode=1 ./sokoban
```
