/**
 * \file    thread_pool.cc
 * \authors \ref contributors
 * \date    Created on February 9, 2024
 * \brief   Example on how to use the ThreadPool.
 *
 * \copyright Copyright 2024 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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

#include <iostream>

#include <gul14/ThreadPool.h>
#include <gul14/time_util.h>

using gul14::sleep;
using gul14::ThreadPool;
using gul14::ThreadPoolEngine;
using std::cout;

using namespace std::literals;

int main()
{
//! [Using the ThreadPool class]
    // Create a pool with 2 threads
    ThreadPool pool(2);

    pool.add_task([]() { cout << "Task 1\n"; });
    pool.add_task([]() { sleep(1); std::cout << "Task 2\n"; });

    // Tasks can be scheduled to start later:
    // This one should start 2 seconds after enqueueing (if a thread is available)
    pool.add_task([]() { cout << "Task 3\n"; }, 2s);

    // Probable output:
    // Task 1
    // Task 2
    // Task 3

    // Tasks can return results
    auto task = pool.add_task([]() { return 42; });

    while (not task.is_complete())
        sleep(0.1);

    // get_result() blocks until the task is complete
    cout << "Task result: " << task.get_result() << "\n";

    // Tasks can also interact with the pool themselves, e.g. to schedule a continuation:
    pool.add_task(
        [](ThreadPoolEngine& pool) {
            cout << "Task 4\n";
            pool.add_task([]() { cout << "Task 5, a second later\n"; }, 1s);
        });
//! [Using the ThreadPool class]

    return 0;
}
