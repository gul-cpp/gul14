/**
 * \file    to_number.cc
 * \brief   Implementation of pow10().
 * \authors \ref contributors
 * \date    Created on 19 Februar 2024
 *
 * \copyright Copyright 2024 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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

#include <cfloat>

#include "gul14/to_number.h"

namespace gul14 {

namespace detail {

long double pow10(int exponent)
{
    // The preprocessor macros below just allow long double with
    //
    // - exponent maximum 511 (typical is 308) which is usually
    //   the case when long double == double (the code needs at least
    //   an allowed maximum of 256 for the constants)
    //
    // OR
    //
    // - exponent maximum 8191 (typical 4932) which is usually
    //   the case for 80 bit 'extended precision' or 'float128'
    //   long doubles (the code needs at least an allowed maximum
    //   of 4096 for the constants)

    static_assert(LDBL_MAX_10_EXP >= 4096 or LDBL_MAX_10_EXP < 512,
        "Unsupported long double type");
    static_assert(LDBL_MIN_10_EXP <= -4096 or LDBL_MIN_10_EXP > -512,
        "Unsupported long double type");

    constexpr const long double direct_powers_pos[] = {
        1E0L, 1E1L, 1E2L, 1E3L, 1E4L, 1E5L, 1E6L, 1E7L,
        1E8L, 1E9L, 1E10L, 1E11L, 1E12L, 1E13L, 1E14L, 1E15L
    };
    constexpr const long double direct_powers_neg[] = {
        1E0L, 1E-1L, 1E-2L, 1E-3L, 1E-4L, 1E-5L, 1E-6L, 1E-7L,
        1E-8L, 1E-9L, 1E-10L, 1E-11L, 1E-12L, 1E-13L, 1E-14L, 1E-15L
    };
    constexpr const long double binary_powers_pos[] = {
        1E16L, 1E32L, 1E64L, 1E128L, 1E256L,
#if LDBL_MAX_10_EXP >= 4096
        1E512L, 1E1024L, 1E2048L, 1E4096L,
#endif
        std::numeric_limits<long double>::infinity()
    };
    constexpr const long double binary_powers_neg[] = {
        1E-16L, 1E-32L, 1E-64L, 1E-128L, 1E-256L,
#if LDBL_MIN_10_EXP <= -4096
        1E-512L, 1E-1024L, 1E-2048L, 1E-4096L,
#endif
        0
    };
    static_assert(sizeof(binary_powers_pos) == sizeof(binary_powers_neg),
        "Precalculated arrays need to have the same size");
    constexpr auto binary_powers_last_index =
        sizeof(binary_powers_pos) / sizeof(*binary_powers_pos) - 1;
    const long double* direct_powers = direct_powers_pos;
    const long double* binary_powers = binary_powers_pos;

    if (exponent < 0) {
        exponent = -exponent;
        direct_powers = direct_powers_neg;
        binary_powers = binary_powers_neg;
    }
    auto result = direct_powers[exponent & 0xF];
    exponent >>= 4;
    for (int exponent_bit = 0; exponent > 0; ++exponent_bit, exponent >>= 1) {
        if (exponent_bit == binary_powers_last_index)
            return binary_powers[exponent_bit];
        if (exponent & 1)
            result *= binary_powers[exponent_bit];
    }
    return result;
}

} // namespace detail

} // namespace gul14
