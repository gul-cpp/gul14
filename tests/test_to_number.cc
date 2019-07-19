/**
 * \file   test_to_number.cc
 * \author \ref contributors
 * \date   Created on July 19, 2019
 * \brief  Test suite for to_number().
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

#include "gul/catch.h"
#include "gul.h"

using gul::to_number;

TEST_CASE("to_number(): Integer types", "[to_number]")
{
    REQUIRE(to_number<char>("0") == 0);
    REQUIRE(to_number<signed char>("127") == 127);
    REQUIRE(to_number<signed char>("-128") == -128);
    REQUIRE(to_number<unsigned char>("255") == 255);
    REQUIRE(to_number<int16_t>("32767") == 32767);
    REQUIRE(to_number<int16_t>("-32768") == -32768);
    REQUIRE(to_number<uint16_t>("65535") == 65535);
    REQUIRE(to_number<int>("42") == 42);
    REQUIRE(to_number<int>("-42") == -42);
    REQUIRE(to_number<int32_t>("2147483647") == 2147483647);
    REQUIRE(to_number<int32_t>("-2147483648") == -2147483648LL);
    REQUIRE_THROWS_AS(to_number<int>(""), std::invalid_argument);
    REQUIRE_THROWS_AS(to_number<int>(" 42"), std::invalid_argument);
    REQUIRE_THROWS_AS(to_number<int>("42 "), std::invalid_argument);
    REQUIRE_THROWS_AS(to_number<signed char>("128"), std::out_of_range);
    REQUIRE_THROWS_AS(to_number<signed char>("-129"), std::out_of_range);
    REQUIRE_THROWS_AS(to_number<unsigned char>("256"), std::out_of_range);
    REQUIRE_THROWS_AS(to_number<int16_t>("32768"), std::out_of_range);
    REQUIRE_THROWS_AS(to_number<int16_t>("-32769"), std::out_of_range);
    REQUIRE_THROWS_AS(to_number<int32_t>("2147483648"), std::out_of_range);
    REQUIRE_THROWS_AS(to_number<int32_t>("-2147483649"), std::out_of_range);
}

TEMPLATE_TEST_CASE("to_number(): Floating point types", "[to_number]", float)
{
    REQUIRE(to_number<TestType>("0") == 0.0);
    REQUIRE(to_number<TestType>("12") == 12.0);
    REQUIRE(to_number<TestType>("-12") == -12.0);
    REQUIRE(to_number<TestType>("0.125") == 0.125);
    REQUIRE(to_number<TestType>("-0.125") == -0.125);
    REQUIRE(to_number<TestType>("5.") == 5.0);
    REQUIRE(to_number<TestType>("-5.") == -5.0);
    REQUIRE(to_number<TestType>(".5") == 0.5);
    REQUIRE(to_number<TestType>("-.5") == -0.5);
    REQUIRE(to_number<TestType>("123456.654321") == Approx(123456.654321));
    REQUIRE(to_number<TestType>("1e2") == 100);
    REQUIRE(to_number<TestType>("1e+2") == 100);
    REQUIRE(to_number<TestType>("1.e2") == 100);
    REQUIRE(to_number<TestType>("1.e+2") == 100);
    REQUIRE(to_number<TestType>("1e-2") == static_cast<TestType>(0.01));
    REQUIRE(to_number<TestType>("-1e2") == -100);
    REQUIRE(to_number<TestType>("-1e+2") == -100);
    REQUIRE(to_number<TestType>("-1.e2") == -100);
    REQUIRE(to_number<TestType>("-1.e+2") == -100);
    REQUIRE(to_number<TestType>("-1e-2") == static_cast<TestType>(-0.01));
    REQUIRE(to_number<TestType>("1E2") == 100);
    REQUIRE(to_number<TestType>("1E+2") == 100);
    REQUIRE(to_number<TestType>("1E-2") == static_cast<TestType>(0.01));
    REQUIRE(to_number<TestType>(".1e2") == 10);
    REQUIRE(to_number<TestType>(".1e+2") == 10);
    REQUIRE(to_number<TestType>(".1e-2") == static_cast<TestType>(0.001));
    REQUIRE(to_number<TestType>("0.1e2") == 10);
    REQUIRE(to_number<TestType>("0.1e+2") == 10);
    REQUIRE(to_number<TestType>("0.1e-2") == static_cast<TestType>(0.001));
    REQUIRE(to_number<TestType>("5e-0") == 5);
    REQUIRE_THROWS_AS(to_number<TestType>(""), std::invalid_argument);
    REQUIRE_THROWS_AS(to_number<TestType>(" 0.1"), std::invalid_argument);
    REQUIRE_THROWS_AS(to_number<TestType>("0.1 "), std::invalid_argument);
    REQUIRE_THROWS_AS(to_number<TestType>("."), std::invalid_argument);
    REQUIRE_THROWS_AS(to_number<TestType>("-."), std::invalid_argument);
    REQUIRE_THROWS_AS(to_number<TestType>("- .1"), std::invalid_argument);
    REQUIRE_THROWS_AS(to_number<TestType>("- .1"), std::invalid_argument);
    REQUIRE_THROWS_AS(to_number<TestType>("1.2e"), std::invalid_argument);
    REQUIRE_THROWS_AS(to_number<TestType>("1e."), std::invalid_argument);
    REQUIRE_THROWS_AS(to_number<TestType>("1e+"), std::invalid_argument);
    REQUIRE_THROWS_AS(to_number<TestType>("1e-"), std::invalid_argument);
}

/* Disabled because doocsdev16's gcc has insufficient constexpr support (but works on
   doocsdev18)
TEMPLATE_TEST_CASE("to_number(): Convert \"42\" to integer types, constexpr", "[to_number]",
                   char, unsigned char, short,
                   unsigned short, int, unsigned int, long, unsigned long, long long,
                   unsigned long long)
{
    constexpr auto cstr = "42";
    constexpr gul::string_view sv{ cstr, 2 };
    constexpr auto a = to_number<TestType>(sv);
    REQUIRE(a == 42);
}

TEMPLATE_TEST_CASE("to_number(): Convert \"-42\" to integer types, constexpr", "[to_number]",
                   signed char, short, int, long, long long)
{
    constexpr auto cstr = "-42";
    constexpr gul::string_view sv{ cstr, 3 };
    constexpr auto a = to_number<TestType>(sv);
    REQUIRE(a == -42);
}
*/

/* Disabled because gul::string_view::find_first_of() is not really constexpr
TEMPLATE_TEST_CASE("to_number(): Floating-point types, constexpr", "[to_number]",
                   float, double, long double)
{
    constexpr auto cstr = "-42.2e1";
    constexpr gul::string_view sv{ cstr, 7 };

    constexpr auto a = to_number<TestType>(sv);
    REQUIRE(a == TestType{ -42.2e1 });
}
*/
