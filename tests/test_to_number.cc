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
#include <cmath>
#include <iomanip>
#include <random>
#include <sstream>

#include "gul/catch.h"
#include "gul/to_number.h"
#include "gul/num_util.h"

using namespace std::literals::string_literals;
using namespace Catch::Matchers;
using gul::to_number;

// Allowed deviation from ideal result in ULP
int constexpr long_double_lenience = 3;
int constexpr double_lenience = 3;

// Settings for the random round trip tests
// To test thoroughly the number of test should be 100'000
struct TestConfig {
    bool static const random_includes_inf_nan{ false };
    unsigned static const number_of_random_tests{ 5'000 };
};

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
    auto const lenience = sizeof(TestType) > sizeof(double) ? long_double_lenience : double_lenience;
    for (auto const& test : cases) {
        CAPTURE(test.input);
        REQUIRE(gul::within_ulp(to_number<TestType>(test.input).value(),
            TestType(test.output), lenience) == true);
    }
    std::array<TestCase, 12> special_cases = {{
        { "inf", std::numeric_limits<long double>::infinity() },
        { "iNf", std::numeric_limits<long double>::infinity() },
        { "INF", std::numeric_limits<long double>::infinity() },
        { "-inf", -std::numeric_limits<long double>::infinity() },
        { "-iNf", -std::numeric_limits<long double>::infinity() },
        { "-INF", -std::numeric_limits<long double>::infinity() },
        { "infinity", std::numeric_limits<long double>::infinity() },
        { "INFINITY", std::numeric_limits<long double>::infinity() },
        { "INFInITY", std::numeric_limits<long double>::infinity() },
        { "-infinity", -std::numeric_limits<long double>::infinity() },
        { "-INFINITY", -std::numeric_limits<long double>::infinity() },
        { "-INFInITY", -std::numeric_limits<long double>::infinity() },
    }};
    for (auto const& test : special_cases) {
        CAPTURE(test.input);
        REQUIRE(to_number<TestType>(test.input).value() == TestType(test.output));
    }
    REQUIRE(std::isnan(to_number<TestType>("nan").value()));
    REQUIRE(std::isnan(to_number<TestType>("nAn").value()));
    REQUIRE(std::isnan(to_number<TestType>("NAN").value()));
    REQUIRE(std::isnan(to_number<TestType>("-nan").value()));
    REQUIRE(std::isnan(to_number<TestType>("-nAn").value()));
    REQUIRE(std::isnan(to_number<TestType>("-NAN").value()));
    REQUIRE(std::isnan(to_number<TestType>("nan()").value()));
    REQUIRE(std::isnan(to_number<TestType>("nan(1234abc_ABC)").value()));
    REQUIRE(std::isnan(to_number<TestType>("-nan(1234abc_ABC)").value()));

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
    REQUIRE(to_number<TestType>("in").has_value() == false);
    REQUIRE(to_number<TestType>("infi").has_value() == false);
    REQUIRE(to_number<TestType>("infinityi").has_value() == false);
    REQUIRE(to_number<TestType>("-in").has_value() == false);
    REQUIRE(to_number<TestType>("-infi").has_value() == false);
    REQUIRE(to_number<TestType>("-infinityi").has_value() == false);
    REQUIRE(to_number<TestType>("na").has_value() == false);
    REQUIRE(to_number<TestType>("nana").has_value() == false);
    REQUIRE(to_number<TestType>("nan(").has_value() == false);
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable: 4127 )
#endif
    if (sizeof(TestType) <= sizeof(double)) {
        // Apple clang 8.0.0 strtod() fails to check this input for long double
        REQUIRE(to_number<TestType>("nan(.)").has_value() == false);
    }
#ifdef _MSC_VER
#pragma warning( pop )
#endif
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
    auto const lenience = sizeof(TestType) > sizeof(double) ? long_double_lenience : double_lenience;

    auto const min = std::numeric_limits<TestType>::min();
    auto ss = std::stringstream{ };
    ss << std::setprecision(std::numeric_limits<TestType>::max_digits10);

    for (auto i = 1; i <= max_divisor; ++i) {
        auto const num = min / i; // Generate a number that is smaller than min (aka subnormal)
        ss.str("");
        ss << num;
        CAPTURE(ss.str());
        REQUIRE(true == gul::within_ulp(to_number<TestType>(ss.str()).value(), num, lenience));
    }
}

TEMPLATE_TEST_CASE("to_number(): max and overflow floating point", "[to_number]", float, double, long double)
{
    auto const max = std::numeric_limits<TestType>::max();

    auto ss = std::stringstream{ };
    ss << std::setprecision(std::numeric_limits<TestType>::max_digits10) << max;
    auto numb = ss.str();

    auto const lenience = sizeof(TestType) > sizeof(double) ? long_double_lenience : double_lenience;
    REQUIRE(true == gul::within_ulp(to_number<TestType>(numb).value(), max, lenience));

    if (numb[0] < '9')
        ++numb[0];
    else
        numb = "10"s + numb.substr(1);

    CAPTURE(numb);
    auto answer = to_number<TestType>(numb).value();
    CAPTURE(answer);
    // Check for infinity
    REQUIRE((not std::isfinite(answer)));
    REQUIRE((not std::isnan(answer)));

}

TEMPLATE_TEST_CASE("to_number(): lowest and overflow floating point", "[to_number]", float, double, long double)
{
    auto const lowest = std::numeric_limits<TestType>::lowest();

    auto ss = std::stringstream{ };
    ss << std::setprecision(std::numeric_limits<TestType>::max_digits10) << lowest;
    auto numb = ss.str();

    auto const lenience = sizeof(TestType) > sizeof(double) ? long_double_lenience : double_lenience;
    REQUIRE(true == gul::within_ulp(to_number<TestType>(numb).value(), lowest, lenience));

    assert(numb[0] == '-');
    if (numb[1] < '9')
        ++numb[1];
    else
        numb = "-10"s + numb.substr(2);

    CAPTURE(numb);
    auto answer = to_number<TestType>(numb).value();
    CAPTURE(answer);
    // Check for infinity
    REQUIRE((not std::isfinite(answer)));
    REQUIRE((not std::isnan(answer)));
}

template <typename Float>
auto random_float() -> Float
{
    struct int128_t {
        int64_t a;
        int64_t b;
    };
    using bit_type = int128_t; // int64_t;

    union convert {
        Float f;
        bit_type i;
    };
    auto converter = convert{};

    static_assert(sizeof(decltype(std::declval<convert>().f))
                <= sizeof(decltype(std::declval<convert>().i)),
                "Integer random buffer too small");
    static_assert(offsetof(convert, f) == offsetof(convert, i),
                "Alignment problem in conversion union");

    static std::random_device rd;
    auto static gen = std::mt19937{ rd() };
    auto static dis = std::uniform_int_distribution<int64_t>{ };

    do {
        converter.i.a = dis(gen);
        converter.i.b = dis(gen);
    }
    while (not TestConfig::random_includes_inf_nan
            and (std::isnan(converter.f) or not std::isfinite(converter.f)));

    return converter.f;
}

TEMPLATE_TEST_CASE("to_number(): random round trip conversion", "[to_number]",
                   float, double, long double)
{
    auto loops = TestConfig::number_of_random_tests;

    // long double is rather time consuming
    if (std::is_same<TestType, long double>::value)
        loops = 100;
    int i_nan{ };
    int i_inf{ };
    int i_sub{ };
    int i_nor{ };
    for (auto i{ 0u }; i < loops; ++i) {
        TestType const num = random_float<TestType>();

        auto ss = std::stringstream{ };
        ss << std::setprecision(std::numeric_limits<TestType>::max_digits10) << num;
        auto numstr = ss.str();

        auto converted = to_number<TestType>(numstr);

        CAPTURE(i);
        CAPTURE(numstr);
        REQUIRE(converted.has_value());
        ss.str("");
        ss << *converted;
        auto conver = ss.str();
        CAPTURE(conver);
        if (std::isnan(num)) {
            CAPTURE("NaN " + std::to_string(++i_nan));
            REQUIRE(std::isnan(*converted));
        } else if (not std::isfinite(num)) {
            CAPTURE("infinity " + std::to_string(++i_inf));
            REQUIRE(std::abs(num) == std::abs(*converted));
            REQUIRE(std::signbit(num) == std::signbit(*converted));
        } else if (not std::isnormal(num)) {
            CAPTURE("subnormal " + std::to_string(++i_sub));
            REQUIRE(true == gul::within_ulp(*converted, num, 30));
        } else {
            CAPTURE("normal " + std::to_string(++i_nor));
            REQUIRE(true == gul::within_ulp(*converted, num, 3));
        }
    }
}

TEMPLATE_TEST_CASE("to_number(): Convert integers with many leading zeros", "[to_number]",
                   signed char, unsigned char, short, unsigned short, int, unsigned int,
                   long, unsigned long, long long, unsigned long long)
{
    REQUIRE(to_number<TestType>("000000000000000000000000000000000000000000010") == TestType{ 10 });
}

TEMPLATE_TEST_CASE("to_number(): Convert floating-point values with many leading and trailing zeros", "[to_number]",
    float, double, long double)
{
    auto const lenience = 0;

    auto const test1 = to_number<TestType>("00000000000000000000000000000000000000000000000000.1");
    REQUIRE(gul::within_ulp(test1.value(), TestType(0.1l), lenience) == true);

    auto const test2 = to_number<TestType>("00000000000000000000000000000000000000000000000000.1"
            "00000000000000000000000000000000000000000000000000");
    REQUIRE(gul::within_ulp(test2.value(), TestType(0.1l), lenience) == true);

    auto const test3 = to_number<TestType>("00000000000000000000000000000000000000000000000001"
            "e000000000000000000000000000000000000000000000000000");
    REQUIRE(gul::within_ulp(test3.value(), TestType(1.0l), lenience) == true);

    auto const test4 = to_number<TestType>("00000000000000000000000000000000000000000000000000.1e"
            "0000000000000000000000000000000000000000000000000");
    REQUIRE(gul::within_ulp(test4.value(), TestType(0.1l), lenience) == true);

    auto const test5 = to_number<TestType>("0.00000000000000000000000000000000000000000000000001"
            "e0000000000000000000000000000000000000000000000001");
    REQUIRE(gul::within_ulp(test5.value(), TestType(1.0e-49l), lenience) == true);
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
