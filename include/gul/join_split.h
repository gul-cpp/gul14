/**
 * \file   split_join.h
 * \brief  Declarations of string utility functions for the General Utility Library.
 * \author Soeren Grunewald, Lars Froehlich, et al
 * \date   Created on 31 August 2018
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
 * Separate a string at all occurences of the delimiter. The strings between the delimiters
 * are collected and returned in a vector.
 *
 * The result has at least one element: If the delimiter is not present in the text the
 * whole text is returned as one.
 * If there are consecutive delimiters the collected string between these is the empty
 * string.
 * If the delimiter is directly at the end of the input the collected string between the
 * end of the input string and the string and is again the empty string.
 *
 * Reverse funtion of gul::join()
 * "'Explode' the text at the delimiter into a vector"
 *
 * It is guaranteed that join(split(text, del), del) == text
 * (if del is not a regex).
 *
 * Note: The regex version does not take string_views, because regexes and string_view are
 * incompatible.
 *
 * \param text      The string that is to be deconstructed
 * \param delimiter The delimiting substring
 *
 * \return Array of substrings that where delimited by delimiter
 */
std::vector<std::string> split(string_view text, string_view delimiter);
std::vector<std::string> split(const std::string& text, const std::regex delimiter);

/**
 * Concatenate the strings from the vector parts into one single string. Between each two
 * elements of the vector the delimiter is placed.
 *
 * Reverse funtion of gul::split()
 * "'Implode' a string array into a single string"
 *
 * It is guaranteed that join(split(text, del), del) == text
 * (if del is not a regex).
 *
 * \param parts Array of Strings that are to be concatenated
 * \param glue  String that is put between each element of parts
 *
 * \return All strings glued together with parameter glue
 * */
std::string join(std::vector<std::string> parts, string_view glue);

} /* namespace gul */

/* vim:set noexpandtab softtabstop=4 tabstop=4 shiftwidth=4 textwidth=90 cindent: */
