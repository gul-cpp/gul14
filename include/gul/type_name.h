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

#include "gul/string_view.h"

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

namespace {

/**
 * Returns the length of the given byte string.
 *
 * The length means the number of characters in a character array whose first
 * element is pointed to by str up to and not including the first null
 * character. The behavior is undefined if there is no null character in the
 * character array pointed to by str.
 *
 * \param str Pointer to the null-terminated byte string to be examined
 * \returns the length of the null-terminated string str
 */
constexpr std::size_t constexpr_strlen(const char* str)
{
    return (*str != '\0') ? 1 + constexpr_strlen(str + 1) : 0;
}

/**
 * Create string_view from byte string.
 *
 * Turn a null terminated byte string into a string_view.
 *
 * The function differs from the normal string_view constructor
 * in the property to be really constexpr, if the input
 * string is also constexpr.
 *
 * \param str Pointer to the null-terminated byte string to be converted
 * \returns a string_view pointing to the byte string
 */
constexpr string_view make_string_view(const char* str)
{
    return { str, constexpr_strlen(str) };
}

/**
 * Determine whether a string starts with another string.
 *
 * The comparison is case sensitive. If the searched-for prefix is empty, the result is
 * true.
 *
 * This variant is constexpr if the inputs are constexpr.
 *
 * \param haystack  The full string to be tested.
 * \param hay       The prefix to be looked for at the beginning of \c haystack.
 * \returns true if \c haystack starts with \c hay, false otherwise.
 *
 * \see starts_with(string_view, char), ends_with(string_view, string_view),
 *      ends_with(string_view, char)
 */
constexpr bool constexpr_starts_with(string_view where, string_view what)
{
    if (what.length() == 0)
        return true;
    if (where.length() < what.length()
            or where.data()[0] != what.data()[0])
        return false;
    // Tail recursion will be unrolled
    return constexpr_starts_with(where.substr(1), what.substr(1));
}

/**
 * Find the first substring equal to the given character sequence.
 *
 * This variant is constexpr if the inputs are constexpr.
 *
 * \param where The string in which to search
 * \param what The string to search for
 * \returns the index of the start of \c what in \c where, or string_view::npos
 */
constexpr std::size_t constexpr_find(string_view where, string_view what)
{
    if (where.length() < what.length())
        return string_view::npos;
    if (constexpr_starts_with(where, what))
        return 0;
    // This recursion can be optimized but it's not guaranteed
    auto const n = constexpr_find(where.substr(1), what);
    if (n == string_view::npos)
        return n;
    return 1 + n;
}

}

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
 * std::cout << "The type is " << gul::type_name<decltype(literal)>() << '\n';
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
    auto const s = make_string_view(__PRETTY_FUNCTION__);
    auto const start_idx = constexpr_find(s, make_string_view(" = ")) + 3; // len(" = ") == 3
    auto const colon_idx = constexpr_find(s.substr(start_idx), make_string_view(";"));
    auto const end_idx =
        (colon_idx != string_view::npos)
        ? start_idx + colon_idx - 1
        : s.length() - 2; // len("]\0") == 2
    return s.substr(start_idx, end_idx - start_idx + 1);
#elif defined(_MSC_VER)
    // MSVC returns something like "return_type function_name<template_parameter>()"
    auto const s = string_view{ __FUNCSIG__ };
    auto const start_idx = s.find("gul::type_name<") + sizeof("gul::type_name<") - 1;
    auto const end_idx = s.find_last_of('>') - 1;
    return s.substr(start_idx, end_idx - start_idx + 1);
#else
    return "";
#endif
}

} // namespace gul

// vi:ts=4:sw=4:et:sts=4
