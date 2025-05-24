#include "../include/big_int.h"
#include <ranges>
#include <exception>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>

constexpr unsigned long long BASE = std::numeric_limits<unsigned int>::max();

big_int &big_int::optimize() &
{
    while (!_digits.empty() && _digits.back() == 0) _digits.pop_back();

    if (_digits.empty()) _sign = true;

    return *this;
}

big_int &big_int::increase_module(unsigned int diff, size_t shift) &
{
    constexpr unsigned int HALF_BITS = sizeof(unsigned int) * 4;
    constexpr unsigned int RM = (1u << HALF_BITS) - 1;
    constexpr unsigned int LM = RM << HALF_BITS;

    if (_digits.size() <= shift) {
        _digits.resize(shift + 1, 0u);
    }

    auto this_iter = _digits.begin() + static_cast<long long>(shift);
    unsigned int carry = 0;

    do {
        unsigned int current = *this_iter;
        unsigned int current_right = current & RM;
        unsigned int current_left = (current & LM) >> HALF_BITS;

        unsigned int diff_right = diff & RM;
        unsigned int diff_left = (diff & LM) >> HALF_BITS;

        unsigned int sum_right = current_right + diff_right + (carry & RM);
        unsigned int new_right = sum_right & RM;
        carry = sum_right >> HALF_BITS;

        unsigned int sum_left = current_left + diff_left + carry;
        unsigned int new_left = sum_left & RM;
        carry = sum_left >> HALF_BITS;

        *this_iter = new_right | (new_left << HALF_BITS);

        diff = 0;
        ++this_iter;
    } while (carry > 0 && this_iter != _digits.end());


    if (carry > 0) {
        _digits.push_back(carry);
    }

    return *this;
}

big_int &big_int::decrease_module(unsigned int diff, size_t shift) &
{
    if (_digits.empty()) return *this;

    constexpr unsigned int HALF_BITS = sizeof(unsigned int) * 4;
    constexpr unsigned int RM = (1u << HALF_BITS) - 1;
    constexpr unsigned int LM = RM << HALF_BITS;

    auto this_iter = _digits.begin() + static_cast<long long>(shift);
    unsigned int borrow = 0;

    unsigned int current = *this_iter;
    unsigned int diff_right = diff & RM;
    unsigned int diff_left = (diff & LM) >> HALF_BITS;

    unsigned int current_right = current & RM;
    bool need_borrow_right = current_right < diff_right;
    unsigned int new_right = (current_right - diff_right) & RM;

    unsigned int current_left = (current & LM) >> HALF_BITS;
    unsigned int adjusted_left = current_left - (need_borrow_right ? 1 : 0);
    bool need_borrow_left = (adjusted_left > current_left) || (adjusted_left < diff_left);
    unsigned int new_left = (adjusted_left - diff_left) & RM;

    *this_iter = new_right | (new_left << HALF_BITS);
    borrow = need_borrow_left ? 1 : 0;
    ++this_iter;

    while (borrow > 0 && this_iter != _digits.end()) {
        if (*this_iter == 0) {
            *this_iter = ~0u;
        } else {
            --(*this_iter);
            borrow = 0;
        }
        ++this_iter;
    }

    return *this;
}

big_int& big_int::plus_assign(const big_int& other, size_t shift) &
{
    if (_sign == other._sign) {
        for (size_t offset = 0; offset < other._digits.size(); ++offset) {
            increase_module(other._digits[offset], shift + offset);
        }

    } else {
        big_int abs_other(other);
        abs_other._sign = true;
        big_int abs_this(*this);
        abs_this._sign = true;

        for (size_t i = 0; i < shift; ++i) {
            abs_other._digits.emplace(abs_other._digits.begin(), 0u);
        }

        auto comp = abs_this <=> abs_other;

        if (comp == std::strong_ordering::less) {
            for (size_t offset = 0; offset < _digits.size(); ++offset) {
                abs_other.decrease_module(_digits[offset], offset);
            }
            _digits = std::move(abs_other._digits);
            _sign = !_sign;

        } else if (comp == std::strong_ordering::greater) {
            for (size_t offset = 0; offset < other._digits.size(); ++offset) {
                decrease_module(abs_other._digits[offset], offset);
            }

        } else {
            _digits.clear();
        }
    }
    optimize();
    return *this;
}

big_int &big_int::minus_assign(const big_int &other, size_t shift) &
{
    if (_sign != other._sign) {
        for (size_t offset = 0; offset < other._digits.size(); ++offset) {
            increase_module(other._digits[offset], shift + offset);
        }

    } else {
        big_int abs_other(other);
        abs_other._sign = true;
        big_int abs_this(*this);
        abs_this._sign = true;

        for (size_t i = 0; i < shift; ++i) {
            abs_other._digits.emplace(abs_other._digits.begin(), 0u);
        }

        auto comp = abs_this <=> abs_other;

        if (comp == std::strong_ordering::less) {
            for (size_t offset = 0; offset < _digits.size(); ++offset) {
                abs_other.decrease_module(_digits[offset], offset);
            }
            _digits = std::move(abs_other._digits);
            _sign = !_sign;

        } else if (comp == std::strong_ordering::greater) {
            for (size_t offset = 0; offset < other._digits.size(); ++offset) {
                decrease_module(abs_other._digits[offset], offset);
            }

        } else {
            _digits.clear();
        }
    }
    optimize();
    return *this;

}

big_int::big_int(const std::vector<unsigned int, pp_allocator<unsigned int>>& digits, bool sign)
        : _sign(sign), _digits(digits)
{
    optimize();
}

big_int::big_int(std::vector<unsigned int, pp_allocator<unsigned int>>&& digits, bool sign) noexcept
        : _sign(sign), _digits(std::move(digits))
{

    optimize();
}

big_int::big_int(const std::string& num, unsigned int radix, pp_allocator<unsigned int> allocator)
        : _sign(true), _digits(allocator)
{
    if (num.empty())
    {
        return;
    }

    std::string number = num;
    bool is_negative = false;
    if (number[0] == '-')
    {
        is_negative = true;
        number = number.substr(1);
    }
    else if (number[0] == '+')
    {
        number = number.substr(1);
    }

    while (number.size() > 1 && number[0] == '0')
    {
        number = number.substr(1);
    }

    if (number.empty())
    {
        return;
    }

    for (char c : number)
    {
        if (!std::isdigit(c))
        {
            throw std::invalid_argument("Invalid character in number string");
        }
        unsigned int digit = c - '0';
        *this *= 10;
        *this += big_int(static_cast<long long>(digit), allocator);
    }

    _sign = !is_negative;

    optimize();
}

big_int::big_int(pp_allocator<unsigned int> allocator)
        : _sign(true), _digits(allocator)
{
    optimize();
}

std::strong_ordering big_int::operator<=>(const big_int& other) const noexcept
{
    if (_sign != other._sign) {
        return _sign ? std::strong_ordering::greater : std::strong_ordering::less;
    }

    bool is_positive = _sign;

    if (_digits.size() != other._digits.size()) {
        if (is_positive) {
            return _digits.size() <=> other._digits.size();

        } else {
            return other._digits.size() <=> _digits.size();
        }
    }

    if (_digits.size() == 0) {
        return std::strong_ordering::equal;
    }

    auto th = --_digits.end();
    auto oth = --other._digits.end();

    while (th >= _digits.begin()) {
        if (*th != *oth) {
            if (is_positive) {
                return *th <=> *oth;

            } else {
                return *oth <=> *th;
            }
        }
        --th; --oth;
    }

    return std::strong_ordering::equal;
}

bool big_int::operator==(const big_int& other) const noexcept
{
    return (*this <=> other) == std::strong_ordering::equal;
}

bool big_int::operator<(const big_int& other) const noexcept
{
    return (*this <=> other) == std::strong_ordering::less;
}

bool big_int::operator>(const big_int& other) const noexcept
{
    return (*this <=> other) == std::strong_ordering::greater;
}

bool big_int::operator<=(const big_int& other) const noexcept
{
    return (*this <=> other) != std::strong_ordering::greater;
}

bool big_int::operator>=(const big_int& other) const noexcept
{
    return (*this <=> other) != std::strong_ordering::less;
}

big_int::operator bool() const noexcept
{
    return !_digits.empty();
}

big_int &big_int::operator++() &
{
    _sign ? increase_module(1, 0) : decrease_module(1, 0);
    optimize();
    return *this;
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
        increase_module(1, 0);

    } else {
        _sign ? decrease_module(1, 0) : increase_module(1, 0);
    }
    optimize();
    return *this;
}

big_int big_int::operator--(int)
{
    auto old = big_int(*this);
    --(*this);
    return old;
}

big_int &big_int::operator+=(const big_int &other) &
{
    return plus_assign(other, 0);
}

big_int &big_int::operator-=(const big_int &other) &
{
    return minus_assign(other, 0);
}

big_int big_int::operator+(const big_int &other) const
{
    big_int _new(*this);
    return _new.plus_assign(other, 0);
}

big_int big_int::operator-(const big_int &other) const
{
    big_int _new(*this);
    return _new.minus_assign(other, 0);
}

big_int big_int::operator*(const big_int& other) const
{
    big_int result(*this);
    result *= other;
    return result;
}

big_int big_int::operator/(const big_int& other) const
{
    big_int result(*this);
    result /= other;
    return result;
}

big_int big_int::operator%(const big_int &other) const
{
    big_int result(*this);
    result %= other;
    return result;
}

big_int big_int::operator&(const big_int &other) const
{
    big_int _new(*this);
    _new &= other;
    return _new;
}

big_int big_int::operator|(const big_int &other) const
{
    big_int _new(*this);
    _new |= other;
    return _new;
}

big_int big_int::operator^(const big_int &other) const
{
    big_int _new(*this);
    _new ^= other;
    return _new;
}

big_int big_int::operator<<(size_t shift) const
{
    big_int _new(*this);
    _new <<= shift;
    return _new;
}

big_int big_int::operator>>(size_t shift) const
{
    big_int _new(*this);
    _new >>= shift;
    return _new;
}

big_int &big_int::operator%=(const big_int &other) &
{
    return modulo_assign(other);
}

big_int big_int::operator~() const
{
    big_int _new(*this);

    for (int i = 0; i < (8 - _new._digits.size() % 8); ++i) _new._digits.emplace_back(0u);

    auto th = _new._digits.begin();
    for (; th < _new._digits.end(); ++th) *th = ~(*th);

    _new.optimize();
    return _new;
}

big_int &big_int::operator&=(const big_int &other) &
{
    auto th = _digits.begin();
    auto oth = other._digits.begin();

    while (oth != other._digits.end() && th != _digits.end()) {
        *th &= *oth;
        ++th; ++oth;
    }

    _digits.erase(th, _digits.end());
    optimize();
    return *this;
}

big_int &big_int::operator|=(const big_int &other) &
{
    for (int i = 0; i < (_digits.size() - other._digits.size()); ++i) _digits.emplace_back(0u);

    auto th = _digits.begin();
    auto oth = other._digits.begin();

    while (oth != other._digits.end()) {
        *th |= *oth;
        ++th; ++oth;
    }

    optimize();
    return *this;
}

big_int &big_int::operator^=(const big_int &other) &
{
    for (int i = 0; i < (_digits.size() - other._digits.size()); ++i) _digits.emplace_back(0u);

    auto th = _digits.begin();
    auto oth = other._digits.begin();

    while (oth != other._digits.end()) {
        *th ^= *oth;
        ++th; ++oth;
    }

    optimize();
    return *this;
}

big_int &big_int::operator>>=(size_t shift) &
{
    if (_digits.empty()) return *this;

    constexpr size_t UINT_BITS = std::numeric_limits<unsigned int>::digits;
    const size_t total_bits = shift;
    const size_t element_shift = total_bits / UINT_BITS;
    const size_t bit_shift = total_bits % UINT_BITS;

    if (element_shift >= _digits.size()) {
        _digits.clear();
        return *this;
    }
    _digits.erase(_digits.begin(), _digits.begin() + static_cast<long long>(element_shift));

    if (bit_shift == 0) {
        optimize();
        return *this;
    }

    auto it = _digits.begin();

    if (!_digits.empty()) {
        for (; it < _digits.end() - 1; ++it) {
            *it >>= bit_shift;
            *it |= (*(it + 1) << (UINT_BITS - bit_shift));
        }
        *it >>= bit_shift;
    }

    optimize();
    return *this;
}

big_int &big_int::operator<<=(size_t shift) &
{
    if (_digits.empty() || shift == 0) return *this;

    constexpr size_t UINT_BITS = std::numeric_limits<unsigned int>::digits;
    const size_t total_bits = shift;
    const size_t element_shift = total_bits / UINT_BITS;
    const size_t bit_shift = total_bits % UINT_BITS;

    if (element_shift > 0) {
        _digits.insert(_digits.begin(), element_shift, 0u);
    }

    if (bit_shift == 0) {
        optimize();
        return *this;
    }

    unsigned int carry = 0;

    for (auto it = _digits.begin(); it < _digits.end(); ++it) {
        unsigned int new_carry = *it >> (UINT_BITS - bit_shift);
        *it <<= bit_shift;
        *it |= carry;
        carry = new_carry;
    }

    if (carry != 0) {
        _digits.insert(_digits.end(), carry);
    }

    optimize();
    return *this;
}



big_int &big_int::operator*=(const big_int &other) &
{
    return multiply_assign(other);
}

big_int &big_int::operator/=(const big_int &other) &
{
    return divide_assign(other);
}

std::string big_int::to_string() const
{
    if(_digits.empty())
        return "0";

    std::stringstream res;
    auto tmp = *this;

    bool sign = tmp._sign;

    tmp._sign = true;

    while (tmp > 0_bi)
    {
        auto val = tmp % 10_bi;
        tmp /= 10_bi;

        res << char('0' + (val._digits.empty() ? 0 : val._digits[0]));
    }

    if (!sign)
    {
        res << '-';
    }

    std::string d = res.str();
    std::reverse(d.begin(), d.end());
    return d;
}

std::ostream &operator<<(std::ostream &stream, const big_int &value)
{
    stream << value.to_string();

    return stream;
}

std::istream &operator>>(std::istream &stream, big_int &value)
{
    std::string val;

    stream >> val;

    value = big_int(val);

    return stream;
}

big_int &big_int::multiply_assign(const big_int &other, big_int::multiplication_rule rule) &
{
    switch (rule)
    {
        case multiplication_rule::Karatsuba:
            return karatsuba(other);
        default:
            return trivial_multiply(other);
    }
}

big_int &big_int::trivial_multiply(const big_int &other) &
{
    _sign = !(_sign ^ other._sign);
    big_int tmp(*this);
    big_int tmp_other(other);
    big_int accumulator(0);

    while (tmp_other) {
        if (tmp_other._digits[0] & 1u) {
            accumulator += tmp;
        }
        tmp <<= 1;
        tmp_other >>= 1;
    }

    _digits = std::move(accumulator._digits);
    optimize();
    return *this;
}

big_int &big_int::divide_assign(const big_int &other, big_int::division_rule rule) &
{
    if (!other)
    {
        throw std::logic_error("Division by zero");
    }

    if (!*this)
    {
        return *this;
    }

    switch (rule)
    {
        default:
            return trivial_division(other);
    }
}

big_int &big_int::trivial_division(const big_int &other) &
{
    big_int abs_this(*this);
    abs_this._sign = true;
    big_int abs_other(other);
    abs_other._sign = true;

    if (abs_this < abs_other)
    {
        _digits.clear();
        _sign = true;
        return *this;
    }

    std::vector<unsigned int, pp_allocator<unsigned int>> quotient(_digits.size() - other._digits.size() + 1, 0, _digits.get_allocator());
    big_int remainder(_digits.get_allocator());
    remainder._digits.clear();

    for (long long i = static_cast<long long>(_digits.size()) - 1; i >= 0; --i)
    {
        remainder._digits.insert(remainder._digits.begin(), _digits[i]);
        remainder.optimize();

        big_int digit("0");
        digit._digits.push_back(0);

        unsigned int &closest = digit._digits[0];
        for (int j = std::numeric_limits<unsigned int>::digits - 1; j >= 0; --j) {
            const unsigned int temp = closest;
            closest |= 1 << j;
            big_int multiplied = abs_other * digit;

            auto comp = multiplied <=> remainder;
            if (comp == std::strong_ordering::equal) {
                break;
            } else if (comp == std::strong_ordering::less) {
                continue;
            } else {
                digit._digits[0] = temp;
            }
        }

        quotient.insert(quotient.begin(), digit._digits[0]);

        big_int prod = abs_other * digit;
        remainder -= prod;
        remainder.optimize();
    }

    _digits = std::move(quotient);
    _sign = (_sign == other._sign);
    optimize();

    return *this;
}

big_int &big_int::modulo_assign(const big_int &other, big_int::division_rule rule) &
{
    if (!other)
    {
        throw std::logic_error("Division by zero");
    }

    if (!*this)
    {
        return *this;
    }

    switch (rule)
    {
        default:
            return trivial_modulo(other);
    }
}

big_int &big_int::trivial_modulo(const big_int &other) &
{
    big_int abs_this(*this);
    abs_this._sign = true;
    big_int abs_other(other);
    abs_other._sign = true;

    if (abs_this < abs_other)
    {
        return *this;
    }

    big_int remainder(_digits.get_allocator());
    remainder._digits.clear();

    for (long long i = static_cast<long long>(_digits.size()) - 1; i >= 0; --i)
    {
        remainder._digits.insert(remainder._digits.begin(), _digits[i]);
        remainder.optimize();

        big_int digit("0");
        digit._digits.push_back(0);

        unsigned int &closest = digit._digits[0];
        for (int j = std::numeric_limits<unsigned int>::digits - 1; j >= 0; --j) {
            const unsigned int temp = closest;
            closest |= 1 << j;
            big_int multiplied = abs_other * digit;

            auto comp = multiplied <=> remainder;
            if (comp == std::strong_ordering::equal) {
                break;
            } else if (comp == std::strong_ordering::less) {
                continue;
            } else {
                digit._digits[0] = temp;
            }
        }

        big_int prod = abs_other * digit;
        remainder -= prod;
        remainder.optimize();
    }

    *this = std::move(remainder);

    return *this;
}


big_int operator""_bi(unsigned long long n)
{
    big_int _new(n);
    return _new;
}

big_int gcd(const big_int &a, const big_int &b)
{
    if (!b) {
        return a > 0_bi ? a : -a;
    } else {
        big_int c = ((a % b) + b) % b;
        return gcd(b, c);
    }
}

big_int &big_int::karatsuba(const big_int &other) &
{
    if (this->_digits.size() < 3 || other._digits.size() < 3)
    {
        return trivial_multiply(other);
    }
    bool act_sign = !(_sign ^ other._sign);

    size_t min_len = (this->_digits.size() <= other._digits.size() ? this->_digits.size() : other._digits.size());
    size_t x_pow = ((min_len + 1)/2 * std::numeric_limits<unsigned int>::digits);

    big_int this_left = (*this >> x_pow);
    big_int other_left = (other >> x_pow);

    big_int this_right = *this - (this_left << x_pow);
    big_int other_right = other - (other_left << x_pow);

    this_left._sign = true;
    other_left._sign = true;
    this_right._sign = true;
    other_right._sign = true;

    big_int first = this_left;
    first.karatsuba(other_left);
    big_int third = this_right;
    third.karatsuba(other_right);

    big_int second = this_left + this_right;
    big_int tmp = other_left + other_right;
    second.karatsuba(tmp);

    first <<= 2 * x_pow;
    second <<= x_pow;

    *this = first + second + third;
    this->_sign = act_sign;
    return *this;
}