/**
 * \file   test_string_util.cc
 * \author \ref contributors
 * \date   Created on August 11, 2021
 * \brief  Unit tests for safe_string() and hex_string().
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

#include <array>
#include <iomanip>
#include <sstream>
#include <vector>
#include "gul14/catch.h"
#include "gul14/SmallVector.h"
#include "gul14/string_util.h"

using gul14::safe_string;
using gul14::hex_string;

using namespace std::literals;

TEST_CASE("hex_string(Integer)", "[string_util]")
{
    std::stringstream ss;

    for (int i = 0; i != 256; ++i)
    {
        auto val = static_cast<unsigned char>(i);
        CAPTURE(val);
        ss.str("");
        ss << std::setfill('0') << std::setw(2) << std::hex << int(val);

        REQUIRE(ss.str() == hex_string(val));
    }

    REQUIRE(hex_string(static_cast<uint16_t>(15)) == "000f");
    REQUIRE(hex_string(static_cast<uint16_t>(16)) == "0010");
    REQUIRE(hex_string(static_cast<int16_t>(15)) == "000f");
    REQUIRE(hex_string(static_cast<int16_t>(16)) == "0010");
    REQUIRE(hex_string(static_cast<uint32_t>(65535)) == "0000ffff");
    REQUIRE(hex_string(static_cast<uint32_t>(65536)) == "00010000");
    REQUIRE(hex_string(static_cast<int32_t>(-1)) == "ffffffff");
    REQUIRE(hex_string(static_cast<int64_t>(-1)) == "ffffffffffffffff");
}

TEST_CASE("hex_string(Iterator, Iterator, string_view)", "[string_util]")
{
    std::array<unsigned char, 4> uc{ { 0, 15, 16, 255 } };
    REQUIRE(hex_string(uc.begin(), uc.begin()) == "");
    REQUIRE(hex_string(uc.begin(), uc.begin(), "-") == "");
    REQUIRE(hex_string(uc.begin(), uc.end()) == "000f10ff");
    REQUIRE(hex_string(uc.begin(), uc.end(), "-") == "00-0f-10-ff");

    uint16_t us[] = { 256, 255 };
    REQUIRE(hex_string(std::begin(us), std::begin(us)) == "");
    REQUIRE(hex_string(std::begin(us), std::begin(us), "-") == "");
    REQUIRE(hex_string(std::begin(us), std::end(us)) == "010000ff");
    REQUIRE(hex_string(std::begin(us), std::end(us), " * ") == "0100 * 00ff");

    int64_t sll[] = { 256, -1, 0 };
    REQUIRE(hex_string(std::begin(sll), std::begin(sll)) == "");
    REQUIRE(hex_string(std::begin(sll), std::begin(sll), "-") == "");
    REQUIRE(hex_string(std::begin(sll), std::end(sll)) ==
            "0000000000000100ffffffffffffffff0000000000000000");
    REQUIRE(hex_string(std::begin(sll), std::end(sll), " ") ==
            "0000000000000100 ffffffffffffffff 0000000000000000");
}

TEST_CASE("hex_string(Array, string_view)", "[string_util]")
{
    uint16_t us[] = { 256, 255 };
    REQUIRE(hex_string(us) == "010000ff");
    REQUIRE(hex_string(us, " - ") == "0100 - 00ff");

    int64_t sll[] = { 256, -1, 0 };
    REQUIRE(hex_string(sll) == "0000000000000100ffffffffffffffff0000000000000000");
    REQUIRE(hex_string(sll, " ") == "0000000000000100 ffffffffffffffff 0000000000000000");
}

TEST_CASE("hex_string(Container, string_view)", "[string_util]")
{
    std::array<unsigned char, 4> uc{ { 0, 15, 16, 255 } };
    REQUIRE(hex_string(uc) == "000f10ff");
    REQUIRE(hex_string(uc, " ") == "00 0f 10 ff");

    std::vector<uint16_t> us = { 256, 255 };
    REQUIRE(hex_string(us) == "010000ff");
    REQUIRE(hex_string(us, "-") == "0100-00ff");

    gul14::SmallVector<int64_t, 3> sll = { 256, -1, 0 };
    REQUIRE(hex_string(sll) == "0000000000000100ffffffffffffffff0000000000000000");
    REQUIRE(hex_string(sll, "/") == "0000000000000100/ffffffffffffffff/0000000000000000");
}

TEST_CASE("safe_string()", "[string_util]")
{
    REQUIRE(safe_string(nullptr, 0) == "");
    REQUIRE(safe_string(nullptr, 10) == "");
    REQUIRE(safe_string("hello", 0) == "");
    REQUIRE(safe_string("hello", 10) == "hello");
    REQUIRE(safe_string("hello\0", 6) == "hello");
    REQUIRE(safe_string("hello\0world", 11) == "hello");
}
