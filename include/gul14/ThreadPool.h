/**
 * \file    ThreadPool.h
 * \authors \ref contributors
 * \date    Created on November 6, 2018
 * \brief   Declaration of the ThreadPool class.
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

// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef GUL14_THREADPOOL_H_
#define GUL14_THREADPOOL_H_

#include <chrono>
#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace gul14 {

/**
 * \addtogroup ThreadPool_h gul14/ThreadPool.h
 * \brief A thread pool and task queue.
 * @{
 */

/**
 * A pool of worker threads with a task queue.
 *
 * A ThreadPool creates the desired number of worker threads when it is constructed and
 * keeps them running until the object gets destroyed. Work is given to the pool with the
 * add_task() function in the form of a function object or function pointer. Tasks are
 * stored in a queue and executed in the order they were added. This makes a ThreadPool
 * with a single thread effectively a serial task queue.
 *
 * Each task is associated with a unique ID. This ID is returned by add_task() and can
 * be used to query the status of the task or to remove it from the queue via
 * remove_pending_task(). Tasks that are already running cannot be canceled.
 *
 * Tasks can be scheduled to start at a specific time point or after a certain delay. Each
 * task can also be given a name, which is mainly useful for debugging.
 *
 * \code{.cpp}
 * ThreadPool pool(2); // Create a pool with 2 threads
 *
 * pool.add_task([]() { std::cout << "Task 1\n"; });
 * pool.add_task([]() { sleep(1); std::cout << "Task 2\n"; });
 *
 * // Start 2 seconds after enqueueing (if a thread is available)
 * pool.add_task([]() { std::cout << "Task 3\n"; }, 2s);
 *
 * // Probable output:
 * // Task 1
 * // Task 2
 * // Task 3
 *
 * sleep(3); // Wait for the tasks above to finish
 *
 * // Tasks can also interact with the pool themselves, e.g. to schedule a continuation:
 * pool.add_task(
 *     [](ThreadPool& pool) {
 *         std::cout << "Task 4\n";
 *         pool.add_task([]() { std::cout << "Task 5, a second later\n"; }, 1s);
 *     });
 * \endcode
 *
 * All public member functions are thread-safe.
 *
 * On Linux, threads in the pool explicitly block the signals SIGALRM, SIGINT, SIGPIPE,
 * SIGTERM, SIGURG, SIGUSR1, and SIGUSR2. This is done to prevent the threads from
 * terminating the whole process if one of these signals is received.
 *
 * \since GUL version 2.11
 */
class ThreadPool
{
public:
    using TaskId = std::uint64_t;
    using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
    using Duration = TimePoint::duration;

    /// Maximum possible capacity for the task queue.
    constexpr static std::size_t max_capacity{ 10'000'000 };

    /// Maximum possible number of threads
    constexpr static std::size_t max_threads{ 10'000 };

    /**
     * Create a thread pool with the desired number of threads and the specified capacity
     * for queuing tasks.
     *
     * This constructor launches the desired number of threads. The threads are joined
     * when the ThreadPool object gets destroyed.
     *
     * \param num_threads  Desired number of threads
     * \param capacity     Maximum number of pending tasks that can be queued
     *
     * \exception std::invalid_argument is thrown if the desired number of threads is
     *            zero or greater than max_threads, or if the requested capacity is zero
     *            or exceeds max_capacity.
     */
    ThreadPool(std::size_t num_threads, std::size_t capacity = 1000);

    /**
     * Destruct the ThreadPool and join all threads.
     *
     * This destructor blocks until all threads have terminated. Work that has not yet
     * been started in one of the threads gets canceled, but work that has already been
     * assigned to a thread continues to be executed until it completes.
     */
    ~ThreadPool();

    /**
     * Enqueue a task.
     *
     * There are multiple overloads of this function for variations of the arguments:
     *
     * \param fct   A function object or function pointer to be executed. This function
     *              must return void and either takes no arguments (`void fct()`) or
     *              a reference to the ThreadPool by which it gets executed
     *              (`void fct(ThreadPool&)`). fct may not be a null pointer.
     * \param start_time  Earliest time point at which the task is to be started
     * \param name  Optional name for the task (mainly for debugging)
     *
     * \returns a unique ID to identify this task.
     * \exception std::runtime_error is thrown if the queue is full. std::invalid_argument
     *            is thrown if a null pointer is given.
     */
    TaskId add_task(std::function<void(ThreadPool&)> fct, TimePoint start_time = {},
        std::string name = {});

    TaskId add_task(std::function<void()> fct, TimePoint start_time = {},
        std::string name = {});

    /**
     * \overload
     *
     * \param fct   A function object or function pointer to be executed. This function
     *              must return void and either takes no arguments (`void fct()`) or
     *              a reference to the ThreadPool by which it gets executed
     *              (`void fct(ThreadPool&)`). fct may not be a null pointer.
     * \param delay_before_start  A time span (from now) that needs to pass before the
     *              task is started
     * \param name  Optional name for the task (mainly for debugging)
     *
     * \returns a unique ID to identify this task.
     * \exception hlc::Error is thrown if the queue is full. std::invalid_argument
     *            is thrown if a null pointer is given.
     */
    TaskId add_task(std::function<void(ThreadPool&)> fct, Duration delay_before_start,
        std::string name = {});

    TaskId add_task(std::function<void()> fct, Duration delay_before_start,
        std::string name = {});

    /// \overload
    TaskId add_task(std::function<void(ThreadPool&)> fct, std::string name);

    /// \overload
    TaskId add_task(std::function<void()> fct, std::string name);

    /// Return the maximum number of pending tasks that can be queued.
    std::size_t capacity() const noexcept { return capacity_; }

    /// Return the number of pending tasks.
    std::size_t count_pending() const;

    /// Return the number of threads in the pool.
    std::size_t count_threads() const noexcept;

    /// Return a vector with the IDs of the tasks that are currently running.
    std::vector<TaskId> get_running_task_ids() const;

    /// Return a vector with the names of the tasks that are currently running.
    std::vector<std::string> get_running_task_names() const;

    /// Determine whether the queue for pending tasks is full (at capacity).
    bool is_full() const noexcept;

    /**
     * Return true if the pool has neither pending tasks nor tasks that are currently
     * being executed.
     */
    bool is_idle() const;

    /// Return whether the thread pool has any pending task under the specified ID.
    bool is_pending(TaskId task_id) const;

    /**
     * Determine whether the thread pool has a task with the specified ID (which is only
     * the case if it is still pending or running).
     */
    bool is_pending_or_running(TaskId task_id) const;

    /// Determine whether the thread pool is currently executing the specified task.
    bool is_running(TaskId task_id) const;

    /**
     * Remove the pending task associated with the specified ID.
     *
     * This call looks for a pending task with the given ID. If one is found, it is
     * immediately removed and true is returned. If none is found, false is returned.
     * This function has no impact on tasks that are currently being executed.
     *
     * \param task_id  Unique ID for the task to be removed
     *
     * \returns true if a task was removed, false if no pending task with the given ID was
     *          found.
     */
    bool remove_pending_task(TaskId task_id);

    /**
     * Remove all pending tasks from the queue.
     *
     * This call removes all tasks that have not yet been started from the queue. It has
     * no impact on tasks that are currently being executed.
     *
     * \returns the number of tasks that were removed.
     */
    std::size_t remove_pending_tasks();

private:
    struct Task
    {
        std::string name_;
        std::function<void(ThreadPool&)> fct_;
        TaskId id_{};
        TimePoint start_time_{}; // When the task is to be started (at least no earlier)

        Task() = default;

        Task(TaskId task_id, std::function<void(ThreadPool&)> fct, std::string name,
             TimePoint start_time)
        : name_{ std::move(name) }
        , fct_{ std::move(fct) }
        , id_{ task_id }
        , start_time_{ start_time }
        {}
    };

    std::size_t capacity_{ 0 };

    /**
     * The threads in the pool. This variable is only modified in the constructor and not
     * protected by the mutex.
     */
    std::vector<std::thread> threads_;

    /**
     * A condition variable used together with mutex_ to wake up a worker thread when a
     * new task is added (or when shutdown is requested).
     */
    std::condition_variable cv_;

    mutable std::mutex mutex_; // Protects the following variables
    std::deque<Task> pending_tasks_;
    std::vector<TaskId> running_task_ids_;
    std::vector<std::string> running_task_names_;
    TaskId next_task_id_ = 0;
    bool cancel_requested_{ false };

    /// Non-locking internal versions of the public functions
    bool is_full_i() const noexcept;
    bool is_pending_i(TaskId task_id) const;
    bool is_running_i(TaskId task_id) const;

    /**
     * The main loop run in the thread; picks one task off the queue and executes it, then
     * repeats until asked to quit.
     */
    void perform_work();
};

/// @}

} // namespace gul14

#endif // GUL14_THREADPOOL_H_
