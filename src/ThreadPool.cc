/**
 * \file    ThreadPool.cc
 * \authors \ref contributors
 * \date    Created on November 6, 2018
 * \brief   Implementation of the ThreadPool class.
 *
 * \copyright Copyright 2018-2024 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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

#include <algorithm>
#include <iostream>

#include <gul14/cat.h>
#include <gul14/ThreadPool.h>
#include <gul14/time_util.h>

#include <signal.h>

namespace gul14 {

ThreadPool::ThreadPool(std::size_t num_threads, std::size_t capacity)
    : capacity_(capacity)
{
    if (num_threads == 0 || num_threads > max_threads)
    {
        throw std::invalid_argument(
            cat("Illegal number of threads for thread pool: ", num_threads));
    }

    if (capacity == 0 || capacity > max_capacity)
        throw std::invalid_argument(cat("Illegal capacity for thread pool: ", capacity));

    threads_.reserve(num_threads);
    while (threads_.size() < num_threads)
        threads_.emplace_back([this]() { perform_work(); });
}

ThreadPool::~ThreadPool()
{
    std::unique_lock<std::mutex> lock(mutex_);
    cancel_requested_ = true;
    lock.unlock();
    cv_.notify_all();

    for (auto& t : threads_)
    {
        if (t.joinable())
            t.join();
    }
}

ThreadPool::TaskId ThreadPool::add_task(
    std::function<void(ThreadPool&)> fct, TimePoint start_time, std::string name)
{
    if (!fct)
        throw std::invalid_argument("ThreadPool does not accept null functions");

    auto task_id = [this, &fct, start_time, &name]()
        {
            std::lock_guard<std::mutex> lock(mutex_);

            if (is_full_i())
            {
                throw std::runtime_error(
                    cat("Cannot add task: Pending queue has reached capacity (",
                        pending_tasks_.size(), ")"));
            }

            const auto id = next_task_id_;
            pending_tasks_.emplace_back(id, std::move(fct), std::move(name), start_time);

            ++next_task_id_;

            return id;
        }();

    cv_.notify_one();

    return task_id;
}

ThreadPool::TaskId ThreadPool::add_task(
    std::function<void()> fct, TimePoint start_time, std::string name)
{
    if (!fct)
        throw std::invalid_argument("ThreadPool does not accept null functions");

    return add_task(
        [f = std::move(fct)](ThreadPool&) { f(); }, start_time, std::move(name));
}

ThreadPool::TaskId ThreadPool::add_task(std::function<void(ThreadPool&)> fct,
    ThreadPool::Duration delay_before_start, std::string name)
{
    return add_task(std::move(fct),
        std::chrono::system_clock::now() + delay_before_start, std::move(name));
}

ThreadPool::TaskId ThreadPool::add_task(std::function<void()> fct,
    ThreadPool::Duration delay_before_start, std::string name)
{
    return add_task(std::move(fct),
        std::chrono::system_clock::now() + delay_before_start, std::move(name));
}

ThreadPool::TaskId ThreadPool::add_task(
    std::function<void(ThreadPool&)> fct, std::string name)
{
    return add_task(std::move(fct), TimePoint{}, std::move(name));
}

ThreadPool::TaskId ThreadPool::add_task(std::function<void()> fct, std::string name)
{
    return add_task(std::move(fct), TimePoint{}, std::move(name));
}

std::size_t ThreadPool::count_pending() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return pending_tasks_.size();
}

std::size_t ThreadPool::count_threads() const noexcept
{
    return threads_.size();
}

/// Return a vector with the IDs of the jobs that are currently running.
std::vector<ThreadPool::TaskId> ThreadPool::get_running_task_ids() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return running_task_ids_;
}

/// Return a vector with the names of the jobs that are currently running.
std::vector<std::string> ThreadPool::get_running_task_names() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return running_task_names_;
}

bool ThreadPool::is_full() const noexcept
{
    std::lock_guard<std::mutex> lock(mutex_);
    return is_full_i();
}

bool ThreadPool::is_full_i() const noexcept
{
    return pending_tasks_.size() >= capacity_;
}

bool ThreadPool::is_idle() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return pending_tasks_.empty() && running_task_ids_.empty();
}

bool ThreadPool::is_pending(const ThreadPool::TaskId task_id) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return is_pending_i(task_id);
}

bool ThreadPool::is_pending_i(const ThreadPool::TaskId task_id) const
{
    const auto it = std::find_if(pending_tasks_.begin(), pending_tasks_.end(),
        [task_id](const Task& t) { return t.id_ == task_id; });

    return it != pending_tasks_.end();
}

bool ThreadPool::is_pending_or_running(const ThreadPool::TaskId task_id) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return is_pending_i(task_id) || is_running_i(task_id);
}

bool ThreadPool::is_running(const ThreadPool::TaskId task_id) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return is_running_i(task_id);
}

bool ThreadPool::is_running_i(const ThreadPool::TaskId task_id) const
{
    auto it = std::find(running_task_ids_.begin(), running_task_ids_.end(), task_id);
    return it != running_task_ids_.end();
}

void ThreadPool::perform_work()
{
#if defined(__APPLE__) || defined(__GNUC__)
    // On unixoid systems, we block a number of signals in the worker threads because we
    // do not explicitly handle them. Otherwise, a worker thread receiving a signal might
    // terminate the whole process.

    sigset_t mask;

    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGPIPE);
    sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGURG);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);

    pthread_sigmask(SIG_BLOCK, &mask, 0);
#endif

    std::unique_lock<std::mutex> lock(mutex_);

    while (!cancel_requested_)
    {
        // mutex is locked
        if (pending_tasks_.empty())
        {
            cv_.wait(lock); // acquires the lock when done
            continue;
        }

        const auto now = std::chrono::system_clock::now();
        auto task_it = std::find_if(pending_tasks_.begin(), pending_tasks_.end(),
            [now](const Task& t) { return t.start_time_ <= now; });

        if (task_it == pending_tasks_.end())
        {
            task_it = std::min_element(pending_tasks_.begin(), pending_tasks_.end(),
                [](const Task& a, const Task& b) { return a.start_time_ < b.start_time_; });

            cv_.wait_until(lock, task_it->start_time_); // acquires the lock when done
            continue;
        }

        auto fct = std::move(task_it->fct_);
        auto id = task_it->id_;
        auto name = std::move(task_it->name_);
        pending_tasks_.erase(task_it);

        running_task_ids_.push_back(id);
        running_task_names_.push_back(name);

        lock.unlock();

        try
        {
            fct(*this);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Exception in task \"" << name << "\": " << e.what() << std::endl;
        }
        catch (...)
        {
            std::cerr << "Unknown exception in task \"" << name << "\"" << std::endl;
        }

        lock.lock();

        auto it = std::find(running_task_ids_.begin(), running_task_ids_.end(), id);
        if (it != running_task_ids_.end())
        {
            running_task_ids_.erase(it);
            running_task_names_.erase(
                running_task_names_.begin() + (it - running_task_ids_.begin()));
        }
    }
}

bool ThreadPool::remove_pending_task(const ThreadPool::TaskId task_id)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = std::find_if(pending_tasks_.begin(), pending_tasks_.end(),
        [task_id](const Task& t) { return t.id_ == task_id; });
    if (it != pending_tasks_.end())
    {
        pending_tasks_.erase(it);
        return true;
    }

    return false;
}

std::size_t ThreadPool::remove_pending_tasks()
{
    std::lock_guard<std::mutex> lock(mutex_);

    std::size_t num_removed = pending_tasks_.size();
    pending_tasks_.clear();

    return num_removed;
}

} // namespace gul14
