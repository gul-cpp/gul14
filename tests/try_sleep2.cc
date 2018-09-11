// This file is just for testing/tryout
// and is intended to be pasted into wandbox et al

#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

//////
// Helper from gul:
inline std::chrono::steady_clock::time_point tic()
{
    return std::chrono::steady_clock::now();
}

template<class TimeUnitType = std::chrono::duration<double>>
auto toc(std::chrono::steady_clock::time_point t0)
{
    return std::chrono::duration_cast<TimeUnitType>(tic() - t0).count();
}

//////
// NewSleep from gul
struct SleepData {
    std::mutex m;
    std::condition_variable cv;
    std::atomic<bool> canceled{ false };

    void abort() noexcept {
        canceled = true;
        cv.notify_all();
    }
};

template< class Rep, class Period >
auto sleep2(const std::chrono::duration<Rep, Period>& sleep_duration, SleepData& sd) {
    auto const end = std::chrono::steady_clock::now() + sleep_duration;
    std::unique_lock<std::mutex> lk(sd.m);
    if (sd.canceled)
        return false;
    sd.cv.wait_until(lk, end,
        [&sd, &end]{ return std::chrono::steady_clock::now() >= end or sd.canceled; });
    return std::chrono::steady_clock::now() >= end;
}

template< class Rep, class Period >
auto sleep2(const std::chrono::duration<Rep, Period>& sleep_duration) {
    std::this_thread::sleep_for(sleep_duration);
    return true;
}

//////
// Implementation of a test/example

void waiter(SleepData& sd) {
    std::cerr << "waiter() entered\n";
    using namespace std::chrono_literals;
    auto full_sleep = sleep2(3s, sd);
    std::cerr << "waiter() resumes, slept the full time: " << full_sleep << "\n";
}

int main()
{
    auto t0 = tic();

    SleepData sld;
    std::thread w(waiter, std::ref(sld));

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);
    std::cerr << "I'm the main thread, I waited: " << toc(t0) << " s\n";

    // Signal the waiter to abort its wait
    // Comment out next line to see how the full sleep time is waited
    sld.abort();

    // we can join, when the waiter finished, either by
    // timeout or when the abort waiting 'signal' was given
    w.join();
    std::cerr << "Waiter returned after a total of " << toc(t0) << " s\n";
}
