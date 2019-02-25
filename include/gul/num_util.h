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
 * Compute the absolute value of a number.
 *
 * This function is almost equal to std::abs() with the exception of unsigned integral
 * types, which are returned unchanged and in their original type. This is especially
 * useful in templates, where std::abs() cannot be used for all arithmetic types.
 *
 * \param n  The number whose absolute value should be determined.
 *
 * \returns the absolute value of n.
 */
template<typename ValueT>
constexpr auto abs(ValueT n) noexcept -> std::enable_if_t<std::is_unsigned<ValueT>::value, ValueT>
{
    return n;
}

/**
 * \overload
 */
template<typename ValueT>
constexpr auto abs(ValueT n) noexcept -> std::enable_if_t<not std::is_unsigned<ValueT>::value, ValueT>
{
    return std::abs(n);
}

/**
 * Determine if two numbers are almost equal by comparing their ratio to 1.
 *
 * This function calculates the ratio of the bigger number to the smaller number and
 * returns true if this ratio deviates from 1.0 by less than 0.1^orders.
 *
 *     within_orders(10, 12, 1):    false (ratio = 1.2   > 1 + 0.1^1)
 *     within_orders(10, 10.9, 1):  true  (ratio = 1.09  < 1 + 0.1^1)
 *     within_orders(10, 10.9, 2):  false (ratio = 1.09  > 1 + 0.1^2)
 *     within_orders(10, 10.09, 2): true  (ratio = 1.009 < 1 + 0.1^2)
 *
 * Because this function needs to calculate the ratio between the two numbers, it does
 * not work if one of them is zero. Unexpected behavior can also result when orders is
 * low (< 3) as the simple concept of orders equals digits does not hold so strict
 * anymore.
 *
 * \param a       The first number to compare (any floating point type)
 * \param b       The second number to compare (any floating point type, same type as a)
 * \param orders  The number of orders of magnitude to take for comparison (any numeric type)
 *
 * \returns true if the ratio of a and b is close enough to 1.
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
 * Determine if two numbers are almost equal, allowing for an absolute difference.
 *
 * All arguments must be the same numeric type (floating point or integer).
 *
 * \param a     The first number to compare
 * \param b     The second number to compare
 * \param tol   The absolute tolerance
 *
 * \returns true if the absolute difference between a and b is smaller than tol.
 */
template<typename NumT>
bool within_abs(NumT a, NumT b, NumT tol) noexcept {
    tol = gul::abs(tol); // Negative diff does not make sense
    if (a > b) {
        if (std::is_floating_point<NumT>::value)
            return a - tol <= b; // different formula needed because of inf/-inf and subnormal values
        else
            return a - b <= tol;
    } else {
        if (std::is_floating_point<NumT>::value)
            return b - tol <= a;
        else
            return b - a <= tol;
    }
}

/**
 * Determine if two numbers are almost equal, allowing for a difference of a given number
 * of units-in-the-last-place (ULPs).
 *
 * One ULP is the spacing between two consecutive floating point representations.
 * There are no possible values in between. Roughly speaking, one ULP is for floating
 * point numbers what the 1 is for integral numbers.
 *
 * All arguments must be of the same floating point type.
 *
 * \param a       The first number to compare
 * \param b       The second number to compare
 * \param ulp     Allowed number of floating point steps in between
 *
 * \returns true if a and b are equal within the given number of ULPs.
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
