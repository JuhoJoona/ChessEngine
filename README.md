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

## Experimental Nature

This project is designed as an **experimental learning platform** for:
- Understanding chess programming concepts
- Practicing modern C++ development
- Exploring bitboard techniques
- Learning UCI protocol implementation
- Experimenting with chess algorithms

The codebase intentionally explores various approaches and may contain experimental implementations that aren't necessarily optimal or bug-free.

## Current Status

The engine can compile and run, but
 has several serious issues that prevent it from playing correctly:

### 🚨 Critical Issues

#### 1. **Duplicate Move Generation**
The engine is generating duplicate moves, as evidenced by the cutechess debug output showing the same move `b2b3` being played multiple times in the same position. This indicates a fundamental bug in the move generation logic.

**Problem Location**: `src/core/game/movegen/movegen.cpp`
- The move generation functions may be adding the same move multiple times
- Move filtering logic may not be working correctly
- Possible issue with move comparison/equality checking

#### 2. **Illegal Move Generation**
The engine is generating and attempting to play illegal moves. This suggests problems in:
- Move validation logic
- Board state tracking
- Piece movement rules implementation

**Problem Location**: `src/core/game/movegen/movegen.cpp` and `src/core/game/board/board.cpp`

#### 3. **Move Validation Issues**
The `isLegalMove()` function may not be correctly filtering out illegal moves, allowing moves that:
- Leave the king in check
- Move pieces incorrectly
- Violate chess rules

#### 4. **Board State Inconsistencies**
The board representation and state management may have bugs that cause:
- Incorrect piece positions
- Wrong side-to-move tracking
- Invalid castling rights
- Incorrect en passant state

### 🔧 Technical Issues

#### 5. **Move Encoding Problems**
The move encoding/decoding system may have issues with:
- Piece type normalization (0-5 vs 0-11 range)
- Move flag handling
- Promotion piece encoding

#### 6. **Attack Table Initialization**
The magic bitboard attack tables may not be properly initialized, leading to:
- Incorrect sliding piece move generation
- Wrong attack detection
- Performance issues

#### 7. **UCI Protocol Compliance**
The engine may not be fully compliant with the UCI protocol, causing issues with:
- Move parsing
- Position setting
- Time management

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

## Development Priorities

### High Priority (Fix First)
1. **Fix duplicate move generation** - This is causing the most obvious issues
2. **Implement proper move validation** - Ensure only legal moves are generated
3. **Fix board state management** - Ensure consistent board representation
4. **Debug move encoding/decoding** - Fix piece type normalization issues

### Medium Priority
1. **Improve attack table initialization** - Ensure magic bitboards work correctly
2. **Add comprehensive testing** - Unit tests for move generation and validation
3. **Implement proper search** - Currently only plays first legal move
4. **Add evaluation function** - Basic material and position evaluation

### Low Priority
1. **Optimize performance** - Bitboard operations, move ordering
2. **Add opening book support**
3. **Implement advanced search features** - Alpha-beta, transposition tables
4. **Add time management**


