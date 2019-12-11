/**
 * \file    bit_manip.h
 * \brief   Declaration of bit manipulation utility functions for the General
 *          Utility Library.
 * \authors \ref contributors
 * \date    Created on 17 October 2019
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

#include <cassert>
#include <type_traits>

#include "gul14/internal.h"

namespace gul {

/**
 * Return type of the bit manipulation functions.
 *
 * We want to SFINAE out of all types except integers.
 * Because bools counts as integer type and we don't want that we
 * specifically exclude it here.
 *
 * The type is then simply the decayed input type, to get rid of
 * const or volatile specifiers.
 *
 * \tparam T Type specified/deduced by bit_*() user
 */
template <typename T>
using BitFunctionReturnType =
    std::enable_if_t<
            std::is_integral<T>::value
            and not std::is_same<std::decay_t<T>, bool>::value,
        std::decay_t<T>
    >;

/**
 * Set a bit in an integral type.
 *
 * Return an integral value of type \b T where the bit number \b bit
 * and only that bit is set.
 *
 * When \b bit is greater or equal to the number of bits in type \b T,
 * std::abort() is called (via assert()).
 *
 * \param bit      Number of the bit that is to be set (LSB == 0)
 *
 * \returns        Integral value with the bit set.
 *
 * \tparam T       Type of the bit-holding integral value.
 *
 * \note If no type \b T is specified an unsigned int will be returned.
 *
 * \since GUL version 1.8
 *
 * \see bit_set(previous, bit), bit_reset(previous, bit),
 *      bit_flip(previous, bit), bit_test(bits, bit)
 */
template <typename T = unsigned, typename ReturnT = BitFunctionReturnType<T>>
auto constexpr inline bit_set(unsigned bit) noexcept -> ReturnT {
    assert(bit < sizeof(T) * 8);
    return (std::make_unsigned_t<T>{1} << bit);
}

/**
 * Set a bit in an integral value.
 *
 * Set the bit number \b bit in the existing value \b previous.
 *
 * When \b bit is greater or equal to the number of bits in type \b T,
 * std::abort() is called (via assert()).
 *
 * \param previous Existing integral value where the bit shall be modified
 * \param bit      Number of the bit that is to be modified (LSB == 0)
 *
 * \returns        New integral value with the bit modified.
 *
 * \tparam T       Type of the bit-holding integral value.
 *
 * \since GUL version 1.8
 *
 * \see bit_set(bit), bit_reset(previous, bit),
 *      bit_flip(previous, bit), bit_test(bits, bit)
 */
template <typename T, typename ReturnT = BitFunctionReturnType<T>>
auto constexpr inline bit_set(T previous, unsigned bit) noexcept -> ReturnT {
    return previous | bit_set<T>(bit);
}

/**
 * Reset a bit in an integral value.
 *
 * Reset (clear) the bit number \b bit in the existing value \b previous.
 *
 * When \b bit is greater or equal to the number of bits in type \b T,
 * std::abort() is called (via assert()).
 *
 * \param previous Existing integral value where the bit shall be modified
 * \param bit      Number of the bit that is to be modified (LSB == 0)
 *
 * \returns        New integral value with the bit modified.
 *
 * \tparam T       Type of the bit-holding integral value.
 *
 * \since GUL version 1.8
 *
 * \see bit_set(bit), bit_set(previous, bit),
 *      bit_flip(previous, bit), bit_test(bits, bit)
 */
template <typename T, typename ReturnT = BitFunctionReturnType<T>>
auto constexpr inline bit_reset(T previous, unsigned bit) noexcept -> ReturnT {
    return previous & ~bit_set<T>(bit);
}

/**
 * Flip a bit in an integral value.
 *
 * Flips the bit number \b bit in the existing value \b previous.
 * This inverts the state of the bit: setting the bit if it was
 * previously not set and resetting the bit if it was previously set.
 *
 * When \b bit is greater or equal to the number of bits in type \b T,
 * std::abort() is called (via assert()).
 *
 * \param previous Existing integral value where the bit shall be modified
 * \param bit      Number of the bit that is to be modified (LSB == 0)
 *
 * \returns        New integral value with the bit modified.
 *
 * \tparam T       Type of the bit-holding integral value.
 *
 * \since GUL version 1.8
 *
 * \see bit_set(bit), bit_set(previous, bit),
 *      bit_reset(previous, bit), bit_test(bits, bit)
 */
template <typename T, typename ReturnT = BitFunctionReturnType<T>>
auto constexpr inline bit_flip(T previous, unsigned bit) noexcept -> ReturnT {
    return previous ^ bit_set<T>(bit);
}

/**
 * Test a bit in an integral value.
 *
 * Test the bit number \b bit in the existing value \b bits.
 *
 * When \b bit is greater or equal to the number of bits in type \b T,
 * std::abort() is called (via assert()).
 *
 * \param bits     Integral value where the bit shall be tested
 * \param bit      Number of the bit that is to be modified (LSB == 0)
 *
 * \returns        True or false
 *
 * \tparam T       Type of the bit-holding integral value.
 *
 * \since GUL version 1.8
 *
 * \see bit_set(bit), bit_set(previous, bit),
 *      bit_reset(previous, bit), bit_flip(previous, bit)
 */
template <typename T>
bool constexpr inline bit_test(T bits, unsigned bit) noexcept {
    return bits & bit_set<T>(bit);
}

} // namespace gul

// vi:ts=4:sw=4:et
