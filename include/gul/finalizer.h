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

#include <type_traits>
#include <utility>

namespace gul {

/**
 * FinalAction allows us to execute something if the FinalAction object leaves the scope.
 *
 * A FinalAction can be used to add RAII like behavior for non RAII object or to do
 * timing measurements.
 *
 * To generate this object you can use the function \ref finally, that leverages
 * template argument deduction of the action's type. This simplifies instantiation a lot.
 *
 * (Implementation is quite similar to what's in the Guideline-Support-Library.)
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
 *
 * A (bad; use containers instead) example is allocation with RAII:
 * \code
 *  #include <string>
 *  #include <new>
 *
 *  std::string bar(float some_float) {
 *      char* buffer = new char[100];
 *      if (buffer == nullptr)
 *          return;
 *      auto _ = gul::finally([&] { delete[] buffer; buffer = nullptr; });
 *
 *      // do stuff that might throw here
 *
 *      snprintf(buffer, 100, "%.1f", some_float);
 *      return { buffer };
 *      // get rid of buffer automagically
 *  }
 * \endcode
 *
 * \tparam F The type of the closure/function to be called.
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
     * It takes any callable as action to be called when the FinalAction destructs
     * (lifetime ends / leaves the scope).
     *
     * The template parameter \b F has to be specified; this can be avoided by using
     * the convenience function \ref finally.
     *
     * \tparam F The type of the closure/function to be called.
     * \param f The closure or function to be called on destruction.
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

    FinalAction() = delete; ///< FinalAction is not is_default_constructible
    FinalAction(const FinalAction&) = delete; ///< FinalAction is not copyable
    FinalAction& operator=(const FinalAction&) = delete; ///< FinalAction is not copyable

    /// Destructor
    /// Calls \b action except when in move contexts.
    ~FinalAction() noexcept {
        if (invoke_)
            action_();
    }
};

/**
 * finally() - convenience function to generate a FinalAction
 *
 * A FinalAction can be used to add RAII like behavior for non RAII object or to do
 * timing measurements. More information given in the FinalAction documentation.
 *
 * \tparam F   The type of the closure/function to be called (normally autodeduced).
 * \param f    The closure or pointer to function to be called on destruction.
 */
template <typename F>
std::enable_if_t<not std::is_lvalue_reference<F>::value, FinalAction<F>> finally(F&& f) noexcept {
    // Restrict universal reference to rvalues references.
    return FinalAction<F>(std::forward<F>(f));
}

/**
 * \overload
 *
 * Variant for some rare \b f that can not be "moved", i.e.
 * - \b f is a function
 * - \b f is a function-reference lvalue
 * - \b f is a closure lvalue
 */
template <typename F>
FinalAction<typename std::decay_t<F>> finally(const F& f) noexcept {
    return FinalAction<typename std::decay_t<F>>(f);
}

} /* namespace gul */

// vi:et:sts=4:sw=4:ts=4
