/**
 * \file    join_split.h
 * \brief   Declarations of string utility functions for the General Utility Library.
 * \authors \ref contributors
 * \date    Created on 31 August 2018
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

#include <string>
#include <regex>
#include <vector>
#include "gul14/internal.h"
#include "gul14/string_view.h"

namespace gul14 {

/**
 * Separate a string at all occurrences of a delimiter, returning the strings between the
 * delimiters in a vector.
 *
 * The result has at least one element. If the delimiter is not present in the text, the
 * whole text is returned. If there are consecutive delimiters, the collected string
 * between them is the empty string. If the delimiter is directly at the end of the
 * input, the collected string between the end of the input and the delimiter is again the
 * empty string.
 *
 * split() is the inverse function of gul14::join(). It is guaranteed that
 * `join(split(text, del), del) == text`.
 *
 * \param text       The string that is to be deconstructed
 * \param delimiter  The delimiting substring
 *
 * \returns an array of substrings that were separated by delimiter in the original
 *          string.
 *
 * \see split_sv() returns a vector of string_views,<br>
 *      split(const std::string &, const std::regex &) splits at a delimiter described by
 *      a regular expression, and<br>
 *      join() can join the vector back into a string.
 */
GUL_EXPORT
std::vector<std::string> split(string_view text, string_view delimiter);

/**
 * Separate a string at all occurrences of a delimiter described by a regular expression,
 * returning the strings between the delimiters in a vector.
 *
 * The result has at least one element. If the delimiter is not present in the text the
 * whole text is returned as one. If there are consecutive delimiters, the collected
 * string between them is the empty string. If the delimiter is directly at the end of the
 * input, the collected string between the end of the input and the delimiter is again the
 * empty string.
 *
 * This version of split() does not accept string_view parameters because standard regexes
 * are not compatible with string_view.
 *
 * \param text       The string that is to be deconstructed
 * \param delimiter  A std::regex object describing the delimiters
 *
 * \returns an array of substrings that were separated by delimiters in the original
 *          string.
 *
 * \see split(gul14::string_view, gul14::string_view) splits at a fixed substring,<br>
 *      split_sv() does the same returning a vector of string_views, and<br>
 *      join() can join the vector back into a string.
 */
GUL_EXPORT
std::vector<std::string> split(const std::string& text, const std::regex& delimiter);

/**
 * Separate a string at all occurrences of a delimiter, returning the text between the
 * delimiters as a vector of string_views.
 *
 * The result has at least one element. If the delimiter is not present in the text, the
 * whole text is returned. If there are consecutive delimiters, the collected string
 * between them is the empty string. If the delimiter is directly at the end of the
 * input, the collected string between the end of the input and the delimiter is again the
 * empty string.
 *
 * The inverse function of split_sv() is gul14::join(). It is guaranteed that
 * `join(split_sv(text, del), del) == text`.
 *
 * \param text       The string that is to be deconstructed
 * \param delimiter  The delimiting substring
 *
 * \returns an array of substrings that were separated by delimiter in the original
 *          string. The substrings are string_view objects that point into the original
 *          string passed to the function.
 *
 * \see split() returns a vector of copied substrings,<br>
 *      split(const std::string &, const std::regex &) splits at a delimiter described by
 *      a regular expression, and<br>
 *      join() can join the vector back into a string.
 */
GUL_EXPORT
std::vector<gul14::string_view> split_sv(string_view text, string_view delimiter);

/**
 * Concatenate a vector of strings into one single string, placing a delimiter between
 * them.
 *
 * This is the inverse function of gul14::split(). It is guaranteed that
 * `join(split(text, del), del) == text` (unless del is a std::regex object).
 *
 * \param parts  Array of strings that are to be concatenated
 * \param glue   String that is put between each element of parts
 *
 * \returns all strings glued together with the delimiter glue.
 *
 * \see join(const std::vector<gul14::string_view> &, string_view) accepts a vector of
 *      string_view objects, split() and associated functions can be used to split a
 *      string into a vector of substrings.
 */
GUL_EXPORT
std::string join(const std::vector<std::string>& parts, string_view glue);

/**
 * Concatenate a vector of string_views into one single string, placing a delimiter
 * between them.
 *
 * This is the inverse function of gul14::split(). It is guaranteed that
 * `join(split(text, del), del) == text` (unless del is a std::regex object).
 *
 * \param parts  Array of strings that are to be concatenated
 * \param glue   String that is put between each element of parts
 *
 * \returns all strings glued together with the delimiter glue.
 *
 * \see join(const std::vector<std::string> &, string_view) accepts a vector of strings,
 *      split() and associated functions can be used to split a string into a vector of
 *      substrings.
 */
GUL_EXPORT
std::string join(const std::vector<string_view>& parts, string_view glue);

} // namespace gul14

/* vim:set noexpandtab softtabstop=4 tabstop=4 shiftwidth=4 textwidth=90 cindent: */
