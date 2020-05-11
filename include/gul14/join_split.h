/**
 * \file    join_split.h
 * \brief   Declarations of string utility functions for the General Utility Library.
 * \authors \ref contributors
 * \date    Created on 31 August 2018
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

#ifndef GUL14_JOIN_SPLIT_H_
#define GUL14_JOIN_SPLIT_H_

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
 * \code
 * std::vector<std::string> parts1 = split(" hello world", " ");
 * assert(parts1.size() == 3);
 * assert(parts1[0] == ""s);
 * assert(parts1[1] == "hello"s);
 * assert(parts1[2] == "world"s);
 *
 * std::vector<std::string> parts2 = split("<>", "<>");
 * assert(parts2.size() == 2);
 * assert(parts2[0] == ""s);
 * assert(parts2[1] == ""s);
 * \endcode
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
 * \code
 * std::vector<std::string> parts = split("one\ntwo\nthree"s, std::regex{"[^[:print:]]"});
 * assert(y.size() == 3);
 * assert(y[0] == "one"s);
 * assert(y[1] == "two"s);
 * assert(y[2] == "three"s);
 * \endcode
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
 * \see split(string_view, string_view) splits at a fixed substring,<br>
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
 * \code
 * std::vector<string_view> parts1 = split(" hello world", " ");
 * assert(parts1.size() == 3);
 * assert(parts1[0] == "");
 * assert(parts1[1] == "hello");
 * assert(parts1[2] == "world");
 *
 * std::vector<string_view> parts2 = split("<>", "<>");
 * assert(parts2.size() == 2);
 * assert(parts2[0] == "");
 * assert(parts2[1] == "");
 * \endcode
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
 * Concatenate all strings in a range, placing a delimiter between them.
 *
 * This is the inverse function of gul14::split(). It is guaranteed that
 * `join(split(text, del), del) == text` (unless del is a std::regex object).
 *
 * \param parts  A container holding strings or string views that are to be concatenated
 * \param glue   String that is put between each element of parts
 *
 * \returns all strings glued together with the delimiter glue.
 *
 * \tparam StringContainer  A container type that holds strings, e.g.
 *                          std::vector<std::string> or std::list<gul14::string_view>.
 *                          The container must provide an STL-like forward iterator
 *                          interface. The string type must support concatenation with
 *                          std::string using operator+=.
 *
 * \see
 * join(Iterator, Iterator, string_view) has a two-iterator interface,
 * split() and associated functions can be used to split a string into a vector of
 * substrings.
 *
 * \since GUL version 2.3, join() accepts arbitrary containers or iterators (it was
 *        limited to std::vector before).
 */
template <typename StringContainer>
std::string join(const StringContainer &parts, string_view glue)
{
    return join(parts.begin(), parts.end(), glue);
}

/**
 * Concatenate all strings in a range, placing a delimiter between them.
 *
 * \param begin  Iterator pointing to the first string
 * \param end    Iterator pointing past the last string
 * \param glue   String that is put between each element of parts
 *
 * \returns all strings glued together with the delimiter glue.
 *
 * \tparam Iterator  A forward iterator type that dereferences to a string type. The
 *                   string type must support concatenation with std::string using
 *                   operator+=.
 *
 * \see
 * join(StringContainer, string_view) is a convenience overload for joining entire
 * containers, split() and associated functions can be used to split a string into a
 * vector of substrings.
 *
 * \since GUL version 2.3
 */
template <typename Iterator>
std::string join(Iterator begin, Iterator end, string_view glue)
{
    std::string result;

    if (begin == end)
        return result; // Return an empty string

    std::size_t num_strings = 0;
    std::size_t len = 0;

    for (auto it = begin; it != end; ++it)
    {
        ++num_strings;
        len += it->size();
    }
    len += (num_strings - 1) * glue.size();

    result.reserve(len);

    auto it = begin;

    // Iterate over all but the last string
    for (std::size_t i = 1; i != num_strings; ++i)
    {
        result += *it;
        result.append(glue.data(), glue.size());

        ++it;
    }
    result += *it;

    return result;
}


} // namespace gul14

#endif

/* vim:set noexpandtab softtabstop=4 tabstop=4 shiftwidth=4 textwidth=90 cindent: */
