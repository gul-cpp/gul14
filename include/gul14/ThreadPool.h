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
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

#include <gul14/cat.h>
#include <gul14/traits.h>

namespace gul14 {

class ThreadPoolEngine;

using TaskId = std::uint64_t;

namespace detail {
bool cancel_task(std::weak_ptr<ThreadPoolEngine> pool, TaskId task_id);
bool is_running(std::weak_ptr<ThreadPoolEngine> pool, TaskId task_id);
bool is_pending(std::weak_ptr<ThreadPoolEngine> pool, TaskId task_id);
} // namespace detail

/**
 * \addtogroup ThreadPool_h gul14/ThreadPool.h
 * \brief A thread pool and task queue.
 *
 * A ThreadPool creates the desired number of worker threads when it is constructed and
 * keeps them running until the object gets destroyed. Work is given to the pool with the
 * add_task() function in the form of a function object or function pointer. Tasks are
 * stored in a queue and executed in the order they were added. This makes a ThreadPool
 * with a single thread effectively a serial task queue.
 *
 * Each task is associated with a TaskHandle. This handle is returned by add_task() and
 * can be used to query the status of the task or to remove it from the queue via
 * cancel(). Tasks that are already running cannot be canceled.
 *
 * Tasks can be scheduled to start at a specific time point or after a certain delay. Each
 * task can also be given a name, which is mainly useful for debugging.
 *
 * \ref thread_pool.cpp "thread_pool.cpp".
 *
 * \example thread_pool.cc
 * An example on how to use the ThreadPool class to schedule tasks.
 *
 * @{
 */

/**
 * A handle for a task that has (or had) been enqueued on a ThreadPool.
 *
 * \code{.cpp}
 * ThreadPool pool(1);
 * auto task = pool.add_task([]() { return 42; });
 * while (not task.is_complete())
 * {
 *     std::cout << "Waiting for task to complete...\n";
 *     sleep(0.1);
 * }
 * std::cout << "Task result: " << task.get_result() << "\n";
 * \endcode
 */
template <typename T>
class TaskHandle
{
public:
    /**
     * Remove the task from the queue if it is still pending.
     *
     * This call has no effect if the task is already running.
     *
     * \returns true if the task was removed from the queue, false if it was not found in
     *          the queue (e.g. because it is already running).
     *
     * \exception std::logic_error is thrown if the associated thread pool does not
     *            exist anymore.
     */
    bool cancel() const { return detail::cancel_task(pool_, id_); }

    /// Return the ID of the task.
    TaskId get_id() const noexcept { return id_; }

    /// Block until the task has finished and return its result.
    T get_result() const { return future_.get(); }

    /// Determine whether the task has finished.
    bool is_complete() const { return future_.wait_for(std::chrono::seconds(0)) == std::future_status::ready; }

    /**
     * Return true if the task is still waiting to be started.
     *
     * \exception std::logic_error is thrown if the associated thread pool does not
     *            exist anymore.
     */
    bool is_pending() const { return detail::is_pending(pool_, id_); }

    /**
     * Return true if the task is currently being executed.
     * \exception std::logic_error is thrown if the associated thread pool does not
     *            exist anymore.
     */
    bool is_running() const { return detail::is_running(pool_, id_); }

private:
    friend class ThreadPoolEngine;

    std::future<T> future_;
    TaskId id_;
    std::weak_ptr<ThreadPoolEngine> pool_;

    TaskHandle(TaskId id, std::future<T> future, std::shared_ptr<ThreadPoolEngine> pool)
    : future_{ std::move(future) }
    , id_{ id }
    , pool_{ pool }
    {}

};

/**
 * The engine behind a ThreadPool, sharing most of the same functionality.
 *
 * This class should rarely be used directly, with the notable exception of tasks that
 * intend to interact with the thread pool themselves. These tasks can receive a reference
 * to the ThreadPoolEngine by which they are executed. For instance, tasks could use this
 * to schedule a continuation after a certain delay or to find out if the pool has been
 * requested to shut down:
 * \code{.cpp}
 * ThreadPool pool(2); // Create a pool with 2 threads
 *
 * pool.add_task(
 *     [](ThreadPoolEngine& pool)
 *     {
 *         for (int i = 0; i != 1000; ++i)
 *         {
 *             do_some_work();
 *             if (pool.is_shutdown_requested())
 *                 return;
 *         }
 *         pool.add_task([]() { std::cout << "Task 2, a second later\n"; }, 1s);
 *     });
 * \endcode
 *
 * All public member functions are thread-safe.
 *
 * \since GUL version 2.11
 */
class ThreadPoolEngine : public std::enable_shared_from_this<ThreadPoolEngine>
{
public:
    using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
    using Duration = TimePoint::duration;

    /// Maximum possible capacity for the task queue.
    constexpr static std::size_t max_capacity{ 10'000'000 };

    /// Maximum possible number of threads
    constexpr static std::size_t max_threads{ 10'000 };

    ThreadPoolEngine(std::size_t num_threads, std::size_t capacity);

    /**
     * Destruct the ThreadPoolEngine and join all threads.
     *
     * This destructor blocks until all threads have terminated. Work that has not yet
     * been started in one of the threads gets canceled, but work that has already been
     * assigned to a thread continues to be executed until it completes.
     */
    ~ThreadPoolEngine();

    /**
     * Enqueue a task.
     *
     * There are multiple overloads of this function for variations of the arguments:
     *
     * \param fct   A function object or function pointer to be executed. This function
     *              can have an arbitrary return type and may either take no arguments
     *              (`T fct()`) or a reference to the ThreadPoolEngine by which it gets
     *              executed (`T fct(ThreadPoolEngine&)`).
     * \param start_time  Earliest time point at which the task is to be started
     * \param name  Optional name for the task (mainly for debugging)
     *
     * \returns a unique ID to identify this task.
     * \exception std::runtime_error is thrown if the queue is full.
     */
    template <typename Function>
    TaskHandle<invoke_result_t<Function, ThreadPoolEngine&>>
    add_task(Function fct, TimePoint start_time = {}, std::string name = {})
    {
        using Result = invoke_result_t<Function, ThreadPoolEngine&>;

        TaskHandle<Result> task_handle = [this, &fct, start_time, &name]()
            {
                std::lock_guard<std::mutex> lock(mutex_);

                if (is_full_i())
                {
                    throw std::runtime_error(cat(
                        "Cannot add task: Pending queue has reached capacity (",
                        pending_tasks_.size(), ')'));
                }

                using PackagedTask = std::packaged_task<Result(ThreadPoolEngine&)>;

                auto named_task_ptr = std::make_unique<NamedTaskImpl<PackagedTask>>(
                    PackagedTask{ std::move(fct) }, std::move(name));

                TaskHandle<Result> handle{
                    next_task_id_, named_task_ptr->fct_.get_future(), shared_from_this() };

                pending_tasks_.emplace_back(
                    next_task_id_, std::move(named_task_ptr), start_time);

                ++next_task_id_;

                return handle;
            }();

        cv_.notify_one();

        return task_handle;
    }

    template <typename Function,
        std::enable_if_t<is_invocable<Function>::value, bool> = true>
    TaskHandle<invoke_result_t<Function>>
    add_task(Function fct, TimePoint start_time = {}, std::string name = {})
    {
        return add_task(
            [f = std::move(fct)](ThreadPoolEngine&) { return f(); },
            start_time, std::move(name));
    }

    template <typename Function,
        std::enable_if_t<is_invocable<Function, ThreadPoolEngine&>::value, bool> = true>
    TaskHandle<invoke_result_t<Function, ThreadPoolEngine&>>
    add_task(Function fct, Duration delay_before_start, std::string name = {})
    {
        return add_task(std::move(fct),
            std::chrono::system_clock::now() + delay_before_start, std::move(name));
    }

    template <typename Function,
        std::enable_if_t<is_invocable<Function>::value, bool> = true>
    TaskHandle<invoke_result_t<Function>>
    add_task(Function fct, Duration delay_before_start, std::string name = {})
    {
        return add_task(std::move(fct),
            std::chrono::system_clock::now() + delay_before_start, std::move(name));
    }

    template <typename Function,
        std::enable_if_t<is_invocable<Function, ThreadPoolEngine&>::value, bool> = true>
    TaskHandle<invoke_result_t<Function, ThreadPoolEngine&>>
    add_task(Function fct, std::string name)
    {
        return add_task(std::move(fct), TimePoint{}, std::move(name));
    }

    template <typename Function,
        std::enable_if_t<is_invocable<Function>::value, bool> = true>
    TaskHandle<invoke_result_t<Function>>
    add_task(Function fct, std::string name)
    {
        return add_task(std::move(fct), TimePoint{}, std::move(name));
    }

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
    bool cancel_pending_task(TaskId task_id);

    /**
     * Remove all pending tasks from the queue.
     *
     * This call removes all tasks that have not yet been started from the queue. It has
     * no impact on tasks that are currently being executed.
     *
     * \returns the number of tasks that were removed.
     */
    std::size_t cancel_pending_tasks();

    /// Return the maximum number of pending tasks that can be queued.
    std::size_t capacity() const noexcept { return capacity_; }

    /// Return the number of pending tasks.
    std::size_t count_pending() const;

    /// Return the number of threads in the pool.
    std::size_t count_threads() const noexcept;

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

    /// Determine whether the thread pool is currently executing the specified task.
    bool is_running(TaskId task_id) const;

    /// Determine whether the thread pool has been requested to shut down.
    bool is_shutdown_requested() const;

private:
    struct NamedTask
    {
        NamedTask(std::string name)
        : name_{ std::move(name) }
        {}

        virtual ~NamedTask() = default;
        virtual void operator()(ThreadPoolEngine& pool) = 0;

        std::string name_;
    };

    template <typename FunctionType>
    struct NamedTaskImpl : public NamedTask
    {
    public:
        NamedTaskImpl(FunctionType fct, std::string name)
        : NamedTask{ std::move(name) }
        , fct_{ std::move(fct) }
        {}

        void operator()(ThreadPoolEngine& pool) override { fct_(pool); }

        FunctionType fct_;
    };

    struct Task
    {
        TaskId id_{};
        std::unique_ptr<NamedTask> named_task_;
        TimePoint start_time_{}; // When the task is to be started (at least no earlier)

        Task() = default;

        Task(TaskId task_id, std::unique_ptr<NamedTask> named_task, TimePoint start_time)
        : id_{ task_id }
        , named_task_{ std::move(named_task) }
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
    std::vector<Task> pending_tasks_;
    std::vector<TaskId> running_task_ids_;
    std::vector<std::string> running_task_names_;
    TaskId next_task_id_ = 0;
    bool shutdown_requested_{ false };

    /// Non-locking internal versions of the public functions
    bool is_full_i() const noexcept;

    /**
     * The main loop run in the thread; picks one task off the queue and executes it, then
     * repeats until asked to quit.
     */
    void perform_work();
};

/**
 * A pool of worker threads with a task queue.
 *
 * A ThreadPool creates the desired number of worker threads when it is constructed and
 * keeps them running until the object gets destroyed. Work is given to the pool with the
 * add_task() function in the form of a function object or function pointer. Tasks are
 * stored in a queue and executed in the order they were added. This makes a ThreadPool
 * with a single thread effectively a serial task queue.
 *
 * Each task is associated with a TaskHandle. This handle is returned by add_task() and
 * can be used to query the status of the task or to remove it from the queue via
 * cancel(). Tasks that are already running cannot be canceled.
 *
 * Tasks can be scheduled to start at a specific time point or after a certain delay. Each
 * task can also be given a name, which is mainly useful for debugging.
 *
 * All public member functions are thread-safe.
 *
 * On Linux, threads in the pool explicitly block the signals SIGALRM, SIGINT, SIGPIPE,
 * SIGTERM, SIGURG, SIGUSR1, and SIGUSR2. This is done to prevent the threads from
 * terminating the whole process if one of these signals is received.
 *
 * \ref thread_pool.cpp "thread_pool.cpp".
 *
 * \since GUL version 2.11
 */
class ThreadPool
{
public:
    using TimePoint = ThreadPoolEngine::TimePoint;
    using Duration = ThreadPoolEngine::Duration;

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
    ThreadPool(std::size_t num_threads, std::size_t capacity = 200)
        : engine_{ std::make_shared<ThreadPoolEngine>(num_threads, capacity) }
    {}

    /**
     * Enqueue a task.
     *
     * There are multiple overloads of this function for variations of the arguments:
     *
     * \param fct  The first argument must be a function object or function pointer to be
     *             executed. This function can have an arbitrary return type and may
     *             either take no arguments (`T fct()`) or a reference to the
     *             ThreadPoolEngine by which it gets executed
     *             (`T fct(ThreadPoolEngine&)`).
     * \param args Start time and/or task name: As an optional second parameter, either a
     *             <b>time point</b> or a <b>duration</b> can be given. A time point
     *             specifies the earliest time at which the task may be started, a
     *             duration specifies a time point relative to the current time.
     *             As an optional last parameter, a <b>task name</b> can be specified.
     *             This is mainly useful for debugging.
     *
     * \returns a TaskHandle for this task.
     * \exception std::runtime_error is thrown if the queue is full.
     *
     * \code{.cpp}
     * ThreadPool pool(2); // Create a pool with 2 threads
     *
     * // A simple task that does not interact with the pool
     * pool.add_task([]() { std::cout << "Task 1\n"; });
     *
     * // A task that schedules another task to start two seconds later
     * pool.add_task(
     *     [](ThreadPoolEngine& pool)
     *     {
     *         std::cout << "Task 2\n";
     *         pool.add_task([]() { std::cout << "Task 3\n"; }, 2s);
     *     });
     *
     * // A task with a name
     * pool.add_task([]() { std::cout << "Task 4\n"; }, "Task 4");
     * \endcode
     */
    template <typename Function, typename... Args>
    auto add_task(Function fct, Args&&... args)
    {
        static_assert(
            is_invocable<Function, ThreadPoolEngine&>::value
            || is_invocable<Function>::value,
            "Invalid function signature: Must be T fct() or T fct(ThreadPoolEngine&)");

        return engine_->add_task(std::forward<Function>(fct), std::forward<Args>(args)...);
    }

    /**
     * Remove all pending tasks from the queue.
     *
     * This call removes all tasks that have not yet been started from the queue. It has
     * no impact on tasks that are currently being executed.
     *
     * \returns the number of tasks that were removed.
     */
    std::size_t cancel_pending_tasks() { return engine_->cancel_pending_tasks(); }

    /// Return the maximum number of pending tasks that can be queued.
    std::size_t capacity() const noexcept { return engine_->capacity(); }

    /// Return the number of pending tasks.
    std::size_t count_pending() const { return engine_->count_pending(); }

    /// Return the number of threads in the pool.
    std::size_t count_threads() const noexcept { return engine_->count_threads(); }

    /// Return a vector with the names of the tasks that are currently running.
    std::vector<std::string> get_running_task_names() const
    {
        return engine_->get_running_task_names();
    }

    /// Determine whether the queue for pending tasks is full (at capacity).
    bool is_full() const noexcept { return engine_->is_full(); }

    /**
     * Return true if the pool has neither pending tasks nor tasks that are currently
     * being executed.
     */
    bool is_idle() const { return engine_->is_idle(); }

private:
    std::shared_ptr<ThreadPoolEngine> engine_;
};

/// @}

} // namespace gul14

#endif // GUL14_THREADPOOL_H_
