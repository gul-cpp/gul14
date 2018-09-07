/**
 * \file    time_util.cc
 * \authors \ref contributors
 * \brief   Implementation of time related functions for the General Utility Library.
 * \date    Created on September 7, 2018
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

#include "gul/time_util.h"
#include <ctime>
#include <thread>

namespace gul {

    
void sleep(double seconds, std::atomic_bool *interrupt)
{
    auto t0 = tic();
    auto seconds_remaining = seconds;

    while (seconds_remaining >= 0)
    {
        // External interrupt requested?
        if (interrupt != nullptr && *interrupt)
            return;

        // Can we just sleep for all the remaining time?
        if (interrupt == nullptr || seconds_remaining < 0.01)
        {
            std::this_thread::sleep_for(std::chrono::duration<double>(seconds_remaining));
            return;
        }

        std::this_thread::sleep_for(std::chrono::microseconds(9500));

        seconds_remaining = seconds - toc(t0);
    }
}

std::chrono::steady_clock::time_point tic()
{
    return std::chrono::steady_clock::now();
}

double toc(std::chrono::steady_clock::time_point t0)
{
    std::chrono::duration<double> delta_t = tic() - t0;
    return delta_t.count();
}


} // namespace gul
