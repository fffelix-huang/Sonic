# Sonic

Sonic is a chess engine written in C++ that supports UCI protocol.

## Installation

You should have `Make` installed in order to build from source code.

```
git clone https://github.com/fffelix-huang/Sonic.git
cd Sonic/src
make
```

It will compile the source code into an executable named `sonic`.

## UCI Options

| Name | Type | Default | Valid | Description |
| :--- | :--: | :-----: | :---: | :---------- |
| `book` | string | None | book_name | Polyglot book file to use. |
| `hash` | integer | $16$ | $[1, 1024]$ | Transposition table size (in MB). |
| `clearhash` | button | | | Clear entries in transposition table. |

## Features

- Search
  - Alpha-Beta Search
  - Quiescence Search
  - Iterative Deepening
  - Check Extension
  - Transposition Table
  - Repetition Detection
  - Null Move Pruning
- Move Ordering
  - Pawn Promotion
  - MVV-LVA
- Evaluation
  - Piece Square Table
  - Passed Pawn Bonus
