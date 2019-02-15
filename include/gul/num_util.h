/**
 * \file    num_util.h
 * \brief   Declarations of numerics utility functions for the General Utility Library.
 * \authors \ref contributors
 * \date    Created on 7 Feb 2019
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

#include <type_traits>

namespace gul {

/**
 * Compute the absolute value of a number
 *
 * This is almost equal to std::abs() with the exception of unsigned integral types. In
 * contrast to the standard library unsigned integrals are returned unchanged, in their
 * original type.
 *
 * For work with templates with unknown arithmetical type one still needs to get the
 * absolute value of some parameters. This is impossible with the std::abs() specification.
 *
 * abs(n) = |  |n|,  when n is represented in signed type
 *          |  n,    when n is represented in unsigned type
 *
 * \param n
 *
 * \returns the absolute value (i.e. n, because n is unsigned anyhow)
 */
template<typename ValueT>
constexpr auto abs(ValueT n) noexcept -> std::enable_if_t<std::is_unsigned<ValueT>::value, ValueT>
{
    return n;
}

/**
 * \overload
 *
 * \param n
 *
 * \returns the absolute value (i.e. |n|) if it is representable (see std::abs())
 */
template<typename ValueT>
constexpr auto abs(ValueT n) noexcept -> std::enable_if_t<not std::is_unsigned<ValueT>::value, ValueT>
{
    return std::abs(n);
}

/**
 * Compare two numbers for almost equality.
 *
 * Checks for two numbers being equal but taking into account only a limited number
 * of significant digits.
 *
 * Unexpected behavior can result when orders is low (< 3) as the simple concept of
 * orders equals digits does not hold so strict anymore.
 *
 * \param a       The first number to compare (any floating point type)
 * \param b       The second number to compare (any floating point type, same type as a)
 * \param orders  The number of orders of magnitude to take for comparison (any numeric type)
 *
 * \returns true if a and b are equal-ish
 */
template<typename NumT, typename OrderT,
    typename = std::enable_if_t<
        std::is_floating_point<NumT>::value
        and std::is_arithmetic<OrderT>::value
    >>
bool within_orders(const NumT a, const NumT b, const OrderT orders) noexcept(false) {
    // std::pow() is not noexcept, which might or might not be true
    auto const r = a > b ? a / b : b / a;
    return std::abs(r - 1.0) < std::pow(static_cast<std::decay_t<NumT>>(0.1), orders);
}

/**
 * Compare two numbers for almost equality.
 *
 * Checks for two numbers being equal but alowing for an absolute difference.
 * All arguments must be the same numeric type (floating point or integer).
 *
 * \param a       The first number to compare
 * \param b       The second number to compare
 * \param diff    The latitude
 *
 * \returns true if a and b are equal-ish
 */
template<typename NumT>
bool within_abs(NumT a, NumT b, NumT diff) noexcept {
    diff = gul::abs(diff); // Negative diff does not make sense
    if (a > b) {
        if (std::is_floating_point<NumT>::value)
            return a - diff <= b; // different formula needed because of inf/-inf and subnormal values
        else
            return a - b <= diff;
    } else {
        if (std::is_floating_point<NumT>::value)
            return b - diff <= a;
        else
            return b - a <= diff;
    }
}

/**
 * Compare two numbers for almost equality.
 *
 * Checks for two numbers being equal thereby alowing them to be ulp
 * 'floating point representations' apart.
 *
 * One ULP is the spacing between two consecutive floating point representations.
 * There are no possible values in between. One ULP is to floating point numbers
 * something like LSB for integral numbers.
 *
 * All arguments must be the same floating point type.
 *
 * \param a       The first number to compare
 * \param b       The second number to compare
 * \param ulp     Allowed number of floating point steps in between
 *
 * \returns true if a and b are equal-ish
 */
template<typename NumT,
    typename = std::enable_if_t<std::is_floating_point<NumT>::value>>
bool within_ulp(NumT a, NumT b, unsigned int ulp)
{
    auto diff = std::abs(a - b);
    return diff <= std::numeric_limits<NumT>::epsilon() * ulp * std::max(std::abs(a), std::abs(b))
        or diff < std::numeric_limits<NumT>::min(); // or the difference is subnormal
}

} // namespace gul

// vi:ts=4:sw=4:et
