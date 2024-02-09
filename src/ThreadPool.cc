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

#include <gul14/cat.h>
#include <gul14/ThreadPool.h>
#include <gul14/time_util.h>

#include <signal.h>

namespace gul14 {

namespace detail {

bool cancel_task(std::weak_ptr<ThreadPoolEngine> pool, TaskId id)
{
    auto shared_ptr = pool.lock();
    if (!shared_ptr)
        throw std::logic_error("Associated thread pool does not exist anymore");

    return shared_ptr->cancel_pending_task(id);
}

bool is_pending(std::weak_ptr<ThreadPoolEngine> pool, TaskId id)
{
    auto shared_ptr = pool.lock();
    if (!shared_ptr)
        throw std::logic_error("Associated thread pool does not exist anymore");

    return shared_ptr->is_pending(id);
}

bool is_running(std::weak_ptr<ThreadPoolEngine> pool, TaskId id)
{
    auto shared_ptr = pool.lock();
    if (!shared_ptr)
        throw std::logic_error("Associated thread pool does not exist anymore");

    return shared_ptr->is_running(id);
}

} // namespace detail


ThreadPoolEngine::ThreadPoolEngine(std::size_t num_threads, std::size_t capacity)
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

ThreadPoolEngine::~ThreadPoolEngine()
{
    std::unique_lock<std::mutex> lock(mutex_);
    shutdown_requested_ = true;
    lock.unlock();
    cv_.notify_all();

    for (auto& t : threads_)
    {
        if (t.joinable())
            t.join();
    }
}

bool ThreadPoolEngine::cancel_pending_task(const TaskId task_id)
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

std::size_t ThreadPoolEngine::cancel_pending_tasks()
{
    std::lock_guard<std::mutex> lock(mutex_);

    std::size_t num_removed = pending_tasks_.size();
    pending_tasks_.clear();

    return num_removed;
}

std::size_t ThreadPoolEngine::count_pending() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return pending_tasks_.size();
}

std::size_t ThreadPoolEngine::count_threads() const noexcept
{
    return threads_.size();
}

std::vector<std::string> ThreadPoolEngine::get_pending_task_names() const
{
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<std::string> names;
    names.resize(pending_tasks_.size());

    std::transform(pending_tasks_.begin(), pending_tasks_.end(), names.begin(),
        [](const Task& t) { return t.named_task_->name_; });

    return names;
}

std::vector<std::string> ThreadPoolEngine::get_running_task_names() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return running_task_names_;
}

bool ThreadPoolEngine::is_full() const noexcept
{
    std::lock_guard<std::mutex> lock(mutex_);
    return is_full_i();
}

bool ThreadPoolEngine::is_full_i() const noexcept
{
    return pending_tasks_.size() >= capacity_;
}

bool ThreadPoolEngine::is_idle() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return pending_tasks_.empty() && running_task_ids_.empty();
}

bool ThreadPoolEngine::is_pending(const TaskId task_id) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    const auto it = std::find_if(pending_tasks_.begin(), pending_tasks_.end(),
        [task_id](const Task& t) { return t.id_ == task_id; });

    return it != pending_tasks_.end();
}

bool ThreadPoolEngine::is_running(const TaskId task_id) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = std::find(running_task_ids_.begin(), running_task_ids_.end(), task_id);
    return it != running_task_ids_.end();
}

bool ThreadPoolEngine::is_shutdown_requested() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return shutdown_requested_;
}

void ThreadPoolEngine::perform_work()
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

    while (!shutdown_requested_)
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

        auto named_task_ptr = std::move(task_it->named_task_);
        auto id = task_it->id_;
        auto name = std::move(named_task_ptr->name_);
        pending_tasks_.erase(task_it);

        running_task_ids_.push_back(id);
        running_task_names_.push_back(name);

        lock.unlock();

        try
        {
            (*named_task_ptr)(*this);
        }
        catch (...)
        {
            // This should not happen because the packaged_task should catch all
            // exceptions itself. But in case of something unexpected, we'll try
            // to continue...
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

} // namespace gul14