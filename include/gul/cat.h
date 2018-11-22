/**
 * \file   cat.h
 * \author \ref contributors
 * \date   Created on August 31, 2018
 * \brief  Declaration of the overload set for cat() and of the associated class
 *         ConvertingStringView.
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
#include "gul/string_view.h"

namespace gul {

/**
 * A string view that can automatically convert numbers into strings.
 * This class is primarily intended for use as a parameter in gul::cat().
 * For string-like arguments, it acts similar to gul::string_view. Number-like
 * arguments (double, int, ...) are converted into strings in a default format using
 * std::to_string. Arguments of type char are converted into a string of length 1.
 */
class ConvertingStringView
{
public:
    ConvertingStringView(const std::string &str) : sv_(str) {} ///< Construct a ConvertingStringView from a std::string.
    ConvertingStringView(string_view sv) : sv_(sv) {} ///< Construct a ConvertingStringView from a string_view.
    ConvertingStringView(const char *str) : sv_(str) {} ///< Construct a ConvertingStringView from a const char *.

    ConvertingStringView(char c) : str_(1, c), sv_(str_) {} ///< Construct a ConvertingStringView from a character.
    ConvertingStringView(int a) : str_(std::to_string(a)), sv_(str_) {} ///< Construct a ConvertingStringView from an integer.
    ConvertingStringView(unsigned int a) : str_(std::to_string(a)), sv_(str_) {} ///< Construct a ConvertingStringView from an unsigned integer.
    ConvertingStringView(long a) : str_(std::to_string(a)), sv_(str_) {} ///< Construct a ConvertingStringView from a long integer.
    ConvertingStringView(unsigned long a) : str_(std::to_string(a)), sv_(str_) {} ///< Construct a ConvertingStringView from an unsigned long integer.
    ConvertingStringView(long long a) : str_(std::to_string(a)), sv_(str_) {} ///< Construct a ConvertingStringView from a long long integer.
    ConvertingStringView(unsigned long long a) : str_(std::to_string(a)), sv_(str_) {} ///< Construct a ConvertingStringView from an unsigned long long integer.
    ConvertingStringView(float a) : str_(std::to_string(a)), sv_(str_) {} ///< Construct a ConvertingStringView from a float.
    ConvertingStringView(double a) : str_(std::to_string(a)), sv_(str_) {} ///< Construct a ConvertingStringView from a double.

    /**
     * Return a pointer to the internal character buffer. The contained string is usually
     * not null-terminated.
     */
    const char *data() const noexcept { return sv_.data(); }

    /// Return the length of the string.
    std::size_t length() const noexcept { return sv_.length(); }

    /// Return the length of the string.
    std::size_t size() const noexcept { return sv_.size(); }

    /// Construct a std::string from this ConvertingStringView.
    explicit operator std::string() const { return std::string(sv_); };

private:
    std::string str_;
    string_view sv_;
};

/**
 * Efficiently concatenate an arbitrary number of strings and numbers.
 * Numbers are converted to strings in the same way as done by std::to_string.
 * cat() reserves the correct amount of space before concatenating the strings and can
 * therefore save several memory allocations compared to concatenation with "+".
 * \code
 * std::string str = gul::cat(13.5, std::string(" quick "), "brown foxes ", "jump over ",
 *                            42, " lazy dogs'", std::string(" heads."));
 * // str == "13.500000 quick brown foxes jump over 42 lazy dogs' heads."
 * \endcode
 * \returns a string containing the concatenation of all arguments.
 */
inline std::string cat() { return std::string(); }

/// \see cat()
inline std::string cat(ConvertingStringView s) { return std::string(s); }

/// \see cat()
std::string cat(ConvertingStringView s1, ConvertingStringView s2);

/// \see cat()
std::string cat(ConvertingStringView s1, ConvertingStringView s2,
                ConvertingStringView s3);

/// \see cat()
std::string cat(std::initializer_list<ConvertingStringView> pieces);

/// \see cat()
template <typename... Args>
inline std::string cat(ConvertingStringView s1, ConvertingStringView s2,
                       ConvertingStringView s3, ConvertingStringView s4,
                       const Args&... args)
{
    return cat({s1, s2, s3, s4, args...});
}

} // namespace gul
