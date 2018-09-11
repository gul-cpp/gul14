/**
 * \file    time_util.h
 * \authors \ref contributors
 * \date    Created on September 7, 2018
 * \brief   Declaration of time related functions for the General Utility Library.
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

#pragma once

#include <atomic>
#include <chrono>

namespace gul {

/**
 * Sleep for a given number of seconds.
 * Optionally, the sleep can be interrupted from another thread by setting a
 * std::atomic_bool value to true. The function polls this value with a period less than
 * or equal to 10 ms.
 * \param seconds    Number of seconds to wait.
 * \param interrupt  Pointer to a std::atomic_bool for interrupting the sleep. If a null
 *                   pointer is passed (default), the sleep is not interruptible.
 */
void sleep(double seconds, const std::atomic_bool *interrupt = nullptr);

/**
 * Sleep for a given time span.
 * Optionally, the sleep can be interrupted from another thread by setting a
 * std::atomic_bool value to true. The function polls this value with a period less than
 * or equal to 10 ms.
 * \param seconds    Number of seconds to wait.
 * \param interrupt  Pointer to a std::atomic_bool for interrupting the sleep. If a null
 *                   pointer is passed (default), the sleep is not interruptible.
 */
inline void sleep(std::chrono::duration<double> duration,
                  const std::atomic_bool *interrupt = nullptr)
{
    sleep(duration.count(), interrupt);
}


////////////////////////
//
// Alternative implementation of sleep, without polling
// Returns true if sleep has not been interrupted (i.e. went as long as expected)

struct SleepData {
    std::mutex m;
    std::condition_variable cv;
    std::atomic<bool> canceled{ false };

    void abort() noexcept {
        canceled = true;
        cv.notify_all();
    }
};

template< class Rep, class Period >
auto sleep2(const std::chrono::duration<Rep, Period>& sleep_duration, SleepData& sd) {
    auto const end = std::chrono::steady_clock::now() + sleep_duration;
    std::unique_lock<std::mutex> lk(sd.m);
    if (sd.canceled)
        return false;
    sd.cv.wait_until(lk, end,
        [&sd, &end]{ return std::chrono::steady_clock::now() >= end or sd.canceled; });
    return std::chrono::steady_clock::now() >= end;
}

template< class Rep, class Period >
auto sleep2(const std::chrono::duration<Rep, Period>& sleep_duration) {
    std::this_thread::sleep_for(sleep_duration);
    return true;
}

//
////////////////////////

/**
 * Return the current time as a std::chrono time_point.
 * This function is intended to be used with the sister function toc() to measure elapsed
 * time.
 * 
 * <h4>Example</h4>
 * \code
 * auto t0 = tic();
 * // Do some work
 * std::cout << "Elapsed time: " << toc(t0) << " seconds.\n";
 * \endcode
 *
 * \see toc()
 */
inline std::chrono::steady_clock::time_point tic()
{
    return std::chrono::steady_clock::now();
}

/**
 * Return the elapsed time in seconds (or a different unit) since the given time point.
 * This function is intended to be used with the sister function tic() to measure elapsed
 * time. toc() is a function template that returns the elapsed seconds as a double value
 * by default; by specifying a different \c chrono type as a template parameter, it can
 * also return other time units and other types.
 *
 * <h4>Example</h4>
 * \code
 * auto t0 = tic();
 *
 * // <do some work>
 * 
 * // Default: Return seconds as a double
 * std::cout << "Elapsed time: " << toc(t0) << " seconds.\n";
 * 
 * // Custom type: Return milliseconds as an integer
 * std::cout << "Elapsed time: " << toc<std::chrono::milliseconds>(t0) << " milliseconds.\n";
 * \endcode
 *
 * \tparam TimeUnitType  The type to be used for calculating the elapsed time since t0.
 *     By default, this is std::chrono::duration<double>, which means that the elapsed
 *     time is returned as a double that represents seconds.
 *
 * \param t0  A time point in the past that should be taken with tic().
 * \returns the elapsed time in the units and base representation of TimeUnitType. By
 *     default, this is a double that represents elapsed seconds. For a TimeUnitType of
 *     std::chrono::milliseconds, it would be an integer representing elapsed
 *     milliseconds.
 *
 * \see tic()
 */
template<class TimeUnitType = std::chrono::duration<double>>
auto toc(std::chrono::steady_clock::time_point t0)
{
    return std::chrono::duration_cast<TimeUnitType>(tic() - t0).count();
}


} // namespace gul
