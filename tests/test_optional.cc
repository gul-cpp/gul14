/**
 * \file   test_optional.cc
 * \author \ref contributors
 * \date   Created on July 26, 2019
 * \brief  Unit tests for optional<>.
 *
 * \copyright Copyright 2019 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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
#include "gul14/optional.h"

using gul::optional;
using namespace std::literals;

TEMPLATE_TEST_CASE("optional: Basics for numeric types", "[optional]",
                   short, int, unsigned long long, float, long double)
{
    optional<TestType> opt;
    REQUIRE(opt.has_value() == false);
    REQUIRE(opt.value_or(42) == 42);
    REQUIRE_THROWS_AS(opt.value(), gul::bad_optional_access);
}

TEMPLATE_TEST_CASE("optional: Basics for string types", "[optional]",
                   std::string, const char *)
{
    optional<TestType> opt;
    REQUIRE(opt.has_value() == false);
    REQUIRE(std::string(opt.value_or("Test")) == "Test");
    REQUIRE_THROWS_AS(opt.value(), gul::bad_optional_access);
}

// vi:ts=4:sw=4:sts=4:et
