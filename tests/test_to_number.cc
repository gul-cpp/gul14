/**
 * \file   test_to_number.cc
 * \author \ref contributors
 * \date   Created on July 19, 2019
 * \brief  Test suite for to_number().
 *
 * \copyright Copyright 2019, 2024 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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
#include <cfloat>
#include <iomanip>
#include <limits>
#include <random>
#include <sstream>

#include "gul14/catch.h"
#include "gul14/to_number.h"
#include "gul14/num_util.h"

using namespace std::literals::string_literals;
using namespace Catch::Matchers;
using gul14::to_number;

// Allowed deviation from ideal result in ULP
int constexpr long_double_lenience = 3;
int constexpr double_lenience = 3;

// Settings for the random round trip tests
// To test thoroughly the number of test should be 100'000
struct TestConfig {
    bool static const random_includes_inf_nan{ false };
    unsigned static const number_of_random_tests{ 5'000 };
};

template <typename T>
int error_in_ulp(T value, T reference)
{
    int offset = 0;
    do {
        if (value == reference) {
            return offset;
        }
        if (value < reference) {
            --offset;
            value = std::nextafter(value, T{ +INFINITY });
        } else {
            ++offset;
            value = std::nextafter(value, T{ -INFINITY });
        }
    } while (offset >= -1000 && offset <= 1000);
    return offset;
}

TEMPLATE_TEST_CASE("test details: error_in_ulp()", "[to_number]",
    float, double, long double)
{
    auto ref = TestType{ 1.01L };
    auto value = ref;
    constexpr auto try_distance{ 10 };

    for (int i = try_distance; i != 0; --i)
        value += std::numeric_limits<TestType>::epsilon();
    REQUIRE(error_in_ulp(value, ref) == try_distance);
    REQUIRE(error_in_ulp(ref, value) == -try_distance);
    value = ref;

    for (int i = try_distance; i != 0; --i)
        value -= std::numeric_limits<TestType>::epsilon();
    REQUIRE(error_in_ulp(value, ref) == -try_distance);
    REQUIRE(error_in_ulp(ref, value) == try_distance);
}

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
        REQUIRE(gul14::within_ulp(to_number<TestType>(test.input).value(),
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
    REQUIRE(to_number<TestType>("nan(.)").has_value() == false);
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
        auto const num = min / static_cast<TestType>(i); // Generate a number that is smaller than min (aka subnormal)
        ss.str("");
        ss << num;
        CAPTURE(ss.str());
        REQUIRE(true == gul14::within_ulp(to_number<TestType>(ss.str()).value(), num, lenience));
    }
}

TEMPLATE_TEST_CASE("to_number(): max and overflow floating point", "[to_number]", float, double, long double)
{
    auto const max = std::numeric_limits<TestType>::max();

    auto ss = std::stringstream{ };
    ss << std::setprecision(std::numeric_limits<TestType>::max_digits10) << max;
    auto numb = ss.str();

    auto const lenience = sizeof(TestType) > sizeof(double) ? long_double_lenience : double_lenience;
    REQUIRE(true == gul14::within_ulp(to_number<TestType>(numb).value(), max, lenience));

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
    REQUIRE(true == gul14::within_ulp(to_number<TestType>(numb).value(), lowest, lenience));

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
            REQUIRE(true == gul14::within_ulp(*converted, num, 30));
        } else {
            CAPTURE("normal " + std::to_string(++i_nor));
            REQUIRE(true == gul14::within_ulp(*converted, num, 3));
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
    REQUIRE(gul14::within_ulp(test1.value(), TestType(0.1l), lenience) == true);

    auto const test2 = to_number<TestType>("00000000000000000000000000000000000000000000000000.1"
            "00000000000000000000000000000000000000000000000000");
    REQUIRE(gul14::within_ulp(test2.value(), TestType(0.1l), lenience) == true);

    auto const test3 = to_number<TestType>("00000000000000000000000000000000000000000000000001"
            "e000000000000000000000000000000000000000000000000000");
    REQUIRE(gul14::within_ulp(test3.value(), TestType(1.0l), lenience) == true);

    auto const test4 = to_number<TestType>("00000000000000000000000000000000000000000000000000.1e"
            "0000000000000000000000000000000000000000000000000");
    REQUIRE(gul14::within_ulp(test4.value(), TestType(0.1l), lenience) == true);

    // higher lenience here:
    auto const test5 = to_number<TestType>("0.00000000000000000000000000000000000000000000000001"
            "e0000000000000000000000000000000000000000000000001");
    REQUIRE(gul14::within_ulp(test5.value(), TestType(1.0e-49l), lenience + 1) == true);
}

/* Disabled because doocsdev16's gcc has insufficient constexpr support (but works on
   doocsdev18)
TEMPLATE_TEST_CASE("to_number(): Convert \"42\" to integer types, constexpr", "[to_number]",
                   char, unsigned char, short,
                   unsigned short, int, unsigned int, long, unsigned long, long long,
                   unsigned long long)
{
    constexpr auto cstr = "42";
    constexpr gul14::string_view sv{ cstr, 2 };
    constexpr auto a = to_number<TestType>(sv);
    REQUIRE(a == 42);
}

TEMPLATE_TEST_CASE("to_number(): Convert \"-42\" to integer types, constexpr", "[to_number]",
                   signed char, short, int, long, long long)
{
    constexpr auto cstr = "-42";
    constexpr gul14::string_view sv{ cstr, 3 };
    constexpr auto a = to_number<TestType>(sv);
    REQUIRE(a == -42);
}
*/

/* Disabled because gul14::string_view::find_first_of() is not really constexpr
TEMPLATE_TEST_CASE("to_number(): Floating-point types, constexpr", "[to_number]",
                   float, double, long double)
{
    constexpr auto cstr = "-42.2e1";
    constexpr gul14::string_view sv{ cstr, 7 };

    constexpr auto a = to_number<TestType>(sv);
    REQUIRE(a == TestType{ -42.2e1 });
}
*/

TEST_CASE("test pow10()", "[to_number]")
{
    // Allow one ULP off
    // (except when we allow two ULPs; for very small numbers)
    constexpr auto lenience{ 1 };

    struct TestCase {
        int exponent_;
        long double reference_;
        int lenience_;

        int deviation() const {
            return std::abs(error_in_ulp(gul14::detail::pow10(exponent_), reference_));
        }
    };

    auto cases = std::vector<TestCase>{
        {    0,    1E0L, lenience }, {    -0,    1E-0L, lenience },
        {    1,    1E1L, lenience }, {    -1,    1E-1L, lenience },
        {    2,    1E2L, lenience }, {    -2,    1E-2L, lenience },
        {    3,    1E3L, lenience }, {    -3,    1E-3L, lenience },
        {    4,    1E4L, lenience }, {    -4,    1E-4L, lenience },
        {    5,    1E5L, lenience }, {    -5,    1E-5L, lenience },
        {    6,    1E6L, lenience }, {    -6,    1E-6L, lenience },
        {    7,    1E7L, lenience }, {    -7,    1E-7L, lenience },
        {    8,    1E8L, lenience }, {    -8,    1E-8L, lenience },
        {    9,    1E9L, lenience }, {    -9,    1E-9L, lenience },
        {   10,   1E10L, lenience }, {   -10,   1E-10L, lenience },
        {   11,   1E11L, lenience }, {   -11,   1E-11L, lenience },
        {   12,   1E12L, lenience }, {   -12,   1E-12L, lenience },
        {   13,   1E13L, lenience }, {   -13,   1E-13L, lenience },
        {   14,   1E14L, lenience }, {   -14,   1E-14L, lenience },
        {   15,   1E15L, lenience }, {   -15,   1E-15L, lenience },
        {   16,   1E16L, lenience }, {   -16,   1E-16L, lenience },
        {   17,   1E17L, lenience }, {   -17,   1E-17L, lenience },
        {   18,   1E18L, lenience }, {   -18,   1E-18L, lenience },
        {   19,   1E19L, lenience }, {   -19,   1E-19L, lenience },
        {   20,   1E20L, lenience }, {   -20,   1E-20L, lenience },
        {   21,   1E21L, lenience }, {   -21,   1E-21L, lenience },
        {   22,   1E22L, lenience }, {   -22,   1E-22L, lenience },
        {   23,   1E23L, lenience }, {   -23,   1E-23L, lenience },
        {   24,   1E24L, lenience }, {   -24,   1E-24L, lenience },
        {   25,   1E25L, lenience }, {   -25,   1E-25L, lenience },
        {   26,   1E26L, lenience }, {   -26,   1E-26L, lenience },
        {   27,   1E27L, lenience }, {   -27,   1E-27L, lenience },
        {   28,   1E28L, lenience }, {   -28,   1E-28L, lenience },
        {   29,   1E29L, lenience }, {   -29,   1E-29L, lenience },
        {   30,   1E30L, lenience }, {   -30,   1E-30L, lenience },
        {  137,  1E137L, lenience }, {  -137,  1E-137L, lenience },
        {  267,  1E267L, lenience }, {  -267,  1E-267L, lenience },
#if LDBL_MAX_10_EXP > 500
        {  678,  1E678L, lenience },
        { 1234, 1E1234L, lenience },
        { 3333, 1E3333L, lenience },
        { 4095, 1E4095L, lenience },
        { 4351, 1E4351L, lenience },
        { 4607, 1E4607L, lenience },
        { 4876, 1E4876L, lenience },
        { 4877, 1E4877L, lenience },
        { 4878, 1E4878L, lenience },
        { 4879, 1E4879L, lenience },
        { 4932, 1E4932L, lenience },
#endif
#if LDBL_MIN_10_EXP < -500
        {  -678,  1E-678L, lenience },
        { -1234, 1E-1234L, lenience },
        { -3333, 1E-3333L, lenience },
        { -4095, 1E-4095L, lenience + 1 },
        { -4351, 1E-4351L, lenience },
        { -4607, 1E-4607L, lenience + 1 },
        { -4876, 1E-4876L, lenience },
        { -4877, 1E-4877L, lenience },
        { -4878, 1E-4878L, lenience },
        { -4879, 1E-4879L, lenience },
        { -4932, 1E-4932L, lenience },
#endif
    };

    for (auto const& c : cases)
        REQUIRE(c.deviation() <= c.lenience_);

    // Overflows...
    REQUIRE(std::isinf(gul14::detail::pow10(5000)));
    REQUIRE(std::isinf(gul14::detail::pow10(8192)));

    REQUIRE(gul14::detail::pow10(-5000) == 0);
    REQUIRE(gul14::detail::pow10(-8192) == 0);
}

// vi:ts=4:sw=4:sts=4:et
