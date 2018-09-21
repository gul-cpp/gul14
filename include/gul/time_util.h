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

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace gul {


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
 * A class that allows sending triggers and waiting for them across different threads.
 *
 * A Trigger object is similar to a digital electric trigger signal. Its state is either
 * high (true) or low (false), and actions are triggered when it is at the high level -
 * that is to say, an arbitrary number of threads can wait until the Trigger object
 * becomes true. Internally, a Trigger is little more than a wrapper around a mutex, a
 * condition variable, and a boolean.
 *
 * There are two ways to use a Trigger:
 *
 * <h3>Using bools</h3>
 *
 * To stay in the picture of a digital electric trigger line, high and low states can
 * be assigned and tested like booleans. This allows for easy communication of state:
 * \code
 * // Data shared between threads
 * Trigger trigger;
 *
 * ...
 *
 * // Thread 1
 * trigger.wait(); // wait until trigger becomes true
 * while (trigger)
 * {
 *     cout << "high\n";
 *     sleep(10ms);
 * }
 * cout << "low\n";
 * ...
 *
 * // Thread 2
 * trigger = true;
 * sleep(100ms);
 * trigger = false;
 * \endcode
 *
 * <h3>Using member functions</h3>
 *
 * Sometimes assignment of boolean values does not fit well to a specific task. The member
 * functions trigger() and reset() are identical to assigning \c true and \c false,
 * respectively:
 * \code
 * // Data shared between threads
 * Trigger trigger;
 *
 * ...
 *
 * // Thread 1
 * while (true)
 * {
 *     trigger.wait();
 *     cout << "Triggered!\n";
 *     trigger.reset();
 * }
 * ...
 *
 * // Thread 2
 * trigger.trigger();
 * sleep(100ms);
 * trigger.trigger();
 * sleep(100ms);
 * trigger.trigger();
 * \endcode
 *
 * \note
 * Trigger is a thread-safe, self-synchronizing class.
 */
class Trigger
{
public:
    explicit Trigger(bool triggered = false) noexcept : triggered_{ triggered }
    {}

    /**
     * Destructor: Interrupt all associated sleep() calls because they can no longer
     * safely monitor this object.
     */
    ~Trigger() noexcept
    {
        trigger();
    }

    /**
     * Return if the trigger has been activated.
     *
     * The boolean operator returns true if the trigger has been activated sleep was interrupted through this
     * SleepInterrupt object, or false otherwise.
     *
     * Example:
     * \code
     * SleepInterrupt si;
     * if (!si) // si now returns false
     *     cout << "No interrupt yet\n";
     * si.interrupt();
     * if (si) // si now returns true
     *     cout << "Interrupt has taken place\n";
     * \endcode
     */
    explicit operator bool() const noexcept;

    /**
     * Assignment: Cause associated sleep() calls to wake up if set to true, or reset
     * the interrupt flag to false.
     */
    Trigger &operator=(bool interrupt) noexcept;

    /**
     * Cause associated sleep() calls to wake up and set the interrupt flag to true.
     */
    void trigger() noexcept;

    /**
     * Reset the interrupt flag.
     * This call has no direct impact on any associated sleep() call.
     */
    void reset() noexcept;

    /**
     * Suspend execution of the current thread until a given time point or until the sleep
     * is interrupted from another thread.
     * For most applications, the free function sleep() is easier to use.
     *
     * \tparam Clock  The type of the underlying clock, e.g. std::chrono::system_clock.
     * \tparam Duration  The duration type to be used, typically Clock::duration.
     *
     * \param t  The function 
     *           time point is reached (or until some other thread has triggered
     *           an interrupt on this object).
     *
     * \returns true if the entire requested sleep time has passed, or false if the sleep
     *          has been interrupted prematurely.
     */
    template <class Clock, class Duration>
    bool sleep_until(const std::chrono::time_point<Clock, Duration> &t) const
    {
        std::unique_lock<std::mutex> lock(mutex_);

        if (triggered_)
            return false;

        cv_.wait_until(lock, t, [this]{ return triggered_; });

        return !triggered_;
    }

private:
    mutable std::mutex mutex_; // Protects private data and is used with the condition variable
    mutable std::condition_variable cv_;
    bool triggered_ = false;
};


/**
 * Sleep for a given time span, with the option of being woken up from another thread.
 * The sleep can be interrupted from another thread via a shared \ref SleepInterrupt
 * object. See \ref SleepInterrupt for more details.
 * \param duration   Time span to wait, as a std::chrono::duration type.
 * \param interrupt  Reference to a SleepInterrupt object that can be used to interrupt
 *                   the delay. If such an interruption occurs, false is returned.
 * \returns true if the entire requested sleep duration has passed, or false if the sleep
 *          has been interrupted prematurely via the SleepInterrupt object.
 */
template< class Rep, class Period >
bool sleep(const std::chrono::duration<Rep, Period>& duration,
           const Trigger& interrupt)
{
    return interrupt.sleep_until(tic() + duration);
}

/**
 * Sleep for a given number of seconds, with the option of being woken up from another
 * thread. The sleep can be interrupted from another thread via a shared \ref SleepInterrupt
 * object. See \ref SleepInterrupt for more details.
 * \param duration   Seconds to wait.
 * \param interrupt  Reference to a SleepInterrupt object that can be used to interrupt
 *                   the delay. If such an interruption occurs, false is returned.
 * \returns true if the entire requested sleep duration has passed, or false if the sleep
 *          has been interrupted prematurely via the SleepInterrupt object.
 */
inline bool sleep(double seconds, const Trigger &interrupt)
{
    return sleep(std::chrono::duration<double>{ seconds }, interrupt);
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
bool sleep(const std::chrono::duration<Rep, Period>& sleep_duration)
{
    std::this_thread::sleep_for(sleep_duration);
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

} // namespace gul
