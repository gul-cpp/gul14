/**
 * \file   type_name.h
 * \author \ref contributors
 * \date   Created on April 11, 2019
 * \brief  Definition of type_name()
 *
 * \copyright Copyright 2019-2023 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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

#ifndef GUL14_TYPE_NAME_H_
#define GUL14_TYPE_NAME_H_

#include "gul14/internal.h"
#include "gul14/string_view.h"

namespace gul14 {

/**
 * \addtogroup type_name_h gul14/type_name.h
 * \brief Generating human-readable descriptions for types.
 * @{
 */

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
 * gul14::FailToInstantiate<decltype(t)>{};
 * \endcode
 * A more realistic example for people shying away from ``auto``:
 * \code
 * template <typename T>
 * void my_function(T param)
 * {
 *     // wondering if T is what I expect...
 *     gul14::FailToInstantiate<T>{};
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
 * Note: `__FUNCSIG__` with MSVC.
 *
 * This function can only be constexpr if `__PRETTY_FUNCTION__` is constexpr.
 * This function is not constexpr with MSVC because of language limits.
 *
 * Use like this:
 * \code
 * auto& literal = "test";
 * std::cout << "The type is " << gul14::type_name<decltype(literal)>() << '\n';
 * \endcode
 *
 * \tparam T Type that shall be described
 *
 * \returns a string view that describes the type of the template parameter
 *
 */
template <class T>
constexpr string_view type_name()
{
#if defined(__GNUC__)
    // Clang returns something like "return_type function_name() [T = template_parameter; ...]"
    // GCC returns something like "return_type function_name() [with T = template_parameter]"
    auto s = string_view{ static_cast<const char*>(__PRETTY_FUNCTION__) };
    auto const start_idx = s.find(" = ") + 3; // len(" = ") == 3
    s.remove_prefix(start_idx);

    auto const colon_idx = s.find(";");
    auto const suff_length =
        (colon_idx != string_view::npos) ? s.length() - colon_idx : 1; // len("]") == 1
    s.remove_suffix(suff_length);
    return s;
#elif defined(_MSC_VER)
    // MSVC returns something like "return_type function_name<template_parameter>()"
    auto s = string_view{ __FUNCSIG__ };
    auto const start_idx = s.find("gul14::type_name<") + sizeof("gul14::type_name<") - 1;
    s.remove_prefix(start_idx);

    for (auto end_idx = s.length() - 1; end_idx; --end_idx) {
        if (s[end_idx] != '>')
            continue;
        s.remove_suffix(s.length() - end_idx);
        break;
    }
    return s;
#else
    return "";
#endif
}

/// @}

} // namespace gul14

#endif

// vi:ts=4:sw=4:et:sts=4
