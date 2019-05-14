/**
 * \file   test_finalizer.cc
 * \author \ref contributors
 * \brief  Test suite for hexdump() and hexdump_stream().
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

#include <gul/finalizer.h>

TEST_CASE("Finalizer Tests", "[finalizer]")
{
    SECTION("Basic block leave test") {
        int foo = 1;
        {
            auto _ = gul::finally([&] { foo += 2; });
        }
        REQUIRE(foo == 3);
    }

    SECTION("Basic block leave on exception") {
        int foo = 1;
        try {
            auto _ = gul::finally([&] { foo += 2; });
            throw "Foo bar";
        } catch (...) {
            foo++;
        }
        REQUIRE(foo == 4);
    }

    SECTION("Assignment operator test") {
        int foo = 1;
        {
            auto a = gul::finally([&] { foo += 2; });
            auto b = std::move(a);
        }
        REQUIRE(foo == 3);
    }

}

// Some preparation for function use-case
static auto global_foo{ 0 };

static void helper() {
    global_foo += 2;
}

TEST_CASE("Finalizer with function", "[finalizer]")
{
    SECTION("Basic block leave test") {
        global_foo = 1;
        {
            auto _ = gul::finally(&helper);
        }
        REQUIRE(global_foo == 3);
    }
}

// vi:et:sts=4:sw=4:ts=4
