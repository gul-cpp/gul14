/**
 * \file    string_util.h
 * \brief   Declarations of string utility functions for the General Utility Library.
 * \authors \ref contributors
 * \date    Created on 31 August 2018
 *
 * \copyright Copyright 2018-2021 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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

#ifndef GUL14_STRING_UTIL_H_
#define GUL14_STRING_UTIL_H_

#include <string>
#include "gul14/internal.h"
#include "gul14/string_view.h"

namespace gul14 {

/// \cond HIDE_SYMBOLS
namespace detail {

// Check if an object has a emplace_back() member function
template <typename T, typename = int>
struct HasEmplaceBack : std::false_type { };
template <typename T>
struct HasEmplaceBack<T, typename std::enable_if_t<true,
    decltype(std::declval<T>().emplace_back(), 0)>> : std::true_type { };

// Add an element to (the back of) a container using emplace_back()
// if availabe or emplace() if not.
template <typename Container, typename Element>
auto emplace_back(Container& c, Element e)
    -> std::enable_if_t<HasEmplaceBack<Container>::value>
{
    c.emplace_back(std::move(e));
}

template <typename Container, typename Element>
auto emplace_back(Container& c, Element e)
    -> std::enable_if_t<!HasEmplaceBack<Container>::value>
{
    c.emplace(std::move(e));
}

} // namespace detail
/// \endcond


/**
 * Safely construct a std::string from a char pointer and a length.
 *
 * If the pointer is null, an empty string is constructed. If there are no zero bytes in
 * the input range, a string of length \c length is constructed. Otherwise, the input
 * string is treated as a C string and the first zero byte is treated as the end of the
 * string.
 *
 * \code
 * auto a = safe_string(nullptr, 5);  // a == ""s
 * auto b = safe_string("ABC", 2);    // b == "AB"s
 * auto c = safe_string("ABC", 4);    // c == "ABC"s, trailing zero byte ends the string
 * auto d = safe_string("AB\0CD", 5); // d == "AB"s, intermediate zero byte ends the string
 * \endcode
 *
 * \param char_ptr  Pointer to a C string, an unterminated string of at least the
 *                  specified length, or null.
 * \param length    Maximum length of the generated string.
 *
 * \since GUL version 2.6
 */
GUL_EXPORT
std::string safe_string(const char* char_ptr, std::size_t length);

/**
 * The default characters that are treated as whitespace by GUL.
 * This is a string view that contains the space and the most common control characters,
 * namely (with their ASCII codes):
 * - Bell/alert (7)
 * - Backspace (8)
 * - Horizontal tabulator (9)
 * - Newline/line feed (10)
 * - Vertical Tab (11)
 * - Form feed (12)
 * - Carriage return (13)
 * - Space (32)
 *
 * \note
 * The null character is not treated as whitespace by default.
 */
GUL_EXPORT
extern const string_view default_whitespace_characters;

} // namespace gul14

#endif

/* vim:set noexpandtab softtabstop=4 tabstop=4 shiftwidth=4 textwidth=90 cindent: */
