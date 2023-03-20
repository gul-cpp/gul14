/**
 * \file   test_expected.cc
 * \author \ref contributors
 * \date   Created on March 20, 2023
 * \brief  Unit tests for the expected class template.
 *
 * \copyright Copyright 2023 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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

#include <string>

#include "gul14/catch.h"
#include "gul14/expected.h"

using namespace std::literals;

// Note: As this is just a backport of std::expected, we do not include a substantial
// test suite. The tests are just to make sure that the backport works roughly as
// expected (no pun intended).

TEST_CASE("expected", "[expected]")
{
    gul14::expected<int, std::string> ex;

    REQUIRE(ex.has_value() == true);
    REQUIRE(*ex == 0);

    ex = gul14::unexpected<std::string>("error");
    REQUIRE(ex.has_value() == false);
    REQUIRE(ex.error() == "error");

    ex = 42;
    REQUIRE(ex.has_value() == true);
    REQUIRE(*ex == 42);
}
