/**
 * \file    tokenize.h
 * \authors \ref contributors
 * \date    Created on September 3, 2018
 * \brief   Declaration of tokenize(), tokenize_sv().
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

#ifndef GUL14_TOKENIZE_H_
#define GUL14_TOKENIZE_H_

#include <string>
#include <vector>
#include "gul14/internal.h"
#include "gul14/string_util.h"
#include "gul14/string_view.h"

namespace gul14 {

/**
 * Split the given string into a vector of substrings (tokens) delimited by any of the
 * characters in the delimiters string. Multiple adjacent delimiters are treated like a
 * single one, and delimiters at the beginning and end of the string are ignored.
 * For example, tokenize("   A  B C  ") yields a vector with the three entries "A", "B",
 * and "C".
 * \see gul14::tokenize_sv() returns a vector<string_view> instead.
 * \param str         The string to be split.
 * \param delimiters  String with delimiter characters. Any of the characters in
 *                    this string marks the beginning/end of a token. By
 *                    default, a wide variety of whitespace and control
 *                    characters is used.
 * \returns a std::vector with the single substrings.
 */
GUL_EXPORT
std::vector<std::string> tokenize(string_view str,
    string_view delimiters = default_whitespace_characters);

/**
 * Split the given string into a vector of substrings (tokens) delimited by any of the
 * characters in the delimiters string. Multiple adjacent delimiters are treated like a
 * single one, and delimiters at the beginning and end of the string are ignored.
 * For example, tokenize("   A  B C  ") yields a vector with the three entries "A", "B",
 * and "C".
 * \see gul14::tokenize() returns a vector<string> instead.
 * \param str         The string to be split.
 * \param delimiters  String with delimiter characters. Any of the characters in
 *                    this string marks the beginning/end of a token. By
 *                    default, a wide variety of whitespace and control
 *                    characters is used.
 * \returns a std::vector with the single substrings as std::string_view objects. These
 *          string views point to the original string str and are valid only for the
 *          lifetime of that argument.
 */
GUL_EXPORT
std::vector<string_view> tokenize_sv(string_view str,
    string_view delimiters = default_whitespace_characters);

} // namespace gul14

#endif
