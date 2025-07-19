#pragma once
#include "../move/move.hpp"
#include "../board/board.hpp"
#include <vector>
#include <array>

constexpr Square A1 = Square(0), B1 = Square(1), C1 = Square(2), D1 = Square(3), E1 = Square(4), F1 = Square(5), G1 = Square(6), H1 = Square(7);
constexpr Square A8 = Square(56), B8 = Square(57), C8 = Square(58), D8 = Square(59), E8 = Square(60), F8 = Square(61), G8 = Square(62), H8 = Square(63);

constexpr int KINGSIDE = 0;
constexpr int QUEENSIDE = 1;

class MoveGen {
public:
    static std::vector<Move> generatePseudoLegalMoves(const Board& board);

    static void initializeAttackTables();

    static bool isLegalMove(const Board& board, const Move& move);

private:
    static void generatePawnMoves(const Board& board, std::vector<Move>& moves);
    static void generateKnightMoves(const Board& board, std::vector<Move>& moves);
    static void generateBishopMoves(const Board& board, std::vector<Move>& moves);
    static void generateRookMoves(const Board& board, std::vector<Move>& moves);
    static void generateQueenMoves(const Board& board, std::vector<Move>& moves);
    static void generateKingMoves(const Board& board, std::vector<Move>& moves);

    static void generateCastlingMoves(const Board& board, std::vector<Move>& moves);
    static void generateEnPassantMoves(const Board& board, std::vector<Move>& moves);

    static bool isSquareAttacked(const Board& board, Square square, Color byColor);
    static bitboard allEnemyAttacks(const Board& board, Color side);

    static std::array<bitboard, 64> KNIGHT_ATTACKS;
    static std::array<bitboard, 64> KING_ATTACKS;
    static std::array<bitboard, 64> PAWN_ATTACKS[2]; // [color][square]
    
    static std::array<bitboard, 64> BISHOP_MASKS;
    static std::array<bitboard, 64> ROOK_MASKS;
    
    static std::array<uint64_t, 64> BISHOP_MAGICS;
    static std::array<uint64_t, 64> ROOK_MAGICS;

    static std::array<std::array<bitboard, 4096>, 64> BISHOP_ATTACKS;
    static std::array<std::array<bitboard, 4096>, 64> ROOK_ATTACKS;
    
    static bool initialized;
    
    static bitboard generateKnightAttacks(Square square);
    static bitboard generateKingAttacks(Square square);
    static bitboard generatePawnAttacks(Square square, Color color);
    static bitboard generateBishopMask(Square square);
    static bitboard generateRookMask(Square square);
    
    static uint64_t findMagicNumber(Square square, bool isBishop);
    static bitboard getBishopAttacks(Square square, bitboard occupancy);
    static bitboard getRookAttacks(Square square, bitboard occupancy);
    static bitboard calculateBishopAttacks(Square square, bitboard occupancy);
    static bitboard calculateRookAttacks(Square square, bitboard occupancy);
}; 