/**
 * \file    Trigger.cc
 * \authors \ref contributors
 * \brief   Implementation of the Trigger class for the General Utility Library.
 * \date    Created on September 21, 2018
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

#include "gul14/Trigger.h"

namespace gul {


Trigger::~Trigger() noexcept
{
    trigger();
}

Trigger &Trigger::operator=(bool interrupt) noexcept
{
    if (interrupt)
    {
        this->trigger();
    }
    else
    {
        std::lock_guard<std::mutex> lock(mutex_);
        triggered_ = false;
    }

    return *this;
}

Trigger::operator bool() const noexcept
{
    std::lock_guard<std::mutex> lock(mutex_);
    return triggered_;
}

void Trigger::reset() noexcept
{
    std::lock_guard<std::mutex> lock(mutex_);
    triggered_ = false;
}

void Trigger::trigger() noexcept
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        triggered_ = true;
    }

    // It is more efficient if we do not hold the lock on mutex_ when notifying other
    // threads, because they need to acquire the lock as well.
    cv_.notify_all();
}

void Trigger::wait() const
{
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this]{ return triggered_; });
}


} // namespace gul
