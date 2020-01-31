/**
 * \file    finalizer.h
 * \brief   Implementation of FinalAction and finally().
 * \authors \ref contributors
 * \date    Created on 23 October 2018
 *
 * \copyright Copyright 2018-2020 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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

#ifndef GUL14_FINALIZER_H_
#define GUL14_FINALIZER_H_

#include <type_traits>
#include <utility>
#include "gul14/internal.h"

namespace gul14 {

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
 * A good example is function call duration measurement. The FinalAction just needs to
 * be created in the beginning of a function and no hassle with diverse possible return
 * points.
 * \code
 * #include <cstdlib>
 * #include <ctime>
 * #include <gul14/gul.h>
 *
 * using gul14::finally;
 * using gul14::sleep;
 * using gul14::tic;
 * using gul14::toc;
 *
 * void foo() {
 *     auto _ = finally([start = tic()] {
 *         std::cerr << "Function foo() took " << toc(start) << " s.\n";
 *     });
 *
 *     std::srand(std::time(nullptr));
 *     if (std::rand() % 2) {
 *         std::cout << "Premature exit\n";
 *         return;
 *     }
 *     sleep(10); // do important stuff (like sleeping)
 *     std::cout << "Normal exit\n";
 * }
 * \endcode
 *
 * A (bad; use containers instead) example is allocation with RAII:
 * \code
 * #include <new>
 * #include <string>
 * #include <gul14/gul.h>
 *
 * using gul14::finally;
 *
 * std::string bar(float some_float) {
 *     char* buffer = new char[100];
 *     if (buffer == nullptr)
 *         return;
 *     auto _ = finally([&] { delete[] buffer; buffer = nullptr; });
 *
 *     // do stuff that might throw here
 *
 *     snprintf(buffer, 100, "%.1f", some_float);
 *     return { buffer };
 *     // get rid of buffer automagically
 * }
 * \endcode
 *
 * \tparam F The type of the closure/function to be called.
 *
 * \since GUL version 1.1
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
        : action_{ std::move(other.action_) }
        , invoke_{ other.invoke_ }
    {
        // don't call callback on moved from FinalAction
        other.invoke_ = false;
    }
    /// Move assignment operator
    FinalAction& operator=(FinalAction&& other) noexcept {
        if (this != &other) {
            this->action_ = std::move(other.action_);
            this->invoke_ = std::move(other.invoke_);
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
 *
 * \since GUL version 1.1
 */
template <typename F>
FinalAction<typename std::decay_t<F>> finally(F&& f) noexcept {
    return FinalAction<typename std::decay_t<F>>(std::forward<typename std::decay_t<F>>(f));
}

} // namespace gul14

#endif

// vi:et:sts=4:sw=4:ts=4
