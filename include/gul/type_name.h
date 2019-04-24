/**
 * \file   type_name.h
 * \author \ref contributors
 * \date   Created on April 11, 2019
 * \brief  Definition of type_name()
 *
 * \copyright Copyright 2019 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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
 *
 * This code has been inspired by posts of Howard Hinnant at Stackoverflow.
 * His code has been inspired by posts of Jamboree at Stackoverflow.
 */

#pragma once

#include <cstddef>
#include <stdexcept>
#include <cstring>
#include <ostream>

namespace gul {

/**
 * A helper class to debug types.
 *
 * This is - on purpose - an incomplete templated class.
 *
 * Trying to instantiate an object of this class will result in a compiler error
 * with a detailed description of template argument T.
 *
 * \tparam T  Type to investigate
 *
 * A using example:
 * \code
 * auto t = some_type_or_another{ };
 * gul::FailToInstantiate<decltype(t)>{};
 * \endcode
 * A more realistic example for people shying away from ``auto``:
 * \code
 * template <typename T>
 * void my_function(T param)
 * {
 *     // wondering if T is what I expect...
 *     gul::FailToInstantiate<T>{};
 * }
 * \endcode
 */
template <typename T>
class FailToInstantiate;

/**
 * Generate a human readable string describing a type.
 *
 * Cuts out the most relevant part of `__PRETTY_FUNCTION__` of itself, ignoring all
 * the stuff that contains information on the function itself, leaving just the
 * description of the template parameter.
 *
 * \tparam T Type that shall be described
 *
 * \returns a string view that describes the type of the template parameter
 *
 */
template <class T>
constexpr string_view type_name()
{
#ifdef __clang__
    auto p = string_view{ __PRETTY_FUNCTION__ };
    return string_view{ p.data() + 35, p.size() - 35 - 1 };

#elif defined(__GNUC__)
    auto p = string_view{ __PRETTY_FUNCTION__ };
    return string_view{ p.data() + 50, p.size() - 50 - 1 };

#elif defined(_MSC_VER)
    auto p = string_view{ __FUNCSIG__ };
    return string_view{ p.data() + 42, p.size() - 42 - 7 };
#endif
}

} // namespace gul

// vi:ts=4:sw=4:et:sts=4
