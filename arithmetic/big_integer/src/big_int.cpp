//
// Created by Des Caldnd on 5/27/2024.
//

#include "../include/big_int.h"
#include <ranges>
#include <exception>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>

big_int &big_int::optimize() &
{
    auto this_iter = _digits.begin();
    auto dig_iter = this_iter;

    while (this_iter < _digits.end()) {

        for (; dig_iter - this_iter < 8; ++dig_iter) {
            if (*dig_iter != 0) {
                dig_iter = this_iter;
                break;
            }
        }
        if (this_iter == dig_iter) break;
        this_iter = dig_iter;
    }

    if (_digits.empty()) _sign = true;
    _digits.erase(_digits.begin(), this_iter);
    return *this;
}

big_int &big_int::increase_module() &
{
    auto this_iter = --_digits.end();

    while (*this_iter == (unsigned int)(-1) && this_iter >= _digits.begin()) {
        *this_iter = 0;
        --this_iter;
    }

    if (this_iter >= _digits.begin()) {
        ++(*this_iter);

    } else {
        _digits.emplace(_digits.begin(), 0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u);
    }

    optimize();
    return *this;
}

big_int &big_int::decrease_module() &
{
    if (_digits.empty()) return *this;

    auto this_iter = --_digits.end();

    while (*this_iter == 0u && this_iter >= _digits.begin()) {
        *this_iter = (unsigned int)(-1);
        --this_iter;
    }

    --(*this_iter);

    optimize();
    return *this;
}

std::strong_ordering big_int::operator<=>(const big_int &other) const noexcept
{
    throw not_implemented("std::strong_ordering big_int::operator<=>(const big_int &) const noexcept", "your code should be here...");
}

big_int::operator bool() const noexcept
{
    return _digits.empty();
}

big_int &big_int::operator++() &
{
    if (_sign) return increase_module();
    return decrease_module();
}

big_int big_int::operator++(int)
{
    auto old = big_int(*this);
    ++(*this);
    return old;
}

big_int &big_int::operator--() &
{
    if (_digits.empty()) {
        _sign = false;
        return increase_module();

    } else if (_sign) return decrease_module();
    return increase_module();
}

big_int big_int::operator--(int)
{
    auto old = big_int(*this);
    --(*this);
    return old;
}

big_int &big_int::operator+=(const big_int &other) &
{
    throw not_implemented("big_int &big_int::operator+=(const big_int &)", "your code should be here...");
}

big_int &big_int::operator-=(const big_int &other) &
{
    throw not_implemented("big_int &big_int::operator-=(const big_int &)", "your code should be here...");
}

big_int big_int::operator+(const big_int &other) const
{
    throw not_implemented("big_int big_int::operator+(const big_int &) const", "your code should be here...");
}

big_int big_int::operator-(const big_int &other) const
{
    throw not_implemented("big_int big_int::operator-(const big_int &) const", "your code should be here...");
}

big_int big_int::operator*(const big_int &other) const
{
    throw not_implemented("big_int big_int::operator*(const big_int &) const", "your code should be here...");
}

big_int big_int::operator/(const big_int &other) const
{
    throw not_implemented("big_int big_int::operator/(const big_int &) const", "your code should be here...");
}

big_int big_int::operator%(const big_int &other) const
{
    throw not_implemented("big_int big_int::operator%(const big_int &) const", "your code should be here...");
}

big_int big_int::operator&(const big_int &other) const
{
    throw not_implemented("big_int big_int::operator&(const big_int &) const", "your code should be here...");
}

big_int big_int::operator|(const big_int &other) const
{
    throw not_implemented("big_int big_int::operator|(const big_int &) const", "your code should be here...");
}

big_int big_int::operator^(const big_int &other) const
{
    throw not_implemented("big_int big_int::operator^(const big_int &) const", "your code should be here...");
}

big_int big_int::operator<<(size_t shift) const
{
    throw not_implemented("big_int big_int::operator<<(size_t ) const", "your code should be here...");
}

big_int big_int::operator>>(size_t shift) const
{
    throw not_implemented("big_int big_int::operator>>(size_t) const", "your code should be here...");
}

big_int &big_int::operator%=(const big_int &other) &
{
    throw not_implemented("big_int &big_int::operator%=(const big_int &)", "your code should be here...");
}

big_int big_int::operator~() const
{
    throw not_implemented("big_int big_int::operator~() const", "your code should be here...");
}

big_int &big_int::operator&=(const big_int &other) &
{
    throw not_implemented("big_int &big_int::operator&=(const big_int &)", "your code should be here...");
}

big_int &big_int::operator|=(const big_int &other) &
{
    throw not_implemented("big_int &big_int::operator|=(const big_int &)", "your code should be here...");
}

big_int &big_int::operator^=(const big_int &other) &
{
    throw not_implemented("big_int &big_int::operator^=(const big_int &)", "your code should be here...");
}

big_int &big_int::operator<<=(size_t shift) &
{
    throw not_implemented("big_int &big_int::operator<<=(size_t)", "your code should be here...");
}

big_int &big_int::operator>>=(size_t shift) &
{
    throw not_implemented("big_int &big_int::operator>>=(size_t)", "your code should be here...");
}

big_int &big_int::plus_assign(const big_int &other, size_t shift) &
{
    throw not_implemented("big_int &big_int::plus_assign(const big_int &, size_t)", "your code should be here...");
}

big_int &big_int::minus_assign(const big_int &other, size_t shift) &
{
    throw not_implemented("big_int &big_int::minus_assign(const big_int &, size_t)", "your code should be here...");
}

big_int &big_int::operator*=(const big_int &other) &
{
    throw not_implemented("big_int &big_int::operator*=(const big_int &)", "your code should be here...");
}

big_int &big_int::operator/=(const big_int &other) &
{
    throw not_implemented("big_int &big_int::operator/=(const big_int &)", "your code should be here...");
}

std::string big_int::to_string() const
{
    throw not_implemented("std::string big_int::to_string() const", "your code should be here...");
}

std::ostream &operator<<(std::ostream &stream, const big_int &value)
{
    throw not_implemented("std::ostream &operator<<(std::ostream &, const big_int &)", "your code should be here...");
}

std::istream &operator>>(std::istream &stream, big_int &value)
{
    throw not_implemented("std::istream &operator>>(std::istream &, big_int &)", "your code should be here...");
}

bool big_int::operator==(const big_int &other) const noexcept
{
    if (_digits.size() != other._digits.size()) return false;

    auto th = _digits.begin();
    auto oth = other._digits.begin();

    while (th < _digits.end()) {
        if (*th != *oth) return false;
        ++th;
        ++oth;
    }

    return true;
}

big_int::big_int(const std::vector<unsigned int, pp_allocator<unsigned int>> &digits, bool sign)
{
    _digits = std::vector<unsigned int, pp_allocator<unsigned int>>(digits);
    _sign = sign;
}

big_int::big_int(std::vector<unsigned int, pp_allocator<unsigned int>> &&digits, bool sign) noexcept
{
    _digits = std::vector<unsigned int, pp_allocator<unsigned int>>(digits);
    _sign = sign;
}

big_int::big_int(const std::string &num, unsigned int radix, pp_allocator<unsigned int>)
{
    throw not_implemented("big_int::big_int(const std::string &num, unsigned int radix, pp_allocator<unsigned int>)", "your code should be here...");
}

big_int::big_int(pp_allocator<unsigned int>)
{
    _digits = std::vector<unsigned int, pp_allocator<unsigned int>>();
    _sign = true;
}

big_int &big_int::multiply_assign(const big_int &other, big_int::multiplication_rule rule) &
{
    throw not_implemented("big_int &big_int::multiply_assign(const big_int &other, big_int::multiplication_rule rule) &", "your code should be here...");
}

big_int &big_int::divide_assign(const big_int &other, big_int::division_rule rule) &
{
    throw not_implemented("big_int &big_int::divide_assign(const big_int &other, big_int::division_rule rule) &", "your code should be here...");
}

big_int &big_int::modulo_assign(const big_int &other, big_int::division_rule rule) &
{
    throw not_implemented("big_int &big_int::modulo_assign(const big_int &other, big_int::division_rule rule) &", "your code should be here...");
}

big_int operator""_bi(unsigned long long n)
{
    throw not_implemented("big_int operator\"\"_bi(unsigned long long n)", "your code should be here...");
}