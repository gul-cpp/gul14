/**
 * \file    string_util.h
 * \brief   Declaration of string utility functions.
 * \authors \ref contributors
 * \date    Created on 31 August 2018
 *
 * \copyright Copyright 2018-2023 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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

#include <array>
#include <iterator>
#include <string>

#include "gul14/internal.h"
#include "gul14/string_view.h"
#include "gul14/traits.h"

namespace gul14 {

/**
 * \addtogroup string_util_h gul14/string_util.h
 * \brief Various string utility functions.
 * @{
 */

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

/// The 16 digits for hexadecimal numbers ("0123456789abcdef").
GUL_EXPORT
extern const std::array<char, 16> hex_digits;

/**
 * Return the hexadecimal ASCII representation of an integer value.
 * The letters 'a' to 'f' are used in lowercase, and the number of hex digits is twice the
 * number of bytes in the input integer type.
 *
 * \code
 * hex_string(static_cast<unsigned char>(1)) == "01"
 * hex_string(static_cast<unsigned char>(255)) == "ff";
 * hex_string(uint32_t{ 256 }) == "00000100";
 * \endcode
 *
 * \param v  Input value
 *
 * \tparam Integer must be an integral type.
 *
 * \since GUL version 2.6
 */
template <typename Integer,
          std::enable_if_t<std::is_integral<Integer>::value, bool> = true>
inline std::string hex_string(Integer v)
{
    auto u = static_cast<typename std::make_unsigned<Integer>::type>(v);
    std::string result;

    for (int idx = static_cast<int>(sizeof(Integer)) - 1; idx >= 0; --idx)
    {
        auto byte = u >> (8 * idx);
        unsigned int upper_nibble = (byte >> 4) & 0xf;
        unsigned int lower_nibble = byte & 0xf;
        result += hex_digits[upper_nibble];
        result += hex_digits[lower_nibble];
    }

    return result;
}

/**
 * Return the hexadecimal ASCII representation of a range of integer values.
 * The letters 'a' to 'f' are used in lowercase, and a user-defined separator can be
 * inserted between individual values.
 *
 * \code
 * std::array<unsigned char, 4> values = { 0, 15, 16, 255 };
 * assert(hex_string(values.begin(), values.end()) == "000f10ff");
 * assert(hex_string(values.begin(), values.end(), "-") == "00-0f-10-ff");
 * \endcode
 *
 * \param begin      Iterator to the first element of the range
 * \param end        Iterator past the last element of the range
 * \param separator  A string that is inserted between the elements to separate them
 *                   visually (empty by default)
 *
 * \tparam Iterator must be a forward iterator.
 *
 * \since GUL version 2.6
 */
template <typename Iterator>
inline std::string
hex_string(Iterator begin, Iterator end, string_view separator = "")
{
    const std::size_t n = static_cast<std::size_t>(std::distance(begin, end));

    std::string result;

    if (n > 0)
    {
        result.reserve(2 * sizeof(*begin) * n + separator.size() * (n - 1));

        result += hex_string(*begin);

        for (auto it = std::next(begin); it != end; ++it)
        {
            result += separator;
            result += hex_string(*it);
        }
    }

    return result;
}

/**
 * Return the hexadecimal ASCII representation of an array with integer values.
 * The letters 'a' to 'f' are used in lowercase, and a user-defined separator can be
 * inserted between individual values.
 * \code
 * uint16_t values[] = { 256, 255 };
 * assert(hex_string(values) == "010000ff");
 * assert(hex_string(values, "-") == "0100-00ff");
 * \endcode
 *
 * \param array      Input array
 * \param separator  A string that is inserted between the elements to separate them
 *                   visually (empty by default)
 *
 * \tparam Integer       must be an integral type.
 * \tparam num_elements  is the number of array elements.
 *
 * \since GUL version 2.6
 */
template <typename Integer, size_t num_elements,
          std::enable_if_t<std::is_integral<Integer>::value, bool> = true>
inline std::string
hex_string(const Integer (&array)[num_elements], string_view separator = "")
{
    return hex_string(std::begin(array), std::end(array), separator);
}

/**
 * Return the hexadecimal ASCII representation of a container with integer values.
 * The letters 'a' to 'f' are used in lowercase, and no separators are inserted between
 * individual values.
 * \code
 * std::array<unsigned char, 4> values = { 0, 15, 16, 255 };
 * assert(hex_string(values) == "000f10ff");
 * \endcode
 *
 * \param container  Input container
 * \param separator  A string that is inserted between the elements to separate them
 *                   visually (empty by default)
 *
 * \tparam Container  must be a gul14::IsContainerLike class
 *
 * \since GUL version 2.6
 */
template <typename Container,
          std::enable_if_t<IsContainerLike<Container>::value, bool> = true>
inline std::string
hex_string(const Container& container, string_view separator = "")
{
    return hex_string(std::cbegin(container), std::cend(container), separator);
}

/**
 * Repeat a string N times.
 * \code
 * std::string str = repeat("du", 3); // str == "dududu"
 * \endcode
 *
 * \param str  String to be repeated
 * \param n    Number of repetitions
 *
 * \returns the N-fold concatenation of the input string.
 *
 * \since GUL version 2.7
 */
GUL_EXPORT
std::string repeat(gul14::string_view str, std::size_t n);

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

/// @}

} // namespace gul14

#endif

// vi:ts=4:sw=4:sts=4:et
