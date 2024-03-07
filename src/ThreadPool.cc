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

std::shared_ptr<ThreadPool> lock_pool_or_throw(std::weak_ptr<ThreadPool> pool)
{
    auto shared_ptr = pool.lock();
    if (!shared_ptr)
        throw std::logic_error("Associated thread pool does not exist anymore");

    return shared_ptr;
}

} // namespace detail


//
// ThreadPool
//

ThreadPool::ThreadPool(std::size_t num_threads, std::size_t capacity)
    : max_threads_{ num_threads }
    , capacity_{ capacity }
{
    if (num_threads == 0 || num_threads > max_threads)
    {
        throw std::invalid_argument(
            cat("Illegal number of threads for thread pool: ", num_threads));
    }

    if (capacity == 0 || capacity > max_capacity)
        throw std::invalid_argument(cat("Illegal capacity for thread pool: ", capacity));

    threads_.reserve(num_threads);
}

ThreadPool::~ThreadPool()
{
    std::unique_lock<std::mutex> lock(mutex_);
    shutdown_requested_ = true;
    lock.unlock();
    cv_.notify_all();

    for (auto& t : threads_)
    {
        if (t.t.joinable())
            t.t.join();
    }
}

bool ThreadPool::cancel_pending_task(const TaskId task_id)
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

std::size_t ThreadPool::cancel_pending_tasks()
{
    std::lock_guard<std::mutex> lock(mutex_);

    std::size_t num_removed = pending_tasks_.size();
    pending_tasks_.clear();

    return num_removed;
}

std::size_t ThreadPool::count_pending() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return pending_tasks_.size();
}

std::size_t ThreadPool::count_threads() const noexcept
{
    std::lock_guard<std::mutex> lock(mutex_);
    return threads_.size();
}

std::vector<std::string> ThreadPool::get_pending_task_names() const
{
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<std::string> names;
    names.resize(pending_tasks_.size());

    std::transform(pending_tasks_.begin(), pending_tasks_.end(), names.begin(),
        [](const Task& t) { return t.named_task_->name_; });

    return names;
}

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

ThreadPool::InternalTaskState ThreadPool::get_task_state(const TaskId task_id) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    const auto itr = std::find(
        running_task_ids_.begin(), running_task_ids_.end(), task_id);
    if (itr != running_task_ids_.end())
        return InternalTaskState::running;

    const auto itp = std::find_if(
        pending_tasks_.begin(), pending_tasks_.end(),
        [task_id](const Task& t) { return t.id_ == task_id; });
    if (itp != pending_tasks_.end())
        return InternalTaskState::pending;

    return InternalTaskState::unknown;
}

bool ThreadPool::is_shutdown_requested() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return shutdown_requested_;
}

void ThreadPool::set_max_threads(std::size_t num_threads)
{
    if (num_threads == 0 || num_threads > max_threads)
    {
        throw std::invalid_argument(
            cat("Illegal number of threads for thread pool: ", num_threads));
    }
    std::lock_guard<std::mutex> lock(mutex_);
    max_threads_ = num_threads;
}

std::shared_ptr<ThreadPool> ThreadPool::make_shared(
    std::size_t num_threads, std::size_t capacity)
{
    // We cannot use std::make_shared() because the constructor is private.
    return std::shared_ptr<ThreadPool>(new ThreadPool(num_threads, capacity));
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

    while (!shutdown_requested_)
    {
        if (threads_.size() > max_threads_)
        {
            auto const new_end = std::remove_if(threads_.begin(), threads_.end(),
                [](Thread& t) {
                    if (not t.running)
                        t.t.join();
                    return not t.running; });
            threads_.erase(new_end, threads_.end());
            // Selfdestruct if we are not the survivor
            if (threads_.size() > max_threads_)
                break;
        }
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

            // Note: We may not pass task_it->start_time_ directly to wait_until() because
            // it may get invalidated when the mutex is unlocked.
            const auto start_time = task_it->start_time_;

            cv_.wait_until(lock, start_time); // acquires the lock when done
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
            const auto idx = it - running_task_ids_.begin();
            running_task_ids_.erase(it);
            running_task_names_.erase(running_task_names_.begin() + idx);
        }
    }
    // Mark ourselves dead before we stop executing
    for (auto& t : threads_) {
        if (std::this_thread::get_id() == t.t.get_id())
            t.running = false;
    }
}

} // namespace gul14
