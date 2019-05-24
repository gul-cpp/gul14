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


#include <gul/finalizer.h>

#include <new>
#include <functional>
#include <string>

#include "catch.h"

TEST_CASE("Finalizer Tests", "[finalizer]")
{
    SECTION("Closure as temporary") {
        int foo = 1;
        {
            auto _ = gul::finally([&] { foo += 2; });
        }
        REQUIRE(foo == 3);
    }

    SECTION("Closure as lvalue") {
        int foo = 1;
        {
            // materialized and copied
            auto xxx = [&] { foo += 2; };
            auto yyy { xxx };

            auto _ = gul::finally(yyy);
        }
        REQUIRE(foo == 3);
    }

    SECTION("Closure as lvalue 2") {
        // Same test as before, but we want to check if the closure is really copied
        // by the code we use, so we need an closure internal memory ('called')
        int foo = 1;
        {
            // materialized and copied
            auto xxx = [&foo, called = 0]() mutable { foo += 2; ++called; return called; };
            REQUIRE(xxx() == 1);
            foo = 1;
            {
                auto yyy{ xxx };
                auto _ = gul::finally(yyy);
            }
            REQUIRE(foo == 3);
            REQUIRE(xxx() == 2);
        }
        REQUIRE(foo == 5);
    }

    SECTION("Direct closure as lvalue") {
        int foo = 1;
        {
            // Use FinalAction directly
            auto xxx = [&] { foo += 2; };
            auto yyy { xxx };

            auto _ = gul::FinalAction<decltype(yyy)>(yyy);
        }
        REQUIRE(foo == 3);
    }

    SECTION("Call on exception") {
        int foo = 1;
        try {
            auto _ = gul::finally([&] { foo += 2; });
            throw "Foo bar";
        } catch (...) {
            foo++;
        }
        REQUIRE(foo == 4);
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

    SECTION("Closure move") {
        int foo = 1;
        {
            auto _1 = gul::finally([&]() { foo += 2; });
            {
                auto _2 = std::move(_1);
                REQUIRE(foo == 1);
            }
            CHECK(foo == 3);
            {
                auto _2 = std::move(_1);
                REQUIRE(foo == 3);
            }
            REQUIRE(foo == 3);
        }
        REQUIRE(foo == 3);
    }
}

// Some preparation for function use-case
static auto global_foo{ 0 };

static void helper() {
    global_foo += 2;
}

static void helper2(int& foo) {
    foo += 2;
}

TEST_CASE("Finalizer with function", "[finalizer]")
{
    SECTION("Function pointer temporary") {
        global_foo = 1;
        {
            auto _ = gul::finally(&helper);
        }
        REQUIRE(global_foo == 3);
    }

    SECTION("Function pointer lvalue") {
        global_foo = 1;
        {
            auto x = &helper;
            auto _ = gul::finally(x);
        }
        REQUIRE(global_foo == 3);
    }

    SECTION("Assignment operator test") {
        global_foo = 1;
        {
            auto x = &helper;
            auto a = gul::finally(x);
            {
                auto b = gul::finally(x);
                b = std::move(a);
                REQUIRE(global_foo == 1);
            }
            REQUIRE(global_foo == 3);
        }
        REQUIRE(global_foo == 3);
    }

    SECTION("Function decays to function pointer temporary") {
         global_foo = 1;
         {
             // Lazy man's version, implicit conversion to function pointer
             auto _ = gul::finally(helper);
         }
         REQUIRE(global_foo == 3);
    }

    SECTION("Direct function pointer temporary") {
        global_foo = 1;
        {
            // Use FinalAction directly
            auto _ = gul::FinalAction<decltype(&helper)>(helper);
        }
        REQUIRE(global_foo == 3);
    }

    SECTION("Direct function pointer lvalue") {
        global_foo = 1;
        {
            // Use FinalAction directly
            auto x = &helper;
            auto _ = gul::FinalAction<decltype(x)>(helper);
        }
        REQUIRE(global_foo == 3);
    }

    SECTION("Direct function decays to function pointer temporary") {
        global_foo = 1;
        {
            // Use FinalAction directly
            auto _ = gul::FinalAction<decltype(&helper)>(helper);
        }
        REQUIRE(global_foo == 3);
    }

    SECTION("finally with std::bind") {
        int foo = 1;
        {
            auto _ = gul::finally(std::bind(&helper2, std::ref(foo)));
            CHECK(foo == 1);
        }
        CHECK(foo == 3);
    }
}

// vi:et:sts=4:sw=4:ts=4
