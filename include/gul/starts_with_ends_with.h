/**
 * \file    starts_with_ends_with.h
 * \brief   Declaration of starts_with() and ends_with().
 * \authors \ref contributors
 * \date    Created on 26 November 2018
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

#include "gul/string_view.h"

namespace gul {

/**
 * Determine whether a string ends with another string.
 * The comparison is case sensitive.
 *
 * \param haystack  The full string to be tested.
 * \param stack     The suffix to be looked for at the end of \c haystack.
 * \returns true if \c haystack ends with \c stack, false otherwise.
 */
constexpr inline bool ends_with(string_view haystack, string_view stack) noexcept
{
    const auto hsl = haystack.length();
    const auto sl = stack.length();

    return hsl >= sl && haystack.compare(hsl - sl, sl, stack) == 0;
}



/**
 * Determine whether a string starts with another string.
 * The comparison is case sensitive.
 *
 * \param haystack  The full string to be tested.
 * \param hay       The prefix to be looked for at the beginning of \c haystack.
 * \returns true if \c haystack starts with \c hay, false otherwise.
 */
constexpr inline bool starts_with(string_view haystack, string_view hay) noexcept
{
    const auto hsl = haystack.length();
    const auto hl = hay.length();

    return hsl >= hl && haystack.compare(0, hl, hay) == 0;
}

} /* namespace gul */

/* vim:set noexpandtab softtabstop=4 tabstop=4 shiftwidth=4 textwidth=90 cindent: */
