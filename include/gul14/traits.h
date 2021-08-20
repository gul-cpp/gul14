/**
 * \file    traits.h
 * \brief   Some metaprogramming traits for the General Utility Library.
 * \authors \ref contributors
 * \date    Created on 20 August 2021
 *
 * \copyright Copyright 2021 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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

#ifndef GUL14_TRAITS_H_
#define GUL14_TRAITS_H_

#include <type_traits>
#include "gul14/internal.h"

namespace gul14 {
/**
 * Helper type trait object to determine if a type is a container.
 *
 * A container is identified by the presense of the ``cbegin()`` and ``cend()``
 * member functions and a ``value_type`` type alias.
 *
 * If the type specified as template parameter has the required members
 * ``std::true_type`` is returned, otherwise ``std::false_type``.
 *
 * \tparam T    Type to check
 * \returns     ``std::true_type`` if the type is probably a container
 *
 * \since GUL version 2.6
 */
template <typename T, typename = int>
struct IsContainerLike : std::false_type { };

template <typename T>
struct IsContainerLike <T,
    typename std::enable_if_t<true,
        decltype(std::declval<T>().cbegin(),
            std::declval<T>().cend(),
            std::declval<typename T::value_type>(),
            0)
    >>
    : std::true_type { };

} // namespace gul14

#endif

// vi:ts=4:sw=4:sts=4:et
