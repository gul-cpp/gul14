/**
 * \file   test_hexdump.cc
 * \author \ref contributors
 * \date   Created on September 25, 2018
 * \brief  Test suite for hexdump() and hexdump_stream().
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

#include "catch.h"
#include <gul.h>
#include <iostream>
#include <limits>

using namespace std::literals::string_literals;

TEST_CASE("Hexdump Test", "[hexdump]")
{
    SECTION("empty dump test") {
        std::string x = gul::hexdump(""s);
        auto answer1 = "000000:                                                  \n"s;
        REQUIRE(x == answer1);

        auto oss1 = std::ostringstream{ };
        oss1 << gul::hexdump_stream(""s);
        REQUIRE(oss1.str() == answer1);
    }
    SECTION("dump strings") {
        auto x = "test\nthe Ä west!\t\r\n"s;

        auto a1 = gul::hexdump(x.data(), x.size(), "deBuk -> ");
        auto answer1 = "deBuk -> 000000: 74 65 73 74 0a 74 68 65 20 c3 84 20 77 65 73 74  test.the .. west\n"
                       "         000010: 21 09 0d 0a                                      !...\n"s;
        REQUIRE(a1 == answer1);

        auto oss1 = std::ostringstream{ };
        oss1 << gul::hexdump_stream(x.data(), x.size(), "deBuk -> ");
        REQUIRE(oss1.str() == answer1);

        auto oss2 = std::ostringstream{ };
        oss2 << gul::hexdump_stream(x.begin(), x.end(), "deBuk -> ");
        REQUIRE(oss2.str() == answer1);

        auto a2 = gul::hexdump(x, "deBak -> ");
        auto answer2 = "deBak -> 000000: 74 65 73 74 0a 74 68 65 20 c3 84 20 77 65 73 74  test.the .. west\n"
                       "         000010: 21 09 0d 0a                                      !...\n"s;
        REQUIRE(a2 == answer2);

        auto oss3 = std::ostringstream{ };
        oss3 << gul::hexdump_stream(x, "deBak -> ");
        REQUIRE(oss3.str() == answer2);
    }
    SECTION("dump full container") {
        auto ar = std::array<int, 8>{{ 0, 1, 5, 2, -0x300fffff, 2, 5, 1999 }};

        auto a1 = gul::hexdump(ar);
        auto answer1 = "000000: 00000000 00000001 00000005 00000002 cff00001 00000002 00000005 000007cf \n"s;
        REQUIRE(a1 == answer1);

        auto oss1 = std::ostringstream{ };
        oss1 << gul::hexdump_stream(ar);
        REQUIRE(oss1.str() == answer1);

        auto oss2 = std::ostringstream{ };
        auto hdp_forward = gul::hexdump_stream(std::array<int, 8>{{ 0, 1, 5, 2, -0x300fffff, 2, 5, 1999 }});
        {
            // We need this complicate construct to replace the remnants of the temporary above with some
            // different object on the stack. ar2 now lives in the memory space of the temporary array
            // if its lifetime has not been extended
            // This works only in gcc, I can't find a way to trigger this error in clang
            auto ar2 = std::array<int, 8>{{ 1, 1, 5, 2, -0x300fffff, 2, 5, 1999 }};
            oss2 << hdp_forward;
            oss1 << ar2.size(); // just to use the variable for something to prevent optimizing it out
        }
        REQUIRE(oss2.str() == answer1);

        // Check if ar has been moved and left us with a corpse
        // Is ar still the same?
        auto oss3 = std::ostringstream{ };
        oss3 << gul::hexdump_stream(ar);
        REQUIRE(oss3.str() == answer1);
    }
    SECTION("dump with iterators") {
        std::array<int, 8> ar = {{ 0, 1, 5, 2, -0x300fffff, 2, 5, 1999 }};

        auto a1 = gul::hexdump(ar.begin(), ar.end());
        auto answer1 = "000000: 00000000 00000001 00000005 00000002 cff00001 00000002 00000005 000007cf \n"s;
        REQUIRE(a1 == answer1);

        auto oss1 = std::ostringstream{ };
        oss1 << gul::hexdump_stream(ar.begin(), ar.end());
        REQUIRE(oss1.str() == answer1);
    }
    SECTION("dump unsigned long long") {
        std::array<unsigned long long, 1> ar = {{ std::numeric_limits<unsigned long long>::max() }};
        auto a1 = gul::hexdump(ar);
        auto answer1 = "000000: " + std::string(sizeof(ar[0]) * 2, 'f') + " \n"s;
        REQUIRE(a1 == answer1);
    }
}

// vi:ts=4:sw=4:et
