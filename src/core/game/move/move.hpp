#pragma once
#include <cstdint>
#include "../../../util/util.hpp"
using namespace util;
struct Move {
    std::uint32_t value = 0;           // 0 = NO_MOVE

    static constexpr std::uint32_t TO_MASK   = 0x0000003F;
    static constexpr std::uint32_t FROM_MASK = 0x00000FC0;
    static constexpr int           FROM_SHIFT = 6;
    static constexpr std::uint32_t PIECE_MASK = 0x0000F000;
    static constexpr int           PIECE_SHIFT = 12;
    static constexpr std::uint32_t CAP_MASK   = 0x000F0000;
    static constexpr int           CAP_SHIFT   = 16;
    static constexpr std::uint32_t PROMO_MASK = 0x00700000;
    static constexpr int           PROMO_SHIFT = 20;
    static constexpr std::uint32_t FLAGS_MASK = 0x03800000;
    static constexpr int           FLAGS_SHIFT = 23;

    constexpr Move() noexcept = default;
    constexpr Move(Square from, Square to,
                   Piece pc, Piece cap = NO_PIECE,
                   Piece promo = NO_PIECE, int fl = 0) noexcept
        : value(
            (static_cast<int>(to) & 0x3F) |
            ((static_cast<int>(from) & 0x3F) << 6) |
            ((static_cast<int>(pc) & 0x0F) << 12) |
            (((cap == NO_PIECE ? 0 : static_cast<int>(cap)) & 0x0F) << 16) |
            (((promo == NO_PIECE ? 0 : static_cast<int>(promo)) & 0x07) << 20) |
            ((fl & 0x07) << 23)) {}

    [[nodiscard]] constexpr Square from()       const noexcept { return Square((value >> 6)  & 0x3F); }
    [[nodiscard]] constexpr Square to()         const noexcept { return Square(value        & 0x3F); }
    [[nodiscard]] constexpr Piece  piece()      const noexcept { return Piece((value >> 12) & 0x0F); }
    [[nodiscard]] constexpr Piece  captured()   const noexcept { return Piece((value >> 16) & 0x0F); }
    [[nodiscard]] constexpr Piece  promotion()  const noexcept {
        int p = (value >> 20) & 0x07;
        return p == 0 ? NO_PIECE : static_cast<Piece>(p);
    }
    [[nodiscard]] constexpr int    flags()      const noexcept { return (value >> 23) & 0x07; }

    [[nodiscard]] constexpr bool isCapture()     const noexcept { return captured()  != NO_PIECE; }
    [[nodiscard]] constexpr bool isPromotion()   const noexcept { return promotion() != NO_PIECE; }
    [[nodiscard]] constexpr bool isEP()          const noexcept { return flags() == EP; }
    [[nodiscard]] constexpr bool isCastle()      const noexcept { return flags() == CASTLE; }
    [[nodiscard]] constexpr bool isDoublePush()  const noexcept { return flags() == DPUSH; }
    [[nodiscard]] constexpr bool isQuiet()       const noexcept { return !isCapture() && !isPromotion() && !isEP() && !isCastle(); }

    enum Flag : int { QUIET = 0, DPUSH = 1, EP = 2, CASTLE = 3 };

    constexpr void setScore(std::uint8_t s) noexcept { value = (value & 0x00FFFFFF) | (s << 24); }
    [[nodiscard]] constexpr std::uint8_t score() const noexcept { return value >> 24; }

    friend constexpr bool operator==(Move a, Move b) { return a.value == b.value; }
    friend constexpr bool operator!=(Move a, Move b) { return !(a == b); }
};

inline constexpr Move makeQuiet   (Square f, Square t, Piece pc)                 { return Move(f,t,pc); }
inline constexpr Move makeCapture (Square f, Square t, Piece pc, Piece cap)      { return Move(f,t,pc,cap); }
inline constexpr Move makePromo   (Square f, Square t, Piece pc, Piece promo, bool cap=false, Piece capPc=NO_PIECE) {
    return Move(f,t,pc,cap?capPc:NO_PIECE,promo);
}
inline constexpr Move makeEP      (Square f, Square t, Piece pc)                 { return Move(f,t,pc,PAWN,NO_PIECE,Move::EP); }
inline constexpr Move makeCastle  (Square f, Square t)                           { return Move(f,t,KING,NO_PIECE,NO_PIECE,Move::CASTLE); }
