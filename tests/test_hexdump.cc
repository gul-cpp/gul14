/**
 * \file   test_hexdump.cc
 * \author \ref contributors
 * \date   Created on September 25, 2018
 * \brief  Part of test suite for string utility functions in the General Utility Library.
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

using namespace std::literals::string_literals;

TEST_CASE("Hexdump Test", "[hexdump]")
{
    SECTION("assignment (cast) test") {
        // This fails to compile if cast is impossible
        std::string x = gul::hexdump(""s);
        std::string y = gul::hexdump("");
    }
    SECTION("dump strings") {
        std::string x = "test\nthe Ä west!\t\r\n";

        auto a1 = gul::hexdump(x.data(), x.size(), "deBuk -> ").str();
        auto answer1 = "deBuk -> 000000: 74 65 73 74 0a 74 68 65 20 c3 84 20 77 65 73 74  test.the .. west\n"
                       "         000010: 21 09 0d 0a                                      !...\n"s;

	REQUIRE(a1 == answer1);
        auto a2 = gul::hexdump(x, "deBak -> ").str();
        auto answer2 = "deBak -> 000000: 74 65 73 74 0a 74 68 65 20 c3 84 20 77 65 73 74  test.the .. west\n"
                       "         000010: 21 09 0d 0a                                      !...\n"s;
	REQUIRE(a2 == answer2);

    }
    SECTION("dump arrays") {
        std::array<int, 8> ar = {{ 0, 1, 5, 2, -0x300fffff, 2, 5, 1999 }};
        auto a1 = gul::hexdump(ar).str();
        auto answer1 = "000000: 00000000 00000001 00000005 00000002 cff00001 00000002 00000005 000007cf"
	               "                                                                          \n"s;
	REQUIRE(a1 == answer1);
    }
}
