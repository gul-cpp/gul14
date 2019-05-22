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

#include <new>
#include <string>
#include "catch.h"

#include <gul/finalizer.h>

TEST_CASE("Finalizer Tests", "[finalizer]")
{
    SECTION("Basic block leave test 1") {
        int foo = 1;
        {
            // closure as temporary
            auto _ = gul::finally([&] { foo += 2; });
        }
        REQUIRE(foo == 3);
    }

    SECTION("Basic block leave test 2") {
        int foo = 1;
        {
            // materialized and copied
            auto xxx = [&] { foo += 2; };
            auto yyy { xxx };

            auto _ = gul::finally(yyy);
        }
        REQUIRE(foo == 3);
    }

    SECTION("Basic block leave test 3") {
        int foo = 1;
        {
            // Use FinalAction directly
            auto xxx = [&] { foo += 2; };
            auto yyy { xxx };

            auto _ = gul::FinalAction<decltype(yyy)>(yyy);
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
            auto b = std::move(a); // must be movable
        }
        REQUIRE(foo == 3);
    }

    SECTION("String filling example") {
        std::string some_string {};
        char* buffer = nullptr;
        float some_float = 123.45;
        do {
            buffer = new char[100];
            if (buffer == nullptr)
                break;
            auto _ = gul::finally([&] { delete[] buffer; buffer = nullptr; });

            snprintf(buffer, 100, "%.1f", some_float);
            some_string = buffer;
            // get rid of buffer automagically
        }
        while (false);

        REQUIRE(some_string == "123.4");
        REQUIRE(buffer == nullptr);

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
