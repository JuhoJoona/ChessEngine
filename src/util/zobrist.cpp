#include "zobrist.hpp"
#include <random>

namespace Zobrist {
    std::array<std::array<uint64_t, 64>, 12> pieceKeys;
    uint64_t sideToMove;
    uint64_t castlingRights[16];
    uint64_t enPassant[8];

    void init() {
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<uint64_t> dis;

        for (int piece = 0; piece < 12; ++piece) {
            for (int square = 0; square < 64; ++square) {
                pieceKeys[piece][square] = dis(gen);
            }
        }

        sideToMove = dis(gen);
        for (int i = 0; i < 16; ++i) {
            castlingRights[i] = dis(gen);
        }
        for (int i = 0; i < 8; ++i) {
            enPassant[i] = dis(gen);
        }
    }

    uint64_t pieceSquare(Piece piece, Square square) {
        return pieceKeys[static_cast<int>(piece)][static_cast<int>(square)];
    }

    uint64_t sideToMoveKey() {
        return sideToMove;
    }

    uint64_t castlingKey(uint8_t rights) {
        return castlingRights[rights];
    }

    uint64_t enPassantKey(int file) {
        return file >= 0 ? enPassant[file] : 0;
    }

    uint64_t hashPosition(const Board& board) {
        uint64_t hash = 0;
        
        for (int piece = 0; piece < 12; ++piece) {
            bitboard bb = board.getPieceBB(static_cast<Piece>(piece));
            while (bb) {
                int square = __builtin_ctzll(bb);
                hash ^= pieceKeys[piece][square];
                bb &= bb - 1;
            }
        }
        
        if (board.getSideToMove() == BLACK) {
            hash ^= sideToMove;
        }
        
        return hash;
    }
} 