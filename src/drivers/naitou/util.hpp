#pragma once

#include <functional>

template <class T, class U, class Comp = std::less<>>
constexpr bool CHMAX(T& xmax, const U& x, Comp comp = {}) noexcept {
    if (comp(xmax, x)) {
        xmax = x;
        return true;
    }
    return false;
}

template <class T, class U, class Comp = std::less<>>
constexpr bool CHMIN(T& xmin, const U& x, Comp comp = {}) noexcept {
    if (comp(x, xmin)) {
        xmin = x;
        return true;
    }
    return false;
}

template <class Integer>
constexpr Integer BIT_I(int i) {
    return Integer(1) << i;
}

template <class Integer>
constexpr Integer BIT_GET(Integer x, int i) {
    return x & BIT_I<Integer>(i);
}

template <class Integer>
constexpr bool BIT_TEST(Integer x, int i) {
    return bool(BIT_GET(x, i));
}

template <class Integer>
constexpr Integer BIT_SET(Integer x, int i) {
    return x | BIT_I<Integer>(i);
}

template <class Integer>
constexpr Integer BIT_UNSET(Integer x, int i) {
    return x & ~BIT_I<Integer>(i);
}

template <class Integer>
constexpr Integer BIT_ASSIGN(Integer x, int i, bool on) {
    return on ? BIT_SET(x, i) : BIT_UNSET(x, i);
}

template <class Integer>
constexpr Integer BIT_FLIP(Integer x, int i) {
    return x ^ BIT_I<Integer>(i);
}
