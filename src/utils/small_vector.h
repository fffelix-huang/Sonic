#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>

namespace sonic {

template<class T, std::size_t N>
class SmallVector {
   public:
    using iterator = typename std::array<T, N>::iterator;

    SmallVector() = default;

    SmallVector(std::size_t n) :
        sz(n) {
        assert(n <= N);
    }

    ~SmallVector() = default;

    void push_back(const T& x) {
        assert(sz < N);
        data[sz++] = x;
    }

    template<class... Args>
    void emplace_back(Args&&... args) {
        assert(sz < N);
        data[sz++] = T(args...);
    }

    void pop_back() {
        assert(sz > 0);
        sz--;
    }

    void resize(std::size_t i) { sz = i; }

    bool contains(const T& x) const {
        return std::find(data.begin(), data.begin() + sz, x) != data.begin() + sz;
    }

    constexpr void        clear() { sz = 0; }
    constexpr std::size_t size() const { return sz; }
    constexpr bool        empty() const { return sz == 0; }

    T& operator[](std::size_t i) {
        assert(i < sz);
        return data[i];
    }

    const T& operator[](std::size_t i) const {
        assert(i < sz);
        return data[i];
    }

    auto begin() { return data.begin(); }
    auto end() { return data.begin() + static_cast<std::ptrdiff_t>(sz); }
    auto begin() const { return data.begin(); }
    auto end() const { return data.begin() + static_cast<std::ptrdiff_t>(sz); }

   private:
    std::array<T, N> data = {};
    std::size_t      sz   = 0;
};

}  // namespace sonic