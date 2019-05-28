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
#include "gul/string_view.h"

// Anonymous namespace with constexpr helper objects
namespace {

class LookupTable
{
public:
    constexpr char operator[](unsigned char c) const noexcept
    {
        return lookup_[c];
    }

protected:
    char lookup_[256] = {};
};

class LowercaseLookupTable : public LookupTable
{
public:
    constexpr LowercaseLookupTable() noexcept
    {
        for (auto c = 0u; c < 256; c++)
        {
            if (c >= static_cast<unsigned char>('A') && c <= static_cast<unsigned char>('Z'))
                lookup_[c] = 'a' + c - 'A';
            else
                lookup_[c] = c;
        }
    }
};

class UppercaseLookupTable : public LookupTable
{
public:
    constexpr UppercaseLookupTable() noexcept
    {
        for (auto c = 0u; c < 256; c++)
        {
            if (c >= static_cast<unsigned char>('a') && c <= static_cast<unsigned char>('z'))
                lookup_[c] = 'A' + c - 'a';
            else
                lookup_[c] = c;
        }
    }
};

constexpr LowercaseLookupTable lowercase_lookup;
constexpr UppercaseLookupTable uppercase_lookup;

} // anonymous namespace



namespace gul {

/**
 * Return the ASCII lowercase equivalent of the given character (or the unchanged
 * character, if it is not an ASCII letter).
 * \param c  The original character.
 */
constexpr char lowercase_ascii(char c) noexcept
{
    return lowercase_lookup[c];
}


/**
 * Return a copy of the given string in which all ASCII characters are replaced by their
 * lowercase equivalents.
 * \param str  The original string.
 */
std::string lowercase_ascii(gul::string_view str);

/**
 * Replace all ASCII characters in a string by their lowercase equivalents.
 * This function modifies the original string and returns a reference to it (which may
 * be helpful for chaining function calls).
 * \param str  The string to be modified.
 * \returns a reference to the string argument.
 */
std::string &lowercase_ascii_inplace(std::string &str) noexcept;

/**
 * Return the ASCII uppercase equivalent of the given character (or the unchanged
 * character, if it is not an ASCII letter).
 * \param c  The original character.
 */
constexpr char uppercase_ascii(char c) noexcept
{
    return uppercase_lookup[c];
}

/**
 * Return a copy of the given string in which all ASCII characters are replaced by their
 * uppercase equivalents.
 * \param str  The original string.
 */
std::string uppercase_ascii(gul::string_view str);

/**
 * Replace all ASCII characters in a string by their uppercase equivalents.
 * This function modifies the original string and returns a reference to it (which may
 * be helpful for chaining function calls).
 * \param str  The string to be modified.
 * \returns a reference to the string argument.
 */
std::string &uppercase_ascii_inplace(std::string &str) noexcept;

} // namespace gul

/* vim:set noexpandtab softtabstop=4 tabstop=4 shiftwidth=4 textwidth=90 cindent: */
