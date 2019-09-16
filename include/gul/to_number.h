/**
 * \file    to_number.h
 * \brief   Declaration of to_number().
 * \authors \ref contributors
 * \date    Created on 19 July 2019
 *
 * \copyright Copyright 2019 Deutsches Elektronen-Synchrotron (DESY), Hamburg
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 2.1 of the license, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstdlib>
#include <cmath>
#include <type_traits>

#include "gul/internal.h"
#include "gul/optional.h"
#include "gul/string_view.h"

namespace gul {

/// \cond HIDE_SYMBOLS
namespace detail {

constexpr inline bool is_digit(char c) noexcept
{
    return c >= '0' && c <= '9';
}

template <typename NumberType>
constexpr inline optional<NumberType> to_unsigned_integer(gul::string_view str) noexcept
{
#ifndef __GNUC__
    constexpr NumberType max_tenth = std::numeric_limits<NumberType>::max() / 10;
#endif

    if (str.empty())
        return nullopt;

    NumberType result = 0;

    for (char c : str)
    {
        if (!is_digit(c))
            return nullopt;

#ifdef __GNUC__
        if (__builtin_mul_overflow(result, NumberType{ 10 }, &result)) // NOLINT(cppcoreguidelines-pro-type-vararg)
            return nullopt;

        if (__builtin_add_overflow(result, static_cast<NumberType>(c - '0'), &result)) // NOLINT(cppcoreguidelines-pro-type-vararg)
            return nullopt;
#else
        if (result > max_tenth)
            return nullopt;

        result *= 10;

        auto last = result;

        result += c - '0';
        if (result < last)
            return nullopt;
#endif
    }

    return result;
}

constexpr optional<int> parse_exponent(string_view str) noexcept
{
    bool negative = false;

    switch (str.front())
    {
    case '+':
        str.remove_prefix(1);
        break;
    case '-':
        str.remove_prefix(1);
        negative = true;
        break;
    default:
        break;
    }

    auto opt_exp = to_unsigned_integer<int>(str);

    if (!opt_exp)
        return nullopt;

    if (negative)
        return -opt_exp.value();
    else
        return opt_exp.value();
}

using FloatConversionIntType = unsigned long long int;

inline constexpr FloatConversionIntType pow10(int exponent) noexcept
{
    FloatConversionIntType constexpr vals[] = {
        1, 10, 100, 1'000, 10'000, 100'000, 1'000'000, 10'000'000, // 0-7
        100'000'000, 1'000'000'000, 10'000'000'000, 100'000'000'000, // 8-11
        1'000'000'000'000, 10'000'000'000'000, 100'000'000'000'000, // 12-14
        1'000'000'000'000'000, 10'000'000'000'000'000, 100'000'000'000'000'000, // 15-17
        1'000'000'000'000'000'000U, 10'000'000'000'000'000'000U // 18-19
    };
    static_assert(std::numeric_limits<FloatConversionIntType>::digits10 == 19,
            "pow10() table does not fit to FloatConversionIntType range");
    if (exponent < 0 or exponent > 19)
        return 0;
    return vals[exponent];
}

template <typename NumberType>
constexpr inline gul::optional<NumberType> to_normalized_float(gul::string_view i1, gul::string_view i2) noexcept
{
    static_assert(std::numeric_limits<FloatConversionIntType>::digits10
            >= std::numeric_limits<NumberType>::max_digits10,
            "FloatConversionIntType is too small for NumberType");

    i1 = i1.substr(0, std::min(i1.length(),
                size_t(std::numeric_limits<FloatConversionIntType>::digits10)));
    i2 = i2.substr(0, std::min(i2.length(),
                size_t(std::numeric_limits<FloatConversionIntType>::digits10) - i1.length()));

    FloatConversionIntType accu{ 0 };

    if (not i1.empty()) {
        auto f1 = to_unsigned_integer<FloatConversionIntType>(i1);
        if (not f1.has_value())
            return nullopt;
        accu += *f1;
    }
    if (not i2.empty()) {
        accu *= pow10(i2.length());
        auto f2 = to_unsigned_integer<FloatConversionIntType>(i2);
        if (not f2.has_value())
            return nullopt;
        accu += *f2;
    }
    return NumberType(accu) / pow10(i1.length() + i2.length() - 1);

}

template <typename NumberType>
constexpr inline optional<NumberType> to_unsigned_float(gul::string_view str) noexcept
{
    if (str.compare("nan") == 0)
        return std::numeric_limits<NumberType>::quiet_NaN();
    if (not is_digit(str[0]) and str[0] != '.')
        return nullopt;
    int exponent = 0;
    auto e_pos = str.find_first_of("eE");
    if (e_pos != gul::string_view::npos)
    {
        if (e_pos + 1 == str.size())
            return nullopt;

        auto str_exponent = str.substr(e_pos + 1);

        str = str.substr(0, e_pos);

        auto opt_exp = detail::parse_exponent(str_exponent);

        if (!opt_exp)
            return nullopt;

        exponent = opt_exp.value();
    }

    gul::string_view str_before_point{ str };
    gul::string_view str_after_point;

    auto point_pos = str.find('.');
    if (point_pos != gul::string_view::npos)
    {
        str_before_point = str.substr(0, point_pos);
        str_after_point = str.substr(point_pos + 1);
    }

    if (str_before_point.empty() && str_after_point.empty())
        return nullopt;

    // Get rid of leading zeros
    while (!str_before_point.empty() and str_before_point[0] == '0')
        str_before_point.remove_prefix(1);

    // Normalize the number
    if (str_before_point.empty()) {
        auto const old_digits = str_after_point.length();
        while (!str_after_point.empty() and str_after_point[0] == '0')
            str_after_point.remove_prefix(1);

        if (str_after_point.empty())
            return { 0 };

        str_before_point = str_after_point.substr(0, 1);
        str_after_point.remove_prefix(1);
        exponent -= static_cast<int>(old_digits - str_after_point.length());
    } else {
        exponent += static_cast<int>(str_before_point.length() - 1);
    }

    // Now the incoming number string is like this:
    // "s.tr_before_point" "str_after_point" E exponent
    //   ^                                           ^
    //   | here is the decimal dot, virtually        | corrected exponent

    using long_double = long double;
    using CalcType = std::conditional_t<
        std::greater<std::size_t>()(sizeof(NumberType), sizeof(double)),
        long_double, double>;

    auto Qval = to_normalized_float<CalcType>(str_before_point, str_after_point);
    if (not Qval.has_value())
        return nullopt;

    NumberType result = std::pow(CalcType(10), CalcType(exponent)) * *Qval;

    // We need to have the result in NumberType before we test for infinity
    if (!std::isfinite(result))
        return nullopt;

    return result;
}

} // namespace detail
/// \endcond


/**
 * Convert an ASCII string_view into a number.
 * This function parses the ASCII representation of a number (e.g. "123" or "1.3e10") into
 * an optional integer or floating-point number.
 *
 * \code
 * gul::optional<int> result = gul::to_number<int>("42");
 *
 * if (result)
 *     std::cout << "The answer is " << result.value() << ".\n";
 * \endcode
 * 
 * <h4>Design Goals</h4>
 * 
 * to_number() shares many characteristics with std::atoi(), std::stod() and the like, but
 * follows its own set of design goals:
 * - Its input type is string_view: No null-termination and no temporary std::string are
 *   required for the conversion.
 * - Conversion errors are reported by returning a gul::optional without a value.
 * - The function does not allocate.
 * - Performance should in most cases be better than and in few cases not much worse than
 *   std::sto[ildf].
 * - to_number<>() can be used in a constexpr context *if* the compiler and standard
 *   library support this. Support for floating-point numbers may lack even if integer
 *   conversion works.
 *
 * <h4>Input Format</h4>
 *
 * The allowed number format depends on the chosen numeric output type. Unsigned integral
 * types accept only digits ("123", "042"=42), signed integral types allow for a leading
 * minus sign as well ("-42"). Floating-point types additionally recognize a decimal point
 * ("1.2", ".5", "12.") and exponential notation using a small or capital "e" ("12e5",
 * "4.2e1", ".2e-4", "2.E5"). The behavior with surrounding whitespace is *undefined*, so
 * it should be removed before passing input to this function. Apart from these
 * considerations, to_number() should accept all input that can be parsed by C++17's
 * from_chars(). It could, in fact, be implemented with it.
 *
 * \param str  The string to be converted into a number.
 * \returns a gul::optional that contains the number if the conversion was successful. If
 *          there was a conversion error, the return value is empty.
 *
 * \note
 * This function has different overloads for unsigned integers, signed integers, and
 * floating-point types.
 *
 * \since GUL version 1.6
 */
// Overload for unsigned integer types.
template <typename NumberType,
          std::enable_if_t<std::is_integral<NumberType>::value &&
                           std::is_unsigned<NumberType>::value, int> = 0>
constexpr inline optional<NumberType> to_number(gul::string_view str) noexcept
{
    return detail::to_unsigned_integer<NumberType>(str);
}

// Overload for signed integer types.
template <typename NumberType,
          std::enable_if_t<std::is_integral<NumberType>::value &&
                           std::is_signed<NumberType>::value, int> = 0>
constexpr inline optional<NumberType> to_number(gul::string_view str) noexcept
{
    if (str.empty())
        return nullopt;

    if (str.front() == '-')
    {
        using UnsignedT = std::make_unsigned_t<NumberType>;
        constexpr auto max_abs_negative_value =
            static_cast<UnsignedT>(std::numeric_limits<NumberType>::max()) + 1;

        str.remove_prefix(1);

        auto result = detail::to_unsigned_integer<UnsignedT>(str);
        if (!result)
            return nullopt;

        if (result.value() == max_abs_negative_value)
            return std::numeric_limits<NumberType>::lowest();
        else if (result.value() > max_abs_negative_value)
            return nullopt;

        return -static_cast<NumberType>(result.value());
    }

    return detail::to_unsigned_integer<NumberType>(str);
}

// Overload for floating-point types float and double.
template <typename NumberType,
    std::enable_if_t<std::is_floating_point<NumberType>::value, long> = 0,
    std::enable_if_t<
        std::greater_equal<std::size_t>()(
            std::numeric_limits<detail::FloatConversionIntType>::digits10,
            std::numeric_limits<NumberType>::max_digits10),
        int> = 0>
constexpr inline optional<NumberType> to_number(gul::string_view str) noexcept
{
    if (str.empty())
        return nullopt;

    if (str.front() == '-')
    {
        str.remove_prefix(1);
        auto result = detail::to_unsigned_float<NumberType>(str);
        if (!result)
            return nullopt;
        return -result.value();
    }

    return detail::to_unsigned_float<NumberType>(str);
}

// Overload for extra resolution floating-point types (long double).
template <typename NumberType,
    std::enable_if_t<std::is_floating_point<NumberType>::value, long> = 0,
    std::enable_if_t<
        std::less<std::size_t>()(
            std::numeric_limits<detail::FloatConversionIntType>::digits10,
            std::numeric_limits<NumberType>::max_digits10),
        int> = 0>
inline optional<NumberType> to_number(gul::string_view str)
{
    if (str.empty())
        return nullopt;

    auto input = std::string{ str };
    char* process_end;
    NumberType Qval;
    // Will be optimized away:
    if (sizeof(NumberType) == sizeof(long double))
        Qval = std::strtold(input.c_str(), &process_end);
    else if (sizeof(NumberType) == sizeof(double))
        Qval = std::strtod(input.c_str(), &process_end);
    else if (sizeof(NumberType) == sizeof(float))
        Qval = std::strtof(input.c_str(), &process_end);
    else return nullopt;

    if (input.data() + input.size() != process_end)
        return nullopt;
    if (!std::isfinite(Qval))
        return nullopt;
    return Qval;
}

} /* namespace gul */

// vi:ts=4:sw=4:et:sts=4
