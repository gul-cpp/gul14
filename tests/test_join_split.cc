/**
 * \file   test_join_split.cc
 * \author \ref contributors
 * \date   Created on August 31, 2018
 * \brief  Test suite for join(), split(), and split_sv().
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
#include "gul/join_split.h"

using namespace std::literals::string_literals;
using gul::string_view;

TEST_CASE("split()", "[join_split]")
{
    auto const x = gul::split("Testmenoe"s, std::regex{"X"});
    REQUIRE(x.size() == 1);
    REQUIRE(x[0] == "Testmenoe"s);

    auto const x2 = gul::split("Testmenoe"s, "X"s);
    REQUIRE(x2.size() == 1);
    REQUIRE(x2[0] == "Testmenoe"s);

    auto const y = gul::split("Test\nme\nnoe"s, std::regex{"[^[:print:]]"});
    REQUIRE(y.size() == 3);
    REQUIRE(y[0] == "Test"s);
    REQUIRE(y[1] == "me"s);
    REQUIRE(y[2] == "noe"s);

    auto const y2 = gul::split("Test\nme\nnoe"s, "\n"s);
    REQUIRE(y2.size() == 3);
    REQUIRE(y2[0] == "Test"s);
    REQUIRE(y2[1] == "me"s);
    REQUIRE(y2[2] == "noe"s);

    auto const yy = gul::split("TaaaT"s, std::regex{"aa"});
    REQUIRE(yy.size() == 2);
    REQUIRE(yy[0] == "T"s);
    REQUIRE(yy[1] == "aT"s);

    auto const xy = gul::split("TaaaT"s, "aa");
    REQUIRE(xy.size() == 2);
    REQUIRE(xy[0] == "T"s);
    REQUIRE(xy[1] == "aT"s);

    // This checks if indefinite loops are broken, somehow
    // Empty regexes are UB anyhow
    auto const z1 = gul::split("TaaaT"s, std::regex{""});

    auto const z2 = gul::split("TaaaT"s, "");
    REQUIRE(z2.size() == 7);
    REQUIRE(z2[0] == ""s);
    REQUIRE(z2[1] == "T"s);
    REQUIRE(z2[2] == "a"s);
    REQUIRE(z2[3] == "a"s);
    REQUIRE(z2[4] == "a"s);
    REQUIRE(z2[5] == "T"s);
    REQUIRE(z2[6] == ""s);
}

TEST_CASE("split_sv()", "[join_split]")
{
    std::vector<string_view> tok = gul::split_sv("Testmenoe", "X");
    REQUIRE(tok.size() == 1);
    REQUIRE(tok[0] == "Testmenoe");

    tok = gul::split_sv("Test\nme\nnoe", "\n");
    REQUIRE(tok.size() == 3);
    REQUIRE(tok[0] == "Test");
    REQUIRE(tok[1] == "me");
    REQUIRE(tok[2] == "noe");

    tok = gul::split_sv("TaaaT", "aa");
    REQUIRE(tok.size() == 2);
    REQUIRE(tok[0] == "T");
    REQUIRE(tok[1] == "aT");

    tok = gul::split_sv("TaaaT", "");
    REQUIRE(tok.size() == 7);
    REQUIRE(tok[0] == "");
    REQUIRE(tok[1] == "T");
    REQUIRE(tok[2] == "a");
    REQUIRE(tok[3] == "a");
    REQUIRE(tok[4] == "a");
    REQUIRE(tok[5] == "T");
    REQUIRE(tok[6] == "");

    tok = gul::split_sv("", "Test");
    REQUIRE(tok.size() == 1);
    REQUIRE(tok[0] == "");

    std::string a = "Hello World";
    tok = gul::split_sv(a, " ");
    REQUIRE(tok.size() == 2);
    REQUIRE(tok[0] == "Hello");
    REQUIRE(tok[1] == "World");

    a[1] = 'a';
    a[10] = '!';
    REQUIRE(tok[0] == "Hallo");
    REQUIRE(tok[1] == "Worl!");
}

TEST_CASE("join()", "[join_split]")
{
    REQUIRE(gul::join(std::vector<std::string>{ }, "lalala") == "");
    REQUIRE(gul::join(std::vector<std::string>{ { "" } }, "lalala") == "");
    REQUIRE(gul::join(std::vector<std::string>{ { "xyzzy" } }, "lalala") == "xyzzy");
    REQUIRE(gul::join(std::vector<std::string>{ { "A" }, { "B" } }, "lalala") == "AlalalaB");

    REQUIRE(gul::join(std::vector<string_view>{ }, "lalala") == "");
    REQUIRE(gul::join(std::vector<string_view>{ { "" } }, "lalala") == "");
    REQUIRE(gul::join(std::vector<string_view>{ { "xyzzy" } }, "lalala") == "xyzzy"s);
    REQUIRE(gul::join(std::vector<string_view>{ { "A" }, { "B" } }, "lalala") == "AlalalaB");
}

TEST_CASE("join(split())", "[join_split]")
{
    REQUIRE(gul::join(gul::split("TestXmzeXnoeX", std::regex{"z"}), "!") == "TestXm!eXnoeX");
    REQUIRE(gul::join(gul::split("TestXmzeXnoeX", std::regex{"X"}), "!") == "Test!mze!noe!");
    REQUIRE(gul::join(gul::split("z", std::regex{"z"}), "!") == "!");
    REQUIRE(gul::join(gul::split("z ", std::regex{"z"}), "!") == "! ");
    REQUIRE(gul::join(gul::split(" z", std::regex{"z"}), "!") == " !");
    REQUIRE(gul::join(gul::split("xyzzy", std::regex{"z"}), "!") == "xy!!y");

    REQUIRE(gul::join(gul::split("TestXmzeXnoeX", "z"), "!") == "TestXm!eXnoeX");
    REQUIRE(gul::join(gul::split("TestXmzeXnoeX", "X"), "!") == "Test!mze!noe!");
    REQUIRE(gul::join(gul::split("z", "z"), "!") == "!");
    REQUIRE(gul::join(gul::split("z ", "z"), "!") == "! ");
    REQUIRE(gul::join(gul::split(" z", "z"), "!") == " !");
    REQUIRE(gul::join(gul::split("xyzzy", "z"), "!") == "xy!!y");

    REQUIRE(gul::join(gul::split_sv("TestXmzeXnoeX", "z"), "!") == "TestXm!eXnoeX");
    REQUIRE(gul::join(gul::split_sv("TestXmzeXnoeX", "X"), "!") == "Test!mze!noe!");
    REQUIRE(gul::join(gul::split_sv("z", "z"), "!") == "!");
    REQUIRE(gul::join(gul::split_sv("z ", "z"), "!") == "! ");
    REQUIRE(gul::join(gul::split_sv(" z", "z"), "!") == " !");
    REQUIRE(gul::join(gul::split_sv("xyzzy", "z"), "!") == "xy!!y");

    auto const x = "EinNegerMitGazelleZagtImRegenNie"s;
    REQUIRE(gul::join(gul::split(x, "e"), "e") == x);
    REQUIRE(gul::join(gul::split_sv(x, "e"), "e") == x);
}
