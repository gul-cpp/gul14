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

#include <array>
#include <iomanip>
#include <sstream>

#include "gul/catch.h"
#include "gul/to_number.h"
#include "gul/num_util.h"

using namespace std::literals::string_literals;
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

TEMPLATE_TEST_CASE("to_number(): Floating point types", "[to_number]", float, double, long double)
{
    struct TestCase {
        char const* input;
        long double output;
    };
    std::array<TestCase, 31> cases = {{
        { "0", 0.0l },
        { "12", 12.0l },
        { "-12", -12.0l },
        { "0.125", 0.125l },
        { "-0.125", -0.125l },
        { "5.", 5.0l },
        { "-5.", -5.0l },
        { ".5", 0.5l },
        { "-.5", -0.5l },
        { "123456.654321", 123456.654321l },
        { "123456789012345678901234567890", 123456789012345678901234567890.0l },
        { "1e2", 100.0l },
        { "1e+2", 100.0l },
        { "1.e2", 100.0l },
        { "1.e+2", 100.0l },
        { "1e-2", 0.01l },
        { "-1e2", -100.0l },
        { "-1e+2", -100.0l },
        { "-1.e2", -100.0l },
        { "-1.e+2", -100.0l },
        { "-1e-2", -0.01l },
        { "1E2", 100.0l },
        { "1E+2", 100.0l },
        { "1E-2", 0.01l },
        { ".1e2", 10.0l },
        { ".1e+2", 10.0l },
        { ".1e-2", 0.001l },
        { "0.1e2", 10.0l },
        { "0.1e+2", 10.0l },
        { "0.1e-2", 0.001l },
        { "5e-0", 5.0l }
    }};
    auto const long_double_lenience = sizeof(TestType) > sizeof(double) ? 1 : 0; // long double std::pow 'bug'
    for (auto const& test : cases) {
        CAPTURE(test.input);
        REQUIRE(gul::within_ulp(to_number<TestType>(test.input).value(),
            TestType(test.output), long_double_lenience) == true);
    }

    REQUIRE(to_number<TestType>("").has_value() == false);
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
                   char, unsigned char, signed char, short, unsigned short, int,
                   unsigned int, long, unsigned long, long long, unsigned long long)
{
    std::stringstream ss;

    ss << std::setprecision(std::numeric_limits<TestType>::max_digits10)
       << static_cast<unsigned long long>(std::numeric_limits<TestType>::max());

    std::string str = ss.str();
    CAPTURE(str);
    REQUIRE(to_number<TestType>(str).value() == std::numeric_limits<TestType>::max());
}

TEMPLATE_TEST_CASE("to_number(): integer lowest() values round-trip", "[to_number]",
                   char, unsigned char, signed char, short, unsigned short, int,
                   unsigned int, long, unsigned long, long long, unsigned long long)
{
    std::stringstream ss;

    ss << std::setprecision(std::numeric_limits<TestType>::max_digits10)
       << static_cast<long long>(std::numeric_limits<TestType>::lowest());

    std::string str = ss.str();
    CAPTURE(str);
    REQUIRE(to_number<TestType>(str).value() == std::numeric_limits<TestType>::lowest());
}

TEMPLATE_TEST_CASE("to_number(): min and subnormal floating point", "[to_number]", float, double, long double)
{
    // Do no try subnormal if the type does not support it
    auto const max_divisor = std::numeric_limits<TestType>::has_denorm ? 4 : 1;
    // Long double std::pow 'bug'
    auto const long_double_lenience = sizeof(TestType) > sizeof(double) ? 1 : 0;

    auto const min = std::numeric_limits<TestType>::min();
    auto ss = std::stringstream{ };
    ss << std::setprecision(std::numeric_limits<TestType>::max_digits10);

    for (auto i = 1; i <= max_divisor; ++i) {
        auto const num = min / i; // Generate a number that is smaller than min (aka subnormal)
        ss.str("");
        ss << num;
        REQUIRE(true == gul::within_ulp(to_number<TestType>(ss.str()).value(), num, long_double_lenience));
    }
}

TEMPLATE_TEST_CASE("to_number(): max and overflow floating point", "[to_number]", float, double, long double)
{
    auto const max = std::numeric_limits<TestType>::max();

    auto ss = std::stringstream{ };
    ss << std::setprecision(std::numeric_limits<TestType>::max_digits10) << max;
    auto numb = ss.str();

    // Long double std::pow 'bug' on Darwin
    auto const long_double_lenience = sizeof(TestType) > sizeof(double) ? 30 : 0;
    REQUIRE(true == gul::within_ulp(to_number<TestType>(numb).value(), max, long_double_lenience));

    if (numb[0] < '9')
        ++numb[0];
    else
        numb = "10"s + numb.substr(1);
    CAPTURE(numb);
    REQUIRE(to_number<TestType>(numb).has_value() == false);
}

TEMPLATE_TEST_CASE("to_number(): lowest and overflow floating point", "[to_number]", float, double, long double)
{
    auto const lowest = std::numeric_limits<TestType>::lowest();

    auto ss = std::stringstream{ };
    ss << std::setprecision(std::numeric_limits<TestType>::max_digits10) << lowest;
    auto numb = ss.str();

    // Long double std::pow 'bug' on Darwin
    auto const long_double_lenience = sizeof(TestType) > sizeof(double) ? 30 : 0;
    REQUIRE(true == gul::within_ulp(to_number<TestType>(numb).value(), lowest, long_double_lenience));

    assert(numb[0] == '-');
    if (numb[1] < '9')
        ++numb[1];
    else
        numb = "-10"s + numb.substr(2);

    CAPTURE(numb);
    REQUIRE(to_number<TestType>(numb).has_value() == false);
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
