/**
 * \file    ThreadPool.h
 * \authors \ref contributors
 * \date    Created on November 6, 2018
 * \brief   Declaration of the ThreadPool class.
 *
 * \copyright Copyright 2018-2025 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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
#include <limits>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

#include <gul14/cat.h>
#include <gul14/traits.h>

namespace gul14 {

class ThreadPool;

namespace detail {

GUL_EXPORT
std::shared_ptr<ThreadPool> lock_pool_or_throw(std::weak_ptr<ThreadPool> pool);

} // namespace detail

/**
 * \addtogroup ThreadPool_h gul14/ThreadPool.h
 * \brief A thread pool and task queue.
 * @{
 */

/**
 * \example thread_pool.cc
 * An example on how to use the ThreadPool class to schedule tasks.
 */

/// An enum describing the state of an individual task.
enum class TaskState
{
    pending,  ///< The task is waiting to be started.
    running,  ///< The task is currently being executed.
    complete, ///< The task has finished (successfully or by throwing an exception).
    canceled  ///< The task was removed from the queue before it was started.
};

/**
 * A pool of worker threads with a task queue.
 *
 * A thread pool is created with make_thread_pool(). It immediately starts the desired
 * number of worker threads and keeps them running until the object gets destroyed. Work
 * is given to the pool with the add_task() function in the form of a function object or
 * function pointer. Tasks are stored in a queue and executed in the order they were
 * added. This makes a ThreadPool with a single thread effectively a serial task queue.
 *
 * Each task is associated with a TaskHandle. This handle is returned by add_task() and
 * can be used to query the status of the task or to remove it from the queue via
 * \ref gul14::ThreadPool::TaskHandle::cancel() "cancel()". Tasks that are already running
 * cannot be canceled.
 *
 * \code{.cpp}
 * auto pool = make_thread_pool(1);
 * auto task = pool->add_task([]() { return 42; });
 * while (not task.is_complete())
 * {
 *     std::cout << "Waiting for task to complete...\n";
 *     sleep(0.1);
 * }
 * std::cout << "Task result: " << task.get_result() << "\n";
 * \endcode
 *
 * Tasks can also be scheduled to start after a specific time point or after a certain
 * delay. Each task can also be given a name, which is mainly useful for debugging. See
 * the \ref thread_pool.cc "example" for an introduction.
 *
 * All public member functions are thread-safe.
 *
 * On Linux, threads in the pool explicitly block the signals SIGALRM, SIGINT, SIGPIPE,
 * SIGTERM, SIGURG, SIGUSR1, and SIGUSR2. This is done to prevent the threads from
 * terminating the whole process if one of these signals is received.
 *
 * \since GUL version 2.11
 */
class ThreadPool : public std::enable_shared_from_this<ThreadPool>
{
public:
    /// A unique identifier for a task.
    using TaskId = std::uint64_t;

    /**
     * A handle for a task that has (or had) been enqueued on a ThreadPool.
     *
     * A TaskHandle can be used to query the status of a task and to retrieve its result.
     *
     * \code{.cpp}
     * auto pool = make_thread_pool(1);
     * auto task = pool->add_task([]() { return 42; });
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
         * Default-construct an invalid TaskHandle.
         *
         * This constructor creates an invalid TaskHandle which has no result and is not
         * associated with a ThreadPool.
         */
        TaskHandle()
        {}

        /**
         * Construct a TaskHandle.
         *
         * This constructor is not meant to be used directly. Instead, TaskHandles are
         * returned by the ThreadPool when a task is enqueued.
         *
         * \param id      Unique ID of the task
         * \param future  A std::future that will eventually contain the result of the
         *                task
         * \param pool    A shared pointer to the ThreadPool that the task is associated
         *                with
         */
        TaskHandle(TaskId id, std::future<T> future, std::shared_ptr<ThreadPool> pool)
            : future_{ std::move(future) }
            , id_{ id }
            , pool_{ std::move(pool) }
        {}

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
        bool cancel()
        {
            future_ = {};
            return detail::lock_pool_or_throw(pool_)->cancel_pending_task(id_);
        }

        /**
         * Block until the task has finished and return its result.
         *
         * If `is_complete() == true`, the result is available immediately. If the task
         * finished by throwing an exception, get_result() rethrows this exception.
         */
        T get_result()
        {
            if (not future_.valid())
                throw std::logic_error("Canceled task has no result");
            return future_.get();
        }

        /**
         * Determine whether the task has completed.
         *
         * This function returns true if the task has finished, either successfully or by
         * throwing an exception. It returns false if the task is still running, waiting
         * to be started, or has been canceled.
         *
         * \note
         * is_complete() does not need to interact with the ThreadPool to determine the
         * state of the task. It is therefore slightly more performant than get_state(),
         * but does not deliver the same fine-grained information.
         */
        bool is_complete() const
        {
            if (not future_.valid())
                return false;

            return future_.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
        }

        /**
         * Determine if the task is running, waiting to be started, completed, or has been
         * canceled.
         *
         * \exception std::logic_error is thrown if the associated thread pool does not
         *            exist anymore.
         *
         * \note
         * If you just need to find out if a task has finished running, prefer
         * is_complete() over this function. It does not need to interact with the
         * ThreadPool and is therefore slightly more performant.
         */
        TaskState get_state() const
        {
            const auto state = detail::lock_pool_or_throw(pool_)->get_task_state(id_);

            if (state == InternalTaskState::unknown)
            {
                if (is_complete())
                    return TaskState::complete;
                else
                    return TaskState::canceled;
            }
            return static_cast<TaskState>(state);
        }

    private:
        std::future<T> future_;
        TaskId id_{ 0 };
        std::weak_ptr<ThreadPool> pool_;
    };


    using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
    using Duration = TimePoint::duration;

    /// Default capacity for the task queue.
    constexpr static std::size_t default_capacity{ 200 };

    /// Maximum possible capacity for the task queue.
    constexpr static std::size_t max_capacity{ 10'000'000 };

    /// Maximum possible number of threads
    constexpr static std::size_t max_threads{ 10'000 };

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
     *              can have an arbitrary return type and may either take no arguments
     *              (`T fct()`) or a reference to the ThreadPool by which it gets
     *              executed (`T fct(ThreadPool&)`).
     * \param start_time  Earliest time point at which the task is to be started
     * \param name  Optional name for the task (mainly for debugging)
     *
     * \returns a TaskHandle that can be used for inquiries about the state of the task
     *          and to retrieve its return value.
     * \exception std::runtime_error is thrown if the queue is full.
     *
     * \code{.cpp}
     * auto pool = make_thread_pool(2); // Create a pool with 2 threads
     *
     * // A simple task that does not interact with the pool
     * pool->add_task([]() { std::cout << "Task 1\n"; });
     *
     * // A task that schedules another task to start two seconds later
     * pool->add_task(
     *     [](ThreadPool& pool)
     *     {
     *         std::cout << "Task 2\n";
     *         pool.add_task([]() { std::cout << "Task 3\n"; }, 2s);
     *     });
     *
     * // A task with a name
     * pool->add_task([]() { std::cout << "Task 4\n"; }, "Task 4");
     * \endcode
     *
     * \since GUL version 2.12.1, add_task() unconditionally accepts mutable function
     *        objects
     */
    template <typename Function>
    TaskHandle<invoke_result_t<Function, ThreadPool&>>
    add_task(Function fct, TimePoint start_time = {}, std::string name = {})
    {
        static_assert(
            is_invocable<Function, ThreadPool&>::value
            || is_invocable<Function>::value,
            "Invalid function signature: Must be T fct() or T fct(ThreadPool&)");

        using Result = invoke_result_t<Function, ThreadPool&>;

        TaskHandle<Result> task_handle = [this, &fct, start_time, &name]()
            {
                std::lock_guard<std::mutex> lock(mutex_);

                if (is_full_i())
                {
                    throw std::runtime_error(cat(
                        "Cannot add task: Pending queue has reached capacity (",
                        pending_tasks_.size(), ')'));
                }

                using PackagedTask = std::packaged_task<Result(ThreadPool&)>;

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
            [f = std::move(fct)](ThreadPool&) mutable { return f(); },
            start_time, std::move(name));
    }

    template <typename Function,
        std::enable_if_t<is_invocable<Function, ThreadPool&>::value, bool> = true>
    TaskHandle<invoke_result_t<Function, ThreadPool&>>
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
        std::enable_if_t<is_invocable<Function, ThreadPool&>::value, bool> = true>
    TaskHandle<invoke_result_t<Function, ThreadPool&>>
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
     * Remove all pending tasks from the queue.
     *
     * This call removes all tasks that have not yet been started from the queue. It has
     * no impact on tasks that are currently being executed.
     *
     * \returns the number of tasks that were removed.
     */
    GUL_EXPORT
    std::size_t cancel_pending_tasks();

    /// Return the maximum number of pending tasks that can be queued.
    GUL_EXPORT
    std::size_t capacity() const noexcept { return capacity_; }

    /// Return the number of pending tasks.
    GUL_EXPORT
    std::size_t count_pending() const;

    /// Return the number of threads in the pool.
    GUL_EXPORT
    std::size_t count_threads() const noexcept;

    /// Return a vector with the names of the tasks that are waiting to be executed.
    GUL_EXPORT
    std::vector<std::string> get_pending_task_names() const;

    /// Return a vector with the names of the tasks that are currently running.
    GUL_EXPORT
    std::vector<std::string> get_running_task_names() const;

    /**
     * If called from a worker thread, return the zero-based index of the thread in the
     * pool (0 for the first thread, 1 for the second, and so on).
     *
     * \exception std::runtime_error is thrown if this function is called from a thread
     *            that is not part of the pool.
     *
     * \since GUL version 2.13
     */
    GUL_EXPORT
    std::size_t get_thread_index() const;

    /// Determine whether the queue for pending tasks is full (at capacity).
    GUL_EXPORT
    bool is_full() const noexcept;

    /**
     * Return true if the pool has neither pending tasks nor tasks that are currently
     * being executed.
     */
    GUL_EXPORT
    bool is_idle() const;

    /// Determine whether the thread pool has been requested to shut down.
    GUL_EXPORT
    bool is_shutdown_requested() const;

    /**
     * Create a thread pool with the desired number of threads and the specified capacity
     * for enqueuing tasks.
     *
     * The thread pool is allocated in a shared pointer, which is necessary so that task
     * handles can access the pool safely. A ThreadPool cannot be constructed directly.
     *
     * \returns a shared pointer to the created ThreadPool object.
     */
    GUL_EXPORT
    static std::shared_ptr<ThreadPool> make_shared(
        std::size_t num_threads, std::size_t capacity = default_capacity);

private:
    /**
     * An enum describing the internal state of a task on the ThreadPool.
     *
     * This enum just describes if the task is pending, running, or neither. The pool has
     * no further knowledge about the task, but a TaskHandle can additionally determine if
     * a task has completed or been canceled.
     */
    enum class InternalTaskState
    {
        /// The task is waiting to be started
        pending = static_cast<int>(TaskState::pending),
        /// The task is currently being executed
        running = static_cast<int>(TaskState::running),
        /// The thread pool has no knowledge about this task
        unknown
    };

    struct NamedTask
    {
        NamedTask(std::string name)
        : name_{ std::move(name) }
        {}

        virtual ~NamedTask() = default;
        virtual void operator()(ThreadPool& pool) = 0;

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

        void operator()(ThreadPool& pool) override { fct_(pool); }

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
     * For worker threads, this is the index of the thread in the threads_ vector.
     * For other threads, the value is meaningless and the variable is initialized to
     * numeric_limits<size_t>::max().
     */
    thread_local static std::size_t thread_index_;

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


    /**
     * Create a thread pool with the desired number of threads and the specified capacity
     * for queuing tasks.
     *
     * This constructor may not be used directly. Use make_thread_pool() to allocate a
     * ThreadPool object in a shared pointer.
     *
     * Upon construction, the desired number of threads is launched. The threads are
     * joined when the ThreadPool object gets destroyed.
     *
     * \param num_threads  Desired number of threads
     * \param capacity     Maximum number of pending tasks that can be queued
     *
     * \exception std::invalid_argument is thrown if the desired number of threads is
     *            zero or greater than max_threads, or if the requested capacity is zero
     *            or exceeds max_capacity.
     */
    ThreadPool(std::size_t num_threads, std::size_t capacity);

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
    GUL_EXPORT
    bool cancel_pending_task(TaskId task_id);

    /**
     * Determine the state of the task with the specified ID.
     *
     * \param task_id  Unique ID of the task
     *
     * \returns InternalTaskState::pending if the task is still waiting to be started,
     *     InternalTaskState::running if the task is currently being executed, or
     *     InternalTaskState::unknown if the thread pool has no knowledge of this task ID.
     */
    GUL_EXPORT
    InternalTaskState get_task_state(TaskId task_id) const;

    /**
     * Determine whether the queue for pending tasks is full (internal non-locking
     * version).
     */
    GUL_EXPORT
    bool is_full_i() const noexcept;

    /**
     * The main loop run in the thread; picks one task off the queue and executes it, then
     * repeats until asked to quit.
     *
     * \param thread_index  Index of the thread in the threads_ vector
     */
    void perform_work(std::size_t thread_index);
};

/**
 * Create a thread pool with the desired number of threads and the specified capacity for
 * queuing tasks.
 *
 * The thread pool is allocated in a shared pointer, which is necessary so that task
 * handles can access the pool safely. A ThreadPool cannot be constructed directly.
 *
 * \returns a shared pointer to the created ThreadPool object.
 */
inline std::shared_ptr<ThreadPool> make_thread_pool(
    std::size_t num_threads, std::size_t capacity = ThreadPool::default_capacity)
{
    return ThreadPool::make_shared(num_threads, capacity);
}

/// @}

} // namespace gul14

#endif // GUL14_THREADPOOL_H_
