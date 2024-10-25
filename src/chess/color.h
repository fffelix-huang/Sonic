#pragma once

namespace sonic {

enum Color {
    WHITE      = 0,
    BLACK      = 1,
    COLOR_NB   = 2,
    COLOR_NONE = 3
};

constexpr Color other_color(Color c) { return c == Color::WHITE ? Color::BLACK : Color::WHITE; }

} // namespace sonic
