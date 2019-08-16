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

#include <iomanip>

#include "gul/catch.h"
#include "gul/to_number.h"

using namespace Catch::Matchers;
using gul::to_number;

TEST_CASE("to_number(): Integer types", "[to_number]")
{
    REQUIRE(to_number<char>("0").value() == 0);
    REQUIRE(to_number<signed char>("127").value() == 127);
    REQUIRE(to_number<signed char>("-128").value() == -128);
    REQUIRE(to_number<unsigned char>("255").value() == 255);
    REQUIRE(to_number<int16_t>("32767").value() == 32767);
    REQUIRE(to_number<int16_t>("-32768").value() == -32768);
    REQUIRE(to_number<uint16_t>("65535").value() == 65535);
    REQUIRE(to_number<int>("42").value() == 42);
    REQUIRE(to_number<int>("-42").value() == -42);
    REQUIRE(to_number<int32_t>("2147483647").value() == 2147483647);
    REQUIRE(to_number<int32_t>("-2147483648").value() == -2147483648LL);
    REQUIRE(to_number<int>("").has_value() == false);
    REQUIRE(to_number<int>(" 42").has_value() == false);
    REQUIRE(to_number<int>("42 ").has_value() == false);
    REQUIRE(to_number<signed char>("128").has_value() == false);
    REQUIRE(to_number<signed char>("-129").has_value() == false);
    REQUIRE(to_number<unsigned char>("256").has_value() == false);
    REQUIRE(to_number<int16_t>("32768").has_value() == false);
    REQUIRE(to_number<int16_t>("-32769").has_value() == false);
    REQUIRE(to_number<int32_t>("2147483648").has_value() == false);
    REQUIRE(to_number<int32_t>("-2147483649").has_value() == false);
}

TEMPLATE_TEST_CASE("to_number(): Floating point types", "[to_number]", float, double)
{
    REQUIRE_THAT(to_number<TestType>("0").value(), WithinULP(TestType(0), 1));
    REQUIRE_THAT(to_number<TestType>("12").value(), WithinULP(TestType(12), 1));
    REQUIRE_THAT(to_number<TestType>("-12").value(), WithinULP(TestType(-12.0), 1));
    REQUIRE_THAT(to_number<TestType>("0.125").value(), WithinULP(TestType(0.125), 1));
    REQUIRE_THAT(to_number<TestType>("-0.125").value(), WithinULP(TestType(-0.125), 1));
    REQUIRE_THAT(to_number<TestType>("5.").value(), WithinULP(TestType(5), 1));
    REQUIRE_THAT(to_number<TestType>("-5.").value(), WithinULP(TestType(-5), 1));
    REQUIRE_THAT(to_number<TestType>(".5").value(), WithinULP(TestType(0.5), 1));
    REQUIRE_THAT(to_number<TestType>("-.5").value(), WithinULP(TestType(-0.5), 1));
    REQUIRE_THAT(to_number<TestType>("123456.654321").value(),
                 WithinULP(TestType(123456.654321), 1));
    REQUIRE_THAT(to_number<TestType>("123456789012345678901234567890").value(),
                 WithinULP(TestType(123456789012345678901234567890.0), 1));
    REQUIRE_THAT(to_number<TestType>("1e2").value(), WithinULP(TestType(100), 1));
    REQUIRE_THAT(to_number<TestType>("1e+2").value(), WithinULP(TestType(100), 1));
    REQUIRE_THAT(to_number<TestType>("1.e2").value(), WithinULP(TestType(100), 1));
    REQUIRE_THAT(to_number<TestType>("1.e+2").value(), WithinULP(TestType(100), 1));
    REQUIRE_THAT(to_number<TestType>("1e-2").value(), WithinULP(TestType(0.01), 1));
    REQUIRE_THAT(to_number<TestType>("-1e2").value(), WithinULP(TestType(-100), 1));
    REQUIRE_THAT(to_number<TestType>("-1e+2").value(), WithinULP(TestType(-100), 1));
    REQUIRE_THAT(to_number<TestType>("-1.e2").value(), WithinULP(TestType(-100), 1));
    REQUIRE_THAT(to_number<TestType>("-1.e+2").value(), WithinULP(TestType(-100), 1));
    REQUIRE_THAT(to_number<TestType>("-1e-2").value(), WithinULP(TestType(-0.01), 1));
    REQUIRE_THAT(to_number<TestType>("1E2").value(), WithinULP(TestType(100), 1));
    REQUIRE_THAT(to_number<TestType>("1E+2").value(), WithinULP(TestType(100), 1));
    REQUIRE_THAT(to_number<TestType>("1E-2").value(), WithinULP(TestType(0.01), 1));
    REQUIRE_THAT(to_number<TestType>(".1e2").value(), WithinULP(TestType(10), 1));
    REQUIRE_THAT(to_number<TestType>(".1e+2").value(), WithinULP(TestType(10), 1));
    REQUIRE_THAT(to_number<TestType>(".1e-2").value(), WithinULP(TestType(0.001), 1));
    REQUIRE_THAT(to_number<TestType>("0.1e2").value(), WithinULP(TestType(10), 1));
    REQUIRE_THAT(to_number<TestType>("0.1e+2").value(), WithinULP(TestType(10), 1));
    REQUIRE_THAT(to_number<TestType>("0.1e-2").value(), WithinULP(TestType(0.001), 1));
    REQUIRE_THAT(to_number<TestType>("5e-0").value(), WithinULP(TestType(5), 1));

    REQUIRE(to_number<TestType>("").has_value() == false);
    REQUIRE(to_number<TestType>(" 0.1").has_value() == false);
    REQUIRE(to_number<TestType>("0.1 ").has_value() == false);
    REQUIRE(to_number<TestType>(".").has_value() == false);
    REQUIRE(to_number<TestType>("-.").has_value() == false);
    REQUIRE(to_number<TestType>("- .1").has_value() == false);
    REQUIRE(to_number<TestType>("- .1").has_value() == false);
    REQUIRE(to_number<TestType>("1.2e").has_value() == false);
    REQUIRE(to_number<TestType>("1e.").has_value() == false);
    REQUIRE(to_number<TestType>("1e+").has_value() == false);
    REQUIRE(to_number<TestType>("1e-").has_value() == false);
}

TEMPLATE_TEST_CASE("to_number(): integer max() values round-trip", "[to_number]",
                   short, unsigned short, int, unsigned int,
                   long, unsigned long, long long, unsigned long long)
{
    std::stringstream ss;
    ss << std::setprecision(std::numeric_limits<TestType>::digits10 + 1);

    if (sizeof(TestType) == 1)
        ss << static_cast<int>(std::numeric_limits<TestType>::max());
    else 
        ss << std::numeric_limits<TestType>::max();

    std::string str = ss.str();
    CAPTURE(str);
    REQUIRE(to_number<TestType>(str).value() == std::numeric_limits<TestType>::max());
}

TEMPLATE_TEST_CASE("to_number(): integer lowest() values round-trip", "[to_number]",
                   short, unsigned short, int, unsigned int,
                   long, unsigned long, long long, unsigned long long)
{
    std::stringstream ss;
    ss << std::setprecision(std::numeric_limits<TestType>::digits10 + 1);

    if (sizeof(TestType) == 1)
        ss << static_cast<int>(std::numeric_limits<TestType>::lowest());
    else
        ss << std::numeric_limits<TestType>::lowest();

    std::string str = ss.str();
    CAPTURE(str);
    REQUIRE(to_number<TestType>(str).value() == std::numeric_limits<TestType>::lowest());
}

TEST_CASE("to_number(): Overflow, big and small numbers (float)", "[to_number]")
{
    REQUIRE_THAT(to_number<float>("3.40282e+38").value(), WithinULP(3.40282e+38f, 1));
    REQUIRE(to_number<float>("3.40282e+39").has_value() == false);
    REQUIRE_THAT(to_number<float>("-3.40282e+38").value(), WithinULP(-3.40282e+38f, 1));
    REQUIRE(to_number<float>("-3.40282e+39").has_value() == false);
    REQUIRE_THAT(to_number<float>("1.17549e-38").value(), WithinULP(1.17549e-38f, 1));
    REQUIRE(to_number<float>("1e-50").value() == 0.0f);
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

// vi:ts=4:sw=4:sts=4:et
