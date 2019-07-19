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

namespace gul {

namespace detail {

constexpr inline bool is_digit(char c) noexcept
{
    return c >= '0' && c <= '9';
}

template <typename NumberType>
constexpr inline NumberType to_unsigned_integer(gul::string_view str)
{
#ifndef __GNUC__
    constexpr NumberType max_tenth = std::numeric_limits<NumberType>::max() / 10;
#endif

    if (str.empty())
        throw std::invalid_argument("Cannot convert empty string to number");

    NumberType result = 0;

    for (char c : str)
    {
        if (!is_digit(c))
            throw std::invalid_argument("Invalid character in number");

#ifdef __GNUC__
        if (__builtin_mul_overflow(result, NumberType{ 10 }, &result))
            throw std::out_of_range("Exceeds the range of the output type");

        if (__builtin_add_overflow(result, static_cast<NumberType>(c - '0'), &result))
            throw std::out_of_range("Exceeds the range of the output type");
#else
        if (result > max_tenth)
            throw std::out_of_range("Exceeds the range of the output type");

        result *= 10;

        auto last = result;

        result += c - '0';
        if (result < last)
            throw std::out_of_range("Exceeds the range of the output type");
#endif
    }

    return result;
}

template <typename NumberType>
constexpr inline NumberType to_unsigned_float(gul::string_view str)
{
    gul::string_view str_exponent;

    auto e_pos = str.find_first_of("eE");
    if (e_pos != str.npos)
    {
        if (e_pos + 1 == str.size())
            throw std::invalid_argument("Invalid exponent");

        str_exponent = str.substr(e_pos + 1);
        str = str.substr(0, e_pos);
    }

    gul::string_view str_before_point{ str };
    gul::string_view str_after_point;

    auto point_pos = str.find('.');
    if (point_pos != str.npos)
    {
        str_before_point = str.substr(0, point_pos);
        str_after_point = str.substr(point_pos + 1);
    }

    if (str_before_point.empty() && str_after_point.empty())
        throw std::invalid_argument("Invalid number");

    NumberType result{ 0 };

    if (!str_before_point.empty())
    {
        result = 
            static_cast<NumberType>( to_unsigned_integer<unsigned long long>(str_before_point) );
    }

    if (!str_after_point.empty())
    {
        result +=
            static_cast<NumberType>( to_unsigned_integer<unsigned long long>(str_after_point) )
            *
            std::pow(static_cast<NumberType>(0.1), static_cast<int>(str_after_point.size()));
    }

    if (!str_exponent.empty())
    {
        int exponent = 0;

        switch (str_exponent.front())
        {
        case '+':
            str_exponent.remove_prefix(1);
            exponent = to_unsigned_integer<int>(str_exponent);
            break;
        case '-':
            str_exponent.remove_prefix(1);
            exponent = -to_unsigned_integer<int>(str_exponent);
            break;
        default:
            exponent = to_unsigned_integer<int>(str_exponent);
            break;
        }

        result *= std::pow(static_cast<NumberType>(10), exponent);
    }

    return result;
}

} // namespace detail


/**
 * Convert an ASCII string_view into a number.
 * This function parses the ASCII representation of a number (e.g. "123" or
 * "1.3e10") into an integer or floating-point number. It therefore shares
 * many characteristics with std::atoi(), std::stod() and the like, but has
 * follows its own set of design goals:
 * - Its input type is string_view: No null-termination and no temporary
 *   std::string are required for the conversion.
 * - Errors are reported by throwing std::invalid_argument or
     std::out_of_range.
 * - The function does not allocate.
 * - Performance should in most cases be better than and in few cases not
 *   much worse than std::sto[ildf].
 * - to_number<>() can be used in a constexpr context *if* the compiler and
 *   standard library support this. Support for floating-point numbers may
 *   lack even if integer conversion works.
 * 
 * <h4>Input Format</h4>
 *
 * The allowed number format depends on the chosen numeric output type.
 * Unsigned integral types accept only digits ("123", "042"=42), signed
 * integral types allow for a leading minus sign as well ("-42").
 * Floating-point types additionally recognize a decimal point ("1.2",
 * ".5", "12.") and exponential notation using a small or capital "e"
 * ("12e5", "4.2e1", ".2e-4", "2.E5").
 * Surrounding whitespace is *not* ignored, but treated like an invalid
 * character. Aside from these considerations, to_number() should accept
 * all input that can be parsed by C++17's from_chars(). It could, in fact,
 * be implemented in terms of it.
 */
template <typename NumberType,
          std::enable_if_t<std::is_integral<NumberType>::value &&
                           std::is_unsigned<NumberType>::value, int> = 0>
constexpr inline NumberType to_number(gul::string_view str)
{
    return detail::to_unsigned_integer<NumberType>(str);
}

/**
 * \overload
 * This is the overload for signed integer types.
 */
template <typename NumberType,
          std::enable_if_t<std::is_integral<NumberType>::value &&
                           std::is_signed<NumberType>::value, int> = 0>
constexpr inline NumberType to_number(gul::string_view str)
{
    if (str.empty())
        throw std::invalid_argument("Cannot convert empty string to number");

    if (str.front() == '-')
    {
        using UnsignedT = std::make_unsigned_t<NumberType>;
        constexpr auto max_abs_negative_value =
            static_cast<UnsignedT>(std::numeric_limits<NumberType>::max()) + 1;

        str.remove_prefix(1);
        auto result = detail::to_unsigned_integer<UnsignedT>(str);
        if (result == max_abs_negative_value)
            return std::numeric_limits<NumberType>::lowest();
        else if (result > max_abs_negative_value)
            throw std::out_of_range("Exceeds the range of the output type");

        return -static_cast<NumberType>(result);
    }

    return detail::to_unsigned_integer<NumberType>(str);
}

/**
 * \overload
 * This is the overload for floating-point types.
 */
template <typename NumberType,
    std::enable_if_t<std::is_floating_point<NumberType>::value, int> = 0>
constexpr inline NumberType to_number(gul::string_view str)
{
    if (str.empty())
        throw std::invalid_argument("Cannot convert empty string to number");

    if (str.front() == '-')
    {
        str.remove_prefix(1);
        return -detail::to_unsigned_float<NumberType>(str);
    }
    
    return detail::to_unsigned_float<NumberType>(str);
}

} /* namespace gul */

/* vim:set noexpandtab softtabstop=4 tabstop=4 shiftwidth=4 textwidth=90 cindent: */
