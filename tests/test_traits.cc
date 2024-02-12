/**
 * \file   test_traits.cc
 * \author \ref contributors
 * \date   Created on February 12, 2024
 * \brief  Unit tests for some of the traits.
 *
 * \copyright Copyright 2024 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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
#include "gul14/traits.h"

using namespace std::literals;

TEST_CASE("is_invocable()", "[traits]")
{
    auto x = 1;
    REQUIRE(gul14::is_invocable<decltype(x)>::value == false);
    auto y = []() {};
    REQUIRE(gul14::is_invocable<decltype(y)>::value == true);
    REQUIRE(gul14::is_invocable_r<void, decltype(y), int>::value == false);
    auto z = [](int) -> float { return { }; };
    REQUIRE(gul14::is_invocable_r<void, decltype(z), int>::value == false);
    REQUIRE(gul14::is_invocable_r<float, decltype(z), double>::value == true);
    REQUIRE(gul14::is_invocable_r<std::string, decltype(z), double>::value == false);
}
