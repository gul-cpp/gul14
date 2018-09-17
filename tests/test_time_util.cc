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

#include "catch.h"
#include "gul.h"

using namespace std::literals;

using gul::tic;
using gul::toc;
using gul::sleep;
using gul::SleepInterrupt;

SCENARIO("After tic() and sleep(), toc() yields the correct time span", "[time]")
{
    auto t0 = tic();

    WHEN("two 5 ms delays are made via sleep()")
    {
        sleep(0.005);

        THEN("toc() measures approximately 5 ms after the first one")
        {
            REQUIRE(toc(t0) > 0.0045);
            REQUIRE(toc(t0) < 0.0055);
            REQUIRE(toc<std::chrono::microseconds>(t0) > 4500);
            REQUIRE(toc<std::chrono::microseconds>(t0) < 5500);
        }

        sleep(0.005);

        THEN("toc() measures approximately 10 ms after the second one")
        {
            REQUIRE(toc(t0) > 0.0095);
            REQUIRE(toc(t0) < 0.0105);
            REQUIRE(toc<std::chrono::microseconds>(t0) > 9500);
            REQUIRE(toc<std::chrono::microseconds>(t0) < 10500);
        }
    }

    WHEN("a 20 ms delay is made via sleep()")
    {
        sleep(20ms);

        THEN("toc() measures approximately 20 ms afterwards")
        {
            REQUIRE(toc(t0) > 0.0195);
            REQUIRE(toc(t0) < 0.0205);
            REQUIRE(toc<std::chrono::milliseconds>(t0) >= 19);
            REQUIRE(toc<std::chrono::milliseconds>(t0) <= 21);
        }
    }
}

SCENARIO("sleep(..., interrupt) respects the SleepInterrupt state", "[time]")
{
    auto t0 = tic();

    WHEN("calling sleep(0.005, interrupt) with interrupt { false }")
    {
        SleepInterrupt interrupt{ false };
        sleep(0.005, interrupt);

        THEN("the elapsed time is approximately 5 ms")
        {
            REQUIRE(toc(t0) > 0.0045);
            REQUIRE(toc(t0) < 0.0055);
            REQUIRE(toc<std::chrono::microseconds>(t0) > 4500);
            REQUIRE(toc<std::chrono::microseconds>(t0) < 5500);
        }
    }

    WHEN("calling sleep(0.005, interrupt) with interrupt { true }")
    {
        SleepInterrupt interrupt { true };
        sleep(0.005, interrupt);

        THEN("the elapsed time is very very small")
        {
            REQUIRE(toc(t0) < 0.0001);
            REQUIRE(toc<std::chrono::microseconds>(t0) < 100);
        }
    }

    WHEN("calling sleep(0.005, interrupt) after interrupt = true")
    {
        SleepInterrupt interrupt;
        interrupt = true;
        sleep(0.005, interrupt);

        THEN("the elapsed time is very very small")
        {
            REQUIRE(toc(t0) < 0.0001);
            REQUIRE(toc<std::chrono::microseconds>(t0) < 100);
        }
    }
}
