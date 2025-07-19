#include "board.hpp"
#include "../../../util/zobrist.hpp"

void Board::makeMove(Move m) {
    if(false){
        throw std::runtime_error("illegal move");
    }

    Color mover = stm;
    Piece pc = static_cast<Piece>(m.piece() + (mover * 6)); // Convert from 0-5 range to 0-11 range
    Square from = static_cast<Square>(m.from());
    Square to = static_cast<Square>(m.to());

    Piece captured = NO_PIECE;
    if (m.isEP()) {
        int offset   = (mover == WHITE ? -8 : 8);
        Square capSq = Square(int(to) + offset);
        captured     = Piece(PAWN + 6 * (1 - int(mover)));

        pieceBB[captured]     &= ~(1ULL << int(capSq));
        occ[1 - int(mover)]   &= ~(1ULL << int(capSq));
        occAll               &= ~(1ULL << int(capSq));
        hashKey              ^= Zobrist::pieceSquare(captured, capSq);
    }
    else if (m.isCapture()) {
        captured = pieceAt(to);
        if (captured != NO_PIECE) { // Extra guard to prevent out-of-range access
            pieceBB[captured]     &= ~(1ULL << int(to));
            occ[1 - int(mover)]   &= ~(1ULL << int(to));
            occAll               &= ~(1ULL << int(to));
            hashKey              ^= Zobrist::pieceSquare(captured, to);
        }
    }

    int oldEp = ep;
    uint8_t oldRights = castlingRights;

    StateInfo state;
    state.hashKey = hashKey;
    state.castlingRights = castlingRights;
    state.epFile = ep;
    state.fiftyMoveCounter = halfmoveClock;
    state.move = m;
    state.captured = captured;
    state.stm = stm;
    state.fullmoveNo = fullmoveNo;
    state.pieceBB = pieceBB;
    state.occ = occ;
    state.occAll = occAll;
    history.push_back(state);

    movePiece(pc, from, to);

    handleSpecialMoves(m, pc, from, to, captured);

    updateGameState(m, pc, mover, oldEp, oldRights);
}

void Board::handleSpecialMoves(Move m, Piece pc, Square from, Square to, Piece captured) {
    Color mover = stm;

    if(pc == static_cast<Piece>(KING + (mover * 6))){
        if(m.to() == Square::G1){
            movePiece(static_cast<Piece>(ROOK + (mover * 6)), Square::H1, Square::F1);
        }
        if(m.to() == Square::C1){
            movePiece(static_cast<Piece>(ROOK + (mover * 6)), Square::A1, Square::D1);
        }
        if(m.to() == Square::G8){
            movePiece(static_cast<Piece>(ROOK + (mover * 6)), Square::H8, Square::F8);
        }
        if(m.to() == Square::C8){
            movePiece(static_cast<Piece>(ROOK + (mover * 6)), Square::A8, Square::D8);
        }
    }

    if(m.isPromotion()){
        pieceBB[pc] &= ~(1ULL << static_cast<int>(to));
        Piece promoted = static_cast<Piece>(m.promotion() + (mover * 6)); // Convert from 0-5 range to 0-11 range
        pieceBB[promoted] |= (1ULL << static_cast<int>(to));

        updateOccupancy();
        hashKey ^= Zobrist::pieceSquare(pc, m.to());  // Remove pawn
        hashKey ^= Zobrist::pieceSquare(promoted, m.to());  // Add promoted piece
    }

    if (m.isDoublePush()) {
        ep = static_cast<int>(to) % 8;
    } else {
        ep = -1;
    }
}

void Board::updateGameState(Move m, Piece pc, Color mover, int oldEp, uint8_t oldRights) {
    if(pc == static_cast<Piece>(PAWN + (mover * 6)) || m.isCapture()){
        halfmoveClock = 0;
    } else {
        halfmoveClock++;
    }

    if(mover == BLACK){
        fullmoveNo++;
    }

    if(oldEp != -1) {
        hashKey ^= Zobrist::enPassantKey(oldEp);
    }
    hashKey ^= Zobrist::castlingKey(oldRights);
    hashKey ^= Zobrist::sideToMoveKey();

    hashKey ^= Zobrist::pieceSquare(pc, m.from());
    hashKey ^= Zobrist::pieceSquare(pc, m.to());

    if(m.isPromotion()) {
        Piece promoted = static_cast<Piece>(m.promotion() + (mover * 6)); // Convert from 0-5 range to 0-11 range
        hashKey ^= Zobrist::pieceSquare(pc, m.to());  // Remove pawn
        hashKey ^= Zobrist::pieceSquare(promoted, m.to());  // Add promoted piece
    }

    if(pc == static_cast<Piece>(KING + (mover * 6)) && m.isCastle()) {
        Square rookFrom = NO_SQUARE, rookTo = NO_SQUARE;
        if(m.to() == Square::G1) {
            rookFrom = Square::H1; rookTo = Square::F1;
        } else if(m.to() == Square::C1) {
            rookFrom = Square::A1; rookTo = Square::D1;
        } else if(m.to() == Square::G8) {
            rookFrom = Square::H8; rookTo = Square::F8;
        } else if(m.to() == Square::C8) {
            rookFrom = Square::A8; rookTo = Square::D8;
        }
        if(rookFrom != NO_SQUARE) {
            Piece rookPiece = static_cast<Piece>(ROOK + (mover * 6)); // Convert from 0-5 range to 0-11 range
            hashKey ^= Zobrist::pieceSquare(rookPiece, rookFrom);
            hashKey ^= Zobrist::pieceSquare(rookPiece, rookTo);
        }
    }

    if(pc == static_cast<Piece>(KING + (mover * 6))) {
        uint8_t newRights = castlingRights & ~(mover == WHITE ? 0x3 : 0xC);  // Clear KQ for moving side
        if(newRights != oldRights) {
            hashKey ^= Zobrist::castlingKey(oldRights);
            hashKey ^= Zobrist::castlingKey(newRights);
            oldRights = newRights;
        }
        castlingRights = newRights;
    } else if(pc == static_cast<Piece>(ROOK + (mover * 6))) {
        uint8_t newRights = castlingRights;
        if(m.from() == Square::A1) newRights &= ~0x1;  // Clear Q for white
        if(m.from() == Square::H1) newRights &= ~0x2;  // Clear K for white
        if(m.from() == Square::A8) newRights &= ~0x4;  // Clear Q for black
        if(m.from() == Square::H8) newRights &= ~0x8;  // Clear K for black
        if(newRights != oldRights) {
            hashKey ^= Zobrist::castlingKey(oldRights);
            hashKey ^= Zobrist::castlingKey(newRights);
            oldRights = newRights;
        }
        castlingRights = newRights;
    } else if(m.isCapture() && history.back().captured == static_cast<Piece>(ROOK + ((1 - mover) * 6))) {
        uint8_t newRights = castlingRights;
        if(m.to() == Square::A1) newRights &= ~0x1;  // Clear Q for white
        if(m.to() == Square::H1) newRights &= ~0x2;  // Clear K for white
        if(m.to() == Square::A8) newRights &= ~0x4;  // Clear Q for black
        if(m.to() == Square::H8) newRights &= ~0x8;  // Clear K for black
        if(newRights != oldRights) {
            hashKey ^= Zobrist::castlingKey(oldRights);
            hashKey ^= Zobrist::castlingKey(newRights);
            oldRights = newRights;
        }
        castlingRights = newRights;
    }

    if(ep != -1) {
        hashKey ^= Zobrist::enPassantKey(ep);
    }
    hashKey ^= Zobrist::castlingKey(castlingRights);

    stm = static_cast<Color>(1 - static_cast<int>(stm));
    hashKey ^= Zobrist::sideToMoveKey();
}

void Board::unmakeMove() {
    if (history.empty()) return;
    
    const StateInfo& lastState = history.back();
    
    // Restore all state
    hashKey = lastState.hashKey;
    castlingRights = lastState.castlingRights;
    ep = lastState.epFile;
    halfmoveClock = lastState.fiftyMoveCounter;
    stm = lastState.stm;
    fullmoveNo = lastState.fullmoveNo;
    pieceBB = lastState.pieceBB;
    occ = lastState.occ;
    occAll = lastState.occAll;
    
    // Update ply
    ply--;
    
    // Remove the last state
    history.pop_back();
}

void Board::movePiece(Piece pc, Square from, Square to) noexcept {

    bitboard m = 1ULL << static_cast<int>(from) | 1ULL << static_cast<int>(to);
    pieceBB[pc] ^= m;  
    updateOccupancy();
}

void Board::setFen(const std::string& fen) {
    // Clear all bitboards
    pieceBB[PAWN] = pieceBB[KNIGHT] = pieceBB[BISHOP] = pieceBB[ROOK] = pieceBB[QUEEN] = pieceBB[KING] = 0ULL;
    occ[WHITE] = occ[BLACK] = 0ULL;
    occAll = 0ULL;

    std::istringstream fenStream(fen);
    std::string boardPos, side, castle, epStr, halfmove, fullmove;

    // Parse all six fields
    fenStream >> boardPos >> side >> castle >> epStr >> halfmove >> fullmove;

    // Parse board position
    int rank = 7;
    int file = 0;

    for (char c : boardPos) {
        if (c == '/') {
            rank--;
            file = 0;
        } else if (c >= '1' && c <= '8') {
            int emptySquares = c - '0';
            file += emptySquares;
        } else {
            int square = rank * 8 + file;
            bitboard bitPosition = 1ULL << square;
            
            switch (c) {
                case 'P': pieceBB[PAWN] |= bitPosition; break;
                case 'N': pieceBB[KNIGHT] |= bitPosition; break;
                case 'B': pieceBB[BISHOP] |= bitPosition; break;
                case 'R': pieceBB[ROOK] |= bitPosition; break;
                case 'Q': pieceBB[QUEEN] |= bitPosition; break;
                case 'K': pieceBB[KING] |= bitPosition; break;
                case 'p': pieceBB[PAWN + 6] |= bitPosition; break;
                case 'n': pieceBB[KNIGHT + 6] |= bitPosition; break;
                case 'b': pieceBB[BISHOP + 6] |= bitPosition; break;
                case 'r': pieceBB[ROOK + 6] |= bitPosition; break;
                case 'q': pieceBB[QUEEN + 6] |= bitPosition; break;
                case 'k': pieceBB[KING + 6] |= bitPosition; break;
            }
            file++;
        }
    }

    // Parse side to move
    stm = (side == "w") ? WHITE : BLACK;

    // Parse castling rights
    castlingRights = 0;
    if(castle != "-") {
        for(char c : castle) {
            switch(c) {
                case 'K': castlingRights |= 0x2; break;
                case 'Q': castlingRights |= 0x1; break;
                case 'k': castlingRights |= 0x8; break;
                case 'q': castlingRights |= 0x4; break;
            }
        }
    }

    // Parse en passant square
    // Parse en passant square. Only the file (0-7) is stored.
    ep = -1;
    if(epStr != "-") {
        ep = epStr[0] - 'a';
    }

    // Parse halfmove clock
    halfmoveClock = std::stoi(halfmove);

    // Parse fullmove number
    fullmoveNo = std::stoi(fullmove);

    // Update occupancy
    updateOccupancy();
}

void Board::updateOccupancy() noexcept {
     occ[WHITE] = occ[BLACK] = 0;
    for (int i = 0; i < 6; ++i) {
        occ[WHITE] |= pieceBB[i];
        occ[BLACK] |= pieceBB[i + 6];
    }
    occAll = occ[WHITE] | occ[BLACK];
}