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
| `Book` | string | None | `<book_name>` | Polyglot book file to use. |
| `Threads` | integer | $1$ | $[1, 1]$ | Number of threads. |
| `Hash` | integer | $16$ | $[1, 1024]$ | Transposition table size (in MB). |
| `Clearhash` | button | | | Clear entries in transposition table. |

## Features

- Search
  - Alpha-Beta Search
  - Quiescence Search
  - Iterative Deepening
  - Check Extension
  - Transposition Table
  - Repetition Detection
  - Insufficient Mating Material
  - Null Move Pruning
  - Aspiration Window
  - Principal Variation Search
  - Mate Distance Pruning
  - Futility Pruning
  - Reverse Futility Pruning
  - Late Move Reduction
  - Delta Pruning
- Move Ordering
  - PV move
  - Pawn Promotion
  - MVV-LVA
- Evaluation
  - Piece Square Table
  - Passed Pawn Bonus
  - Piece Mobility
  - Tapered Evaluation
