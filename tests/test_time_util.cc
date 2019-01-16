/**
 * \file   test_time_util.cc
 * \author \ref contributors
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

namespace {

constexpr int MS_BEFORE = 1;
constexpr int MS_AFTER  = 18;
constexpr int US_BEFORE = MS_BEFORE * 1000;
constexpr int US_AFTER  = MS_AFTER * 1000;
constexpr float S_BEFORE = MS_BEFORE * 1e-3f;
constexpr float S_AFTER = MS_AFTER * 1e-3f;

} // anonymous namespace

SCENARIO("After tic() and sleep(), toc() yields the correct time span", "[time]")
{
    auto t0 = tic();

    WHEN("two 50 ms delays are made via sleep()")
    {
        sleep(0.050);

        THEN("toc() measures approximately 50 ms after the first one")
        {
            const auto toc_s = toc(t0);
            const auto toc_us = toc<std::chrono::microseconds>(t0);

            REQUIRE(toc_s > 0.050 - S_BEFORE);
            REQUIRE(toc_s < 0.050 + S_AFTER);
            REQUIRE(toc_us > 50000 - US_BEFORE);
            REQUIRE(toc_us < 50000 + US_AFTER);
        }

        sleep(0.050);

        THEN("toc() measures approximately 100 ms after the second one")
        {
            const auto toc_s = toc(t0);
            const auto toc_us = toc<std::chrono::microseconds>(t0);

            REQUIRE(toc_s > 0.1 - S_BEFORE);
            REQUIRE(toc_s < 0.1 + S_AFTER);
            REQUIRE(toc_us > 100000 - US_BEFORE);
            REQUIRE(toc_us < 100000 + US_AFTER);
        }
    }

    WHEN("a 50 ms delay is made via sleep()")
    {
        sleep(50ms);

        THEN("toc() measures approximately 50 ms afterwards")
        {
            const auto toc_s = toc(t0);
            const auto toc_ms = toc<std::chrono::milliseconds>(t0);

            REQUIRE(toc_s > 0.05 - S_BEFORE);
            REQUIRE(toc_s < 0.05 + S_AFTER);
            REQUIRE(toc_ms >= 50 - MS_BEFORE);
            REQUIRE(toc_ms <= 50 + MS_AFTER);
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
            REQUIRE(toc(t0) < 0.001);
            REQUIRE(toc<std::chrono::microseconds>(t0) < 1000);
        }
    }

    WHEN("sleep(0.0) is executed")
    {
        sleep(0.0);

        THEN("the elapsed time is very very small")
        {
            REQUIRE(toc(t0) < 0.001);
            REQUIRE(toc<std::chrono::microseconds>(t0) < 1000);
        }
    }

    WHEN("sleep(-50ms) is executed")
    {
        sleep(-50ms);

        THEN("the elapsed time is very very small")
        {
            REQUIRE(toc(t0) < 0.001);
            REQUIRE(toc<std::chrono::microseconds>(t0) < 1000);
        }
    }

    WHEN("sleep(-0.5) is executed")
    {
        sleep(-0.5);

        THEN("the elapsed time is very very small")
        {
            REQUIRE(toc(t0) < 0.001);
            REQUIRE(toc<std::chrono::microseconds>(t0) < 1000);
        }
    }
}

SCENARIO("sleep(..., interrupt) respects the SleepInterrupt state on a single thread", "[time]")
{
    auto t0 = tic();

    WHEN("calling sleep(0.01, interrupt) with interrupt { false }")
    {
        Trigger interrupt{ false };
        sleep(0.01, interrupt);

        THEN("the elapsed time is approximately 10 ms")
        {
            REQUIRE(toc(t0) > 0.01 - S_BEFORE);
            REQUIRE(toc(t0) < 0.01 + S_AFTER);
            REQUIRE(toc<std::chrono::milliseconds>(t0) >= 10 - MS_BEFORE);
            REQUIRE(toc<std::chrono::milliseconds>(t0) <= 10 + MS_AFTER);
        }
    }

    WHEN("calling sleep(0.01, interrupt) with interrupt { true }")
    {
        Trigger interrupt { true };
        sleep(0.01, interrupt);

        THEN("the elapsed time is very very small")
        {
            REQUIRE(toc(t0) < 0.001);
            REQUIRE(toc<std::chrono::microseconds>(t0) < 1000);
        }
    }

    WHEN("calling sleep(0.01, interrupt) after interrupt = true")
    {
        Trigger interrupt;
        interrupt = true;
        sleep(0.01, interrupt);

        THEN("the elapsed time is very very small")
        {
            REQUIRE(toc(t0) < 0.001);
            REQUIRE(toc<std::chrono::microseconds>(t0) < 1000);
        }
    }
}

SCENARIO("sleep(..., interrupt) can be interrupted from another thread", "[time]")
{
    WHEN("interrupting sleep(2s, interrupt) after 15 ms")
    {
        Trigger interrupt;
        std::chrono::steady_clock::time_point t0;

        auto future = std::async(std::launch::async,
                [&t0, &interrupt]
                {
                    t0 = tic();
                    sleep(15ms);
                    interrupt = true;
                });

        sleep(2s, interrupt);

        THEN("the elapsed time is approximately 15 ms")
        {
            const auto toc_s = toc(t0);
            const auto toc_ms = toc<std::chrono::milliseconds>(t0);

            REQUIRE(toc_s > 0.015 - S_BEFORE);
            REQUIRE(toc_s < 0.015 + S_AFTER);
            REQUIRE(toc_ms >= 15 - MS_BEFORE);
            REQUIRE(toc_ms <= 15 + MS_AFTER);
        }

        THEN("an additional sleep does not wait anymore")
        {
            auto t1 = tic();
            sleep(1s, interrupt);
            REQUIRE(toc<std::chrono::microseconds>(t1) < 1000);
        }

        THEN("after setting interrupt to false again, an additional sleep works as expected")
        {
            interrupt = false;

            auto t1 = tic();

            sleep(15ms, interrupt);

            REQUIRE(toc<std::chrono::milliseconds>(t1) >= 15 - MS_BEFORE);
            REQUIRE(toc<std::chrono::milliseconds>(t1) <= 15 + MS_AFTER);
        }
    }
}
