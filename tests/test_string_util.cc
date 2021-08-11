/**
 * \file   test_string_util.cc
 * \author \ref contributors
 * \date   Created on August 11, 2021
 * \brief  Unit tests for safe_string().
 *
 * \copyright Copyright 2021 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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

#include "gul14/catch.h"
#include "gul14/string_util.h"

using gul14::safe_string;

using namespace std::literals;

TEST_CASE("safe_string()", "[string_util]")
{
    REQUIRE(safe_string(nullptr, 0) == "");
    REQUIRE(safe_string(nullptr, 10) == "");
    REQUIRE(safe_string("hello", 0) == "");
    REQUIRE(safe_string("hello", 10) == "hello");
    REQUIRE(safe_string("hello\0", 6) == "hello");
    REQUIRE(safe_string("hello\0world", 11) == "hello");
}
