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

#include "gul/string_view.h"
#include "gul/internal.h"

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
        if (__builtin_mul_overflow(result, NumberType{ 10 }, &result))
            return nullopt;

        if (__builtin_add_overflow(result, static_cast<NumberType>(c - '0'), &result))
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

template <typename NumberType>
constexpr inline optional<NumberType> to_unsigned_float(gul::string_view str) noexcept
{
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

    NumberType digit_value{ 1 };
    NumberType result{ 0 };

    if (str_before_point.empty())
    {
        digit_value = std::pow(NumberType(10), exponent - 1);
    }
    else
    {
        // Try optimized integer conversion if the number fits into an unsigned long long.
        if (str_before_point.size() <= std::numeric_limits<unsigned long long>::digits10)
        {
            digit_value = std::pow(NumberType(10), exponent);

            auto opt = to_unsigned_integer<unsigned long long>(str_before_point);
            if (!opt)
                return nullopt;
            result = opt.value() * digit_value;

            digit_value *= NumberType(0.1);
        }
        else
        {
            digit_value = std::pow(NumberType(10), exponent + str_before_point.size() - 1);

            for (char c : str_before_point)
            {
                if (!is_digit(c))
                    return nullopt;

                result += (c - '0') * digit_value;
                digit_value *= NumberType(0.1);
            }
        }
    }

    if (!str_after_point.empty())
    {
        // Try optimized integer conversion if the number fits into an unsigned long long.
        if (str_after_point.size() <= std::numeric_limits<unsigned long long>::digits10)
        {
            auto opt = to_unsigned_integer<unsigned long long>(str_after_point);
            if (!opt)
                return nullopt;

            result += opt.value() * std::pow(NumberType(10),
                                             exponent - static_cast<int>(str_after_point.size()));
        }
        else
        {
            for (char c : str_after_point)
            {
                if (!is_digit(c))
                    return nullopt;

                result += (c - '0') * digit_value;
                digit_value *= NumberType(0.1);
            }
        }
    }

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

// Overload for floating-point types.
template <typename NumberType,
    std::enable_if_t<std::is_floating_point<NumberType>::value, int> = 0>
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

} /* namespace gul */

/* vim:set noexpandtab softtabstop=4 tabstop=4 shiftwidth=4 textwidth=90 cindent: */
