/**
 * \file   type_name.h
 * \author \ref contributors
 * \date   Created on April 11, 2019
 * \brief  Definition of type_name()
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
 *
 * This code has been inspired by posts of Howard Hinnant at Stackoverflow.
 * His code has been inspired by posts of Jamboree at Stackoverflow.
 */

#pragma once

#include <cstddef>
#include <stdexcept>
#include <cstring>
#include <ostream>

namespace gul {

/**
 * A helper class to debug types.
 *
 * This is - on purpose - an incomplete templated class.
 *
 * Trying to instantiate an object of this class will result in a compiler error
 * with a detailed description of template argument T.
 *
 * \tparam T  Type to investigate
 *
 * A using example:
 * \code
 * auto t = some_type_or_another{ };
 * gul::FailToInstantiate<decltype(t)>{};
 * \endcode
 * A more realistic example for people shying away from ``auto``:
 * \code
 * template <typename T>
 * void my_function(T param)
 * {
 *     // wondering if T is what I expect...
 *     gul::FailToInstantiate<T>{};
 * }
 * \endcode
 */
template <typename T>
class FailToInstantiate;

namespace detail {

/**
 * A constexpr string
 *
 * That means a constexpr string literal that can be cut arbitrarily.
 * The string is not necessarily null terminated, as that would require
 * writing something when the string is cut.
 *
 * \code
 * Member functions:
 *   Element access:
 *     operator[]        Access element, index is bound checked
 *     data              Address of first element, note: string not null terminated
 *   Iterators:
 *     begin             Iterator to the first character
 *     end               Iterator one past the last character
 *   Capacity:
 *     size              Length of the string
 * \endcode
 */
class static_string
{
    char const* const string_; ///< Pointer to begin of string literal or string data
    std::size_t const size_; ///< Length of this string

public:
    using const_iterator = char const*; ///< Type used to iterate over the constexpr string

    /**
     * Constructor from string literal
     *
     * \param lit String literal to reference
     */
    template <std::size_t N>
    constexpr static_string(char const(&lit)[N]) noexcept
        : string_(lit)
        , size_(N-1)
    {}

    /**
     * Constructor from pointer/iterator and size
     *
     * The new length is not checked and has to be shorter than
     * or equal to the length of the source string of course.
     *
     * \param begin Begin of the string to store (iterator or pointer)
     * \param length Length of the new string
     */
    explicit constexpr static_string(char const* begin, std::size_t length) noexcept
        : string_(begin)
        , size_(length)
    {}

    /// Return a pointer to the begin of the string.
    ///
    /// Be careful, the string is not null terminated.
    constexpr char const* data() const noexcept
    {
        return string_;
    }

    /// Return the length of the string.
    constexpr std::size_t size() const noexcept
    {
        return size_;
    }

    /// Return an iterator to the begin of the string.
    constexpr const_iterator begin() const noexcept
    {
        return string_;
    }

    /// Return an iterator one past the last character of the string.
    constexpr const_iterator end() const noexcept
    {
        return string_ + size_;
    }

    /// Access one character within the string.
    ///
    /// The index is bounds checked and access outside will throw an exception.
    constexpr char operator[](std::size_t n) const
    {
        return n < size_ ? string_[n] : throw std::out_of_range("static_string");
    }
};

} // namespace detail

/**
 * Generate a human readable string describing a type.
 *
 * Cuts out the most relevant part of `__PRETTY_FUNCTION__` of itself, ignoring all
 * the stuff that contains information on the function itself, leaving just the
 * description of the template parameter.
 *
 * \tparam T Type that shall be described
 *
 * \returns A constexpr string that describes the type of the template parameter
 *
 */
template <class T>
constexpr detail::static_string type_name()
{
#ifdef __clang__
    auto p = detail::static_string{ __PRETTY_FUNCTION__ };
    return detail::static_string{ p.data() + 40, p.size() - 40 - 1 };

#elif defined(__GNUC__)
    auto p = detail::static_string{ __PRETTY_FUNCTION__ };
    return detail::static_string{ p.data() + 55, p.size() - 55 - 1 };

#elif defined(_MSC_VER)
    auto p = detail::static_string{ __FUNCSIG__ };
    return detail::static_string{ p.data() + 47, p.size() - 47 - 7 };
#endif
}

} // namespace gul

/// Output a constexpr string to a stream
inline std::ostream& operator<<(std::ostream& os, gul::detail::static_string const& s)
{
    return os.write(s.data(), s.size());
}

// vi:ts=4:sw=4:et:sts=4
