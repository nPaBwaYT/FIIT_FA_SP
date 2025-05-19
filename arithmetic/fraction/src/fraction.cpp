#include "../include/fraction.h"
#include <cmath>
#include <numeric>
#include <sstream>
#include <regex>

void fraction::optimise()
{
    if (_denominator < 0_bi) {
        _numerator = -_numerator;
        _numerator.optimize();
        _denominator = -_denominator;
    }

    big_int _gcd = gcd(_numerator, _denominator);
    if (_gcd > 1_bi) {
        _numerator /= _gcd;
        _denominator /= _gcd;
    }
}


fraction::fraction(const pp_allocator<big_int::value_type> allocator)
        : _numerator(0, allocator), _denominator(1, allocator) {
}

fraction &fraction::operator+=(fraction const &other) & {
    _numerator = _numerator * other._denominator + _denominator * other._numerator;
    _denominator = _denominator * other._denominator;
    optimise();
    return *this;
}

fraction fraction::operator+(fraction const &other) const {
    fraction result = *this;
    result += other;
    return result;
}

fraction &fraction::operator-=(fraction const &other) & {
    _numerator = _numerator * other._denominator - _denominator * other._numerator;
    _denominator = _denominator * other._denominator;
    optimise();
    return *this;
}

fraction fraction::operator-(fraction const &other) const {
    fraction result = *this;
    result -= other;
    return result;
}

fraction fraction::operator-() const {
    fraction _new(*this);
    _new._numerator = -_new._numerator;
    _new.optimise();
    return _new;
}

fraction &fraction::operator*=(fraction const &other) & {
    _numerator *= other._numerator;
    _denominator *= other._denominator;
    optimise();
    return *this;
}

fraction fraction::operator*(fraction const &other) const {
    fraction result = *this;
    result *= other;
    return result;
}

fraction &fraction::operator/=(fraction const &other) & {
    if (other._numerator == 0) {
        throw std::invalid_argument("Division by zero");
    }
    _numerator *= other._denominator;
    _denominator *= other._numerator;
    optimise();
    return *this;
}

fraction fraction::operator/(fraction const &other) const {
    fraction result = *this;
    result /= other;
    return result;
}

bool fraction::operator==(fraction const &other) const noexcept {
    return _numerator == other._numerator && _denominator == other._denominator;
}

std::partial_ordering fraction::operator<=>(const fraction& other) const noexcept {
    big_int lhs = _numerator * other._denominator;
    big_int rhs = _denominator * other._numerator;
    if (lhs < rhs) return std::partial_ordering::less;
    if (lhs > rhs) return std::partial_ordering::greater;
    return std::partial_ordering::equivalent;
}

std::ostream &operator<<(std::ostream &stream, fraction const &obj) {
    stream << obj.to_string();
    return stream;
}

std::istream &operator>>(std::istream &stream, fraction &obj) {
    std::string input;
    stream >> input;
    std::regex fraction_regex(R"(^([-+]?[0-9]+)(?:/([-+]?[0-9]+))?$)");
    std::smatch match;
    if (std::regex_match(input, match, fraction_regex)) {
        big_int numerator(match[1].str(), 10);
        big_int denominator(1);
        if (match[2].matched) {
            denominator = big_int(match[2].str(), 10);
        }
        obj = fraction(numerator, denominator);
    } else {
        throw std::invalid_argument("Invalid fraction format");
    }
    return stream;
}

std::string fraction::to_string() const {
    std::stringstream ss;
    ss << _numerator << "/" << _denominator;
    return ss.str();
}

fraction fraction::sin(fraction const &epsilon) const
{
    fraction x = *this;
    fraction x_squared = x * x;
    fraction result = x;
    fraction term = x;
    big_int divisor = 1;
    fraction numerator = x;

    for (int n = 1; ; ++n) {
        divisor *= (2*n) * (2*n + 1);
        numerator *= x_squared;
        term = numerator / fraction(divisor, 1);

        if (term.abs() <= epsilon) break;

        result += (n % 2 == 1) ? -term : term;
    }

    return result;
}

fraction fraction::cos(fraction const &epsilon) const
{
    fraction x = *this;
    fraction x_squared = x * x;
    fraction result(1, 1);
    fraction term(1, 1);
    big_int divisor = 1;
    fraction num(1, 1);

    for (int n = 1; ; ++n) {
        divisor *= (2*n - 1) * (2*n);
        num *= x_squared;
        term = num / fraction(divisor, 1);

        if (term.abs() <= epsilon) break;

        result += (n % 2 == 1) ? -term : term;
    }

    return result;

}

fraction fraction::tg(fraction const &epsilon) const {
    fraction cosine = this->cos(epsilon);
    if (cosine._numerator == 0) {
        throw std::domain_error("Tangent undefined");
    }
    return this->sin(epsilon) / cosine;
}

fraction fraction::ctg(fraction const &epsilon) const {
    fraction sine = this->sin(epsilon);
    if (sine._numerator == 0) {
        throw std::domain_error("Cotangent undefined");
    }
    return this->cos(epsilon) / sine;
}

fraction fraction::sec(fraction const &epsilon) const {
    fraction cosine = this->cos(epsilon);
    if (cosine._numerator == 0) {
        throw std::domain_error("Secant undefined");
    }
    return fraction(1, 1) / cosine;
}

fraction fraction::cosec(fraction const &epsilon) const {
    fraction sine = this->sin(epsilon);
    if (sine._numerator == 0) {
        throw std::domain_error("Cosecant undefined");
    }
    return fraction(1, 1) / sine;
}

fraction fraction::arctg(fraction const &epsilon) const {
    if (this->abs() > fraction(1, 1)) {

        if (*this > fraction(0, 1)) {
            return fraction(2,1) * calculate_half_pi(epsilon/fraction(2,1)) / fraction(2, 1) - (fraction(1, 1) / *this).arctg(epsilon);
        } else {
            return -fraction(2,1) * calculate_half_pi(epsilon/fraction(2,1)) / fraction(2, 1) - (fraction(1, 1) / *this).arctg(epsilon);
        }
    }

    fraction x = *this;
    fraction x_power = x;
    fraction result = x;
    int sign = -1;

    for (int n = 1; ; ++n) {
        x_power *= x * x;
        fraction term = x_power / fraction(2 * n + 1, 1);
        if (sign > 0) {
            result += term;
        } else {
            result -= term;
        }

        if (term.abs() <= epsilon) {
            break;
        }

        sign *= -1;
    }

    return result;
}

fraction fraction::pow(size_t degree) const {
    if (degree == 0) {
        return fraction(1, 1);
    }
    fraction base = *this;
    fraction result(1, 1);
    while (degree > 0) {
        if (degree & 1) {
            result *= base;
        }
        base *= base;
        degree >>= 1;
    }
    return result;
}

fraction fraction::root(size_t degree, fraction const &epsilon) const {
    if (degree == 0) {
        throw std::invalid_argument("Degree cannot be zero");
    }
    if (degree == 1) {
        return *this;
    }
    if (_numerator < 0 && degree % 2 == 0) {
        throw std::domain_error("Even root of negative number is not real");
    }
    fraction x = *this;
    if (x._numerator < 0) x = -x;
    fraction guess = *this / fraction(degree, 1);
    fraction prev_guess;
    do {
        prev_guess = guess;
        fraction power = guess.pow(degree - 1);
        if (power._numerator == 0) {
            throw std::runtime_error("Division by zero in root calculation");
        }
        guess = (fraction(degree - 1, 1) * guess + *this / power) / fraction(degree, 1);
    } while ((guess - prev_guess > epsilon) || (prev_guess - guess > epsilon));
    if (_numerator < 0 && degree % 2 == 1) {
        guess = -guess;
    }
    return guess;
}

fraction fraction::log2(fraction const &epsilon) const
{
    if (_numerator <= 0 || _denominator <= 0) {
        throw std::domain_error("Logarithm of non-positive number is undefined");
    }
    fraction ln2 = fraction(2, 1).ln(epsilon);
    return this->ln(epsilon) / ln2;
}

fraction fraction::ln_normalized(fraction const &x, fraction const &epsilon)
{

    if (x == fraction(1, 1)) {
        return fraction(0, 1);
    }



    fraction one(1, 1);
    fraction y = (x - one) / (x + one);
    fraction y_power = y;
    fraction result = y;
    fraction term = y;

    for (int n = 3; ; n += 2) {
        y_power *= y * y;
        term = y_power / fraction(n, 1);
        result += term;

        if (term.abs() <= epsilon) {
            break;
        }
    }

    return result * fraction(2, 1);
}

fraction fraction::ln(fraction const& epsilon) const {
    if (_numerator <= 0 || _denominator <= 0) {
        throw std::domain_error("Natural logarithm of non-positive number is undefined");
    }

    fraction x = *this;
    fraction result(0, 1);
    fraction two(2, 1);
    fraction one(1, 1);

    int k = 0;


    while (x >= two) {
        x /= two;
        ++k;
    }

    while (x < fraction(1, 2)) {
        x *= two;
        --k;
    }


    return ln_normalized(x, epsilon) + fraction(k, 1) * ln_normalized(two, epsilon);
}

fraction fraction::lg(fraction const &epsilon) const
{
    if (_numerator <= 0 || _denominator <= 0) {
        throw std::domain_error("Base-10 logarithm of non-positive number is undefined");
    }
    fraction ln10 = fraction(10, 1).ln(epsilon);
    return this->ln(epsilon) / ln10;
}


fraction fraction::arcsin(fraction const &epsilon) const {
    if (*this < fraction(-1, 1) || *this > fraction(1, 1)) {
        throw std::domain_error("arcsin is only defined for values in [-1, 1]");
    }

    if (*this == fraction(1, 1)) return calculate_half_pi(epsilon);
    if (*this == fraction(-1, 1)) return -calculate_half_pi(epsilon);
    if (*this == fraction(0, 1)) return fraction(0, 1);

    fraction x = *this;
    fraction x_power = x;
    fraction result = x;
    fraction term = x;

    big_int n = 1;
    big_int factorial_2n = 1;
    big_int factorial_n = 1;
    big_int four_pow_n = 1;

    for (int i = 1; ; ++i) {

        factorial_2n *= big_int(2 * i - 1);
        factorial_2n *= big_int(2 * i);
        factorial_n *= big_int(i);
        four_pow_n *= big_int(4);

        x_power *= x * x;


        big_int numerator = factorial_2n * x_power._numerator;

        big_int denominator = four_pow_n * factorial_n * factorial_n * big_int(2 * i + 1) * x_power._denominator;

        term = fraction(numerator, denominator);

        result += term;

        if (term.abs() <= epsilon) {
            break;
        }
    }

    return result;
}

fraction fraction::arccos(fraction const &epsilon) const {

    fraction abs_val = this->abs();
    if (abs_val > fraction(1, 1)) {
        throw std::domain_error("arccos определен только для значений в диапазоне [-1, 1]");
    }


    if (*this == fraction(1, 1)) return fraction(0, 1);
    if (*this == fraction(-1, 1)) return fraction(2,1) * calculate_half_pi(epsilon);
    if (*this == fraction(0, 1)) return calculate_half_pi(epsilon);


    fraction arcsin_val = this->arcsin(epsilon);

    return calculate_half_pi(epsilon) - arcsin_val;

}

fraction fraction::arcctg(fraction const &epsilon) const {

    fraction arctg_val = this->arctg(epsilon);

    return calculate_half_pi(epsilon) - arctg_val;
}

fraction fraction::arcsec(fraction const &epsilon) const {

    fraction abs_val = this->abs();
    if (abs_val < fraction(1, 1)) {
        throw std::domain_error("arcsec defined |x| >= 1");
    }


    fraction reciprocal = fraction(1, 1) / *this;
    return reciprocal.arccos(epsilon);
}

fraction fraction::arccosec(fraction const &epsilon) const {

    fraction abs_val = this->abs();
    if (abs_val < fraction(1, 1)) {
        throw std::domain_error("arccosec defined |x| >= 1");
    }


    fraction reciprocal = fraction(1, 1) / *this;
    return reciprocal.arcsin(epsilon);
}

fraction fraction::abs() const {
    big_int abs_numerator = _numerator >= 0_bi ? _numerator : -_numerator;
    big_int abs_denominator = _denominator >= 0_bi ? _denominator : -_denominator;
    fraction abs_frac(abs_numerator, abs_denominator);
    return abs_frac;
}

fraction fraction::calculate_half_pi(const fraction &epsilon) {


















    long double pi_2 = M_PI_2;
    big_int tmp = epsilon._denominator;

    fraction PI_2(1,epsilon._denominator * 10_bi);
    while (tmp) {
        pi_2 *= 10;
        while (pi_2 > 10.0) {
            pi_2 -= 10;
        }
        PI_2._numerator *= 10_bi;
        PI_2._numerator += big_int(static_cast<unsigned long long>(pi_2));
        tmp /= 10_bi;
    }
    return PI_2;
}
