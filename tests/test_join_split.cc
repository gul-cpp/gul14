/**
 * \file   test_join_split.cc
 * \author \ref contributors
 * \date   Created on August 31, 2018
 * \brief  Test suite for join(), split(), and split_sv().
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

#include <forward_list>
#include <list>
#include <queue>
#include <regex>
#include <set>
#include <string>
#include <type_traits>
#include <vector>
#include "gul14/catch.h"
#include "gul14/join_split.h"
#include "gul14/SmallVector.h"

using namespace std::literals::string_literals;
using gul14::SmallVector;
using gul14::split;
using gul14::split_sv;
using gul14::string_view;
using gul14::join;

TEST_CASE("split(string_view, string_view) with default return type", "[join_split]")
{
    auto const x = split("Hello world", " ");
    using ReturnType = typename std::remove_const_t<decltype(x)>;
    static_assert(std::is_same<ReturnType, std::vector<std::string>>::value == true,
        "split() returns wrong type");
    REQUIRE(x.size() == 2);
    REQUIRE(x[0] == "Hello");
    REQUIRE(x[1] == "world");
}

TEMPLATE_TEST_CASE("split(string_view, string_view)", "[join_split]",
    std::vector<std::string>, std::vector<gul14::string_view>,
    (SmallVector<std::string, 3>), (SmallVector<gul14::string_view, 4>))
{
    auto const x = split<TestType>("Testmenoe", "X");
    using ReturnType = typename std::remove_const<decltype(x)>::type;
    static_assert(std::is_same<ReturnType, TestType>::value == true,
        "split() returns wrong type");
    REQUIRE(x.size() == 1);
    REQUIRE(x[0] == "Testmenoe");

    auto const y2 = split<TestType>("Test\nme\nnoe", "\n");
    REQUIRE(y2.size() == 3);
    REQUIRE(y2[0] == "Test"s);
    REQUIRE(y2[1] == "me"s);
    REQUIRE(y2[2] == "noe"s);

    auto const xy = split<TestType>("TaaaT", "aa");
    REQUIRE(xy.size() == 2);
    REQUIRE(xy[0] == "T"s);
    REQUIRE(xy[1] == "aT"s);

    auto const z2 = split<TestType>("TaaaT", "");
    REQUIRE(z2.size() == 7);
    REQUIRE(z2[0] == ""s);
    REQUIRE(z2[1] == "T"s);
    REQUIRE(z2[2] == "a"s);
    REQUIRE(z2[3] == "a"s);
    REQUIRE(z2[4] == "a"s);
    REQUIRE(z2[5] == "T"s);
    REQUIRE(z2[6] == ""s);
}

TEMPLATE_TEST_CASE("split<std::list<...>>()", "[join_split]",
    std::list<std::string>, std::list<gul14::string_view>)
{
    auto const x = split<TestType>("Hello World", " ");

    REQUIRE(x.size() == 2);
    REQUIRE(x.front() == "Hello");
    REQUIRE(x.back() == "World");
}

TEMPLATE_TEST_CASE("split<std::queue<...>>()", "[join_split]",
    std::queue<std::string>, std::queue<gul14::string_view>)
{
    auto emplace = [](TestType& c, gul14::string_view sv) { c.emplace(sv); };

    auto const x = split<TestType>("Hello World", " ", emplace);

    REQUIRE(x.size() == 2);
    REQUIRE(x.front() == "Hello");
    REQUIRE(x.back() == "World");
}

TEMPLATE_TEST_CASE("split() with associative containers", "[join_split]",
    std::set<std::string>, std::multiset<std::string>)
{
    auto emplace = [](TestType& c, gul14::string_view sv) { c.emplace(sv); };

    auto const x = split<TestType>("Hello World", " ", emplace);

    REQUIRE(x.size() == 2);
    REQUIRE(std::find(x.begin(), x.end(), "Hello") != x.end());
    REQUIRE(std::find(x.begin(), x.end(), "World") != x.end());
}

TEMPLATE_TEST_CASE("split(string_view, regex)", "[join_split]",
    std::vector<std::string>, std::vector<gul14::string_view>,
    (SmallVector<std::string, 3>), (SmallVector<gul14::string_view, 4>))
{
    auto const x = split<TestType>("Testmenoe", std::regex{ "X" });
    REQUIRE(x.size() == 1);
    REQUIRE(x[0] == "Testmenoe");

    auto const y = split<TestType>("Test\nme\nnoe", std::regex{ "[^[:print:]]" });
    REQUIRE(y.size() == 3);
    REQUIRE(y[0] == "Test");
    REQUIRE(y[1] == "me");
    REQUIRE(y[2] == "noe");

    auto const yy = split<TestType>("TaaaT", std::regex{ "aa" });
    REQUIRE(yy.size() == 2);
    REQUIRE(yy[0] == "T");
    REQUIRE(yy[1] == "aT");

    // This checks if indefinite loops are broken, somehow
    // Empty regexes are UB anyhow
    auto const z1 = split<TestType>("TaaaT", std::regex{ "" });
}

TEST_CASE("split_sv() with default return type", "[join_split]")
{
    std::string a = "Hello World";
    auto tok = split_sv(a, " ");

    using ReturnType = typename std::remove_const<decltype(tok)>::type;
    static_assert(std::is_same<ReturnType, std::vector<gul14::string_view>>::value == true,
        "split_sv() returns wrong type");

    REQUIRE(tok.size() == 2);
    REQUIRE(tok[0] == "Hello");
    REQUIRE(tok[1] == "World");

    a[1] = 'a';
    a[10] = '!';
    REQUIRE(tok[0] == "Hallo");
    REQUIRE(tok[1] == "Worl!");
}

TEMPLATE_TEST_CASE("split_sv()", "[join_split]",
    std::vector<std::string>, std::vector<gul14::string_view>,
    (SmallVector<std::string, 2>), (SmallVector<gul14::string_view, 3>))
{
    auto tok = split_sv<TestType>("Testmenoe", "X");
    using ReturnType = typename std::remove_const<decltype(tok)>::type;
    static_assert(std::is_same<ReturnType, TestType>::value == true,
        "split_sv() returns wrong type");
    REQUIRE(tok.size() == 1);
    REQUIRE(tok[0] == "Testmenoe");

    tok = split_sv<TestType>("Test\nme\nnoe", "\n");
    REQUIRE(tok.size() == 3);
    REQUIRE(tok[0] == "Test");
    REQUIRE(tok[1] == "me");
    REQUIRE(tok[2] == "noe");

    tok = split_sv<TestType>("TaaaT", "aa");
    REQUIRE(tok.size() == 2);
    REQUIRE(tok[0] == "T");
    REQUIRE(tok[1] == "aT");

    tok = split_sv<TestType>("TaaaT", "");
    REQUIRE(tok.size() == 7);
    REQUIRE(tok[0] == "");
    REQUIRE(tok[1] == "T");
    REQUIRE(tok[2] == "a");
    REQUIRE(tok[3] == "a");
    REQUIRE(tok[4] == "a");
    REQUIRE(tok[5] == "T");
    REQUIRE(tok[6] == "");

    tok = split_sv<TestType>("", "Test");
    REQUIRE(tok.size() == 1);
    REQUIRE(tok[0] == "");

    std::string a = "Hello World";
    tok = split_sv<TestType>(a, " ");
    REQUIRE(tok.size() == 2);
    REQUIRE(tok[0] == "Hello");
    REQUIRE(tok[1] == "World");
}

TEMPLATE_TEST_CASE("split_sv<std::list<...>>()", "[join_split]",
    std::list<std::string>, std::list<gul14::string_view>)
{
    auto const x = split_sv<TestType>("Hello World", " ");

    REQUIRE(x.size() == 2);
    REQUIRE(x.front() == "Hello");
    REQUIRE(x.back() == "World");
}

TEMPLATE_TEST_CASE("split_sv<std::queue<...>>()", "[join_split]",
    std::queue<std::string>, std::queue<gul14::string_view>)
{
    auto emplace = [](TestType& c, gul14::string_view sv) { c.emplace(sv); };

    auto const x = split_sv<TestType>("Hello World", " ", emplace);

    REQUIRE(x.size() == 2);
    REQUIRE(x.front() == "Hello");
    REQUIRE(x.back() == "World");
}

TEMPLATE_TEST_CASE("split_sv() with associative containers", "[join_split]",
    std::set<std::string>, std::multiset<std::string>)
{
    auto emplace = [](TestType& c, gul14::string_view sv) { c.emplace(sv); };

    auto const x = split_sv<TestType>("Hello World", " ", emplace);

    REQUIRE(x.size() == 2);
    REQUIRE(std::find(x.begin(), x.end(), "Hello") != x.end());
    REQUIRE(std::find(x.begin(), x.end(), "World") != x.end());
}

TEST_CASE("join()", "[join_split]")
{
    REQUIRE(join(std::vector<std::string>{ }, "lalala") == "");
    REQUIRE(join(std::vector<std::string>{ { "" } }, "lalala") == "");
    REQUIRE(join(std::vector<std::string>{ { "xyzzy" } }, "lalala") == "xyzzy");
    REQUIRE(join(std::vector<std::string>{ { "A" }, { "B" } }, "lalala") == "AlalalaB");

    REQUIRE(join(std::vector<string_view>{ }, "lalala") == "");
    REQUIRE(join(std::vector<string_view>{ { "" } }, "lalala") == "");
    REQUIRE(join(std::vector<string_view>{ { "xyzzy" } }, "lalala") == "xyzzy"s);
    REQUIRE(join(std::vector<string_view>{ { "A" }, { "B" } }, "lalala") == "AlalalaB");

    std::forward_list<gul14::string_view> list;
    REQUIRE(join(list, "-") == "");
    REQUIRE(join(list.begin(), list.end(), "-") == "");

    list.emplace_front("two");
    REQUIRE(join(list, "-") == "two");
    REQUIRE(join(list.begin(), list.end(), "-") == "two");

    list.emplace_front("forty");
    REQUIRE(join(list, "-") == "forty-two");
    REQUIRE(join(list.begin(), list.end(), "-") == "forty-two");
}

TEST_CASE("join(split())", "[join_split]")
{
    REQUIRE(join(split("TestXmzeXnoeX", std::regex{"z"}), "!") == "TestXm!eXnoeX");
    REQUIRE(join(split("TestXmzeXnoeX", std::regex{"X"}), "!") == "Test!mze!noe!");
    REQUIRE(join(split("z", std::regex{"z"}), "!") == "!");
    REQUIRE(join(split("z ", std::regex{"z"}), "!") == "! ");
    REQUIRE(join(split(" z", std::regex{"z"}), "!") == " !");
    REQUIRE(join(split("xyzzy", std::regex{"z"}), "!") == "xy!!y");

    REQUIRE(join(split("TestXmzeXnoeX", "z"), "!") == "TestXm!eXnoeX");
    REQUIRE(join(split("TestXmzeXnoeX", "X"), "!") == "Test!mze!noe!");
    REQUIRE(join(split("z", "z"), "!") == "!");
    REQUIRE(join(split("z ", "z"), "!") == "! ");
    REQUIRE(join(split(" z", "z"), "!") == " !");
    REQUIRE(join(split("xyzzy", "z"), "!") == "xy!!y");

    REQUIRE(join(split_sv("TestXmzeXnoeX", "z"), "!") == "TestXm!eXnoeX");
    REQUIRE(join(split_sv("TestXmzeXnoeX", "X"), "!") == "Test!mze!noe!");
    REQUIRE(join(split_sv("z", "z"), "!") == "!");
    REQUIRE(join(split_sv("z ", "z"), "!") == "! ");
    REQUIRE(join(split_sv(" z", "z"), "!") == " !");
    REQUIRE(join(split_sv("xyzzy", "z"), "!") == "xy!!y");

    auto const x = "EinNegerMitGazelleZagtImRegenNie"s;
    REQUIRE(join(split(x, "e"), "e") == x);
    REQUIRE(join(split_sv(x, "e"), "e") == x);
}

// vi:ts=4:sw=4:sts=4:et
