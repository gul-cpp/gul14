/**
 * \file    traits.h
 * \brief   Some metaprogramming traits for the General Utility Library.
 * \authors \ref contributors, Michael Park
 * \date    Created on 20 August 2021
 *
 * Copyright 2015-2017 Michael Park (implementations of invoke, invoke_result,
 * invoke_result_t, is_invocable, is_invocable_r), copyright 2021-2024 Deutsches
 * Elektronen-Synchrotron (DESY), Hamburg (other implementations and modifications)
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See \ref license_boost_1_0 or http://boost.org/LICENSE_1_0.txt)
 */

#ifndef GUL14_TRAITS_H_
#define GUL14_TRAITS_H_

#if __cplusplus >= 201703L
#define GUL14_USE_CPP17_FEATURES
#include <functional> // for std::invoke on C++17 compilers
#endif

#include <type_traits>
#include <utility>

#include "gul14/internal.h"

namespace gul14 {

/**
 * \addtogroup traits_h gul14/traits.h
 * \brief Type traits and helpers for metaprogramming.
 * @{
 */

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

/**
 * A template metafunction that removes const, volatile, and reference qualifiers from a
 * type. The stripped type is available in the member typedef \c type.
 *
 * This is a substitute for C++20's
 * [std::remove_cvref](https://en.cppreference.com/w/cpp/types/remove_cvref).
 *
 * \since GUL version 2.9
 */
template <typename T>
using remove_cvref = typename std::remove_cv<std::remove_reference_t<T>>;

/**
 * A template metafunction that removes const, volatile, and reference qualifiers from a
 * type.
 *
 * This is a substitute for C++20's
 * [std::remove_cvref_t](https://en.cppreference.com/w/cpp/types/remove_cvref_t).
 *
 * \since GUL version 2.9
 */
template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

/**
 * A type mapping an arbitrary list of types to void (for SFINAE).
 *
 * This is a helper metafunction to substitute C++17's
 * [std::void_t](https://en.cppreference.com/w/cpp/types/void_t).
 *
 * \since GUL version 2.9
 */
template <typename...>
using void_t = void;


//
// invoke & friends
//
#ifdef GUL14_USE_CPP17_FEATURES

using std::invoke;
using std::invoke_result;
using std::invoke_result_t;
using std::is_invocable;
using std::is_invocable_r;

#undef GUL14_USE_CPP17_FEATURES

#else

namespace detail_invoke {

#define GUL14_INVOKE_RETURN(...) \
  noexcept(noexcept(__VA_ARGS__)) -> decltype(__VA_ARGS__) { return __VA_ARGS__; }

template <typename T>
struct is_reference_wrapper : std::false_type {};

template <typename T>
struct is_reference_wrapper<std::reference_wrapper<T>>
    : std::true_type {};

template <bool, int>
struct Invoke;

template <>
struct Invoke<true /* pmf */, 0 /* is_base_of */> {
    template <typename R, typename T, typename Arg, typename... Args>
    inline static constexpr auto invoke(R T::*pmf, Arg &&arg, Args &&... args)
    GUL14_INVOKE_RETURN((std::forward<Arg>(arg).*pmf)(std::forward<Args>(args)...))
};

template <>
struct Invoke<true /* pmf */, 1 /* is_reference_wrapper */> {
    template <typename R, typename T, typename Arg, typename... Args>
    inline static constexpr auto invoke(R T::*pmf, Arg &&arg, Args &&... args)
    GUL14_INVOKE_RETURN((std::forward<Arg>(arg).get().*pmf)(std::forward<Args>(args)...))
};

template <>
struct Invoke<true /* pmf */, 2 /* otherwise */> {
    template <typename R, typename T, typename Arg, typename... Args>
    inline static constexpr auto invoke(R T::*pmf, Arg &&arg, Args &&... args)
    GUL14_INVOKE_RETURN(((*std::forward<Arg>(arg)).*pmf)(std::forward<Args>(args)...))
};

template <>
struct Invoke<false /* pmo */, 0 /* is_base_of */> {
    template <typename R, typename T, typename Arg>
    inline static constexpr auto invoke(R T::*pmo, Arg &&arg)
    GUL14_INVOKE_RETURN(std::forward<Arg>(arg).*pmo)
};

template <>
struct Invoke<false /* pmo */, 1 /* is_reference_wrapper */> {
    template <typename R, typename T, typename Arg>
    inline static constexpr auto invoke(R T::*pmo, Arg &&arg)
    GUL14_INVOKE_RETURN(std::forward<Arg>(arg).get().*pmo)
};

template <>
struct Invoke<false /* pmo */, 2 /* otherwise */> {
    template <typename R, typename T, typename Arg>
    inline static constexpr auto invoke(R T::*pmo, Arg &&arg)
        GUL14_INVOKE_RETURN((*std::forward<Arg>(arg)).*pmo)
};

template <typename R, typename T, typename Arg, typename... Args>
inline constexpr auto invoke(R T::*f, Arg&& arg, Args&&... args)
    GUL14_INVOKE_RETURN(
        Invoke<std::is_function<R>::value,
                (std::is_base_of<T, std::decay_t<Arg>>::value
                    ? 0
                    : is_reference_wrapper<std::decay_t<Arg>>::value
                        ? 1
                        : 2)>::invoke(f, std::forward<Arg>(arg),
                                      std::forward<Args>(args)...))

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100)
#endif
template <typename F, typename... Args>
inline constexpr auto invoke(F &&f, Args &&... args)
    GUL14_INVOKE_RETURN(std::forward<F>(f)(std::forward<Args>(args)...))
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#undef GUL14_INVOKE_RETURN

} // namespace detail_invoke

/**
 * Invoke a callable object f with the given arguments.
 *
 * This is a backport of C++17's
 * [std::invoke](https://en.cppreference.com/w/cpp/utility/functional/invoke).
 *
 * \since GUL version 2.11
 */
template <typename F, typename... Args>
inline constexpr auto
invoke(F&& f, Args&&... args) noexcept(
    noexcept(detail_invoke::invoke(std::forward<F>(f), std::forward<Args>(args)...)))
    -> decltype(detail_invoke::invoke(std::forward<F>(f), std::forward<Args>(args)...))
{
    return detail_invoke::invoke(std::forward<F>(f), std::forward<Args>(args)...);
}


namespace detail_invoke_result {

template <typename T>
struct identity { using type = T; };

template <typename Void, typename, typename...>
struct invoke_result {};

template <typename F, typename... Args>
struct invoke_result<void_t<decltype(invoke(
                            std::declval<F>(), std::declval<Args>()...))>,
                        F,
                        Args...>
    : identity<decltype(
            invoke(std::declval<F>(), std::declval<Args>()...))> {};

} // namespace detail_invoke_result

/**
 * A metafunction that computes the result of invoking a callable object of type F with
 * the given arguments. The result is available in the member typedef \c type.
 *
 * This is a backport of C++17's
 * [std::invoke_result](https://en.cppreference.com/w/cpp/types/result_of).
 *
 * \since GUL version 2.11
 */
template <typename F, typename... Args>
using invoke_result = detail_invoke_result::invoke_result<void, F, Args...>;

/**
 * A shortcut for invoke_result<...>::type.
 *
 * This is a backport of C++17's
 * [std::invoke_result_t](https://en.cppreference.com/w/cpp/types/result_of).
 *
 * \since GUL version 2.11
 */
template <typename F, typename... Args>
using invoke_result_t = typename invoke_result<F, Args...>::type;


namespace detail_invocable {

template <typename Void, typename, typename...>
struct is_invocable : std::false_type {};

template <typename F, typename... Args>
struct is_invocable<void_t<invoke_result_t<F, Args...>>, F, Args...>
    : std::true_type {};

template <typename Void, typename, typename, typename...>
struct is_invocable_r : std::false_type {};

template <typename R, typename F, typename... Args>
struct is_invocable_r<void_t<invoke_result_t<F, Args...>>,
                        R,
                        F,
                        Args...>
    : std::is_convertible<invoke_result_t<F, Args...>, R> {};

} // namespace detail_invocable

/**
 * A type trait that checks whether a callable object of type F can be invoked with the
 * given arguments. The boolean result is available in the member \c value.
 *
 * This is a backport of C++17's
 * [std::is_invocable](https://en.cppreference.com/w/cpp/types/is_invocable).
 *
 * \since GUL version 2.11
 */
template <typename F, typename... Args>
using is_invocable = detail_invocable::is_invocable<void, F, Args...>;

/**
 * A type trait that checks whether a callable object of type F can be invoked with the
 * given arguments and returns a result convertible to R. The boolean result is available
 * in the member \c value.
 *
 * This is a backport of C++17's
 * [std::is_invocable_r](https://en.cppreference.com/w/cpp/types/is_invocable).
 *
 * \since GUL version 2.11
 */
template <typename R, typename F, typename... Args>
using is_invocable_r = detail_invocable::is_invocable_r<void, R, F, Args...>;

#endif // __cplusplus < 201703L

/// @}

} // namespace gul14

#endif

// vi:ts=4:sw=4:sts=4:et
