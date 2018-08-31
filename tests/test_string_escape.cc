/**
 * \file   test_string_escape.cc
 * \author Soeren Grunewald
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

TEST_CASE("Compare escaped strings", "[string_util]")
{
	REQUIRE(gul::escape("foo bar baz") == "\"foo bar baz\"");
	REQUIRE(gul::escape("foo\rbar\nfoobar\tbaz\\qux") == "\"foo\\rbar\\nfoobar\\tbaz\\\\qux\""s);

	// TODO: add more tests cases...for ACK NAK ENQ REQ STX ETX...
}
