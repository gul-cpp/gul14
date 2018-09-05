/**
 * \file   test_string_split.cc
 * \author Fini Jastrow
 * \date   Created on August 31, 2018
 * \brief  Part of test suite for string utility functions in the General Utility Library.
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

#include "catch.h"
#include <gul.h>

using namespace std::literals::string_literals;

TEST_CASE("String Split Test", "[string_util]")
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


    auto const z1 = gul::split("TaaaT"s, std::regex{""});
    REQUIRE(z1.size() == 7);
    REQUIRE(z1[0] == ""s);
    REQUIRE(z1[1] == "T"s);
    REQUIRE(z1[2] == "a"s);
    REQUIRE(z1[3] == "a"s);
    REQUIRE(z1[4] == "a"s);
    REQUIRE(z1[5] == "T"s);
    REQUIRE(z1[6] == ""s);

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

TEST_CASE("String Join Test", "[string_util]")
{
    REQUIRE(gul::join(std::vector<std::string>{ }, "lalala") == ""s);
    REQUIRE(gul::join(std::vector<std::string>{ { "" } }, "lalala") == ""s);
    REQUIRE(gul::join(std::vector<std::string>{ { "xyzzy" } }, "lalala") == "xyzzy"s);
    //throw(1);
}

TEST_CASE("String Split-Join Test", "[string_util]")
{
    REQUIRE(gul::join(gul::split("TestXmzeXnoeX"s, std::regex{"z"}), "!") == "TestXm!eXnoeX"s);
    REQUIRE(gul::join(gul::split("TestXmzeXnoeX"s, std::regex{"X"}), "!") == "Test!mze!noe!"s);
    REQUIRE(gul::join(gul::split("z"s, std::regex{"z"}), "!") == "!"s);
    REQUIRE(gul::join(gul::split("z "s, std::regex{"z"}), "!") == "! "s);
    REQUIRE(gul::join(gul::split(" z"s, std::regex{"z"}), "!") == " !"s);
    REQUIRE(gul::join(gul::split("xyzzy"s, std::regex{"z"}), "!") == "xy!!y"s);

    auto const x = "EinNegerMitGazelleZagtImRegenNie"s;
    REQUIRE(gul::join(gul::split(x, "e"), "e") == x);
}
