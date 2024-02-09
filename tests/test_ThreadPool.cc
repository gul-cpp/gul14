/**
 * \file    test_ThreadPool.h
 * \authors \ref contributors
 * \date    Created on March 17, 2023
 * \brief   Declaration of the ThreadPool class.
 *
 * \copyright Copyright 2023-2024 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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

// SPDX-License-Identifier: LGPL-2.1-or-later

#include <atomic>

#include "gul14/catch.h"
#include "gul14/ThreadPool.h"
#include "gul14/time_util.h"

using namespace gul14;
using namespace std::literals;

//
// TaskHandle class
//

TEST_CASE("TaskHandle: cancel()", "[ThreadPool]")
{
    auto pool = std::make_unique<ThreadPool>(1);

    std::atomic<bool> stop{ false };

    auto task1 = pool->add_task([&stop]() { while (!stop) gul14::sleep(10us); });
    auto task2 = pool->add_task([&stop]() { while (!stop) gul14::sleep(10us); });
    auto task3 = pool->add_task([&stop]() { while (!stop) gul14::sleep(10us); });

    while (not task1.is_running())
        gul14::sleep(1ms);

    REQUIRE(task1.is_pending() == false);
    REQUIRE(task2.is_pending());
    REQUIRE(task3.is_pending());

    REQUIRE(task1.cancel() == false);
    REQUIRE(task2.cancel() == true);

    REQUIRE(task2.is_pending() == false);
    REQUIRE(task3.is_pending());

    stop = true;

    // Make sure the pool is removed before any of the atomic variables go out of scope
    pool.reset();
}

TEST_CASE("TaskHandle: is_complete()", "[ThreadPool][TaskHandle]")
{
    auto pool = std::make_unique<ThreadPool>(1);

    std::atomic<bool> stop{ false };

    auto task1 = pool->add_task([&stop]() { while (!stop) gul14::sleep(10us); });
    auto task2 = pool->add_task([&stop]() { while (!stop) gul14::sleep(10us); });
    auto task3 = pool->add_task([&stop]() { while (!stop) gul14::sleep(10us); });

    REQUIRE(task1.is_complete() == false);
    REQUIRE(task2.is_complete() == false);
    REQUIRE(task3.is_complete() == false);

    stop = true;

    // Make sure the pool is removed before any of the atomic variables go out of scope
    pool.reset();
}

TEST_CASE("TaskHandle: is_pending()", "[ThreadPool][TaskHandle]")
{
    auto pool = std::make_unique<ThreadPool>(1);

    std::atomic<bool> stop{ false };

    pool->add_task([&stop]() { while (!stop) gul14::sleep(10us); });
    auto task2 = pool->add_task([&stop]() { while (!stop) gul14::sleep(10us); });
    auto task3 = pool->add_task([&stop]() { while (!stop) gul14::sleep(10us); });

    // id1 might have been assigned to the work thread already, but in any case the other
    // two must still be pending.
    REQUIRE(task2.is_pending());
    REQUIRE(task3.is_pending());

    stop = true;

    // Make sure the pool is removed before any of the atomic variables go out of scope
    pool.reset();
}

TEST_CASE("TaskHandle: is_running()", "[ThreadPool][TaskHandle]")
{
    auto pool = std::make_unique<ThreadPool>(1);

    std::atomic<bool> stop{ false };

    auto task = pool->add_task([&stop]() { while (!stop) gul14::sleep(10us); });

    auto t0 = gul14::tic();
    while (not task.is_running())
    {
        if (gul14::toc(t0) > 1.0)
            FAIL("Timeout waiting for work item to start");
        gul14::sleep(1ms);
    }

    stop = true;

    t0 = gul14::tic();
    while (task.is_running())
    {
        if (gul14::toc(t0) > 1.0)
            FAIL("Timeout waiting for work item to stop");
        gul14::sleep(1ms);
    }

    // Make sure the pool is removed before any of the atomic variables go out of scope
    pool.reset();
}



//
// ThreadPool class
//

TEST_CASE("ThreadPool: Constructor", "[ThreadPool]")
{
    SECTION("Create a thread pool with 2 threads, default capacity")
    {
        ThreadPool pool{ 2 };
        REQUIRE(pool.count_threads() == 2);
        REQUIRE(pool.capacity() >= 10); // default capacity
    }

    SECTION("Create a thread pool with 1 thread, capacity 42")
    {
        ThreadPool pool{ 1, 42 };
        REQUIRE(pool.count_threads() == 1);
        REQUIRE(pool.capacity() == 42);
    }

    SECTION("Cannot create an empty thread pool")
    {
        REQUIRE_THROWS_AS(ThreadPool{ 0 }, std::invalid_argument);
    }
}

TEST_CASE("ThreadPool: add_task()", "[ThreadPool]")
{
    auto pool = std::make_unique<ThreadPool>(1);

    // Without start times
    std::atomic<bool> start{ false };
    std::atomic<bool> done{ false };

    pool->add_task([&start](ThreadPoolEngine&) { while (!start) gul14::sleep(10us); });
    auto handle = pool->add_task([&done](ThreadPoolEngine&) { done = true; }, "Task 2");

    REQUIRE(pool->count_pending() >= 1);
    REQUIRE(pool->count_pending() <= 2);
    REQUIRE(handle.is_pending());

    start = true;

    while (not pool->is_idle())
        gul14::sleep(1ms);

    REQUIRE(done);

    // With start time as time point
    std::atomic<int> last_job{ 0 };

    pool->add_task([&last_job](ThreadPoolEngine&) { last_job = 1; },
        std::chrono::system_clock::now() + 2ms);
    pool->add_task([&last_job](ThreadPoolEngine&) { last_job = 2; });

    while (last_job == 0)
        gul14::sleep(50us);
    REQUIRE(last_job == 2);

    while (last_job == 2)
        gul14::sleep(50us);
    REQUIRE(last_job == 1);

    // With start time as duration
    last_job = 0;
    pool->add_task([&last_job]() { last_job = 1; }, 2ms);
    pool->add_task([&last_job]() { last_job = 2; });

    while (last_job == 0)
        gul14::sleep(50us);
    REQUIRE(last_job == 2);

    while (last_job == 2)
        gul14::sleep(50us);
    REQUIRE(last_job == 1);

    // Make sure the pool is removed before any of the atomic variables go out of scope
    pool.reset();
}

TEST_CASE("ThreadPool: capacity()", "[ThreadPool]")
{
    ThreadPool pool{ 1, 128 };
    REQUIRE(pool.capacity() == 128);
}

TEST_CASE("ThreadPool: count_pending()", "[ThreadPool]")
{
    auto pool = std::make_unique<ThreadPool>(1);

    std::atomic<bool> stop{ false };

    // Insert 11 work items
    for (std::size_t i = 0; i <= 10; ++i)
    {
        REQUIRE(pool->is_full() == false);
        pool->add_task([&stop]() { while (!stop) gul14::sleep(10us); });
    }

    // One work item might already be executing, the queue should have 10 or 11 pending
    // items
    REQUIRE(pool->count_pending() >= 10);
    REQUIRE(pool->count_pending() <= 11);

    stop = true;

    // Make sure the pool is removed before any of the atomic variables go out of scope
    pool.reset();
}

TEST_CASE("ThreadPool: count_threads()", "[ThreadPool]")
{
    for (std::size_t i = 1; i <= 2; ++i)
    {
        ThreadPool pool{ i };
        REQUIRE(pool.count_threads() == i);
    }
}

TEST_CASE("ThreadPool: get_running_task_names()", "[ThreadPool]")
{
    auto pool = std::make_unique<ThreadPool>(1);

    REQUIRE(pool->get_running_task_names().empty());

    std::atomic<bool> stop{ false };

    pool->add_task([&stop]() { while (!stop) gul14::sleep(10us); }, "1");
    pool->add_task([&stop]() { while (!stop) gul14::sleep(10us); }, "2");

    while (pool->count_pending() == 2)
        gul14::sleep(1ms);

    auto running_names = pool->get_running_task_names();
    REQUIRE(running_names.size() == 1);
    REQUIRE(running_names[0] == "1");

    stop = true;

    // Make sure the pool is removed before any of the atomic variables go out of scope
    pool.reset();
}

TEST_CASE("ThreadPool: is_full()", "[ThreadPool]")
{
    auto pool = std::make_unique<ThreadPool>(1);

    std::atomic<bool> stop{ false };

    for (std::size_t i = 1; i <= pool->capacity() + 1; ++i)
    {
        REQUIRE(pool->is_full() == false);
        pool->add_task([&stop]() { while (!stop) gul14::sleep(10us); });
    }

    REQUIRE(pool->is_full()); // One work item is currently executing, the queue is full

    stop = true;

    // Make sure the pool is removed before any of the atomic variables go out of scope
    pool.reset();
}

TEST_CASE("ThreadPool: is_idle()", "[ThreadPool]")
{
    ThreadPool pool{ 1 };
    REQUIRE(pool.is_idle());
}

TEST_CASE("ThreadPool: cancel_pending_tasks()", "[ThreadPool]")
{
    auto pool = std::make_unique<ThreadPool>(1);

    std::atomic<bool> stop{ false };

    auto task1 = pool->add_task([&stop]() { while (!stop) gul14::sleep(10us); });
    auto task2 = pool->add_task([&stop]() { while (!stop) gul14::sleep(10us); });
    auto task3 = pool->add_task([&stop]() { while (!stop) gul14::sleep(10us); });

    while (not task1.is_running())
        gul14::sleep(1ms);

    REQUIRE(task1.is_pending() == false);
    REQUIRE(task2.is_pending());
    REQUIRE(task3.is_pending());

    REQUIRE(pool->cancel_pending_tasks() == 2);
    REQUIRE(pool->count_pending() == 0);
    REQUIRE(pool->cancel_pending_tasks() == 0);

    stop = true;

    // Make sure the pool is removed before any of the atomic variables go out of scope
    pool.reset();
}

TEST_CASE("ThreadPool: Run 100 functions on a single thread, check order", "[ThreadPool]")
{
    auto pool = std::make_unique<ThreadPool>(1);

    std::vector<std::function<void()>> functions;

    std::mutex mutex;
    std::vector<int> output;

    for (int i = 1; i <= 100; ++i)
    {
        functions.emplace_back(
            [i, &mutex, &output]()
            {
                std::lock_guard<std::mutex> lock(mutex);
                output.push_back(i);
            });
    }

    for (auto& fct : functions)
        pool->add_task(fct);

    while (not pool->is_idle())
    {
        REQUIRE(pool->count_pending() <= functions.size());
        REQUIRE(pool->count_pending() <= pool->capacity());
        gul14::sleep(1ms);
    }

    REQUIRE(output.size() == 100);

    for (int i = 0; i != 100; ++i)
        REQUIRE(output[i] == i + 1);

    // Make sure the pool is removed before any of the captured variables go out of scope
    pool.reset();
}

TEST_CASE("ThreadPool: Run 100 functions on 4 threads", "[ThreadPool]")
{
    auto pool = std::make_unique<ThreadPool>(4);

    std::mutex mutex;
    std::vector<int> output;

    for (int i = 1; i <= 100; ++i)
    {
        pool->add_task(
            [i, &mutex, &output]()
            {
                {
                    std::lock_guard<std::mutex> lock(mutex);
                    output.push_back(i);
                }
                gul14::sleep(100us);
            });
    }

    while (not pool->is_idle())
    {
        REQUIRE(pool->count_pending() <= 100);
        REQUIRE(pool->count_pending() <= pool->capacity());
        gul14::sleep(1ms);
    }

    REQUIRE(output.size() == 100);

    std::sort(output.begin(), output.end());

    for (int i = 0; i != 100; ++i)
        REQUIRE(output[i] == i + 1);

    // Make sure the pool is removed before any of the captured variables go out of scope
    pool.reset();
}

TEST_CASE("ThreadPool: Capacity limit", "[ThreadPool]")
{
    std::size_t max_jobs{ 10 };
    auto pool = std::make_unique<ThreadPool>(1, max_jobs);

    std::atomic<bool> go{ false };

    for (std::size_t i = 0; i != max_jobs; ++i)
    {
        pool->add_task(
            [&go]()
            {
                while (!go)
                    gul14::sleep(100us);
            });
    }

    // Wait until first job has started
    while (pool->count_pending() != max_jobs - 1)
        gul14::sleep(1ms);

    // Add yet another task, filling the queue
    pool->add_task(
        [&go]()
        {
            while (!go)
                gul14::sleep(100us);
        });
    REQUIRE(pool->is_full());

    REQUIRE_THROWS_AS(pool->add_task([]() {}), std::runtime_error);

    go = true;

    while (not pool->is_idle())
    {
        REQUIRE(pool->count_pending() <= max_jobs);
        gul14::sleep(1ms);
    }

    REQUIRE_NOTHROW(pool->add_task([]() {}));

    // Make sure the pool is removed before any captured variable goes out of scope
    pool.reset();
}
