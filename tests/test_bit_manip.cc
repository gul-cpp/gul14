/**
 * \file   test_bit_manip.cc
 * \author \ref contributors
 * \date   Created on 17 Oct 2019
 * \brief  Unit tests for bit_set(), bit_reset(), bit_flip(), bit_test().
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

#include <type_traits>

#include "gul14/catch.h"
#include "gul14/bit_manip.h"

TEMPLATE_TEST_CASE("All bit_*() functions", "[bit_manip]",
        char, unsigned char, signed char, short, unsigned short, int,
        unsigned int, long, unsigned long, long long, unsigned long long)
{
    auto x = gul14::bit_set<TestType>(2); // 0x04
    REQUIRE(x == 0x04);
    REQUIRE(std::is_same<decltype(x), TestType>::value == true);

    auto y = gul14::bit_set(x, 4); // 0x10
    REQUIRE(y == 0x14);
    REQUIRE(std::is_same<decltype(y), TestType>::value == true);

    auto z = gul14::bit_flip(y, 0); // 0x01
    REQUIRE(z == 0x15);
    REQUIRE(std::is_same<decltype(z), TestType>::value == true);

    z = gul14::bit_flip(z, 2); // 0x04
    REQUIRE(z == 0x11);

    auto a = gul14::bit_reset(z, 4); // 0x10
    REQUIRE(a == 0x01);
    REQUIRE(std::is_same<decltype(a), TestType>::value == true);

    a = gul14::bit_reset(a, 3); // 0x08
    REQUIRE(a == 0x01);

    auto b = gul14::bit_test(a, 0); // 0x01
    REQUIRE(b == true);
    REQUIRE(std::is_same<decltype(b), bool>::value == true);

    b = gul14::bit_test(a, 1); // 0x02
    REQUIRE(b == false);
}

TEMPLATE_TEST_CASE("Access MSB", "[bit_manip]",
        char, unsigned char, signed char, short, unsigned short, int,
        unsigned int, long, unsigned long, long long, unsigned long long)
{
    auto const msb = sizeof(TestType) * 8 - 1;

    auto x = gul14::bit_set<TestType>(msb);

    if (std::is_signed<TestType>::value)
        REQUIRE(x < 0);
    else
        REQUIRE(x > 0);

    REQUIRE(gul14::bit_test(x, msb) == true);
    REQUIRE(gul14::bit_test(x, msb - 1) == false);
}

// vi:ts=4:sw=4:sts=4:et
