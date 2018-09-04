/**
 * \file   test_string_substitute.cc
 * \author Soeren Grunewald, Lars Froehlich
 * \date   Created on August 31, 2018
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

using namespace std::literals;
using namespace gul;

TEST_CASE("Replace parts of a string in-place", "[string_util]")
{
	const std::string input{ "foo bar baz" };

	// should be equal
	std::string foo = input;
	REQUIRE(replace_inplace(foo, "foo", "foo") == input); // replace the one with one

	foo = input;
	REQUIRE(replace_inplace(foo, "world", "foo") == input); // no match

	foo = input;
	REQUIRE(replace_inplace(foo, "foo", "world") == "world bar baz");

	foo = input;
	REQUIRE(replace_inplace(foo, "f", "m") == "moo bar baz"s );

	foo = input;
	REQUIRE(replace_inplace(foo, " ", "\t") == "foo\tbar\tbaz"s );

	try {
		replace_inplace(foo, "", "\t");
	} catch (const std::exception& e) {
		REQUIRE("needle is empty"s == e.what());
	}

	foo = "";
	REQUIRE(replace_inplace(foo, " ", "\t") == ""s);
}

TEST_CASE("Replace parts of a string using replace()", "[string_util]")
{
    REQUIRE(replace("hello lovely helpful world", "he", "KOU") == "KOUllo lovely KOUlpful world");
    REQUIRE(replace("hello lovely helpful world", "he", "A") == "Allo lovely Alpful world");
    REQUIRE(replace("hello lovely helpful world", "", "AGA") == "hello lovely helpful world");
    REQUIRE(replace("hello lovely helpful world", "lo", "y") == "hely yvely helpful world");
    REQUIRE(replace("hello lovely helpful world", "l", "") == "heo ovey hepfu word");
}
