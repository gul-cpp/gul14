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
template<typename num_t, typename order_t,
    typename = std::enable_if_t<
        std::is_floating_point<num_t>::value
        and std::is_arithmetic<order_t>::value
    >>
bool within_orders(const num_t a, const num_t b, const order_t orders) noexcept(false) {
    // std::pow() is not noexcept, which might or might not be true
    auto const r = a > b ? a / b : b / a;
    return std::abs(r - 1.0) < std::pow(static_cast<std::decay_t<num_t>>(0.1), orders);
}

// For within_abs() we need abs() that can take unsigned integers. The standard library
// does not provide it (rather uses either a narrowing conversion to a signed integer or
// declares programs doing it ill-formed.
// With C++17's constexpr if this would be trivial to solve, but here I struggled with
// several easier tries before the following one.
// It may look quite convoluted, but with -O2 it is all optimized away. Unfortunately
// optimizing with -O1 is not sufficient.
//
// Boiler plate rules...
namespace {

template<typename num_t, typename = void>
struct unsigned_save_abs {
    num_t operator()(num_t x) {
        return std::abs(x);
    }
};

template<typename num_t>
struct unsigned_save_abs<num_t, typename std::enable_if_t<
        std::is_unsigned<num_t>::value>> {
    num_t operator()(num_t x){
        return x;
    }
};

} // anonymous namespace

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
template<typename num_t>
bool within_abs(num_t a, num_t b, num_t diff) noexcept {
    diff = unsigned_save_abs<num_t>{}(diff); // Negative diff does not make sense. Use abs() from anon NS.
    if (a > b) {
        if (std::is_floating_point<num_t>::value)
            return a - diff <= b; // different formula needed because of inf/-inf and subnormal values
        else
            return a - b <= diff;
    } else {
        if (std::is_floating_point<num_t>::value)
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
template<typename num_t,
    typename = std::enable_if_t<std::is_floating_point<num_t>::value>>
bool within_ulp(num_t a, num_t b, unsigned int ulp)
{
    auto diff = std::abs(a - b);
    return diff <= std::numeric_limits<num_t>::epsilon() * ulp * std::max(std::abs(a), std::abs(b))
        or diff < std::numeric_limits<num_t>::min(); // or the difference is subnormal
}

} // namespace gul

// vi:ts=4:sw=4:et
