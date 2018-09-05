/**
 * \file    tokenize.h
 * \authors \ref contributors
 * \date    Created on September 3, 2018
 * \brief   Declaration of tokenize(), tokenize_string_view().
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

#include <string>
#include <vector>
#include "gul/string_view.h"

namespace gul {

/**
 * Split the given string into a vector of substrings (tokens) delimited by any of the
 * characters in the delimiters string. Multiple adjacent delimiters are treated like a
 * single one, and delimiters at the beginning and end of the string are ignored.
 * For example, tokenize("   A  B C  ") yields a vector with the three entries "A", "B",
 * and "C".
 * \see gul::tokenize_string_view() returns a vector<string_view> instead.
 * \param str         The string to be split.
 * \param delimiters  String with delimiter characters. Any of the characters in
 *                    this string marks the beginning/end of a token. By
 *                    default, a wide variety of whitespace and control
 *                    characters is used.
 * \returns a std::vector with the single substrings.
 */
std::vector<std::string> tokenize(string_view str,
    string_view delimiters = " \t\r\n\a\b\f\v");

/**
 * Split the given string into a vector of substrings (tokens) delimited by any of the
 * characters in the delimiters string. Multiple adjacent delimiters are treated like a
 * single one, and delimiters at the beginning and end of the string are ignored.
 * For example, tokenize("   A  B C  ") yields a vector with the three entries "A", "B",
 * and "C".
 * \see gul::tokenize() returns a vector<string> instead.
 * \param str         The string to be split.
 * \param delimiters  String with delimiter characters. Any of the characters in
 *                    this string marks the beginning/end of a token. By
 *                    default, a wide variety of whitespace and control
 *                    characters is used.
 * \returns a std::vector with the single substrings as std::string_view objects. These
 *          string views point to the original string str and are valid only for the
 *          lifetime of that argument.
 */
std::vector<string_view> tokenize_string_view(string_view str,
    string_view delimiters = " \t\r\n\a\b\f\v");

} // namespace gul
