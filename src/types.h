#pragma once

namespace sonic {

constexpr int MAX_DEPTH = 128;

enum Direction : int {
	NORTH = 8,
	EAST = 1,
	SOUTH = -NORTH,
	WEST = -EAST,

	NORTH_EAST = NORTH + EAST,
	SOUTH_EAST = SOUTH + EAST,
	NORTH_WEST = NORTH + WEST,
	SOUTH_WEST = SOUTH + WEST
};

constexpr Direction operator+(Direction a, Direction b) { return Direction(int(a) + int(b)); }
constexpr Direction operator*(Direction d, int i) { return Direction(i * int(d)); }
constexpr Direction operator*(int i, Direction d) { return Direction(i * int(d)); }

} // namespace sonic
