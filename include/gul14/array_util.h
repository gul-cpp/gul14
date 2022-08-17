/* SPDX-License-Identifier: GPL-2.0 */
/*
 * \Copyright 2022 Joshua Ashton
 * \Copyright 2022 DESY
 */

#ifndef GUL14_ARRAY_UTIL
#define GUL14_ARRAY_UTIL

#include <array>
#include <utility>

namespace gul14 {

/**
 * This simplifies the usage of std::array, as one does not have
 * to specifiy the array size:
 * <code>
 * -  auto const foo = std::array<std::string, 3>{{"foo", "bar", "baz"}};
 * +  auto const foo = gul14::array_of<std::string>("foo", "bar", "baz");
 * </code>
 * \Note: This is taken from DXVK project
 */
template <typename V, typename... T>
constexpr std::array<V, sizeof...(T)> array_of(T&&... t) {
	return {{ std::forward<T>(t)... }};
}

}

#endif
