#pragma once

#include <stdexcept>
#include <tuple>

template <class T>
constexpr T gcd(const T a, const T b, T &x, T &y)
{
    static_assert(std::is_signed<T>::value, "integral type T must be signed");

    x = 1, y = 0;
    T x1 = 0, y1 = 1, a1 = a, b1 = b;
    while (b1)
    {
        T q = a1 / b1;

        // std::tie(x, x1) = std::make_tuple(x1, x - q * x1); // not constexpr(?)
        T tmp1 = x - q * x1;
        x = x1;
        x1 = tmp1;

        // std::tie(y, y1) = std::make_tuple(y1, y - q * y1); // not constexpr(?)
        T tmp2 = y - q * y1;
        y = y1;
        y1 = tmp2;

        // std::tie(a1, b1) = std::make_tuple(b1, a1 - q * b1); // not constexpr(?)
        T tmp3 = a1 - q * b1;
        a1 = b1;
        b1 = tmp3;
    }
    return a1;
}

template <class T>
constexpr T mod_inv(const T a, const T m)
{
    T x;
    T y;
    T g = gcd(a, m, x, y);
    if (g != 1)
    {
        throw std::logic_error("Inverse doesn't exist");
    };
    T inv = (x % m + m) % m;
    return inv;
}