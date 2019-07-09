/**
 * \file    case_ascii.h
 * \brief   Declarations of lowercase_ascii(), lowercase_ascii_inplace(),
 *          uppercase_ascii(), and uppercase_ascii_inplace().
 * \authors \ref contributors
 * \date    Created on 28 May 2019
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
 */

#pragma once

#include <string>
#include "gul/internal.h"
#include "gul/string_view.h"

namespace gul {

/**
 * Return the ASCII lowercase equivalent of the given character (or the unchanged
 * character, if it is not an ASCII letter).
 * \param c  The original character.
 * \since GUL version 1.2
 * \see lowercase_ascii(gul::string_view), lowercase_ascii_inplace(std::string &),
 *      uppercase_ascii(char)
 */
constexpr char lowercase_ascii(char c) noexcept
{
    if (c >= 'A' && c <= 'Z')
        return c - 'A' + 'a';
    return c;
}

/**
 * Return a copy of the given string in which all ASCII characters are replaced by their
 * lowercase equivalents.
 * \param str  The original string.
 * \since GUL version 1.2
 * \see lowercase_ascii(char), lowercase_ascii_inplace(std::string &),
 *      uppercase_ascii(gul::string_view)
 */
GUL_EXPORT
std::string lowercase_ascii(gul::string_view str);

/**
 * Replace all ASCII characters in a string by their lowercase equivalents.
 * This function modifies the original string and returns a reference to it (which may
 * be helpful for chaining function calls).
 * \param str  The string to be modified.
 * \returns a reference to the string argument.
 * \since GUL version 1.2
 * \see lowercase_ascii(char), lowercase_ascii(gul::string_view),
 *      uppercase_ascii_inplace(std::string &)
 */
GUL_EXPORT
std::string &lowercase_ascii_inplace(std::string &str) noexcept;

/**
 * Return the ASCII uppercase equivalent of the given character (or the unchanged
 * character, if it is not an ASCII letter).
 * \param c  The original character.
 * \since GUL version 1.2
 * \see uppercase_ascii(gul::string_view), uppercase_ascii_inplace(std::string &),
 *      lowercase_ascii(char)
 */
constexpr char uppercase_ascii(char c) noexcept
{
    if (c >= 'a' && c <= 'z')
        return c - 'a' + 'A';
    return c;
}

/**
 * Return a copy of the given string in which all ASCII characters are replaced by their
 * uppercase equivalents.
 * \param str  The original string.
 * \since GUL version 1.2
 * \see uppercase_ascii(char), uppercase_ascii_inplace(std::string &),
 *      lowercase_ascii(gul::string_view)
 */
GUL_EXPORT
std::string uppercase_ascii(gul::string_view str);

/**
 * Replace all ASCII characters in a string by their uppercase equivalents.
 * This function modifies the original string and returns a reference to it (which may
 * be helpful for chaining function calls).
 * \param str  The string to be modified.
 * \returns a reference to the string argument.
 * \since GUL version 1.2
 * \see uppercase_ascii(char), uppercase_ascii(gul::string_view),
 *      lowercase_ascii_inplace(std::string &)
 */
GUL_EXPORT
std::string &uppercase_ascii_inplace(std::string &str) noexcept;

} // namespace gul

/* vim:set expandtab softtabstop=4 tabstop=4 shiftwidth=4 textwidth=90 cindent: */
