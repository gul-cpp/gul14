/**
 * \file   test_starts_with_ends_with.cc
 * \author \ref contributors
 * \date   Created on November 26, 2018
 * \brief  Test suite for starts_with() and ends_with() from the General Utility Library.
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

#include "gul/catch.h"
#include <gul.h>

using namespace std::literals;
using namespace gul;

TEST_CASE("starts_with() works as expected", "[string][starts_with/ends_with]")
{
    REQUIRE(!starts_with("Hello World", "Hello World2"));
    REQUIRE(starts_with("Hello World", "Hello World"));
    REQUIRE(starts_with("Hello World", "Hello"));
    REQUIRE(starts_with("Hello World", ""));
    REQUIRE(!starts_with("", "Hello"));
    REQUIRE(starts_with("", ""));
    REQUIRE(starts_with("\0Hello World"s, "\0"s));
    REQUIRE(!starts_with("Hello World", "\0"s));
}

TEST_CASE("Replace parts of a string using replace()", "[string_util]")
{
    REQUIRE(!ends_with("Hello World", "Hello World2"));
    REQUIRE(ends_with("Hello World", "Hello World"));
    REQUIRE(ends_with("Hello World", "World"));
    REQUIRE(ends_with("Hello World", ""));
    REQUIRE(!ends_with("", "Hello"));
    REQUIRE(ends_with("", ""));
    REQUIRE(ends_with("Hello World\0"s, "\0"s));
    REQUIRE(!ends_with("Hello World", "\0"s));
}
