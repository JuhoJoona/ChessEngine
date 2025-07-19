#include "engine.hpp"
#include <iostream>
#include "../util/logger.hpp"
#include "../core/game/movegen/movegen.hpp"
#include "../core/game/move/move.hpp"

Engine::Engine() {
    board = Board();
    LOG("=== Engine initialized ===" << std::endl);
}

Engine::~Engine() {
}

void Engine::initUci() {
    LOG("\n=== UCI Initialization ===" << std::endl);
    std::cout << "id name Chess Engine v2" << std::endl;
    std::cout << "id author Juhis" << std::endl;
    std::cout << "option name Hash type spin default 128 min 1 max 1024" << std::endl;
    std::cout << "option name Threads type spin default 1 min 1 max 8" << std::endl;
    std::cout << "option name MultiPV type spin default 1 min 1 max 5" << std::endl;
    std::cout << "uciok" << std::endl;
    std::cout.flush();
    LOG("=== UCI Initialization Complete ===" << std::endl);
}

void Engine::onIsReady() {
    LOG("\n=== Ready Check ===" << std::endl);
    std::cout << "readyok" << std::endl;
    std::cout.flush();
    LOG("=== Ready Check Complete ===" << std::endl);
}

void Engine::onPosition(const util::PositionCmd& pos) {
    LOG("\n=== Processing Position ===" << std::endl);
    
    std::string token;
    if (!(pos.ss >> token)) {
        LOG("ERROR: No position type specified" << std::endl);
        return;
    }
    
    if (token == "startpos") {
        board.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        LOG("Loaded startpos" << std::endl);

        if (!(pos.ss >> token)) {
            token = "";
        }
    } else if (token == "fen") {
        std::string fen;
        while (pos.ss >> token && token != "moves") {
            fen += token + " ";
        }
        LOG("Loading FEN: " << fen << std::endl);
        board.setFen(fen);
    } else {
        LOG("ERROR: Invalid position type: " << token << std::endl);
        return;
    }
    
    if (token == "moves") {
        LOG("Processing moves:" << std::endl);
        while (pos.ss >> token) {
            if (token.length() < 4) {
                LOG("ERROR: Invalid move format: " << token << std::endl);
                continue;
            }
            
            int fromFile = token[0] - 'a';
            int fromRank = token[1] - '1';
            int toFile = token[2] - 'a';
            int toRank = token[3] - '1';
            
            if (fromFile < 0 || fromFile > 7 || fromRank < 0 || fromRank > 7 ||
                toFile < 0 || toFile > 7 || toRank < 0 || toRank > 7) {
                LOG("ERROR: Invalid move coordinates: " << token << std::endl);
                continue;
            }
            
            int from = fromRank * 8 + fromFile;
            int to = toRank * 8 + toFile;
            
            Piece piece = board.pieceAt(static_cast<Square>(from));
            if (piece == NO_PIECE) {
                LOG("ERROR: No piece at source square: " << token << std::endl);
                continue;
            }
            
            Piece normalizedPiece = piece != NO_PIECE
                ? static_cast<Piece>(static_cast<int>(piece) % 6)
                : NO_PIECE;
            
            Piece captured = board.pieceAt(static_cast<Square>(to));
            Piece normalizedCaptured = captured != NO_PIECE
                ? static_cast<Piece>(static_cast<int>(captured) % 6)
                : NO_PIECE;
            
            Move move;
            
            if (normalizedPiece == PAWN) {
                int rankDiff = toRank - fromRank;
                if (abs(rankDiff) == 2 && ((piece == PAWN && fromRank == 1) || (piece == PAWN + 6 && fromRank == 6))) {
                    move = Move(static_cast<Square>(from), static_cast<Square>(to), normalizedPiece, normalizedCaptured, NO_PIECE, Move::DPUSH);
                } else {
                    move = Move(static_cast<Square>(from), static_cast<Square>(to), normalizedPiece, normalizedCaptured);
                }
            } else {
                move = Move(static_cast<Square>(from), static_cast<Square>(to), normalizedPiece, normalizedCaptured);
            }
            
            if (token.length() == 5) {
                Piece promoPiece;
                switch (token[4]) {
                    case 'q': promoPiece = QUEEN; break;
                    case 'r': promoPiece = ROOK; break;
                    case 'b': promoPiece = BISHOP; break;
                    case 'n': promoPiece = KNIGHT; break;
                    default: promoPiece = QUEEN; break;
                }
                move = Move(static_cast<Square>(from), static_cast<Square>(to), normalizedPiece, normalizedCaptured, promoPiece);
            }
            
            if (!MoveGen::isLegalMove(board, move)) {
                LOG("ERROR: Illegal move: " << token << std::endl);
                continue;
            }
            
            LOG("Making move: " << static_cast<int>(move.from()) << " to " << static_cast<int>(move.to()));
            if (move.isPromotion()) {
                LOG(" (promotion)");
            }
            LOG(std::endl);
            
            board.makeMove(move);
        }
    }
    LOG("=== Position Processing Complete ===" << std::endl);
}

void Engine::onGo(const util::GoCmd& go) {
    LOG("\n=== Processing Go Command ===" << std::endl);
    
    std::string token;
    bool infinite = false;
    while (go.ss >> token) {
        if (token == "infinite") {
            infinite = true;
            LOG("Infinite search mode" << std::endl);
        }
    }
    
    std::vector<Move> pseudoLegalMoves = MoveGen::generatePseudoLegalMoves(board);
    LOG("Generated " << pseudoLegalMoves.size() << " pseudo-legal moves" << std::endl);
    
    std::vector<Move> legalMoves;
    for (const Move& move : pseudoLegalMoves) {
        if (MoveGen::isLegalMove(board, move)) {
            legalMoves.push_back(move);
        }
    }
    
    LOG("Filtered to " << legalMoves.size() << " legal moves" << std::endl);
    
    if (!legalMoves.empty()) {
        Move bestMove = legalMoves[0];
        
        int from = static_cast<int>(bestMove.from());
        int to = static_cast<int>(bestMove.to());
        
        char fromFile = 'a' + (from % 8);
        char fromRank = '1' + (from / 8);
        char toFile = 'a' + (to % 8);
        char toRank = '1' + (to / 8);
        
        std::cout << "bestmove " << fromFile << fromRank << toFile << toRank;
        
        if (bestMove.isPromotion()) {
            if ((board.getSideToMove() == WHITE && toRank == '8') || 
                (board.getSideToMove() == BLACK && toRank == '1')) {
                char promoPiece;
                switch (bestMove.promotion()) {
                    case QUEEN: promoPiece = 'q'; break;
                    case ROOK: promoPiece = 'r'; break;
                    case BISHOP: promoPiece = 'b'; break;
                    case KNIGHT: promoPiece = 'n'; break;
                    default: promoPiece = 'q'; break;
                }
                std::cout << promoPiece;
            }
        }
        std::cout << std::endl;
        std::cout.flush();
        LOG("Best move sent: " << fromFile << fromRank << toFile << toRank);
        if (bestMove.isPromotion()) {
            LOG(" (promotion)");
        }
        LOG(std::endl);
    } else {
        std::cout << "bestmove 0000" << std::endl;
        std::cout.flush();
        LOG("No legal moves, sent 0000" << std::endl);
    }
    
    if (!infinite) {
        LOG("=== Go Command Processing Complete ===" << std::endl);
    } else {
        LOG("=== Go Command Processing Complete (Infinite Mode) ===" << std::endl);
    }
}

void Engine::onStop() {
    LOG("\n=== Stop Command Received ===" << std::endl);
}

void Engine::onSetOption(std::istringstream& ss) {
    LOG("\n=== SetOption Command Received ===" << std::endl);
}

void Engine::onNewGame() {
    LOG("\n=== New Game Command Received ===" << std::endl);
    board = Board();
}



