#pragma once

#include <cassert>
#include <cstdint>
#include <string>

#include "../utils/small_vector.h"
#include "bitboard.h"

namespace sonic {

class Move {
public:
	enum class Promotion : std::uint8_t { None, Queen, Rook, Bishop, Knight };

	constexpr Move() = default;
	constexpr explicit Move(std::uint16_t num) : data(num) {}
	constexpr Move(Square from, Square to) : data(from.to_int() | (to.to_int() << 6)) {}
	constexpr Move(Square from, Square to, Promotion promotion) : data(from.to_int() | (to.to_int() << 6) | (static_cast<std::uint8_t>(promotion) << 12)) {}

	Square from() const { return Square(data & kFromMask); }
	Square to() const { return Square((data & kToMask) >> 6); }
	Promotion promotion() const { return Promotion((data & kPromotionMask) >> 12); }

	void set_from(Square from) { data = (data & ~kFromMask) | from.to_int(); }
	void set_to(Square to) { data = (data & ~kToMask) | (to.to_int() << 6); }
	void set_promotion(Promotion promotion) { data = (data & ~kPromotionMask) | (static_cast<std::uint8_t>(promotion) << 12); }

	std::uint16_t to_int() const { return data; }

	// Returns the move in UCI format.
	std::string to_string() const {
		// NO_MOVE
		if(data == 0) {
			return "(none)";
		}
		std::string res = from().to_string() + to().to_string();
		switch(promotion()) {
			case Promotion::None: return res;
			case Promotion::Queen: return res + 'q';
			case Promotion::Rook: return res + 'r';
			case Promotion::Bishop: return res + 'b';
			case Promotion::Knight: return res + 'n';
		}
		assert(false);
	}

	constexpr bool operator==(const Move& other) const { return data == other.data; }
	constexpr bool operator!=(const Move& other) const { return data != other.data; }

private:
	// bits 0~5 -> from
	// bits 6~11 -> to
	// bits 12~14 -> promotion
	// Null move if all bits are set to 1.
	std::uint16_t data = 0;

	enum Masks : std::uint16_t {
		kFromMask = 0b0000000000111111,
		kToMask = 0b0000111111000000,
		kPromotionMask = 0b0111000000000000,
	};
};

using MoveList = SmallVector<Move, 218>;

} // namespace sonic