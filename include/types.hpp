#pragma once

using int_t = long long;
using real_t = double;

template <typename T>
constexpr T c(T x) noexcept {
    return x;
}