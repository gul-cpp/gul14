/**
 * \file    time_util.h
 * \authors \ref contributors
 * \date    Created on September 7, 2018
 * \brief   Declaration of time related functions for the General Utility Library.
 *
 * \copyright Copyright 2018-2019 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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

#include <chrono>
#include <thread>
#include "gul14/internal.h"
#include "gul14/Trigger.h"

namespace gul14 {


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

/**
 * Sleep for a given time span, with the option of being woken up from another thread.
 * The sleep can be interrupted from another thread via a shared \ref Trigger object.
 * \param duration   Time span to wait, as a std::chrono::duration type.
 * \param trg        Reference to a SleepInterrupt object that can be used to interrupt
 *                   the delay. If such an interruption occurs, false is returned.
 * \returns true if the entire requested sleep duration has passed, or false if the sleep
 *          has been interrupted prematurely via the Trigger object.
 *
 * \see \ref Trigger
 */
template< class Rep, class Period >
bool sleep(const std::chrono::duration<Rep, Period>& duration, const Trigger& trg)
{
    return !trg.wait_for(duration);
}

/**
 * Sleep for a given number of seconds, with the option of being woken up from another
 * thread. The sleep can be interrupted from another thread via a shared \ref Trigger
 * object.
 * \param seconds    Seconds to wait.
 * \param trg        Reference to a SleepInterrupt object that can be used to interrupt
 *                   the delay. If such an interruption occurs, false is returned.
 * \returns true if the entire requested sleep duration has passed, or false if the sleep
 *          has been interrupted prematurely via the Trigger object.
 *
 * \see \ref Trigger
 */
inline bool sleep(double seconds, const Trigger &trg)
{
    return sleep(std::chrono::duration<double>{ seconds }, trg);
}

/**
 * Sleep for a given time span.
 * 
 * Example:
 * \code
 * using std::literals; // for the "ms" suffix
 * sleep(5ms); // Wait 5 milliseconds
 * \endcode
 *
 * \param duration   Time span to wait, as a std::chrono::duration type.
 * \returns true to signalize that the entire requested sleep duration has passed. This is
 *          for symmetry with the interruptible version of sleep() only.
 */
template< class Rep, class Period >
bool sleep(const std::chrono::duration<Rep, Period>& duration)
{
    std::this_thread::sleep_for(duration);
    return true;
}

/**
 * Sleep for a given number of seconds.
 *
 * Example:
 * \code
 * sleep(0.005); // Wait 5 milliseconds
 * \endcode
 *
 * \param seconds  Seconds to wait.
 * \returns true to signalize that the entire requested sleep duration has passed. This is
 *          for symmetry with the interruptible version of sleep() only.
 */
inline bool sleep(double seconds)
{
    return sleep(std::chrono::duration<double>{ seconds });
}

} // namespace gul14
