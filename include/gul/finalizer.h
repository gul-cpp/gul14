/**
 * \file    finalizer.h
 * \brief   Finalizer implementation for the General Utility Library.
 * \authors \ref contributors
 * \date    Created on 23 October 2018
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

namespace gul {

/**
 * FinalAction allows use to execute something if we leave the scope.
 * (implementation is quite similar to what the Guideline-Support-Library)
 *
 * A good example is time measurement:
 * \code
 *  #include <chrono>
 *  #include <iostream>
 *
 *  using namespace std::chrono;
 *
 *  void foo() {
 *
 *      const auto start = system_clock::now();
 *      const auto _ = finally([start] {
 *          cerr << "function took "
 *               << duration_cast<milliseconds>(system_clock::now() - start).count()
 *               << "ms\n";
 *      });
 *
 *      if (rand() % 2) {
 *          sleep(10);
 *          cout << "Delayed exit\n";
 *          return;
 *      }
 *      cout << "Normal exit\n";
 *  }
 * \endcode
 */
template <typename F>
class FinalAction
{
    F action_;
    bool invoke_ {true};

public:
    /**
     * Creates a new FinalAction object
     *
     * \param f The lambda or function to be called on destruction.
     */
    explicit FinalAction(F f) noexcept
        : action_(std::move(f))
    {}
    /// Move constructor
    FinalAction(FinalAction&& other) noexcept
        : action_(std::move(other.action_))
        , invoke_(other.invoke_)
    {
        // don't call callback on moved from FinalAction
        other.invoke_ = false;
    }
    /// Move assignment operator
    FinalAction& operator=(FinalAction&& other) noexcept {
        if (this != &other) {
            this->action_(std::move(other.action_));
            this->invoke_ = other.invoke_;
            // don't call callback on moved from FinalAction
            other.invoke_ = false;
        }
        return *this;
    }

    FinalAction() = delete;
    FinalAction(const FinalAction&) = delete;
    FinalAction& operator=(const FinalAction&) = delete;

    ~FinalAction() noexcept {
        if (invoke_)
            action_();
    }
};

/**
 * finally() - convenience function to generate a FinalAction
 *
 * \param f The lambda or function to be called on destruction.
 */
template <typename F>
FinalAction<F> finally(const F& f) noexcept
{
    return FinalAction<F>(f);
}

} /* namespace gul */

/* vim:set noexpandtab softtabstop=4 tabstop=4 shiftwidth=4 textwidth=90 cindent: */
