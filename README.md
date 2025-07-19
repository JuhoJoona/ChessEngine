# Chess Engine v2

**⚠️ EXPERIMENTAL PROJECT - WORK IN PROGRESS - NOT STABLE ⚠️**

This is an **experimental** C++ chess engine implementing the UCI (Universal Chess Interface) protocol. The engine is currently in active development and contains several critical bugs that need to be addressed. This project serves as a learning experiment in chess programming and modern C++ development.

## ⚠️ Important Disclaimer

**This repository's primary purpose is to showcase the chess engine's architecture, design patterns, and implementation approach as an experimental learning project.** 

**This is NOT a working chess engine yet.** The code demonstrates:
- Modern C++ chess engine architecture
- Bitboard-based board representation
- Magic bitboard move generation techniques
- UCI protocol implementation structure
- Object-oriented design patterns for chess programming
- Experimental approaches to chess algorithm implementation

While the engine can compile and run, it contains critical bugs that prevent proper gameplay. This repository serves as a learning resource and architectural reference rather than a functional chess engine.

If you're looking for a working chess engine, please consider established engines like Stockfish, Leela Chess Zero, or other mature implementations.

## Project Structure

```
src/
├── core/
│   ├── game/
│   │   ├── board/          # Board representation and state
│   │   ├── move/           # Move encoding/decoding
│   │   └── movegen/        # Move generation (⚠️ BUGGY)
│   ├── eval/               # Position evaluation
│   ├── search/             # Search algorithms
│   └── knowledge/          # Opening books, etc.
├── engine/                 # Main engine logic
├── uci/                    # UCI protocol implementation
└── util/                   # Utilities (zobrist hashing, etc.)
```

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Testing

The engine can be tested with cutechess-cli, but expect illegal moves and crashes:

```bash
cutechess-cli -engine cmd=./chess-engine -engine cmd=./chess-engine -each tc=5+0.1 proto=uci
```

## Known Issues from Debug Output

Based on the cutechess debug output provided:

1. **Move Repetition**: The engine plays `b2b3` multiple times in the same position
2. **Illegal Moves**: The engine attempts moves that violate chess rules
3. **State Corruption**: The board state becomes inconsistent after certain moves
4. **Protocol Issues**: The engine may not properly handle UCI commands

