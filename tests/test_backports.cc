/**
 * \file   test_backports.cc
 * \author \ref contributors
 * \date   Created on August 30, 2018
 * \brief  Test suite for standard library backports in the General Utility Library.
 *
 * \copyright Copyright 2018-2019 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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

#include "gul/catch.h"
#include "gul/span.h"
#include "gul/string_view.h"

using namespace std::literals;

TEST_CASE("gul::string_view accepts a string as both char * and std::string, and both "
          "compare equal", "[string_view]")
{
    REQUIRE( gul::string_view{"Test"} == gul::string_view{"Test"s} );
    REQUIRE( gul::string_view{""} == gul::string_view{""s} );
}

TEMPLATE_TEST_CASE("span", "[span]", signed char, unsigned char, short, unsigned short,
                   int, unsigned int, long, unsigned long, long long, unsigned long long)
{
    TestType arr[5] { 1, 2, 3, 4, 5 };

    gul::span<TestType> s;
    REQUIRE(s.empty());

    s = arr;
    REQUIRE(s.size() == 5u);

    for (auto i = 0u; i < s.size(); i++)
        REQUIRE(arr[i] == s[i]);
}

// vi:ts=4:sw=4:sts=4:et
