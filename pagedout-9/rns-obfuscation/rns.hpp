#pragma once

#include <iostream>
#include <array>
#include <cstdint>
#include <cstddef>
#include <cstdio>

#include "maths.hpp"

template <class TElem, class TValue, TElem... M>
class RNS
{
    static constexpr std::size_t Mlen = sizeof...(M);
    static constexpr std::array<TElem, Mlen> Ms = {M...};

private:
    TElem value[Mlen];

public:
    const static inline constexpr TValue Mmax = []() constexpr
    {
        TValue Mmax = 1;
        for (std::size_t i = 0; i < Mlen; i++)
        {
            Mmax *= Ms[i];
        }
        return Mmax;
    }();

    const static inline constexpr std::array<TElem, Mlen> Minvs = []() constexpr
    {
        std::array<TElem, Mlen> Minvs{};
        for (std::size_t i = 0; i < Mlen; i++)
        {
            Minvs[i] = mod_inv<int64_t>(Mmax / Ms[i], Ms[i]);
        }
        return Minvs;
    }();

    constexpr RNS() : value(0) {}

    explicit constexpr RNS(const std::array<TElem, Mlen> &x) {
        for (std::size_t i = 0; i < Mlen; i++)
        {
            value[i] = x[i];
        }
    }

    constexpr RNS(const TValue x)
    {
        for (std::size_t i = 0; i < Mlen; i++)
        {
            value[i] = x % Ms[i];
        }
    }

    constexpr TValue decode() const
    {
        TValue solution = 0;
        for (std::size_t i = 0; i < Mlen; i++)
        {
            const TValue a_i = value[i];
            const TValue M_i = Mmax / Ms[i];
            solution = (solution + a_i * M_i % Mmax * Minvs[i]) % Mmax;
        }
        return solution;
    }

    constexpr RNS &operator+=(const RNS &other)
    {
        for (std::size_t i = 0; i < Mlen; i++)
        {
            value[i] = (value[i] + other.value[i]) % Ms[i];
        }
        return *this;
    }

    constexpr RNS operator+(const RNS &other) const
    {
        RNS result(*this);
        result += other;
        return result;
    }

    constexpr RNS &operator=(const RNS &other)
    {
        value = other.value;
        return *this;
    }

    constexpr bool operator==(const RNS &other) const
    {
        for (std::size_t i = 0; i < Mlen; i++)
        {
            if(value[i] != other.value[i]) {
                return false;
            }
        }
        return true;
    }

    constexpr bool operator!=(const RNS &other) const
    {
        return !(*this == other);
    }

    constexpr RNS &operator-=(const RNS &other)
    {
        for (std::size_t i = 0; i < Mlen; i++)
        {
            value[i] = (value[i] - other.value[i] + Ms[i]) % Ms[i];
        }
        return *this;
    }

    constexpr RNS operator-(const RNS &other) const
    {
        RNS result(*this);
        result -= other;
        return result;
    }

    constexpr RNS &operator*=(const RNS &other)
    {
        for (std::size_t i = 0; i < Mlen; i++)
        {
            value[i] = (value[i] * other.value[i]) % Ms[i];
        }
        return *this;
    }

    constexpr RNS operator*(const RNS &other) const
    {
        RNS result(*this);
        result *= other;
        return result;
    }
};
