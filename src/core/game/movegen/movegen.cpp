#include "movegen.hpp"
#include <array>
#include <memory>
#include <iostream>

std::vector<Move> MoveGen::generatePseudoLegalMoves(const Board& board) {
    // Ensure attack tables are initialized
    if (!initialized) {
        initializeAttackTables();
    }
    
    std::vector<Move> moves;
    moves.reserve(218); 
    
    // Generate moves for each piece type
    generatePawnMoves(board, moves);
    generateKnightMoves(board, moves);
    generateBishopMoves(board, moves);
    generateRookMoves(board, moves);
    generateQueenMoves(board, moves);
    generateKingMoves(board, moves);
    
    // Generate special moves
    generateCastlingMoves(board, moves);
    generateEnPassantMoves(board, moves);
    
    return moves;
}

void MoveGen::generatePawnMoves(const Board& board, std::vector<Move>& moves) {
    Color side = board.getSideToMove();
    bitboard pawns = board.pawns(side);
    bitboard occAll = board.allOccupancy();
    
    // For each pawn on the board
    while (pawns) {
        Square from = static_cast<Square>(__builtin_ctzll(pawns));
        int rank = static_cast<int>(from) / 8;
        int file = static_cast<int>(from) % 8;
        
        // Forward moves
        Square forwardOne = static_cast<Square>(static_cast<int>(from) + (side == WHITE ? 8 : -8));
        if (static_cast<int>(forwardOne) >= 0 && static_cast<int>(forwardOne) < 64 && !(occAll & (1ULL << static_cast<int>(forwardOne)))) {
            // Check if this is a promotion
            if ((side == WHITE && rank == 6) || (side == BLACK && rank == 1)) {
                // Add promotion moves for all piece types
                moves.push_back(Move(from, forwardOne, static_cast<Piece>(PAWN), NO_PIECE, QUEEN));
                moves.push_back(Move(from, forwardOne, static_cast<Piece>(PAWN), NO_PIECE, ROOK));
                moves.push_back(Move(from, forwardOne, static_cast<Piece>(PAWN), NO_PIECE, BISHOP));
                moves.push_back(Move(from, forwardOne, static_cast<Piece>(PAWN), NO_PIECE, KNIGHT));
            } else {
                moves.push_back(Move(from, forwardOne, static_cast<Piece>(PAWN)));
                
                // Two square move from starting position
                if ((side == WHITE && rank == 1) || (side == BLACK && rank == 6)) {
                    Square forwardTwo = static_cast<Square>(static_cast<int>(from) + (side == WHITE ? 16 : -16));
                    Square via = static_cast<Square>(static_cast<int>(from) + (side == WHITE ? 8 : -8));
                    if (!(occAll & (1ULL << static_cast<int>(via))) && !(occAll & (1ULL << static_cast<int>(forwardTwo)))) {
                        moves.push_back(Move(from, forwardTwo, static_cast<Piece>(PAWN), NO_PIECE, NO_PIECE, Move::DPUSH));
                    }
                }
            }
        }
        
        // Capture moves
        bitboard attacks = PAWN_ATTACKS[side][static_cast<int>(from)];
        while (attacks) {
            Square to = static_cast<Square>(__builtin_ctzll(attacks));
            Piece captured = board.pieceAt(to);
            
            if (captured != NO_PIECE) {
                // Normalize captured piece to white piece index (0-5)
                Piece normalizedCaptured = static_cast<Piece>(static_cast<int>(captured) % 6);
                
                // Check if this is a promotion
                if ((side == WHITE && rank == 6) || (side == BLACK && rank == 1)) {
                    // Add promotion moves for all piece types
                    moves.push_back(Move(from, to, static_cast<Piece>(PAWN), normalizedCaptured, QUEEN));
                    moves.push_back(Move(from, to, static_cast<Piece>(PAWN), normalizedCaptured, ROOK));
                    moves.push_back(Move(from, to, static_cast<Piece>(PAWN), normalizedCaptured, BISHOP));
                    moves.push_back(Move(from, to, static_cast<Piece>(PAWN), normalizedCaptured, KNIGHT));
                } else {
                    moves.push_back(Move(from, to, static_cast<Piece>(PAWN), normalizedCaptured));
                }
            }
            
            attacks &= attacks - 1;
        }
        
        pawns &= pawns - 1;
    }
}

void MoveGen::generateKnightMoves(const Board& board, std::vector<Move>& moves) {
    Color side = board.getSideToMove();
    bitboard knights = board.knights(side);
    
    while (knights) {
        Square from = static_cast<Square>(__builtin_ctzll(knights));
        bitboard attacks = KNIGHT_ATTACKS[static_cast<int>(from)] & ~board.occupancy(side);
        
        while (attacks) {
            Square to = static_cast<Square>(__builtin_ctzll(attacks));
            Piece captured = board.pieceAt(to);
            
            if (captured != NO_PIECE) {
                // Normalize captured piece to white piece index (0-5)
                Piece normalizedCaptured = static_cast<Piece>(static_cast<int>(captured) % 6);
                moves.push_back(Move(from, to, static_cast<Piece>(KNIGHT), normalizedCaptured));
            } else {
                moves.push_back(Move(from, to, static_cast<Piece>(KNIGHT)));
            }
            
            attacks &= attacks - 1;
        }
        
        knights &= knights - 1;
    }
}

void MoveGen::generateBishopMoves(const Board& board, std::vector<Move>& moves) {
    Color side = board.getSideToMove();
    bitboard bishops = board.bishops(side);
    
    while (bishops) {
        Square from = static_cast<Square>(__builtin_ctzll(bishops));
        bitboard attacks = getBishopAttacks(from, board.allOccupancy()) & ~board.occupancy(side);
        
        while (attacks) {
            Square to = static_cast<Square>(__builtin_ctzll(attacks));
            Piece captured = board.pieceAt(to);
            
            if (captured != NO_PIECE) {
                // Normalize captured piece to white piece index (0-5)
                Piece normalizedCaptured = static_cast<Piece>(static_cast<int>(captured) % 6);
                moves.push_back(Move(from, to, static_cast<Piece>(BISHOP), normalizedCaptured));
            } else {
                moves.push_back(Move(from, to, static_cast<Piece>(BISHOP)));
            }
            
            attacks &= attacks - 1;
        }
        
        bishops &= bishops - 1;
    }
}

void MoveGen::generateRookMoves(const Board& board, std::vector<Move>& moves) {
    Color side = board.getSideToMove();
    bitboard rooks = board.rooks(side);
    
    while (rooks) {
        Square from = static_cast<Square>(__builtin_ctzll(rooks));
        bitboard attacks = getRookAttacks(from, board.allOccupancy()) & ~board.occupancy(side);
        
        while (attacks) {
            Square to = static_cast<Square>(__builtin_ctzll(attacks));
            Piece captured = board.pieceAt(to);
            
            if (captured != NO_PIECE) {
                // Normalize captured piece to white piece index (0-5)
                Piece normalizedCaptured = static_cast<Piece>(static_cast<int>(captured) % 6);
                moves.push_back(Move(from, to, static_cast<Piece>(ROOK), normalizedCaptured));
            } else {
                moves.push_back(Move(from, to, static_cast<Piece>(ROOK)));
            }
            
            attacks &= attacks - 1;
        }
        
        rooks &= rooks - 1;
    }
}

void MoveGen::generateQueenMoves(const Board& board, std::vector<Move>& moves) {
    Color side = board.getSideToMove();
    bitboard queens = board.queens(side);
    
    while (queens) {
        Square from = static_cast<Square>(__builtin_ctzll(queens));
        bitboard attacks = (getRookAttacks(from, board.allOccupancy()) | getBishopAttacks(from, board.allOccupancy())) & ~board.occupancy(side);
        
        while (attacks) {
            Square to = static_cast<Square>(__builtin_ctzll(attacks));
            Piece captured = board.pieceAt(to);
            
            if (captured != NO_PIECE) {
                // Normalize captured piece to white piece index (0-5)
                Piece normalizedCaptured = static_cast<Piece>(static_cast<int>(captured) % 6);
                moves.push_back(Move(from, to, static_cast<Piece>(QUEEN), normalizedCaptured));
            } else {
                moves.push_back(Move(from, to, static_cast<Piece>(QUEEN)));
            }
            
            attacks &= attacks - 1;
        }
        
        queens &= queens - 1;
    }
}

void MoveGen::generateKingMoves(const Board& board, std::vector<Move>& moves) {
    Color side = board.getSideToMove();
    bitboard king = board.king(side);
    
    if (king) {
        Square from = static_cast<Square>(__builtin_ctzll(king));
        bitboard enemyAtt = allEnemyAttacks(board, side);
        bitboard attacks = KING_ATTACKS[static_cast<int>(from)] & ~board.occupancy(side) & ~enemyAtt;
        
        while (attacks) {
            Square to = static_cast<Square>(__builtin_ctzll(attacks));
            Piece captured = board.pieceAt(to);
            
            if (captured != NO_PIECE) {
                // Normalize captured piece to white piece index (0-5)
                Piece normalizedCaptured = static_cast<Piece>(static_cast<int>(captured) % 6);
                moves.push_back(Move(from, to, static_cast<Piece>(KING), normalizedCaptured));
            } else {
                moves.push_back(Move(from, to, static_cast<Piece>(KING)));
            }
            
            attacks &= attacks - 1;
        }
    }
}

void MoveGen::generateCastlingMoves(const Board& board, std::vector<Move>& moves) {
    Color side = board.getSideToMove();
    Color enemy = static_cast<Color>(!side);
    bitboard occAll = board.allOccupancy();
    
    // Define castling paths
    const Square KS_PASS[2][2] = {{F1, G1}, {F8, G8}};
    const Square QS_PASS[2][3] = {{D1, C1, B1}, {D8, C8, B8}};
    
    // Check kingside castling
    if (board.hasCastlingRight(side, KINGSIDE)) {
        Square kingFrom = side == WHITE ? E1 : E8;
        Square kingTo = side == WHITE ? G1 : G8;
        Square rookFrom = side == WHITE ? H1 : H8;
        
        // Verify rook is present
        Piece rookPiece = static_cast<Piece>(ROOK + (side * 6));
        if (board.pieceAt(rookFrom) != rookPiece) {
            // Skip to queenside check
        } else {
            // Check if squares between king and rook are empty
            bool squaresEmpty = true;
            for (int i = 0; i < 2; i++) {
                if (occAll & (1ULL << static_cast<int>(KS_PASS[side][i]))) {
                    squaresEmpty = false;
                    break;
                }
            }
            
            if (squaresEmpty) {
                // Check if king and squares it moves through are not attacked
                bool squaresSafe = !isSquareAttacked(board, kingFrom, enemy);
                if (squaresSafe) {
                    for (int i = 0; i < 2; i++) {
                        if (isSquareAttacked(board, KS_PASS[side][i], enemy)) {
                            squaresSafe = false;
                            break;
                        }
                    }
                }
                
                if (squaresSafe) {
                    moves.push_back(Move(kingFrom, kingTo, KING, NO_PIECE, NO_PIECE, Move::CASTLE));
                }
            }
        }
    }
    
    // Check queenside castling
    if (board.hasCastlingRight(side, QUEENSIDE)) {
        Square kingFrom = side == WHITE ? E1 : E8;
        Square kingTo = side == WHITE ? C1 : C8;
        Square rookFrom = side == WHITE ? A1 : A8;
        
        // Verify rook is present
        Piece rookPiece = static_cast<Piece>(ROOK + (side * 6));
        if (board.pieceAt(rookFrom) != rookPiece) {
            return;
        }
        
        // Check if squares between king and rook are empty
        bool squaresEmpty = true;
        for (int i = 0; i < 3; i++) {
            if (occAll & (1ULL << static_cast<int>(QS_PASS[side][i]))) {
                squaresEmpty = false;
                break;
            }
        }
        
        if (squaresEmpty) {
            // Check if king and squares it moves through are not attacked
            bool squaresSafe = !isSquareAttacked(board, kingFrom, enemy);
            if (squaresSafe) {
                for (int i = 0; i < 3; i++) {
                    if (isSquareAttacked(board, QS_PASS[side][i], enemy)) {
                        squaresSafe = false;
                        break;
                    }
                }
            }
            
            if (squaresSafe) {
                moves.push_back(Move(kingFrom, kingTo, KING, NO_PIECE, NO_PIECE, Move::CASTLE));
            }
        }
    }
}

void MoveGen::generateEnPassantMoves(const Board& board, std::vector<Move>& moves) {
    int epFile = board.getEpFile();
    if (epFile == -1) return;
    
    Color side = board.getSideToMove();
    Piece pawnPiece = static_cast<Piece>(PAWN + (side * 6));
    int epRank = side == WHITE ? 4 : 3;  // En passant rank is always 3 for white, 4 for black
    
    // Check adjacent files for pawns that can capture en passant
    for (int fileOffset : {-1, 1}) {
        int file = epFile + fileOffset;
        if (file < 0 || file > 7) continue;
        
        Square from = static_cast<Square>(epRank * 8 + file);
        if (board.pieceAt(from) != pawnPiece) continue;
        
        Square epSquare = static_cast<Square>(epRank * 8 + epFile);
        
        // Create a test board to verify king safety
        Board testBoard = board;
        Move epMove(from, epSquare, PAWN, NO_PIECE, NO_PIECE, Move::EP);
        testBoard.makeMove(epMove);
        
        // Get king's square after the move
        Piece kingPiece = static_cast<Piece>(KING + (side * 6));
        bitboard kingBB = testBoard.getPieceBB(kingPiece);
        Square kingSquare = static_cast<Square>(__builtin_ctzll(kingBB));
        
        // Only add the move if it doesn't leave the king in check
        if (!isSquareAttacked(testBoard, kingSquare, static_cast<Color>(!side))) {
            moves.push_back(epMove);
        }
    }
}

bool MoveGen::isSquareAttacked(const Board& board, Square square, Color byColor) {
    // Check for pawn attacks
    bitboard pawns = byColor == WHITE ? board.getPieceBB(static_cast<Piece>(PAWN)) : board.getPieceBB(static_cast<Piece>(PAWN + 6));
    bitboard pawnAttacks = PAWN_ATTACKS[byColor][static_cast<int>(square)];
    if (pawnAttacks & pawns) {
        return true;
    }
    
    // Check for knight attacks
    bitboard knights = byColor == WHITE ? board.getPieceBB(static_cast<Piece>(KNIGHT)) : board.getPieceBB(static_cast<Piece>(KNIGHT + 6));
    bitboard knightAttacks = KNIGHT_ATTACKS[static_cast<int>(square)];
    if (knightAttacks & knights) {
        return true;
    }
    
    // Check for king attacks
    bitboard king = byColor == WHITE ? board.getPieceBB(static_cast<Piece>(KING)) : board.getPieceBB(static_cast<Piece>(KING + 6));
    bitboard kingAttacks = KING_ATTACKS[static_cast<int>(square)];
    if (kingAttacks & king) {
        return true;
    }
    
    // Check for bishop/queen attacks (diagonal)
    bitboard bishopsQueens = (byColor == WHITE ? board.getPieceBB(static_cast<Piece>(BISHOP)) : board.getPieceBB(static_cast<Piece>(BISHOP + 6))) | 
                            (byColor == WHITE ? board.getPieceBB(static_cast<Piece>(QUEEN)) : board.getPieceBB(static_cast<Piece>(QUEEN + 6)));
    bitboard bishopAttacks = getBishopAttacks(square, board.allOccupancy());
    if (bishopAttacks & bishopsQueens) {
        return true;
    }
    
    // Check for rook/queen attacks (orthogonal)
    bitboard rooksQueens = (byColor == WHITE ? board.getPieceBB(static_cast<Piece>(ROOK)) : board.getPieceBB(static_cast<Piece>(ROOK + 6))) | 
                          (byColor == WHITE ? board.getPieceBB(static_cast<Piece>(QUEEN)) : board.getPieceBB(static_cast<Piece>(QUEEN + 6)));
    bitboard rookAttacks = getRookAttacks(square, board.allOccupancy());
    if (rookAttacks & rooksQueens) {
        return true;
    }
    
    return false;
}

bool MoveGen::isLegalMove(const Board& board, const Move& move) {
    Color side = board.getSideToMove();
    Color enemy = static_cast<Color>(1 - side);

    // --- Basic sanity checks -------------------------------------------------

    // Get the piece being moved from the board
    Piece movingPiece = board.pieceAt(move.from());
    if (movingPiece == NO_PIECE) {
        return false; // no piece on source square
    }

    // Ensure the piece belongs to the side to move
    if ((side == WHITE && movingPiece >= 6) ||
        (side == BLACK && movingPiece < 6)) {
        return false;
    }

    // Verify the move's piece type matches the board
    if (move.piece() != static_cast<Piece>(static_cast<int>(movingPiece) % 6)) {
        return false;
    }

    // Destination square must not contain our own piece
    Piece destPiece = board.pieceAt(move.to());
    if (destPiece != NO_PIECE) {
        if ((side == WHITE && destPiece < 6) || (side == BLACK && destPiece >= 6)) {
            return false;
        }
    }

    // --- Pseudo-legal check -------------------------------------------------

    // Generate pseudo-legal moves and ensure the move exists
    std::vector<Move> pseudoMoves = generatePseudoLegalMoves(board);
    bool found = false;
    for (const Move& m : pseudoMoves) {
        if (m == move) { found = true; break; }
    }
    if (!found) {
        return false;
    }

    // --- King safety check --------------------------------------------------

    // Copy the board and play the move
    Board testBoard = board;
    testBoard.makeMove(move);

    // Determine king square after the move
    Piece kingPiece = static_cast<Piece>(KING + (side * 6));
    bitboard kingBB = testBoard.getPieceBB(kingPiece);
    Square kingSquare = static_cast<Square>(__builtin_ctzll(kingBB));

    return !isSquareAttacked(testBoard, kingSquare, enemy);
}

bitboard MoveGen::allEnemyAttacks(const Board& board, Color side) {
    Color enemy = static_cast<Color>(!side);
    bitboard attacks = 0;
    bitboard occAll = board.allOccupancy();
    
    // Pawn attacks
    bitboard pawns = board.getPieceBB(static_cast<Piece>(PAWN + (enemy * 6)));
    while (pawns) {
        Square from = static_cast<Square>(__builtin_ctzll(pawns));
        attacks |= PAWN_ATTACKS[enemy][static_cast<int>(from)];
        pawns &= pawns - 1;
    }
    
    // Knight attacks
    bitboard knights = board.getPieceBB(static_cast<Piece>(KNIGHT + (enemy * 6)));
    while (knights) {
        Square from = static_cast<Square>(__builtin_ctzll(knights));
        attacks |= KNIGHT_ATTACKS[static_cast<int>(from)];
        knights &= knights - 1;
    }
    
    // King attacks
    bitboard king = board.getPieceBB(static_cast<Piece>(KING + (enemy * 6)));
    if (king) {
        Square from = static_cast<Square>(__builtin_ctzll(king));
        attacks |= KING_ATTACKS[static_cast<int>(from)];
    }
    
    // Bishop/Queen attacks
    bitboard bishopsQueens = board.getPieceBB(static_cast<Piece>(BISHOP + (enemy * 6))) | 
                            board.getPieceBB(static_cast<Piece>(QUEEN + (enemy * 6)));
    while (bishopsQueens) {
        Square from = static_cast<Square>(__builtin_ctzll(bishopsQueens));
        attacks |= getBishopAttacks(from, occAll);
        bishopsQueens &= bishopsQueens - 1;
    }
    
    // Rook/Queen attacks
    bitboard rooksQueens = board.getPieceBB(static_cast<Piece>(ROOK + (enemy * 6))) | 
                          board.getPieceBB(static_cast<Piece>(QUEEN + (enemy * 6)));
    while (rooksQueens) {
        Square from = static_cast<Square>(__builtin_ctzll(rooksQueens));
        attacks |= getRookAttacks(from, occAll);
        rooksQueens &= rooksQueens - 1;
    }
    
    return attacks;
}

// attack tables

// Initialize lookup tables
std::array<bitboard, 64> MoveGen::KNIGHT_ATTACKS;
std::array<bitboard, 64> MoveGen::KING_ATTACKS;
std::array<bitboard, 64> MoveGen::PAWN_ATTACKS[2];
std::array<bitboard, 64> MoveGen::BISHOP_MASKS;
std::array<bitboard, 64> MoveGen::ROOK_MASKS;
std::array<uint64_t, 64> MoveGen::BISHOP_MAGICS;
std::array<uint64_t, 64> MoveGen::ROOK_MAGICS;
std::array<std::array<bitboard, 4096>, 64> MoveGen::BISHOP_ATTACKS;
std::array<std::array<bitboard, 4096>, 64> MoveGen::ROOK_ATTACKS;
bool MoveGen::initialized = false;

// Magic numbers for rooks
const uint64_t ROOK_MAGIC_NUMBERS[64] = {
    0x0080001020400080ULL, 0x0040001000200040ULL, 0x0080081000200080ULL, 0x0080040800100080ULL,
    0x0080020400080080ULL, 0x0080010200040080ULL, 0x0080008001000200ULL, 0x0080002040800100ULL,
    0x0000800020400080ULL, 0x0000400020005000ULL, 0x0000801000200080ULL, 0x0000800800100080ULL,
    0x0000800400080080ULL, 0x0000800200040080ULL, 0x0000800100020080ULL, 0x0000800040800100ULL,
    0x0000208000400080ULL, 0x0000404000201000ULL, 0x0000808010002000ULL, 0x0000808008001000ULL,
    0x0000808004000800ULL, 0x0000808002000400ULL, 0x0000010100020004ULL, 0x0000020000408104ULL,
    0x0000208080004000ULL, 0x0000200040005000ULL, 0x0000100080200080ULL, 0x0000080080100080ULL,
    0x0000040080080080ULL, 0x0000020080040080ULL, 0x0000010080800200ULL, 0x0000800080004100ULL,
    0x0000204000800080ULL, 0x0000200040401000ULL, 0x0000100080802000ULL, 0x0000080080801000ULL,
    0x0000040080800800ULL, 0x0000020080800400ULL, 0x0000020001010004ULL, 0x0000800040800100ULL,
    0x0000204000808000ULL, 0x0000200040008080ULL, 0x0000100020008080ULL, 0x0000080010008080ULL,
    0x0000040008008080ULL, 0x0000020004008080ULL, 0x0000800100020080ULL, 0x0000800041000080ULL,
    0x00FFFCDDFCED714AULL, 0x007FFCDDFCED714AULL, 0x003FFFCDFFD88096ULL, 0x0000040810002101ULL,
    0x0001000204080011ULL, 0x0001000204000801ULL, 0x0001000082000401ULL, 0x0001FFFAABFAD1A2ULL
};

// Magic numbers for bishops
const uint64_t BISHOP_MAGIC_NUMBERS[64] = {
    0x0002020202020200ULL, 0x0002020202020000ULL, 0x0004010202000000ULL, 0x0004040080000000ULL,
    0x0001104000000000ULL, 0x0000821040000000ULL, 0x0000410410400000ULL, 0x0000104104104000ULL,
    0x0000040404040400ULL, 0x0000020202020200ULL, 0x0000040102020000ULL, 0x0000040400800000ULL,
    0x0000011040000000ULL, 0x0000008210400000ULL, 0x0000004104104000ULL, 0x0000002082082000ULL,
    0x0004000808080800ULL, 0x0002000404040400ULL, 0x0001000202020200ULL, 0x0000800802004000ULL,
    0x0000800400A00000ULL, 0x0000200100884000ULL, 0x0000400082082000ULL, 0x0000200041041000ULL,
    0x0002080010101000ULL, 0x0001040008080800ULL, 0x0000208004010400ULL, 0x0000404004010200ULL,
    0x0000840000802000ULL, 0x0000404002011000ULL, 0x0000808001041000ULL, 0x0000404000820800ULL,
    0x0001041000202000ULL, 0x0000820800101000ULL, 0x0000104400080800ULL, 0x0000020080080080ULL,
    0x0000404040040100ULL, 0x0000808100020100ULL, 0x0001010100020800ULL, 0x0000808080010400ULL,
    0x0000820820004000ULL, 0x0000410410002000ULL, 0x0000082088001000ULL, 0x0000002011000800ULL,
    0x0000080100400400ULL, 0x0001010101000200ULL, 0x0002020202000400ULL, 0x0001010101000200ULL,
    0x0000410410400000ULL, 0x0000208208200000ULL, 0x0000002084100000ULL, 0x0000000020880000ULL,
    0x0000001002020000ULL, 0x0000040408020000ULL, 0x0004040404040000ULL, 0x0002020202020000ULL,
    0x0000104104104000ULL, 0x0000002082082000ULL, 0x0000000020841000ULL, 0x0000000000208800ULL,
    0x0000000010020200ULL, 0x0000000404080200ULL, 0x0000040404040400ULL, 0x0002020202020200ULL
};

// Magic shifts for rooks
const int ROOK_MAGIC_SHIFTS[64] = {
    52, 53, 53, 53, 53, 53, 53, 52,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    52, 53, 53, 53, 53, 53, 53, 52
};

// Magic shifts for bishops
const int BISHOP_MAGIC_SHIFTS[64] = {
    58, 59, 59, 59, 59, 59, 59, 58,
    59, 59, 59, 59, 59, 59, 59, 59,
    59, 59, 57, 57, 57, 57, 59, 59,
    59, 59, 57, 55, 55, 57, 59, 59,
    59, 59, 57, 55, 55, 57, 59, 59,
    59, 59, 57, 57, 57, 57, 59, 59,
    59, 59, 59, 59, 59, 59, 59, 59,
    58, 59, 59, 59, 59, 59, 59, 58
};

// Initialize the attack tables
void MoveGen::initializeAttackTables() {
    if (initialized) return;
    
    // Initialize knight attacks
    for (int square = 0; square < 64; square++) {
        KNIGHT_ATTACKS[square] = generateKnightAttacks(static_cast<Square>(square));
    }

    // Initialize king attacks
    for (int square = 0; square < 64; square++) {
        KING_ATTACKS[square] = generateKingAttacks(static_cast<Square>(square));
    }

    // Initialize pawn attacks
    for (int square = 0; square < 64; square++) {
        PAWN_ATTACKS[WHITE][square] = generatePawnAttacks(static_cast<Square>(square), WHITE);
        PAWN_ATTACKS[BLACK][square] = generatePawnAttacks(static_cast<Square>(square), BLACK);
    }

    // Initialize bishop and rook masks
    for (int square = 0; square < 64; square++) {
        BISHOP_MASKS[square] = generateBishopMask(static_cast<Square>(square));
        ROOK_MASKS[square] = generateRookMask(static_cast<Square>(square));
    }

    // Copy magic numbers
    std::copy(ROOK_MAGIC_NUMBERS, ROOK_MAGIC_NUMBERS + 64, ROOK_MAGICS.begin());
    std::copy(BISHOP_MAGIC_NUMBERS, BISHOP_MAGIC_NUMBERS + 64, BISHOP_MAGICS.begin());

    // Initialize magic bitboard attack tables
    for (int square = 0; square < 64; square++) {
        // Initialize bishop attacks
        bitboard mask = BISHOP_MASKS[square];
        int n = __builtin_popcountll(mask);
        for (int i = 0; i < (1 << n); i++) {
            bitboard occupancy = 0;
            bitboard temp = mask;
            for (int j = 0; j < n; j++) {
                int lsb = __builtin_ctzll(temp);
                if (i & (1 << j)) {
                    occupancy |= (1ULL << lsb);
                }
                temp &= temp - 1;
            }
            int index = (occupancy * BISHOP_MAGICS[square]) >> BISHOP_MAGIC_SHIFTS[square];
            BISHOP_ATTACKS[square][index] = calculateBishopAttacks(static_cast<Square>(square), occupancy);
        }

        // Initialize rook attacks
        mask = ROOK_MASKS[square];
        n = __builtin_popcountll(mask);
        for (int i = 0; i < (1 << n); i++) {
            bitboard occupancy = 0;
            bitboard temp = mask;
            for (int j = 0; j < n; j++) {
                int lsb = __builtin_ctzll(temp);
                if (i & (1 << j)) {
                    occupancy |= (1ULL << lsb);
                }
                temp &= temp - 1;
            }
            int index = (occupancy * ROOK_MAGICS[square]) >> ROOK_MAGIC_SHIFTS[square];
            ROOK_ATTACKS[square][index] = calculateRookAttacks(static_cast<Square>(square), occupancy);
        }
    }
    
    initialized = true;
}

bitboard MoveGen::generateKnightAttacks(Square square) {
    bitboard attacks = 0;
    int rank = static_cast<int>(square) / 8;
    int file = static_cast<int>(square) % 8;

    // All possible knight moves
    const int knightMoves[8][2] = {
        {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
        {1, -2}, {1, 2}, {2, -1}, {2, 1}
    };

    for (const auto& move : knightMoves) {
        int newRank = rank + move[0];
        int newFile = file + move[1];
        
        if (newRank >= 0 && newRank < 8 && newFile >= 0 && newFile < 8) {
            attacks |= 1ULL << (newRank * 8 + newFile);
        }
    }

    return attacks;
}

bitboard MoveGen::generateKingAttacks(Square square) {
    bitboard attacks = 0;
    int rank = static_cast<int>(square) / 8;
    int file = static_cast<int>(square) % 8;

    // All possible king moves
    const int kingMoves[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1}, {0, 1},
        {1, -1}, {1, 0}, {1, 1}
    };

    for (const auto& move : kingMoves) {
        int newRank = rank + move[0];
        int newFile = file + move[1];
        
        if (newRank >= 0 && newRank < 8 && newFile >= 0 && newFile < 8) {
            attacks |= 1ULL << (newRank * 8 + newFile);
        }
    }

    return attacks;
}

bitboard MoveGen::generatePawnAttacks(Square square, Color color) {
    bitboard attacks = 0;
    int rank = static_cast<int>(square) / 8;
    int file = static_cast<int>(square) % 8;

    if (color == WHITE) {
        if (rank < 7) {
            if (file > 0) attacks |= 1ULL << ((rank + 1) * 8 + (file - 1));
            if (file < 7) attacks |= 1ULL << ((rank + 1) * 8 + (file + 1));
        }
    } else {
        if (rank > 0) {
            if (file > 0) attacks |= 1ULL << ((rank - 1) * 8 + (file - 1));
            if (file < 7) attacks |= 1ULL << ((rank - 1) * 8 + (file + 1));
        }
    }

    return attacks;
}

bitboard MoveGen::generateBishopMask(Square square) {
    bitboard mask = 0;
    int rank = static_cast<int>(square) / 8;
    int file = static_cast<int>(square) % 8;

    // Generate mask for all possible bishop moves
    for (int r = rank + 1, f = file + 1; r < 7 && f < 7; r++, f++) mask |= 1ULL << (r * 8 + f);
    for (int r = rank + 1, f = file - 1; r < 7 && f > 0; r++, f--) mask |= 1ULL << (r * 8 + f);
    for (int r = rank - 1, f = file + 1; r > 0 && f < 7; r--, f++) mask |= 1ULL << (r * 8 + f);
    for (int r = rank - 1, f = file - 1; r > 0 && f > 0; r--, f--) mask |= 1ULL << (r * 8 + f);

    return mask;
}

bitboard MoveGen::generateRookMask(Square square) {
    bitboard mask = 0;
    int rank = static_cast<int>(square) / 8;
    int file = static_cast<int>(square) % 8;

    // Generate mask for all possible rook moves
    for (int r = rank + 1; r < 7; r++) mask |= 1ULL << (r * 8 + file);
    for (int r = rank - 1; r > 0; r--) mask |= 1ULL << (r * 8 + file);
    for (int f = file + 1; f < 7; f++) mask |= 1ULL << (rank * 8 + f);
    for (int f = file - 1; f > 0; f--) mask |= 1ULL << (rank * 8 + f);

    return mask;
}

bitboard MoveGen::calculateBishopAttacks(Square square, bitboard occupancy) {
    bitboard attacks = 0;
    int rank = static_cast<int>(square) / 8;
    int file = static_cast<int>(square) % 8;

    // Generate attacks in all four directions
    for (int r = rank + 1, f = file + 1; r < 8 && f < 8; r++, f++) {
        attacks |= 1ULL << (r * 8 + f);
        if (occupancy & (1ULL << (r * 8 + f))) break;
    }
    for (int r = rank + 1, f = file - 1; r < 8 && f >= 0; r++, f--) {
        attacks |= 1ULL << (r * 8 + f);
        if (occupancy & (1ULL << (r * 8 + f))) break;
    }
    for (int r = rank - 1, f = file + 1; r >= 0 && f < 8; r--, f++) {
        attacks |= 1ULL << (r * 8 + f);
        if (occupancy & (1ULL << (r * 8 + f))) break;
    }
    for (int r = rank - 1, f = file - 1; r >= 0 && f >= 0; r--, f--) {
        attacks |= 1ULL << (r * 8 + f);
        if (occupancy & (1ULL << (r * 8 + f))) break;
    }

    return attacks;
}

bitboard MoveGen::calculateRookAttacks(Square square, bitboard occupancy) {
    bitboard attacks = 0;
    int rank = static_cast<int>(square) / 8;
    int file = static_cast<int>(square) % 8;

    // Generate attacks in all four directions
    for (int r = rank + 1; r < 8; r++) {
        attacks |= 1ULL << (r * 8 + file);
        if (occupancy & (1ULL << (r * 8 + file))) break;
    }
    for (int r = rank - 1; r >= 0; r--) {
        attacks |= 1ULL << (r * 8 + file);
        if (occupancy & (1ULL << (r * 8 + file))) break;
    }
    for (int f = file + 1; f < 8; f++) {
        attacks |= 1ULL << (rank * 8 + f);
        if (occupancy & (1ULL << (rank * 8 + f))) break;
    }
    for (int f = file - 1; f >= 0; f--) {
        attacks |= 1ULL << (rank * 8 + f);
        if (occupancy & (1ULL << (rank * 8 + f))) break;
    }

    return attacks;
}

bitboard MoveGen::getBishopAttacks(Square square, bitboard occupancy) {
    occupancy &= BISHOP_MASKS[static_cast<int>(square)];
    occupancy *= BISHOP_MAGICS[static_cast<int>(square)];
    occupancy >>= BISHOP_MAGIC_SHIFTS[static_cast<int>(square)];
    return BISHOP_ATTACKS[static_cast<int>(square)][occupancy];
}

bitboard MoveGen::getRookAttacks(Square square, bitboard occupancy) {
    occupancy &= ROOK_MASKS[static_cast<int>(square)];
    occupancy *= ROOK_MAGICS[static_cast<int>(square)];
    occupancy >>= ROOK_MAGIC_SHIFTS[static_cast<int>(square)];
    return ROOK_ATTACKS[static_cast<int>(square)][occupancy];
} 

