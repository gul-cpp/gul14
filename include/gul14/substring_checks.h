/**
 * \file    substring_checks.h
 * \brief   Definition of contains(), ends_with(), and starts_with().
 * \authors \ref contributors
 * \date    Created on 26 November 2018
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

#ifndef GUL14_SUBSTRING_CHECKS_H_
#define GUL14_SUBSTRING_CHECKS_H_

#include "gul14/case_ascii.h"
#include "gul14/internal.h"
#include "gul14/string_view.h"

namespace gul14 {

/**
 * Determine whether a string contains another string.
 * The comparison is case sensitive. If the searched-for string is empty, the result is
 * true.
 *
 * \param haystack  The string in which to search.
 * \param needle    The string that should be searched for.
 * \returns true if \c haystack contains at least one occurrence of \c needle, false
 *          otherwise.
 *
 * \see contains(string_view, char), contains_nocase(string_view, string_view),
 *      ends_with(string_view, string_view), starts_with(string_view, string_view)
 */
constexpr inline bool contains(string_view haystack, string_view needle) noexcept
{
    return haystack.find(needle) != string_view::npos;
}

/**
 * Determine whether a string contains a certain character.
 * The comparison is case sensitive.
 *
 * \param haystack  The string in which to search.
 * \param needle    The character that should be searched for.
 * \returns true if \c haystack contains at least one occurrence of \c needle, false
 *          otherwise.
 *
 * \see contains(string_view, string_view), contains_nocase(string_view, char),
 *      ends_with(string_view, char), starts_with(string_view, char)
 */
constexpr inline bool contains(string_view haystack, char needle) noexcept
{
    return haystack.find(needle) != string_view::npos;
}

/**
 * Determine whether a string ends with another string.
 * The comparison is case sensitive. If the searched-for suffix is empty, the result is
 * true.
 *
 * \param str     The full string to be tested.
 * \param suffix  The suffix to be looked for at the end of \c str.
 * \returns true if \c str ends with \c suffix, false otherwise.
 *
 * \see ends_with(string_view, char), ends_with_nocase(string_view, string_view),
 *      starts_with(string_view, string_view), contains(string_view, string_view)
 */
constexpr inline bool ends_with(string_view str, string_view suffix) noexcept
{
    const auto hsl = str.length();
    const auto sl = suffix.length();

    return hsl >= sl && str.compare(hsl - sl, sl, suffix) == 0;
}

/**
 * Determine whether a string ends with a certain character.
 * The comparison is case sensitive.
 *
 * \param str  The string to be tested.
 * \param c    The character to be looked for at the end of \c str.
 * \returns true if \c str ends with \c c, false otherwise.
 *
 * \see ends_with(string_view, string_view), ends_with_nocase(string_view, char),
 *      starts_with(string_view, char), contains(string_view, char)
 */
constexpr inline bool ends_with(string_view str, char c) noexcept
{
    return !str.empty() && str.back() == c;
}

/**
 * Determine whether a string starts with another string.
 * The comparison is case sensitive. If the searched-for prefix is empty, the result is
 * true.
 *
 * \param str     The full string to be tested.
 * \param prefix  The prefix to be looked for at the beginning of \c str.
 * \returns true if \c str starts with \c prefix, false otherwise.
 *
 * \see starts_with(string_view, char), starts_with_nocase(string_view, string_view),
 *      ends_with(string_view, string_view), contains(string_view, string_view)
 */
constexpr inline bool starts_with(string_view str, string_view prefix) noexcept
{
    const auto hsl = str.length();
    const auto hl = prefix.length();

    return hsl >= hl && string_view{ str.data(), hl }.compare(prefix) == 0;
}

/**
 * Determine whether a string starts with a certain character.
 * The comparison is case sensitive.
 *
 * \param str  The string to be tested.
 * \param c    The character to be looked for at the beginning of \c str.
 * \returns true if \c str starts with \c c, false otherwise.
 *
 * \see starts_with(string_view, string_view), starts_with_nocase(string_view, char),
 *      ends_with(string_view, char), contains(string_view, char)
 */
constexpr inline bool starts_with(string_view str, char c) noexcept
{
    return !str.empty() && str.front() == c;
}

//
// Case insensitive variants following
//

/**
 * Determine whether a string is equal to another one, making no distinction between upper
 * and lower case ASCII characters.
 * In other terms, this function performs a case insensitive string comparison using the C
 * locale.
 *
 * \param str1, str2  The two strings that should be compared.
 * \returns true if the ASCII-lowercase versions of \c str1 and \c str2 are equal, or
 *          false otherwise.
 *
 * \since GUL version 2.1
 *
 * \see contains_nocase(string_view, string_view),
 *      ends_with_nocase(string_view, string_view),
 *      starts_with_nocase(string_view, string_view)
 */
constexpr inline bool equals_nocase(string_view str1, string_view str2) noexcept
{
    if (str1.size() != str2.size())
        return false;

    // Hand-rolled version of std::equal() to maintain constexprness prior to C++20
    auto it1 = str1.cbegin();
    auto it2 = str2.cbegin();

    while (it1 != str1.cend())
    {
        if (lowercase_ascii(*it1) != lowercase_ascii(*it2))
            return false;

        ++it1;
        ++it2;
    }

    return true;
}

/**
 * Determine whether a string contains another string.
 * The comparison is case insensitive as far as ASCII characters are concerned (C locale).
 * If the searched-for string is empty, the result is true.
 *
 * \param haystack  The string in which to search.
 * \param needle    The string that should be searched for.
 * \returns true if \c haystack contains at least one occurrence of \c needle, false
 *          otherwise.
 *
 * \note
 * This function does not allocate memory to buffer the case-transformed input strings.
 * It might therefore have lower performance than combining contains() and
 * lowercase_ascii().
 *
 * \since GUL version 1.7
 *
 * \see contains_nocase(string_view, char), contains(string_view, string_view),
 *      ends_with_nocase(string_view, string_view),
 *      starts_with_nocase(string_view, string_view)
 */
constexpr inline bool contains_nocase(string_view haystack, string_view needle) noexcept
{
    auto const haysize = haystack.size();
    auto const needlesize = needle.size();
    if (haysize < needlesize)
        return false;
    for (std::size_t i = 0; i <= haysize - needlesize; ++i) {
        auto j = needlesize;
        while (j-- > 0) {
            if (lowercase_ascii(haystack[j]) != lowercase_ascii(needle[j]))
                break;
        }
        if (j == std::size_t(-1)) {
            return true;
        }
        haystack.remove_prefix(1);
    }
    return false;
}

/**
 * Determine whether a string contains a certain character.
 * The comparison is case insensitive as far as ASCII characters are concerned (C locale).
 *
 * \param haystack  The string in which to search.
 * \param needle    The character that should be searched for.
 * \returns true if \c haystack contains at least one occurrence of \c needle, false
 *          otherwise.
 *
 * \since GUL version 1.7
 *
 * \see contains_nocase(string_view, string_view), contains(string_view, char),
 *      ends_with_nocase(string_view, char), starts_with_nocase(string_view, char)
 */
constexpr inline bool contains_nocase(string_view haystack, char needle) noexcept
{
    needle = lowercase_ascii(needle);
    while (not haystack.empty()) {
        if (lowercase_ascii(haystack.front()) == needle)
            return true;
        haystack.remove_prefix(1);
    }
    return false;
}

/**
 * Determine whether a string ends with another string.
 * The comparison is case insensitive as far as ASCII characters are concerned (C locale).
 * If the searched-for suffix is empty, the result is true.
 *
 * \param str     The full string to be tested.
 * \param suffix  The suffix to be looked for at the end of \c str.
 * \returns true if \c str ends with \c suffix, false otherwise.
 *
 * \since GUL version 1.7
 *
 * \see ends_with_nocase(string_view, char), ends_with(string_view, string_view),
 *      starts_with_nocase(string_view, string_view),
 *      contains_nocase(string_view, string_view)
 */
constexpr inline bool ends_with_nocase(string_view str, string_view suffix) noexcept
{
    while (not str.empty()) {
        if (suffix.empty())
            return true;
        if (lowercase_ascii(str.back()) != lowercase_ascii(suffix.back()))
            return false;
        str.remove_suffix(1);
        suffix.remove_suffix(1);
    }
    return suffix.empty();
}

/**
 * Determine whether a string ends with a certain character.
 * The comparison is case insensitive as far as ASCII characters are concerned (C locale).
 *
 * \param str  The string to be tested.
 * \param c    The character to be looked for at the end of \c str.
 * \returns true if \c str ends with \c c, false otherwise.
 *
 * \since GUL version 1.7
 *
 * \see ends_with_nocase(string_view, string_view), ends_with(string_view, char),
 *      starts_with_nocase(string_view, char), contains_nocase(string_view, char)
 */
constexpr inline bool ends_with_nocase(string_view str, char c) noexcept
{
    return !str.empty() && lowercase_ascii(str.back()) == lowercase_ascii(c);
}

/**
 * Determine whether a string starts with another string.
 * The comparison is case insensitive as far as ASCII characters are concerned (C locale).
 * If the searched-for prefix is empty, the result is true.
 *
 * \param str     The full string to be tested.
 * \param prefix  The prefix to be looked for at the beginning of \c str.
 * \returns true if \c str starts with \c prefix, false otherwise.
 *
 * \since GUL version 1.7
 *
 * \see starts_with_nocase(string_view, char), starts_with(string_view, string_view),
 *      ends_with_nocase(string_view, string_view),
 *      contains_nocase(string_view, string_view)
 */
constexpr inline bool starts_with_nocase(string_view str, string_view prefix) noexcept
{
    while (not str.empty()) {
        if (prefix.empty())
            return true;
        if (lowercase_ascii(str.front()) != lowercase_ascii(prefix.front()))
            return false;
        str.remove_prefix(1);
        prefix.remove_prefix(1);
    }
    return prefix.empty();
}

/**
 * Determine whether a string starts with a certain character.
 * The comparison is case insensitive as far as ASCII characters are concerned (C locale).
 *
 * \param str  The string to be tested.
 * \param c    The character to be looked for at the beginning of \c str.
 * \returns true if \c str starts with \c c, false otherwise.
 *
 * \since GUL version 1.7
 *
 * \see starts_with_nocase(string_view, string_view), starts_with(string_view, char),
 *      ends_with_nocase(string_view, char), contains_nocase(string_view, char)
 */
constexpr inline bool starts_with_nocase(string_view str, char c) noexcept
{
    return !str.empty() && lowercase_ascii(str.front()) == lowercase_ascii(c);
}

} // namespace gul14

#endif

/* vim:set expandtab softtabstop=4 tabstop=4 shiftwidth=4 textwidth=90 cindent: */
