#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include "../move/move.hpp"
#include <cassert>
#include "../../../util/util.hpp"
#include "../../../util/zobrist.hpp"

using namespace util;

static constexpr Square NO_SQUARE = static_cast<Square>(-1);

struct StateInfo {
    uint64_t hashKey;
    uint8_t  castlingRights;
    int      epFile;
    uint8_t  fiftyMoveCounter;
    Move     move;
    Piece    captured;
    Color    stm;
    uint16_t fullmoveNo;
    std::array<bitboard,12> pieceBB;
    std::array<bitboard,2>  occ;
    bitboard occAll;
};

class Board {
    public:
        Board() {
            Zobrist::init();
            setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        }
        ~Board() = default;

        static constexpr std::array<bitboard, 8> FileBB{
            0x0101010101010101ULL, 0x0202020202020202ULL,
            0x0404040404040404ULL, 0x0808080808080808ULL,
            0x1010101010101010ULL, 0x2020202020202020ULL,
            0x4040404040404040ULL, 0x8080808080808080ULL
        };

        static constexpr std::array<bitboard, 8> RankBB{
            0x00000000000000FFULL, 0x000000000000FF00ULL,
            0x0000000000FF0000ULL, 0x00000000FF000000ULL,
            0x000000FF00000000ULL, 0x0000FF0000000000ULL,
            0x00FF000000000000ULL, 0xFF00000000000000ULL
        };

        static constexpr bitboard DarkSquares = 0xAA55AA55AA55AA55ULL;

        const bitboard& pawns(Color c) const noexcept { return pieceBB[PAWN + 6*c]; }
        const bitboard& knights (Color c) const noexcept { return pieceBB[KNIGHT + 6*c]; }
        const bitboard& bishops (Color c) const noexcept { return pieceBB[BISHOP + 6*c]; }
        const bitboard& rooks   (Color c) const noexcept { return pieceBB[ROOK   + 6*c]; }
        const bitboard& queens  (Color c) const noexcept { return pieceBB[QUEEN  + 6*c]; }
        const bitboard& king    (Color c) const noexcept { return pieceBB[KING   + 6*c]; }

    
        void makeMove(Move m);
        void unmakeMove();
        void setFen(const std::string& fen);
    
        Color getSideToMove() const { return stm; }
        bitboard getPieceBB(Piece p) const {
            assert(static_cast<unsigned>(p) < 12);   // triggers before std::array does
            return pieceBB[p];
        }
        const bitboard& occupancy(Color c) const noexcept { return occ[c]; }
        bitboard allOccupancy() const noexcept { return occAll; }
        bitboard getWhitePieces() const { return occ[WHITE]; }
        bitboard getBlackPieces() const { return occ[BLACK]; }
        int getEpFile() const { return ep; }

        bool hasCastlingRight(Color side, int type) const {
            return castlingRights & (1 << (side * 2 + type));
        }

        Piece pieceAt(Square square) const {
            bitboard bb = 1ULL << static_cast<int>(square);
            constexpr int PIECE_NB = 12;
            for (int p = 0; p < PIECE_NB; ++p)
                if (pieceBB[p] & bb) return Piece(p);

            return NO_PIECE;
        }

    private:
        std::array<bitboard,12> pieceBB{};
        std::array<bitboard,2>  occ{};
        bitboard                occAll{};

        uint64_t hashKey{};
        uint8_t  castlingRights{};
        int8_t   ep{-1};                 // file 0-7, -1 means none
        Color    stm{WHITE};
        uint8_t halfmoveClock{};
        uint16_t fullmoveNo{1};

        std::vector<StateInfo> history;
        int ply{0};

        void updateOccupancy() noexcept;
        void movePiece(Piece pc, Square from, Square to) noexcept;
        
        void handleSpecialMoves(Move m, Piece pc, Square from, Square to, Piece captured);
        void updateGameState(Move m, Piece pc, Color mover, int oldEp, uint8_t oldRights);
        void updateMoveHistory(Move m);
};