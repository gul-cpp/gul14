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
void sleep(double seconds, std::atomic_bool *interrupt = nullptr);

/**
 * Sleep for a given time span.
 * Optionally, the sleep can be interrupted from another thread by setting a
 * std::atomic_bool value to true. The function polls this value with a period less than
 * or equal to 10 ms.
 * \param seconds    Number of seconds to wait.
 * \param interrupt  Pointer to a std::atomic_bool for interrupting the sleep. If a null
 *                   pointer is passed (default), the sleep is not interruptible.
 */
inline void sleep(std::chrono::duration<double> duration, std::atomic_bool *interrupt = nullptr)
{
    sleep(duration.count(), interrupt);
}

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
std::chrono::steady_clock::time_point tic();

/**
 * Return the elapsed seconds since the given time point.
 * This function is intended to be used with the sister function tic() to measure elapsed
 * time.
 *
 * <h4>Example</h4>
 * \code
 * auto t0 = tic();
 * // Do some work
 * std::cout << "Elapsed time: " << toc(t0) << " seconds.\n";
 * \endcode
 *
 * \see tic()
 */
double toc(std::chrono::steady_clock::time_point t0);

} // namespace gul
