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
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <type_traits>
#include <unordered_set>
#include "gul14/catch.h"
#include "gul14/SmallVector.h"
#include "gul14/string_view.h"
#include "gul14/tokenize.h"
#include "gul14/case_ascii.h"

using namespace std::literals;

using gul14::tokenize;
using gul14::tokenize_sv;


#define TEST_BOTH_INTO(tokens) \
    for (auto i{ 0u }, j{ 0u }; j = 0, i < token.size(); ++i) \
        for (auto tokens = std::move(token[i]); j < 1; ++j)
#define TEST_BOTH_MESSAGE() \
        INFO("Test using tokenize"s + (i == 1 ? "_sv" : "") + "()")

TEMPLATE_TEST_CASE("tokenize(\"Hello World\")", "[tokenize]",
    std::vector<std::string>, std::vector<gul14::string_view>,
    (gul14::SmallVector<std::string, 2>), (gul14::SmallVector<gul14::string_view, 3>))
{
    auto token = std::vector<TestType>{
        tokenize<TestType>("Hello World"),
        tokenize_sv<TestType>("Hello World")
    };
    TEST_BOTH_INTO(tokens) {
        TEST_BOTH_MESSAGE();

        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0] == "Hello");
        REQUIRE(tokens[0].length() == 5);
        REQUIRE(tokens[1] == "World");
        REQUIRE(tokens[1].length() == 5);
    }
}

TEMPLATE_TEST_CASE("tokenize(): \" Hello World\" with odd whitespace", "[tokenize]",
    std::vector<std::string>, std::vector<gul14::string_view>,
    (gul14::SmallVector<std::string, 2>), (gul14::SmallVector<gul14::string_view, 3>))
{
    auto token = std::vector<TestType>{
        tokenize<TestType>("\t Hello\n\rWorld\t\t  "),
        tokenize_sv<TestType>("\t Hello\n\rWorld\t\t  ")
    };
    TEST_BOTH_INTO(tokens) {
        TEST_BOTH_MESSAGE();

        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0] == "Hello");
        REQUIRE(tokens[1] == "World");
    }
}

TEMPLATE_TEST_CASE("tokenize(): custom delimiters and null characters", "[tokenize]",
    std::vector<std::string>, std::vector<gul14::string_view>,
    (gul14::SmallVector<std::string, 2>), (gul14::SmallVector<gul14::string_view, 3>))
{
    const auto input = "\t Hel\0lo\n\rWorld\t\t  "s;
    auto token = std::vector<TestType>{
        tokenize<TestType>(input, " \t\n\r"),
        tokenize_sv<TestType>(input, " \t\n\r")
    };
    TEST_BOTH_INTO(tokens) {
        TEST_BOTH_MESSAGE();

        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0] == "Hel\0lo"s);
        REQUIRE(tokens[0].length() == 6);
        REQUIRE(tokens[1] == "World");
    }
}

TEMPLATE_TEST_CASE("tokenize(): empty delimiter string", "[tokenize]",
    std::vector<std::string>, std::vector<gul14::string_view>,
    (gul14::SmallVector<std::string, 2>), (gul14::SmallVector<gul14::string_view, 3>))
{
    auto token = std::vector<TestType>{
        tokenize<TestType>("Hello World", ""),
        tokenize_sv<TestType>("Hello World", "")
    };
    TEST_BOTH_INTO(tokens) {
        TEST_BOTH_MESSAGE();

        REQUIRE(tokens.size() == 1);
        REQUIRE(tokens[0] == "Hello World");
    }
}

TEMPLATE_TEST_CASE("tokenize(\"Hello World\") into associative containers", "[tokenize]",
    std::set<std::string>, std::multiset<std::string>,
    std::unordered_set<std::string>, std::unordered_multiset<std::string>)
{
    auto token = std::vector<TestType>{
        tokenize<TestType>("Hello World"),
        tokenize_sv<TestType>("Hello World")
    };

    TEST_BOTH_INTO(tokens) {
        TEST_BOTH_MESSAGE();

        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens.find("Hello") != tokens.end());
        REQUIRE(tokens.find("Hello")->length() == 5);
        REQUIRE(tokens.find("World") != tokens.end());
        REQUIRE(tokens.find("World")->length() == 5);
    }
}

TEMPLATE_TEST_CASE("tokenize(\"Hello World\") into std::list and std::queue", "[tokenize]",
    std::list<std::string>, std::deque<gul14::string_view>, std::queue<std::string>)
{
    auto token = std::vector<TestType>{
        tokenize<TestType>("Hello World"),
        tokenize_sv<TestType>("Hello World")
    };

    TEST_BOTH_INTO(tokens) {
        TEST_BOTH_MESSAGE();

        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens.front().compare("Hello") == 0);
        REQUIRE(tokens.back().compare("World") == 0);
    }
}

TEMPLATE_TEST_CASE("tokenize(\"Hello World\") into std::stack", "[tokenize]",
    std::stack<std::string>)
{
    auto token = std::vector<TestType>{
        tokenize<TestType>("Hello World"),
        tokenize_sv<TestType>("Hello World")
    };

    TEST_BOTH_INTO(tokens) {
        TEST_BOTH_MESSAGE();

        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens.top() == "World"s);
        tokens.pop();
        REQUIRE(tokens.top() == "Hello"s);
    }
}

TEMPLATE_TEST_CASE("tokenize(\"Hello World\")'s return type", "[tokenize]",
    std::vector<std::string>, std::vector<gul14::string_view>,
    (gul14::SmallVector<std::string, 2>), (gul14::SmallVector<gul14::string_view, 3>),
    std::set<std::string>, std::multiset<std::string>,
    std::unordered_set<std::string>, std::unordered_multiset<std::string>,
    std::list<std::string>, std::deque<gul14::string_view>, std::queue<std::string>,
    std::stack<std::string>)
{
    decltype(auto) ret = tokenize<TestType>("A Three Legged Horse");
    REQUIRE(std::is_same<std::remove_const_t<decltype(ret)>, TestType>::value == true);
    decltype(auto) ret_sv = tokenize_sv<TestType>("A Three Legged Horse");
    REQUIRE(std::is_same<std::remove_const_t<decltype(ret_sv)>, TestType>::value == true);
}

TEST_CASE("tokenize(\"Hello World\") into std::map", "[tokenize]")
{
    using TestType = std::map<std::string, std::string>;
    auto emplace = [](TestType& c, gul14::string_view sv) { c.emplace(sv, gul14::uppercase_ascii(sv)); };

    auto token = std::vector<TestType>{
        tokenize<TestType>("Hello World", " ", emplace),
        tokenize_sv<TestType>("Hello World", " ", emplace)
    };

    TEST_BOTH_INTO(tokens) {
        TEST_BOTH_MESSAGE();

        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens.find("Hello") != tokens.end());
        REQUIRE(tokens["Hello"] == "HELLO"s);
        REQUIRE(tokens.find("World") != tokens.end());
        REQUIRE(tokens["World"] == "WORLD"s);
    }

    decltype(auto) ret = tokenize<TestType>("Hello Moon", " ", emplace);
    REQUIRE(std::is_same<std::remove_const_t<decltype(ret)>, TestType>::value == true);
    decltype(auto) ret_sv = tokenize_sv<TestType>("Hello Moon", " ", emplace);
    REQUIRE(std::is_same<std::remove_const_t<decltype(ret_sv)>, TestType>::value == true);
}

// vi:ts=4:sw=4:sts=4:et
