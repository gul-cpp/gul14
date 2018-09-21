/**
 * \file   test_Trigger.cc
 * \author Lars Froehlich
 * \date   Created on September 21, 2018
 * \brief  Test suite for the Trigger class from the General Utility Library.
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
#include "catch.h"
#include "gul.h"

using namespace std::literals;

using gul::tic;
using gul::toc;
using gul::sleep;
using gul::Trigger;

TEST_CASE("Construction, assignment, equality and bool operator work", "[concurrency]")
{
    Trigger trg;
    REQUIRE(trg == false);
    trg = true;
    REQUIRE(trg == true);

    Trigger trg2{ true };
    REQUIRE(trg2 == true);
    trg2 = false;
    REQUIRE(trg2 == false);

}

SCENARIO("Trigger::wait*() resumes if another thread calls trigger()", "[concurrency]")
{
    GIVEN("a trigger and a helper thread that sets it to true after 10 ms")
    {
        Trigger trg;

        auto future = std::async(std::launch::async,
                [&trg]
                {
                    sleep(10ms);
                    trg = true;
                });

        auto t0 = tic();

        WHEN("the main thread calls wait()")
        {
            trg.wait();
            auto time_ms = toc<std::chrono::milliseconds>(t0);

            THEN("it resumes after approximately 10 ms")
            {
                REQUIRE(time_ms >= 9);
                REQUIRE(time_ms <= 11);
            }
        }

        WHEN("the main thread calls wait_for(1s)")
        {
            trg.wait_for(1s);
            auto time_ms = toc<std::chrono::milliseconds>(t0);

            THEN("it resumes after approximately 10 ms")
            {
                REQUIRE(time_ms >= 9);
                REQUIRE(time_ms <= 11);
            }
        }

        WHEN("the main thread calls wait_until(now + 1s)")
        {
            trg.wait_until(std::chrono::system_clock::now() + 1s);
            auto time_ms = toc<std::chrono::milliseconds>(t0);

            THEN("it resumes after approximately 10 ms")
            {
                REQUIRE(time_ms >= 9);
                REQUIRE(time_ms <= 11);
            }
        }
    }
}

SCENARIO("Trigger::wait_for() and wait_until() wait the requested amount of time", "[concurrency]")
{
    Trigger trg;

    auto t0 = tic();

    WHEN("calling wait_for(10ms)")
    {
        trg.wait_for(10ms);
        auto time_ms = toc<std::chrono::milliseconds>(t0);

        THEN("execution resumes after approximately 10 ms")
        {
            REQUIRE(time_ms >= 9);
            REQUIRE(time_ms <= 11);
        }
    }

    WHEN("calling wait_until(now + 10ms)")
    {
        trg.wait_until(std::chrono::system_clock::now() + 10ms);
        auto time_ms = toc<std::chrono::milliseconds>(t0);

        THEN("execution resumes after approximately 10 ms")
        {
            REQUIRE(time_ms >= 9);
            REQUIRE(time_ms <= 11);
        }
    }
}
