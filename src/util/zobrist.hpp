#pragma once
#include <array>
#include <cstdint>
#include "../core/game/board/board.hpp"

namespace Zobrist {
    extern std::array<std::array<uint64_t, 64>, 12> pieceKeys;  // [piece][square]
    extern uint64_t sideToMove;
    extern uint64_t castlingRights[16];
    extern uint64_t enPassant[8];

    void init();
    uint64_t pieceSquare(Piece piece, Square square);
    uint64_t sideToMoveKey();
    uint64_t castlingKey(uint8_t rights);
    uint64_t enPassantKey(int file);

    uint64_t hashPosition(const Board& board);
} 