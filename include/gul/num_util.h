/**
 * \file    num_util.h
 * \brief   Declarations of numerics utility functions for the General Utility Library.
 * \authors \ref contributors
 * \date    Created on 7 Feb 2019
 *
 * \copyright Copyright 2018 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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
bool withinOrders(const num_t a, const num_t b, const order_t orders) noexcept(false) {
    // std::pow() is not noexcept, which might or might not be true
    auto const r = a > b ? a / b : b / a;
    return r - 1.0 < std::pow(static_cast<std::decay_t<num_t>>(0.1), orders);
}

/**
 * Compare two numbers for almost equality.
 *
 * Checks for two numbers being equal but alowing for an absolute difference.
 * All arguments must be the same floating point type.
 *
 * \param a       The first number to compare
 * \param b       The second number to compare
 * \param diff    The latitude
 *
 * \returns true if a and b are equal-ish
 */
template<typename num_t,
    typename = std::enable_if_t<
        std::is_floating_point<num_t>::value
    >>
bool withinAbs(num_t a, num_t b, num_t diff) noexcept {
    if (a > b)
        return a - diff < b;
    else
        return b - diff < a;
}

} // namespace gul

// vi:ts=4:sw=4:et
