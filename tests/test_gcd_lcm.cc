/**
 * \file   test_gcd_lcm.cc
 * \author \ref contributors
 * \date   Created on August 5, 2022
 * \brief  Test suite for gcd() and lcm().
 *
 * \copyright Copyright 2022 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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

#include <type_traits>

#include "gul14/catch.h"
#include "gul14/gcd_lcm.h"

using gul14::gcd;
using gul14::lcm;

TEST_CASE("gcd(): Explicit test cases", "[gcd_lcm]")
{
    REQUIRE(gcd(2, 2) == 2);
    REQUIRE(gcd(2, 3) == 1);
    REQUIRE(gcd(3, 2) == 1);
    REQUIRE(gcd(2, 4) == 2);
    REQUIRE(gcd(4, 2) == 2);

    REQUIRE(gcd(10, 15) == 5);
    REQUIRE(gcd(15, 10) == 5);
    REQUIRE(gcd(-10, 15) == 5);
    REQUIRE(gcd(15, -10) == 5);

    REQUIRE(gcd(42, 2) == 2);
    REQUIRE(gcd(42, -2) == 2);
    REQUIRE(gcd(-42, -2) == 2);
    REQUIRE(gcd(-42, 2) == 2);

    REQUIRE(gcd(9, 42) == 3);
    REQUIRE(gcd(42, 9) == 3);
    REQUIRE(gcd(-42, 9) == 3);
    REQUIRE(gcd(-9, 42) == 3);

    REQUIRE(gcd(4'000'000'020, 3'999'999'990) == 30);

    REQUIRE(gcd(std::int16_t{ -256 }, std::int8_t{ -128 }) == std::int16_t{ 128 }); // Fails with GCC 9.3.0 std::gcd()
    REQUIRE(gcd(-288LL, std::int8_t{ -128 }) == 32LL);

    // Zeros
    REQUIRE(gcd(0, 2) == 2);
    REQUIRE(gcd(2, 0) == 2);
    REQUIRE(gcd(0, 0) == 0);
}

TEST_CASE("gcd: Mixed types", "[gcd_lcm]")
{
    static_assert(std::is_same<decltype(gcd(42LL, 2)), long long>::value,
        "gcd(long long, int) -> long long");
    static_assert(std::is_same<decltype(gcd(42, 2LL)), long long>::value,
        "gcd(int, long long) -> long long");
    static_assert(std::is_same<decltype(gcd(42, std::int8_t{ 2 })), int>::value,
        "gcd(int, int8_t) -> int");
    static_assert(std::is_same<decltype(gcd(short{ 2 }, 42L)), long>::value,
        "gcd(short, long) -> long");

    static_assert(std::is_same<decltype(gcd(42ULL, 2U)), unsigned long long>::value,
        "gcd(unsigned long long, unsigned int) -> unsigned long long");
    static_assert(std::is_same<decltype(gcd(42U, 2ULL)), unsigned long long>::value,
        "gcd(unsigned int, unsigned long long) -> unsigned long long");
    static_assert(std::is_same<decltype(gcd(42U, std::uint8_t{ 2 })), unsigned int>::value,
        "gcd(unsigned int, uint8_t) -> unsigned int");
    static_assert(std::is_same<decltype(gcd(static_cast<unsigned short>(2), 42UL)), unsigned long>::value,
        "gcd(unsigned short, unsigned long) -> unsigned long");
}

TEST_CASE("lcm(): Explicit test cases", "[gcd_lcm]")
{
    REQUIRE(lcm(1, 2) == 2);
    REQUIRE(lcm(2, 1) == 2);
    REQUIRE(lcm(2, 3) == 6);
    REQUIRE(lcm(3, 2) == 6);
    REQUIRE(lcm(2, 4) == 4);
    REQUIRE(lcm(4, 2) == 4);

    REQUIRE(lcm(10, 15) == 30);
    REQUIRE(lcm(15, 10) == 30);
    REQUIRE(lcm(-10, 15) == 30);
    REQUIRE(lcm(15, -10) == 30);

    REQUIRE(lcm(7, 5) == 35);
    REQUIRE(lcm(-7, 5) == 35);
    REQUIRE(lcm(7, -5) == 35);
    REQUIRE(lcm(-7, -5) == 35);

    REQUIRE(lcm(6, 10) == 30);
    REQUIRE(lcm(60, 100) == 300);
    REQUIRE(lcm(600, 1'000) == 3'000);
    REQUIRE(lcm(6'000, 10'000) == 30'000);

    REQUIRE(lcm(60'000, 100'000) == 300'000);
    REQUIRE(lcm(-60'000, 100'000) == 300'000);
    REQUIRE(lcm(60'000, -100'000) == 300'000);
    REQUIRE(lcm(-60'000, -100'000) == 300'000);
    REQUIRE(lcm(60'000U, 100'000U) == 300'000U);

    #if defined(INT64_MAX)
    REQUIRE(lcm(60'000'000'000, 100'000'000'000) == 300'000'000'000);
    REQUIRE(lcm(-60'000'000'000, 100'000'000'000) == 300'000'000'000);
    REQUIRE(lcm(60'000'000'000, -100'000'000'000) == 300'000'000'000);
    REQUIRE(lcm(-60'000'000'000, -100'000'000'000) == 300'000'000'000);
    REQUIRE(lcm(60'000'000'000U, 100'000'000'000U) == 300'000'000'000U);
    #endif

    REQUIRE(lcm(std::int16_t{ -256 }, std::int8_t{ -128 }) == std::int16_t{ 256 }); // Fails with GCC 9.3.0 std::lcm()
    REQUIRE(lcm(-288LL, std::int8_t{ -128 }) == 1152LL);

    // Zeros
    REQUIRE(lcm(0, 2) == 0);
    REQUIRE(lcm(2, 0) == 0);
    REQUIRE(lcm(0, 0) == 0);
}

TEST_CASE("lcm: Mixed types", "[gcd_lcm]")
{
    static_assert(std::is_same<decltype(lcm(42LL, 2)), long long>::value,
        "lcm(long long, int) -> long long");
    static_assert(std::is_same<decltype(lcm(42, 2LL)), long long>::value,
        "lcm(int, long long) -> long long");
    static_assert(std::is_same<decltype(lcm(42, std::int8_t{ 2 })), int>::value,
        "lcm(int, int8_t) -> int");
    static_assert(std::is_same<decltype(lcm(short{ 2 }, 42L)), long>::value,
        "lcm(short, long) -> long");

    static_assert(std::is_same<decltype(lcm(42ULL, 2U)), unsigned long long>::value,
        "lcm(unsigned long long, unsigned int) -> unsigned long long");
    static_assert(std::is_same<decltype(lcm(42U, 2ULL)), unsigned long long>::value,
        "lcm(unsigned int, unsigned long long) -> unsigned long long");
    static_assert(std::is_same<decltype(lcm(42U, std::uint8_t{ 2 })), unsigned int>::value,
        "lcm(unsigned int, uint8_t) -> unsigned int");
    static_assert(std::is_same<decltype(lcm(static_cast<unsigned short>(2), 42UL)), unsigned long>::value,
        "lcm(unsigned short, unsigned long) -> unsigned long");
}
