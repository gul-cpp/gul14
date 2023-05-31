/**
 * \file   test_variant.cc
 * \author \ref contributors
 * \date   Created on May 31, 2023
 * \brief  Unit tests for the variant class template.
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

#include <memory>
#include <string>

#include "gul14/catch.h"
#include "gul14/expected.h" // to check if there are clashing declarations, e.g. in_place_t
#include "gul14/optional.h" // to check if there are clashing declarations, e.g. in_place_t
#include "gul14/variant.h"

using namespace std::literals;

// Note: As this is just a backport of std::variant, we do not include a substantial
// test suite. The tests are just to make sure that the backport works roughly as
// expected.

TEST_CASE("variant: Default constructor", "[variant]")
{
    gul14::variant<int, std::string, float, std::unique_ptr<double>> v;
    REQUIRE(gul14::holds_alternative<int>(v));
    REQUIRE(gul14::get<int>(v) == 0);
}

TEST_CASE("variant: get()", "[variant]")
{
    gul14::variant<int, std::string, float, std::unique_ptr<double>> v;

    v = 42;
    REQUIRE(gul14::get<int>(v) == 42);
    REQUIRE_THROWS_AS(gul14::get<std::string>(v), gul14::bad_variant_access);

    v = 1.5f;
    REQUIRE(gul14::get<float>(v) == 1.5f);
    REQUIRE_THROWS_AS(gul14::get<std::string>(v), gul14::bad_variant_access);

    v = "Hello"s;
    REQUIRE(gul14::get<std::string>(v) == "Hello"s);
    REQUIRE_THROWS_AS(gul14::get<int>(v), gul14::bad_variant_access);

    v = std::make_unique<double>(3.14);
    REQUIRE(gul14::get<std::unique_ptr<double>>(v) != nullptr);
    REQUIRE(*gul14::get<std::unique_ptr<double>>(v) == 3.14);
    REQUIRE_THROWS_AS(gul14::get<std::string>(v), gul14::bad_variant_access);
}

TEST_CASE("variant: get_if()", "[variant]")
{
    gul14::variant<int, std::string, float, std::unique_ptr<double>> v;

    REQUIRE(gul14::get_if<int>(static_cast<decltype(v)*>(nullptr)) == nullptr);

    v = 42;
    REQUIRE(gul14::get_if<int>(&v) != nullptr);
    REQUIRE(*gul14::get_if<int>(&v) == 42);
    REQUIRE(gul14::get_if<std::string>(&v) == nullptr);

    v = 1.5f;
    REQUIRE(gul14::get_if<float>(&v) != nullptr);
    REQUIRE(*gul14::get_if<float>(&v) == 1.5f);
    REQUIRE(gul14::get_if<std::string>(&v) == nullptr);

    v = "Hello"s;
    REQUIRE(gul14::get_if<std::string>(&v) != nullptr);
    REQUIRE(*gul14::get_if<std::string>(&v) == "Hello"s);
    REQUIRE(gul14::get_if<int>(&v) == nullptr);

    v = std::make_unique<double>(3.14);
    REQUIRE(gul14::get_if<std::unique_ptr<double>>(&v) != nullptr);
    REQUIRE(*gul14::get_if<std::unique_ptr<double>>(&v) != nullptr);
    REQUIRE(**gul14::get_if<std::unique_ptr<double>>(&v) == 3.14);
    REQUIRE(gul14::get_if<float>(&v) == nullptr);
}

TEST_CASE("variant: holds_alternative()", "[variant]")
{
    gul14::variant<int, std::string, float, std::unique_ptr<double>> v;

    v = 42;
    REQUIRE(gul14::holds_alternative<int>(v) == true);
    REQUIRE(gul14::holds_alternative<std::string>(v) == false);

    v = 1.5f;
    REQUIRE(gul14::holds_alternative<float>(v) == true);
    REQUIRE(gul14::holds_alternative<int>(v) == false);

    v = "Hello"s;
    REQUIRE(gul14::holds_alternative<std::string>(v) == true);
    REQUIRE(gul14::holds_alternative<std::unique_ptr<double>>(v) == false);

    v = std::make_unique<double>(3.14);
    REQUIRE(gul14::holds_alternative<std::unique_ptr<double>>(v) == true);
    REQUIRE(gul14::holds_alternative<float>(v) == false);
}
