/**
 * \file   test_time_util.cc
 * \author Lars Froehlich
 * \date   Created on September 7, 2018
 * \brief  Test suite for tic(), toc(), and sleep() from the General Utility Library.
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

#include <future>
#include "gul/catch.h"
#include "gul.h"

using namespace std::literals;

using gul::tic;
using gul::toc;
using gul::sleep;
using gul::Trigger;

SCENARIO("After tic() and sleep(), toc() yields the correct time span", "[time]")
{
    auto t0 = tic();

    WHEN("two 5 ms delays are made via sleep()")
    {
        sleep(0.005);

        THEN("toc() measures approximately 5 ms after the first one")
        {
            const auto toc_s = toc(t0);
            const auto toc_us = toc<std::chrono::microseconds>(t0);

            REQUIRE(toc_s > 0.004);
            REQUIRE(toc_s < 0.006);
            REQUIRE(toc_us > 4000);
            REQUIRE(toc_us < 6000);
        }

        sleep(0.005);

        THEN("toc() measures approximately 10 ms after the second one")
        {
            const auto toc_s = toc(t0);
            const auto toc_us = toc<std::chrono::microseconds>(t0);

            REQUIRE(toc_s > 0.009);
            REQUIRE(toc_s < 0.011);
            REQUIRE(toc_us > 9000);
            REQUIRE(toc_us < 11000);
        }
    }

    WHEN("a 20 ms delay is made via sleep()")
    {
        sleep(20ms);

        THEN("toc() measures approximately 20 ms afterwards")
        {
            const auto toc_s = toc(t0);
            const auto toc_ms = toc<std::chrono::milliseconds>(t0);

            REQUIRE(toc_s > 0.019);
            REQUIRE(toc_s < 0.021);
            REQUIRE(toc_ms >= 19);
            REQUIRE(toc_ms <= 21);
        }
    }
}

SCENARIO("Negative or zero times make sleep() not wait", "[time]")
{
    auto t0 = tic();

    WHEN("sleep(0ms) is executed")
    {
        sleep(0ms);

        THEN("the elapsed time is very very small")
        {
            REQUIRE(toc(t0) < 0.0005);
            REQUIRE(toc<std::chrono::microseconds>(t0) < 500);
        }
    }

    WHEN("sleep(0.0) is executed")
    {
        sleep(0.0);

        THEN("the elapsed time is very very small")
        {
            REQUIRE(toc(t0) < 0.0005);
            REQUIRE(toc<std::chrono::microseconds>(t0) < 500);
        }
    }

    WHEN("sleep(-50ms) is executed")
    {
        sleep(-50ms);

        THEN("the elapsed time is very very small")
        {
            REQUIRE(toc(t0) < 0.0005);
            REQUIRE(toc<std::chrono::microseconds>(t0) < 500);
        }
    }

    WHEN("sleep(-0.5) is executed")
    {
        sleep(-0.5);

        THEN("the elapsed time is very very small")
        {
            REQUIRE(toc(t0) < 0.0005);
            REQUIRE(toc<std::chrono::microseconds>(t0) < 500);
        }
    }
}

SCENARIO("sleep(..., interrupt) respects the SleepInterrupt state on a single thread", "[time]")
{
    auto t0 = tic();

    WHEN("calling sleep(0.005, interrupt) with interrupt { false }")
    {
        Trigger interrupt{ false };
        sleep(0.005, interrupt);

        THEN("the elapsed time is approximately 5 ms")
        {
            REQUIRE(toc(t0) > 0.004);
            REQUIRE(toc(t0) < 0.006);
            REQUIRE(toc<std::chrono::milliseconds>(t0) >= 4);
            REQUIRE(toc<std::chrono::milliseconds>(t0) <= 6);
        }
    }

    WHEN("calling sleep(0.005, interrupt) with interrupt { true }")
    {
        Trigger interrupt { true };
        sleep(0.005, interrupt);

        THEN("the elapsed time is very very small")
        {
            REQUIRE(toc(t0) < 0.0005);
            REQUIRE(toc<std::chrono::microseconds>(t0) < 500);
        }
    }

    WHEN("calling sleep(0.005, interrupt) after interrupt = true")
    {
        Trigger interrupt;
        interrupt = true;
        sleep(0.005, interrupt);

        THEN("the elapsed time is very very small")
        {
            REQUIRE(toc(t0) < 0.0005);
            REQUIRE(toc<std::chrono::microseconds>(t0) < 500);
        }
    }
}

SCENARIO("sleep(..., interrupt) can be interrupted from another thread", "[time]")
{
    WHEN("interrupting sleep(2s, interrupt) after 10 ms")
    {
        Trigger interrupt;

        auto future = std::async(std::launch::async,
                [&interrupt]
                {
                    sleep(10ms);
                    interrupt = true;
                });

        auto t0 = tic();

        sleep(2s, interrupt);

        THEN("the elapsed time is approximately 10 ms")
        {
            REQUIRE(toc(t0) > 0.009);
            REQUIRE(toc(t0) < 0.011);
            REQUIRE(toc<std::chrono::milliseconds>(t0) >=  9);
            REQUIRE(toc<std::chrono::milliseconds>(t0) <= 11);
        }

        THEN("an additional sleep does not wait anymore")
        {
            auto t1 = tic();
            sleep(1s, interrupt);
            REQUIRE(toc<std::chrono::microseconds>(t1) < 500);
        }

        THEN("after setting interrupt to false again, an additional sleep works as expected")
        {
            interrupt = false;

            auto t1 = tic();

            sleep(5ms, interrupt);

            REQUIRE(toc<std::chrono::milliseconds>(t1) >= 4);
            REQUIRE(toc<std::chrono::milliseconds>(t1) <= 6);
        }
    }
}
