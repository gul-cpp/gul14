/**
 * \file   test_case_ascii.cc
 * \author \ref contributors
 * \date   Created on May 28, 2019
 * \brief  Test suite for lowercase_ascii(), lowercase_ascii_inplace(), uppercase_ascii(),
 *         and uppercase_ascii_inplace().
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

#include "gul14/case_ascii.h"
#include "gul14/catch.h"

using namespace std::literals;
using namespace gul;

TEST_CASE("lowercase_ascii()", "[case_ascii]")
{
    const std::string ustr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ-+öä";
    const std::string lstr = "abcdefghijklmnopqrstuvwxyz-+öä";

    REQUIRE(ustr.length() == lstr.length());

    REQUIRE(lowercase_ascii(ustr) == lstr);

    for (auto i = 0u; i < ustr.length(); i++)
        REQUIRE(lowercase_ascii(ustr[i]) == lstr[i]);
}

TEST_CASE("lowercase_ascii(char) is constexpr", "[case_ascii]")
{
    constexpr char c = lowercase_ascii('B');
    REQUIRE(c == 'b');
}

TEST_CASE("lowercase_ascii_inplace()", "[case_ascii]")
{
    std::string ustr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ-+öä";
    const std::string lstr = "abcdefghijklmnopqrstuvwxyz-+öä";

    REQUIRE(ustr.length() == lstr.length());

    const auto adr_old = &ustr;

    auto &ref = lowercase_ascii_inplace(ustr);

    REQUIRE(ustr == lstr);

    auto adr_returned = &ref;

    REQUIRE(adr_returned == adr_old);
}

TEST_CASE("uppercase_ascii()", "[case_ascii]")
{
    const std::string ustr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ-+öä";
    const std::string lstr = "abcdefghijklmnopqrstuvwxyz-+öä";

    REQUIRE(ustr.length() == lstr.length());

    REQUIRE(uppercase_ascii(lstr) == ustr);

    for (auto i = 0u; i < ustr.length(); i++)
        REQUIRE(uppercase_ascii(lstr[i]) == ustr[i]);
}

TEST_CASE("uppercase_ascii(char) is constexpr", "[case_ascii]")
{
    constexpr char c = uppercase_ascii('b');
    REQUIRE(c == 'B');
}

TEST_CASE("uppercase_ascii_inplace()", "[case_ascii]")
{
    const std::string ustr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ-+öä";
    std::string lstr = "abcdefghijklmnopqrstuvwxyz-+öä";

    REQUIRE(ustr.length() == lstr.length());

    const auto adr_old = &lstr;

    auto &ref = uppercase_ascii_inplace(lstr);

    REQUIRE(lstr == ustr);

    auto adr_returned = &ref;

    REQUIRE(adr_returned == adr_old);
}

// vi:ts=4:sw=4:sts=4:et
