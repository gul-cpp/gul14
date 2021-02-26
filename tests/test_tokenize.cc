/**
 * \file   test_tokenize.cc
 * \author \ref contributors
 * \date   Created on September 3, 2018
 * \brief  Test suite for tokenize() and tokenize_sv().
 *
 * \copyright Copyright 2018-2021 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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

#include <deque>
#include <list>
#include <set>
#include <type_traits>
#include "gul14/catch.h"
#include "gul14/SmallVector.h"
#include "gul14/string_view.h"
#include "gul14/tokenize.h"

using namespace std::literals;

using gul14::tokenize;
using gul14::tokenize_sv;

TEMPLATE_TEST_CASE("tokenize(\"Hello World\")", "[tokenize]",
    std::vector<std::string>, std::vector<gul14::string_view>,
    (gul14::SmallVector<std::string, 2>), (gul14::SmallVector<gul14::string_view, 3>))
{
    auto tokens = tokenize<TestType>("Hello World");

    using ReturnType = typename std::remove_const<decltype(tokens)>::type;
    static_assert(std::is_same<ReturnType, TestType>::value == true,
        "tokenize() returns wrong type");

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0] == "Hello");
    REQUIRE(tokens[0].length() == 5);
    REQUIRE(tokens[1] == "World");
    REQUIRE(tokens[1].length() == 5);
}

TEMPLATE_TEST_CASE("tokenize(): \" Hello World\" with odd whitespace", "[tokenize]",
    std::vector<std::string>, std::vector<gul14::string_view>,
    (gul14::SmallVector<std::string, 2>), (gul14::SmallVector<gul14::string_view, 3>))
{
    auto tokens = tokenize<TestType>("\t Hello\n\rWorld\t\t  ");

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0] == "Hello");
    REQUIRE(tokens[1] == "World");
}

TEMPLATE_TEST_CASE("tokenize(): custom delimiters and null characters", "[tokenize]",
    std::vector<std::string>, std::vector<gul14::string_view>,
    (gul14::SmallVector<std::string, 2>), (gul14::SmallVector<gul14::string_view, 3>))
{
    const auto input = "\t Hel\0lo\n\rWorld\t\t  "s;
    auto tokens = tokenize<TestType>(input, " \t\n\r");

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0] == "Hel\0lo"s);
    REQUIRE(tokens[0].length() == 6);
    REQUIRE(tokens[1] == "World");
}

TEMPLATE_TEST_CASE("tokenize(): empty delimiter string", "[tokenize]",
    std::vector<std::string>, std::vector<gul14::string_view>,
    (gul14::SmallVector<std::string, 2>), (gul14::SmallVector<gul14::string_view, 3>))
{
    auto tokens = tokenize<TestType>("Hello World", "");

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0] == "Hello World");
}

TEMPLATE_TEST_CASE("tokenize(\"Hello World\"): associative containers", "[tokenize]",
    std::set<std::string>, std::multiset<std::string>)
{
    auto emplace = [](TestType& c, gul14::string_view sv) { c.emplace(sv); };

    const auto x = tokenize<TestType>("Hello World", " ", emplace);

    REQUIRE(x.size() == 2);
    REQUIRE(std::find(x.begin(), x.end(), "Hello") != x.end());
    REQUIRE(std::find(x.begin(), x.end(), "World") != x.end());
}

TEMPLATE_TEST_CASE("tokenize(\"Hello World\"): standard containers", "[tokenize]",
    std::list<std::string>, std::deque<gul14::string_view>)
{
    auto tokens = tokenize<TestType>("Hello World");

    using ReturnType = typename std::remove_const<decltype(tokens)>::type;
    static_assert(std::is_same<ReturnType, TestType>::value == true,
        "tokenize() returns wrong type");

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens.front().compare("Hello") == 0);
    REQUIRE(tokens.back().compare("World") == 0);
}

TEMPLATE_TEST_CASE("tokenize_sv(): \"Hello World\"", "[tokenize]",
    std::vector<std::string>, std::vector<gul14::string_view>,
    (gul14::SmallVector<std::string, 2>), (gul14::SmallVector<gul14::string_view, 3>))
{
    auto tokens = tokenize_sv<TestType>("Hello World");

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0] == "Hello");
    REQUIRE(tokens[0].length() == 5);
    REQUIRE(tokens[1] == "World");
    REQUIRE(tokens[1].length() == 5);
}

TEMPLATE_TEST_CASE("tokenize_sv(): \" Hello World\" with odd whitespace", "[tokenize]",
    std::vector<std::string>, std::vector<gul14::string_view>,
    (gul14::SmallVector<std::string, 2>), (gul14::SmallVector<gul14::string_view, 3>))
{
    auto tokens = tokenize_sv<TestType>("\t Hello\n\rWorld\t\t  ");

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0] == "Hello");
    REQUIRE(tokens[1] == "World");
}

TEMPLATE_TEST_CASE("tokenize_sv(): custom delimiters and null characters", "[tokenize]",
    std::vector<std::string>, std::vector<gul14::string_view>,
    (gul14::SmallVector<std::string, 2>), (gul14::SmallVector<gul14::string_view, 3>))
{
    const auto input = "\t Hel\0lo\n\rWorld\t\t  "s;
    auto tokens = tokenize_sv<TestType>(input, " \t\n\r");

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0] == "Hel\0lo"s);
    REQUIRE(tokens[0].length() == 6);
    REQUIRE(tokens[1] == "World");
}

TEMPLATE_TEST_CASE("tokenize_sv(): empty delimiter string", "[tokenize]",
    std::vector<std::string>, std::vector<gul14::string_view>,
    (gul14::SmallVector<std::string, 2>), (gul14::SmallVector<gul14::string_view, 3>))
{
    auto tokens = tokenize_sv<TestType>("Hello World", "");

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0] == "Hello World");
}

TEMPLATE_TEST_CASE("tokenize_sv(\"Hello World\"): standard containers", "[tokenize]",
    std::list<gul14::string_view>, std::deque<gul14::string_view>)
{
    auto tokens = tokenize_sv<TestType>("Hello World");

    using ReturnType = typename std::remove_const<decltype(tokens)>::type;
    static_assert(std::is_same<ReturnType, TestType>::value == true,
        "tokenize_sv() returns wrong type");

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens.front().compare("Hello") == 0);
    REQUIRE(tokens.back().compare("World") == 0);
}

TEMPLATE_TEST_CASE("tokenize_sv(\"Hello World\"): associative containers", "[tokenize]",
    std::set<gul14::string_view>, std::multiset<gul14::string_view>)
{
    auto emplace = [](TestType& c, gul14::string_view sv) { c.emplace(sv); };

    const auto x = tokenize<TestType>("Hello World", " ", emplace);

    REQUIRE(x.size() == 2);
    REQUIRE(std::find(x.begin(), x.end(), "Hello") != x.end());
    REQUIRE(std::find(x.begin(), x.end(), "World") != x.end());
}

// vi:ts=4:sw=4:sts=4:et
