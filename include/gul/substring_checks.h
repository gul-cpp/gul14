/**
 * \file    substring_checks.h
 * \brief   Definition of contains(), ends_with(), and starts_with().
 * \authors \ref contributors
 * \date    Created on 26 November 2018
 *
 * \copyright Copyright 2018-2019 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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

#include "gul/internal.h"
#include "gul/string_view.h"

namespace gul {

/**
 * Determine whether a string contains another string.
 * The comparison is case sensitive. If the searched-for string is empty, the result is
 * true.
 *
 * \param haystack  The string in which to search.
 * \param needle    The string that should be searched for.
 * \returns true if \c haystack contains at least one occurrence of \c needle, false
 *          otherwise.
 *
 * \see contains(char), ends_with(), starts_with()
 */
constexpr inline bool contains(string_view haystack, string_view needle) noexcept
{
    return haystack.find(needle) != string_view::npos;
}

/**
 * Determine whether a string contains a certain character.
 * The comparison is case sensitive.
 *
 * \param haystack  The string in which to search.
 * \param needle    The character that should be searched for.
 * \returns true if \c haystack contains at least one occurrence of \c needle, false
 *          otherwise.
 *
 * \see contains(string_view), ends_with(), starts_with()
 */
constexpr inline bool contains(string_view haystack, char needle) noexcept
{
    return haystack.find(needle) != string_view::npos;
}

/**
 * Determine whether a string ends with another string.
 * The comparison is case sensitive. If the searched-for suffix is empty, the result is
 * true.
 *
 * \param haystack  The full string to be tested.
 * \param stack     The suffix to be looked for at the end of \c haystack.
 * \returns true if \c haystack ends with \c stack, false otherwise.
 *
 * \see ends_with(string_view, char), starts_with(string_view, string_view),
 *      starts_with(string_view, char)
 */
constexpr inline bool ends_with(string_view haystack, string_view stack) noexcept
{
    const auto hsl = haystack.length();
    const auto sl = stack.length();

    return hsl >= sl && haystack.compare(hsl - sl, sl, stack) == 0;
}

/**
 * Determine whether a string ends with a certain character.
 * The comparison is case sensitive.
 *
 * \param str  The string to be tested.
 * \param c    The character to be looked for at the end of \c str.
 * \returns true if \c str ends with \c c, false otherwise.
 *
 * \see ends_with(string_view, string), starts_with(string_view, string_view),
 *      starts_with(string_view, char)
 */
constexpr inline bool ends_with(string_view str, char c) noexcept
{
    return !str.empty() && str.back() == c;
}

/**
 * Determine whether a string starts with another string.
 * The comparison is case sensitive. If the searched-for prefix is empty, the result is
 * true.
 *
 * \param haystack  The full string to be tested.
 * \param hay       The prefix to be looked for at the beginning of \c haystack.
 * \returns true if \c haystack starts with \c hay, false otherwise.
 *
 * \see starts_with(string_view, char), ends_with(string_view, string_view),
 *      ends_with(string_view, char)
 */
constexpr inline bool starts_with(string_view haystack, string_view hay) noexcept
{
    const auto hsl = haystack.length();
    const auto hl = hay.length();

    return hsl >= hl && haystack.compare(0, hl, hay) == 0;
}

/**
 * Determine whether a string starts with a certain character.
 * The comparison is case sensitive.
 *
 * \param str  The string to be tested.
 * \param c    The character to be looked for at the beginning of \c str.
 * \returns true if \c str starts with \c c, false otherwise.
 *
 * \see starts_with(string_view, string_view), ends_with(string_view, string_view),
 *      ends_with(string_view, char)
 */
constexpr inline bool starts_with(string_view str, char c) noexcept
{
    return !str.empty() && str.front() == c;
}

} /* namespace gul */

/* vim:set noexpandtab softtabstop=4 tabstop=4 shiftwidth=4 textwidth=90 cindent: */
