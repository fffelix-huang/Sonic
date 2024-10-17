# Sonic

Sonic is a chess engine written in C++ that supports the UCI (Universal Chess Interface) protocol. It is designed for performance and extensibility, making it a great choice for both developers and chess enthusiasts.

## Current Status

Sonic is still under development, with a current Elo rating of around 1700. There is plenty of room for improvement, and contributions are welcome!

## Installation

To build Sonic from the source code, you need to have `Make` installed. Follow these steps to get started:

```bash
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

## ⚙️Features

Sonic includes a variety of advanced chess engine features:

### Search Algorithms
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

### 🔀Move Ordering
- PV move
- Pawn Promotion
- MVV-LVA (Most Valuable Victim - Least Valuable Aggressor)

### 🔍Evaluation
- Piece Square Table
- Passed Pawn Bonus
- Piece Mobility
- Tapered Evaluation

## 🤝Contribution Guidelines

I'm excited to invite contributions to Sonic! Here are some areas where your input can make a difference:

- **Better Documentation**: Help us improve the clarity and comprehensiveness of the documentation.
- **Code Readability**: Enhance the code structure and style for better readability.
- **Workflow Improvements**: Optimize the development workflow for better efficiency.
- **Search Function Enhancements**: Propose and implement improvements to the search functions.
- **Evaluation Enhancements**: Work on better evaluation metrics for the engine.
- **Anything Else**: If you have any other ideas or improvements, feel free to contribute!

## 🤝How to make Contribution

We welcome contributions from developers of all skill levels! Whether you're fixing a bug, adding new features, or improving documentation, your help is appreciated. 

To contribute:

1. ⭐ Star the repository to show your support.
2. 📝 Create an issue outlining how you'd like to contribute to the project.
3. 🍴 Fork the repository to make your own copy:
   ```sh
   # Click on the "Fork" button at the top right of the repository page
4. 💻 Implement your changes in the forked repository by creating a new branch for your feature or fix:
   ```
   git checkout -b feature-or-fix-name
   ```
5. Make your changes and commit them using Conventional Commits:
   ```
   git commit -m "feat: describe your changes"
   ```
6. 🔄 Push your branch:
   ```
   git push origin feature-or-fix-name
   ```
7. Open a pull request and describe the changes you made, mentioning the issue number you're addressing.
8. ⏳ Wait for review and feedback from the maintainers.


## 🛠️Getting Help
If you have any questions or need assistance, please feel free to reach out. Your contributions are valued, and together we can make Sonic a better chess engine!
