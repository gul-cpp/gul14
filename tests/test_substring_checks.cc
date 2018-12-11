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

TEST_CASE("contains(string_view, string_view)", "[string][substring checks]")
{
    REQUIRE(contains("Hello World", "Hello"));
    REQUIRE(contains("Hello World", "World"));
    REQUIRE(contains("Hello World", "o Wo"));
    REQUIRE(!contains("Hello World", "lll"));
    REQUIRE(contains("Hello\0World\0"s, "d\0"s));
    REQUIRE(contains("Hello\0World\0"s, "\0W"s));
    REQUIRE(!contains("Hello World", "\0"s));
}

TEST_CASE("contains(string_view, char)", "[string][substring checks]")
{
    REQUIRE(contains("Hello World", 'H'));
    REQUIRE(contains("Hello World", 'd'));
    REQUIRE(contains("Hello World", ' '));
    REQUIRE(!contains("Hello World", 'g'));
    REQUIRE(contains("Hello\0World\0"s, '\0'));
    REQUIRE(contains("Hello\0World\0"s, 'd'));
    REQUIRE(!contains("Hello World", '\0'));
}

TEST_CASE("ends_with(string_view, string_view)", "[string][starts_with/ends_with]")
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

TEST_CASE("ends_with(string_view, char)", "[string][starts_with/ends_with]")
{
    REQUIRE(ends_with("Hello World", 'd'));
    REQUIRE(!ends_with("Hello World", 'D'));
    REQUIRE(!ends_with("", 'd'));
    REQUIRE(ends_with("Hello World\0"s, '\0'));
}

TEST_CASE("starts_with(string_view, string_view)", "[string][starts_with/ends_with]")
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

TEST_CASE("starts_with(string_view, char)", "[string][starts_with/ends_with]")
{
    REQUIRE(starts_with("Hello World", 'H'));
    REQUIRE(!starts_with("Hello World", 'h'));
    REQUIRE(!starts_with("", 'H'));
    REQUIRE(starts_with("\0Hello World"s, '\0'));
}
