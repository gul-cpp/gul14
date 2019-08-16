/**
 * \file   test_tokenize.cc
 * \author \ref contributors
 * \date   Created on September 3, 2018
 * \brief  Test suite for tokenize() and tokenize_sv() from the General Utility
 *         Library.
 *
 * \copyright Copyright 2018 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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
#include "gul/tokenize.h"

using namespace std::literals;

using gul::tokenize;
using gul::tokenize_sv;

TEST_CASE("gul::tokenize() works with \"Hello World\"", "[tokenize]")
{
    auto tokens = tokenize("Hello World");

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0] == "Hello");
    REQUIRE(tokens[0].length() == 5);
    REQUIRE(tokens[1] == "World");
    REQUIRE(tokens[1].length() == 5);
}

TEST_CASE("gul::tokenize() works with \" Hello World\" with odd whitespace", "[tokenize]")
{
    auto tokens = tokenize("\t Hello\n\rWorld\t\t  ");

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0] == "Hello");
    REQUIRE(tokens[1] == "World");
}

TEST_CASE("gul::tokenize() works with custom delimiters and null characters", "[tokenize]")
{
    const auto input = "\t Hel\0lo\n\rWorld\t\t  "s;
    auto tokens = tokenize(input, " \t\n\r");
    
    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0] == "Hel\0lo"s);
    REQUIRE(tokens[0].length() == 6);
    REQUIRE(tokens[1] == "World");
}

TEST_CASE("gul::tokenize() works with empty delimiter string", "[tokenize]")
{
    auto tokens = tokenize("Hello World", "");
    
    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0] == "Hello World");
}

TEST_CASE("gul::tokenize_sv() works with \"Hello World\"", "[tokenize]")
{
    auto tokens = tokenize_sv("Hello World");

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0] == "Hello");
    REQUIRE(tokens[0].length() == 5);
    REQUIRE(tokens[1] == "World");
    REQUIRE(tokens[1].length() == 5);
}

TEST_CASE("gul::tokenize_sv() works with \" Hello World\" with odd whitespace", "[tokenize]")
{
    auto tokens = tokenize_sv("\t Hello\n\rWorld\t\t  ");

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0] == "Hello");
    REQUIRE(tokens[1] == "World");
}

TEST_CASE("gul::tokenize_sv() works with custom delimiters and null characters", "[tokenize]")
{
    const auto input = "\t Hel\0lo\n\rWorld\t\t  "s;
    auto tokens = tokenize_sv(input, " \t\n\r");

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0] == "Hel\0lo"s);
    REQUIRE(tokens[0].length() == 6);
    REQUIRE(tokens[1] == "World");
}

TEST_CASE("gul::tokenize_sv() works with empty delimiter string", "[tokenize]")
{
    auto tokens = tokenize_sv("Hello World", "");

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0] == "Hello World");
}
