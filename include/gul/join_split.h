/**
 * \file    join_split.h
 * \brief   Declarations of string utility functions for the General Utility Library.
 * \authors \ref contributors
 * \date    Created on 31 August 2018
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
#include <regex>
#include <vector>
#include "gul/string_view.h"

namespace gul {

/**
 * Separate a string at all occurrences of a delimiter, returning the strings between the
 * delimiters in a vector.
 *
 * The result has at least one element. If the delimiter is not present in the text the
 * whole text is returned as one. If there are consecutive delimiters, the collected
 * string between them is the empty string. If the delimiter is directly at the end of the
 * input, the collected string between the end of the input and the delimiter is again the
 * empty string.
 *
 * split() is the reverse funtion of gul::join(). It is guaranteed that
 * `join(split(text, del), del) == text` (unless del is a std::regex object).
 *
 * Note: The regex version does not take string_views, because regexes and string_view are
 * incompatible.
 *
 * \param text      The string that is to be deconstructed
 * \param delimiter The delimiting substring
 *
 * \returns an array of substrings that were separated by delimiter in the original
 *          string.
 */
std::vector<std::string> split(string_view text, string_view delimiter);
std::vector<std::string> split(const std::string& text, const std::regex& delimiter);

/**
 * Concatenate a vector of strings into one single string, placing a delimiter between
 * them.
 *
 * This is the reverse funtion of gul::split(). It is guaranteed that
 * `join(split(text, del), del) == text` (unless del is a std::regex object).
 *
 * \param parts Array of strings that are to be concatenated
 * \param glue  String that is put between each element of parts
 *
 * \returns all strings glued together with the delimiter glue.
 */
std::string join(const std::vector<std::string>& parts, string_view glue);

} /* namespace gul */

/* vim:set noexpandtab softtabstop=4 tabstop=4 shiftwidth=4 textwidth=90 cindent: */
