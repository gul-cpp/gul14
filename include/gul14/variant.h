/**
 * \file    variant.h
 * \brief   Definition of the variant class template and associated functions.
 * \authors Michael Park, (modifications for GUL14) Lars Froehlich
 *
 * \details
 * This header file provides a gul14::variant with the same API as C++17's
 * [std::variant](https://en.cppreference.com/w/cpp/utility/variant). It is based on
 * version 1.4.0 of the variant library by Michael Park
 * (https://github.com/mpark/variant). Multiple header files from the original source have
 * been merged into this single file, and multiple conditional compilation blocks have
 * been removed because GUL14 does not support pre-C++14 compilers.
 *
 * \copyright
 * Copyright 2015-2017 Michael Park; 2023 Deutsches Elektronen-Synchrotron (DESY), Hamburg
 * Distributed under the Boost Software License, Version 1.0.
 * (See \ref license_boost_1_0 or http://boost.org/LICENSE_1_0.txt)
 */
#ifndef GUL14_VARIANT_H_
#define GUL14_VARIANT_H_

#include <cstddef>
#include <exception>
#include <functional>
#include <initializer_list>
#include <limits>
#include <new>
#include <type_traits>
#include <utility>

#include "gul14/internal.h"
#include "gul14/traits.h"
#include "gul14/utility.h" // in_place*, monostate

namespace gul14 {

/**
 * \addtogroup variant_h gul14/variant.h
 * \brief Backport of std::variant from C++17.
 * @{
 */

/**
 * The exception thrown if the wrong type is accessed on a gul14::variant.
 *
 * \since GUL version 2.9.0
 */
class bad_variant_access : public std::exception
{
public:
    virtual const char* what() const noexcept override
    {
        return "bad_variant_access";
    }
};

/**
 * A "type-safe union".
 *
 * Like a plain union, a variant can hold a value of one of a specified set of types.
 * Unlike a union, it can be queried for the type it is currently holding and ensures that
 * only the stored type is accessed. The implementation in the library is a backport from
 * C++17 and should behave like
 * [std::variant](https://en.cppreference.com/w/cpp/utility/variant).
 */
template <typename... Ts>
class variant;

} // namespace gul14

/// \cond HIDE_SYMBOLS

namespace gul14 {

#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

#ifndef __has_builtin
#define __has_builtin(x) 0
#endif

#if __has_attribute(always_inline) || defined(__GNUC__)
#define GUL14_ALWAYS_INLINE __attribute__((__always_inline__)) inline
#elif defined(_MSC_VER)
#define GUL14_ALWAYS_INLINE __forceinline
#else
#define GUL14_ALWAYS_INLINE inline
#endif

#if __has_builtin(__builtin_unreachable) || defined(__GNUC__)
#define GUL14_BUILTIN_UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER)
#define GUL14_BUILTIN_UNREACHABLE __assume(false)
#else
#define GUL14_BUILTIN_UNREACHABLE
#endif

#define GUL14_RETURN(...) \
  noexcept(noexcept(__VA_ARGS__)) -> decltype(__VA_ARGS__) { return __VA_ARGS__; }


namespace detail_variant {

template <std::size_t I, typename T>
struct indexed_type : std::integral_constant<std::size_t, I> { using type = T; };

template <typename T, std::size_t N>
struct array {
    constexpr const T &operator[](std::size_t index) const { return data[index]; }
    T data[N == 0 ? 1 : N];
};

template <typename T, bool>
struct dependent_type : T {};

#if __has_builtin(__type_pack_element) && !(defined(__ICC))
    template <std::size_t I, typename... Ts>
    using type_pack_element_t = __type_pack_element<I, Ts...>;
#else
    template <std::size_t I, typename... Ts>
    struct type_pack_element_impl {
      private:
      template <typename>
      struct set;

      template <std::size_t... Is>
      struct set<std::index_sequence<Is...>> : indexed_type<Is, Ts>... {};

      template <typename T>
      inline static std::enable_if<true, T> impl(indexed_type<I, T>);

      inline static std::enable_if<false> impl(...);

      public:
      using type = decltype(impl(set<std::index_sequence_for<Ts...>>{}));
    };

    template <std::size_t I, typename... Ts>
    using type_pack_element = typename type_pack_element_impl<I, Ts...>::type;

    template <std::size_t I, typename... Ts>
    using type_pack_element_t = typename type_pack_element<I, Ts...>::type;
#endif

template <typename T>
struct identity { using type = T; };

template <bool... Bs>
using all = std::is_same<std::integer_sequence<bool, true, Bs...>,
                         std::integer_sequence<bool, Bs..., true>>;

template <typename T>
struct is_reference_wrapper : std::false_type {};

template <typename T>
struct is_reference_wrapper<std::reference_wrapper<T>>
    : std::true_type {};


namespace detail_invoke {

template <bool, int>
struct Invoke;

template <>
struct Invoke<true /* pmf */, 0 /* is_base_of */> {
    template <typename R, typename T, typename Arg, typename... Args>
    inline static constexpr auto invoke(R T::*pmf, Arg &&arg, Args &&... args)
    GUL14_RETURN((std::forward<Arg>(arg).*pmf)(std::forward<Args>(args)...))
};

template <>
struct Invoke<true /* pmf */, 1 /* is_reference_wrapper */> {
    template <typename R, typename T, typename Arg, typename... Args>
    inline static constexpr auto invoke(R T::*pmf, Arg &&arg, Args &&... args)
    GUL14_RETURN((std::forward<Arg>(arg).get().*pmf)(std::forward<Args>(args)...))
};

template <>
struct Invoke<true /* pmf */, 2 /* otherwise */> {
    template <typename R, typename T, typename Arg, typename... Args>
    inline static constexpr auto invoke(R T::*pmf, Arg &&arg, Args &&... args)
    GUL14_RETURN(((*std::forward<Arg>(arg)).*pmf)(std::forward<Args>(args)...))
};

template <>
struct Invoke<false /* pmo */, 0 /* is_base_of */> {
    template <typename R, typename T, typename Arg>
    inline static constexpr auto invoke(R T::*pmo, Arg &&arg)
    GUL14_RETURN(std::forward<Arg>(arg).*pmo)
};

template <>
struct Invoke<false /* pmo */, 1 /* is_reference_wrapper */> {
    template <typename R, typename T, typename Arg>
    inline static constexpr auto invoke(R T::*pmo, Arg &&arg)
    GUL14_RETURN(std::forward<Arg>(arg).get().*pmo)
};

template <>
struct Invoke<false /* pmo */, 2 /* otherwise */> {
    template <typename R, typename T, typename Arg>
    inline static constexpr auto invoke(R T::*pmo, Arg &&arg)
        GUL14_RETURN((*std::forward<Arg>(arg)).*pmo)
};

template <typename R, typename T, typename Arg, typename... Args>
inline constexpr auto invoke(R T::*f, Arg &&arg, Args &&... args)
    GUL14_RETURN(
        Invoke<std::is_function<R>::value,
                (std::is_base_of<T, std::decay_t<Arg>>::value
                    ? 0
                    : is_reference_wrapper<std::decay_t<Arg>>::value
                        ? 1
                        : 2)>::invoke(f,
                                        std::forward<Arg>(arg),
                                        std::forward<Args>(args)...))

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100)
#endif
template <typename F, typename... Args>
inline constexpr auto invoke(F &&f, Args &&... args)
    GUL14_RETURN(std::forward<F>(f)(std::forward<Args>(args)...))
#ifdef _MSC_VER
#pragma warning(pop)
#endif

} // namespace detail_invoke

template <typename F, typename... Args>
inline constexpr auto invoke(F &&f, Args &&... args)
GUL14_RETURN(detail_invoke::invoke(std::forward<F>(f), std::forward<Args>(args)...))

namespace detail_invoke_result {

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

template <typename F, typename... Args>
using invoke_result = detail_invoke_result::invoke_result<void, F, Args...>;

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

template <typename F, typename... Args>
using is_invocable = detail_invocable::is_invocable<void, F, Args...>;

template <typename R, typename F, typename... Args>
using is_invocable_r = detail_invocable::is_invocable_r<void, R, F, Args...>;

namespace detail_swappable {

using std::swap;

template <typename T>
struct is_swappable {
private:
template <typename U,
            typename = decltype(swap(std::declval<U &>(),
                                    std::declval<U &>()))>
inline static std::true_type test(int);

template <typename U>
inline static std::false_type test(...);

public:
static constexpr bool value = decltype(test<T>(0))::value;
};

template <bool IsSwappable, typename T>
struct is_nothrow_swappable {
static constexpr bool value =
    noexcept(swap(std::declval<T &>(), std::declval<T &>()));
};

template <typename T>
struct is_nothrow_swappable<false, T> : std::false_type {};

}  // namespace detail_swappable

using detail_swappable::is_swappable;

template <typename T>
using is_nothrow_swappable =
    detail_swappable::is_nothrow_swappable<is_swappable<T>::value, T>;

} // namespace detail_variant

#define AUTO_REFREF_RETURN(...) { return __VA_ARGS__; }
#define DECLTYPE_AUTO_RETURN(...) { return __VA_ARGS__; }

[[noreturn]] inline void throw_bad_variant_access() {
    throw bad_variant_access{};
}

  template <typename T>
  struct variant_size;

  template <typename T>
  constexpr std::size_t variant_size_v = variant_size<T>::value;

  template <typename T>
  struct variant_size<const T> : variant_size<T> {};

  template <typename T>
  struct variant_size<volatile T> : variant_size<T> {};

  template <typename T>
  struct variant_size<const volatile T> : variant_size<T> {};

  template <typename... Ts>
  struct variant_size<variant<Ts...>> : std::integral_constant<std::size_t, sizeof...(Ts)> {};

  template <std::size_t I, typename T>
  struct variant_alternative;

  template <std::size_t I, typename T>
  using variant_alternative_t = typename variant_alternative<I, T>::type;

  template <std::size_t I, typename T>
  struct variant_alternative<I, const T>
      : std::add_const<variant_alternative_t<I, T>> {};

  template <std::size_t I, typename T>
  struct variant_alternative<I, volatile T>
      : std::add_volatile<variant_alternative_t<I, T>> {};

  template <std::size_t I, typename T>
  struct variant_alternative<I, const volatile T>
      : std::add_cv<variant_alternative_t<I, T>> {};

  template <std::size_t I, typename... Ts>
  struct variant_alternative<I, variant<Ts...>> {
    static_assert(I < sizeof...(Ts),
                  "index out of bounds in `std::variant_alternative<>`");
    using type = typename detail_variant::type_pack_element_t<I, Ts...>;
  };

  constexpr std::size_t variant_npos = static_cast<std::size_t>(-1);

  namespace detail_variant {

    constexpr std::size_t not_found = static_cast<std::size_t>(-1);
    constexpr std::size_t ambiguous = static_cast<std::size_t>(-2);

    template <typename T, typename... Ts>
    inline constexpr std::size_t find_index() {
      constexpr detail_variant::array<bool, sizeof...(Ts)> matches = {
          {std::is_same<T, Ts>::value...}
      };
      std::size_t result = not_found;
      for (std::size_t i = 0; i < sizeof...(Ts); ++i) {
        if (matches[i]) {
          if (result != not_found) {
            return ambiguous;
          }
          result = i;
        }
      }
      return result;
    }

    template <std::size_t I>
    using find_index_sfinae_impl =
        std::enable_if_t<I != not_found && I != ambiguous,
                         std::integral_constant<std::size_t, I>>;

    template <typename T, typename... Ts>
    using find_index_sfinae = find_index_sfinae_impl<find_index<T, Ts...>()>;

    template <std::size_t I>
    struct find_index_checked_impl : std::integral_constant<std::size_t, I> {
      static_assert(I != not_found, "the specified type is not found.");
      static_assert(I != ambiguous, "the specified type is ambiguous.");
    };

    template <typename T, typename... Ts>
    using find_index_checked = find_index_checked_impl<find_index<T, Ts...>()>;

    struct valueless_t {};

    enum class Trait { TriviallyAvailable, Available, Unavailable };

    template <typename T,
              template <typename> class IsTriviallyAvailable,
              template <typename> class IsAvailable>
    inline constexpr Trait trait() {
      return IsTriviallyAvailable<T>::value
                 ? Trait::TriviallyAvailable
                 : IsAvailable<T>::value ? Trait::Available
                                         : Trait::Unavailable;
    }

    template <typename... Traits>
    inline constexpr Trait common_trait(Traits... traits_) {
      Trait result = Trait::TriviallyAvailable;
      detail_variant::array<Trait, sizeof...(Traits)> traits = {{traits_...}};
      for (std::size_t i = 0; i < sizeof...(Traits); ++i) {
        Trait t = traits[i];
        if (static_cast<int>(t) > static_cast<int>(result)) {
          result = t;
        }
      }
      return result;
    }

    template <typename... Ts>
    struct traits {
      static constexpr Trait copy_constructible_trait =
          common_trait(trait<Ts,
                             std::is_trivially_copy_constructible,
                             std::is_copy_constructible>()...);

      static constexpr Trait move_constructible_trait =
          common_trait(trait<Ts,
                             std::is_trivially_move_constructible,
                             std::is_move_constructible>()...);

      static constexpr Trait copy_assignable_trait =
          common_trait(copy_constructible_trait,
                       trait<Ts,
                             std::is_trivially_copy_assignable,
                             std::is_copy_assignable>()...);

      static constexpr Trait move_assignable_trait =
          common_trait(move_constructible_trait,
                       trait<Ts,
                             std::is_trivially_move_assignable,
                             std::is_move_assignable>()...);

      static constexpr Trait destructible_trait =
          common_trait(trait<Ts,
                             std::is_trivially_destructible,
                             std::is_destructible>()...);
    };

    namespace access {

      struct recursive_union {
        template <typename V>
        inline static constexpr auto &&get_alt(V &&v, in_place_index_t<0>) {
          return std::forward<V>(v).head_;
        }

        template <typename V, std::size_t I>
        inline static constexpr auto &&get_alt(V &&v, in_place_index_t<I>) {
          return get_alt(std::forward<V>(v).tail_, in_place_index_t<I - 1>{});
        }
      };

      struct base {
        template <std::size_t I, typename V>
        inline static constexpr auto&& get_alt(V &&v)
#ifdef _MSC_VER
          AUTO_REFREF_RETURN(recursive_union::get_alt(
              std::forward<V>(v).data_, in_place_index_t<I>{}))
#else
          AUTO_REFREF_RETURN(recursive_union::get_alt(
              data(std::forward<V>(v)), in_place_index_t<I>{}))
#endif
      };

      struct variant {
        template <std::size_t I, typename V>
        inline static constexpr auto&& get_alt(V &&v)
          AUTO_REFREF_RETURN(base::get_alt<I>(std::forward<V>(v).impl_))
      };

    }  // namespace access

    namespace visitation {

#if !defined(_MSC_VER)
#define GUL14_VARIANT_SWITCH_VISIT
#endif

      struct base {
        template <typename Visitor, typename... Vs>
        using dispatch_result_t = decltype(
            invoke(std::declval<Visitor>(),
                        access::base::get_alt<0>(std::declval<Vs>())...));

        template <typename Expected>
        struct expected {
          template <typename Actual>
          inline static constexpr bool but_got() {
            return std::is_same<Expected, Actual>::value;
          }
        };

        template <typename Expected, typename Actual>
        struct visit_return_type_check {
          static_assert(
              expected<Expected>::template but_got<Actual>(),
              "`visit` requires the visitor to have a single return type");

          template <typename Visitor, typename... Alts>
          inline static constexpr decltype(auto)
          invoke(Visitor &&visitor, Alts &&... alts)
          {
              return ::gul14::detail_variant::invoke(
                  std::forward<Visitor>(visitor), std::forward<Alts>(alts)...);
          }
        };

#ifdef GUL14_VARIANT_SWITCH_VISIT
        template <bool B, typename R, typename... ITs>
        struct dispatcher;

        template <typename R, typename... ITs>
        struct dispatcher<false, R, ITs...> {
          template <std::size_t B, typename F, typename... Vs>
          GUL14_ALWAYS_INLINE static constexpr R dispatch(
              F &&, typename ITs::type &&..., Vs &&...) {
            GUL14_BUILTIN_UNREACHABLE;
          }

          template <std::size_t I, typename F, typename... Vs>
          GUL14_ALWAYS_INLINE static constexpr R dispatch_case(F &&, Vs &&...) {
            GUL14_BUILTIN_UNREACHABLE;
          }

          template <std::size_t B, typename F, typename... Vs>
          GUL14_ALWAYS_INLINE static constexpr R dispatch_at(std::size_t,
                                                             F &&,
                                                             Vs &&...) {
            GUL14_BUILTIN_UNREACHABLE;
          }
        };

        template <typename R, typename... ITs>
        struct dispatcher<true, R, ITs...> {
          template <std::size_t B, typename F>
          GUL14_ALWAYS_INLINE static constexpr R dispatch(
              F &&f, typename ITs::type &&... visited_vs) {
            using Expected = R;
            using Actual = decltype(invoke(
                std::forward<F>(f),
                access::base::get_alt<ITs::value>(
                    std::forward<typename ITs::type>(visited_vs))...));
            return visit_return_type_check<Expected, Actual>::invoke(
                std::forward<F>(f),
                access::base::get_alt<ITs::value>(
                    std::forward<typename ITs::type>(visited_vs))...);
          }

          template <std::size_t B, typename F, typename V, typename... Vs>
          GUL14_ALWAYS_INLINE static constexpr R dispatch(
              F &&f, typename ITs::type &&... visited_vs, V &&v, Vs &&... vs) {
#define GUL14_DISPATCH(I)                                                   \
  dispatcher<(I < std::decay_t<V>::size()),                                 \
             R,                                                             \
             ITs...,                                                        \
             detail_variant::indexed_type<I, V>>::                                  \
      template dispatch<0>(std::forward<F>(f),                              \
                           std::forward<typename ITs::type>(visited_vs)..., \
                           std::forward<V>(v),                              \
                           std::forward<Vs>(vs)...)

#define GUL14_DEFAULT(I)                                                      \
  dispatcher<(I < std::decay_t<V>::size()), R, ITs...>::template dispatch<I>( \
      std::forward<F>(f),                                                     \
      std::forward<typename ITs::type>(visited_vs)...,                        \
      std::forward<V>(v),                                                     \
      std::forward<Vs>(vs)...)

            switch (v.index()) {
              case B + 0: return GUL14_DISPATCH(B + 0);
              case B + 1: return GUL14_DISPATCH(B + 1);
              case B + 2: return GUL14_DISPATCH(B + 2);
              case B + 3: return GUL14_DISPATCH(B + 3);
              case B + 4: return GUL14_DISPATCH(B + 4);
              case B + 5: return GUL14_DISPATCH(B + 5);
              case B + 6: return GUL14_DISPATCH(B + 6);
              case B + 7: return GUL14_DISPATCH(B + 7);
              case B + 8: return GUL14_DISPATCH(B + 8);
              case B + 9: return GUL14_DISPATCH(B + 9);
              case B + 10: return GUL14_DISPATCH(B + 10);
              case B + 11: return GUL14_DISPATCH(B + 11);
              case B + 12: return GUL14_DISPATCH(B + 12);
              case B + 13: return GUL14_DISPATCH(B + 13);
              case B + 14: return GUL14_DISPATCH(B + 14);
              case B + 15: return GUL14_DISPATCH(B + 15);
              case B + 16: return GUL14_DISPATCH(B + 16);
              case B + 17: return GUL14_DISPATCH(B + 17);
              case B + 18: return GUL14_DISPATCH(B + 18);
              case B + 19: return GUL14_DISPATCH(B + 19);
              case B + 20: return GUL14_DISPATCH(B + 20);
              case B + 21: return GUL14_DISPATCH(B + 21);
              case B + 22: return GUL14_DISPATCH(B + 22);
              case B + 23: return GUL14_DISPATCH(B + 23);
              case B + 24: return GUL14_DISPATCH(B + 24);
              case B + 25: return GUL14_DISPATCH(B + 25);
              case B + 26: return GUL14_DISPATCH(B + 26);
              case B + 27: return GUL14_DISPATCH(B + 27);
              case B + 28: return GUL14_DISPATCH(B + 28);
              case B + 29: return GUL14_DISPATCH(B + 29);
              case B + 30: return GUL14_DISPATCH(B + 30);
              case B + 31: return GUL14_DISPATCH(B + 31);
              default: return GUL14_DEFAULT(B + 32);
            }

#undef GUL14_DEFAULT
#undef GUL14_DISPATCH
          }

          template <std::size_t I, typename F, typename... Vs>
          GUL14_ALWAYS_INLINE static constexpr R dispatch_case(F &&f,
                                                               Vs &&... vs) {
            using Expected = R;
            using Actual = decltype(
                invoke(std::forward<F>(f),
                            access::base::get_alt<I>(std::forward<Vs>(vs))...));
            return visit_return_type_check<Expected, Actual>::invoke(
                std::forward<F>(f),
                access::base::get_alt<I>(std::forward<Vs>(vs))...);
          }

          template <std::size_t B, typename F, typename V, typename... Vs>
          GUL14_ALWAYS_INLINE static constexpr R dispatch_at(std::size_t index,
                                                             F &&f,
                                                             V &&v,
                                                             Vs &&... vs) {
            static_assert(detail_variant::all<(std::decay_t<V>::size() ==
                                    std::decay_t<Vs>::size())...>::value,
                          "all of the variants must be the same size.");
#define GUL14_DISPATCH_AT(I)                                               \
  dispatcher<(I < std::decay_t<V>::size()), R>::template dispatch_case<I>( \
      std::forward<F>(f), std::forward<V>(v), std::forward<Vs>(vs)...)

#define GUL14_DEFAULT(I)                                                 \
  dispatcher<(I < std::decay_t<V>::size()), R>::template dispatch_at<I>( \
      index, std::forward<F>(f), std::forward<V>(v), std::forward<Vs>(vs)...)

            switch (index) {
              case B + 0: return GUL14_DISPATCH_AT(B + 0);
              case B + 1: return GUL14_DISPATCH_AT(B + 1);
              case B + 2: return GUL14_DISPATCH_AT(B + 2);
              case B + 3: return GUL14_DISPATCH_AT(B + 3);
              case B + 4: return GUL14_DISPATCH_AT(B + 4);
              case B + 5: return GUL14_DISPATCH_AT(B + 5);
              case B + 6: return GUL14_DISPATCH_AT(B + 6);
              case B + 7: return GUL14_DISPATCH_AT(B + 7);
              case B + 8: return GUL14_DISPATCH_AT(B + 8);
              case B + 9: return GUL14_DISPATCH_AT(B + 9);
              case B + 10: return GUL14_DISPATCH_AT(B + 10);
              case B + 11: return GUL14_DISPATCH_AT(B + 11);
              case B + 12: return GUL14_DISPATCH_AT(B + 12);
              case B + 13: return GUL14_DISPATCH_AT(B + 13);
              case B + 14: return GUL14_DISPATCH_AT(B + 14);
              case B + 15: return GUL14_DISPATCH_AT(B + 15);
              case B + 16: return GUL14_DISPATCH_AT(B + 16);
              case B + 17: return GUL14_DISPATCH_AT(B + 17);
              case B + 18: return GUL14_DISPATCH_AT(B + 18);
              case B + 19: return GUL14_DISPATCH_AT(B + 19);
              case B + 20: return GUL14_DISPATCH_AT(B + 20);
              case B + 21: return GUL14_DISPATCH_AT(B + 21);
              case B + 22: return GUL14_DISPATCH_AT(B + 22);
              case B + 23: return GUL14_DISPATCH_AT(B + 23);
              case B + 24: return GUL14_DISPATCH_AT(B + 24);
              case B + 25: return GUL14_DISPATCH_AT(B + 25);
              case B + 26: return GUL14_DISPATCH_AT(B + 26);
              case B + 27: return GUL14_DISPATCH_AT(B + 27);
              case B + 28: return GUL14_DISPATCH_AT(B + 28);
              case B + 29: return GUL14_DISPATCH_AT(B + 29);
              case B + 30: return GUL14_DISPATCH_AT(B + 30);
              case B + 31: return GUL14_DISPATCH_AT(B + 31);
              default: return GUL14_DEFAULT(B + 32);
            }

#undef GUL14_DEFAULT
#undef GUL14_DISPATCH_AT
          }
        };
#else
        template <typename T>
        inline static constexpr const T &at(const T &elem) noexcept {
          return elem;
        }

        template <typename T, std::size_t N, typename... Is>
        inline static constexpr const lib::remove_all_extents_t<T> &at(
            const detail_variant::array<T, N> &elems, std::size_t i, Is... is) noexcept {
          return at(elems[i], is...);
        }

        template <typename F, typename... Fs>
        inline static constexpr detail_variant::array<std::decay_t<F>, sizeof...(Fs) + 1>
        make_farray(F &&f, Fs &&... fs) {
          return {{std::forward<F>(f), std::forward<Fs>(fs)...}};
        }

        template <typename F, typename... Vs>
        struct make_fmatrix_impl {

          template <std::size_t... Is>
          inline static constexpr dispatch_result_t<F, Vs...> dispatch(
              F &&f, Vs &&... vs) {
            using Expected = dispatch_result_t<F, Vs...>;
            using Actual = decltype(invoke(
                std::forward<F>(f),
                access::base::get_alt<Is>(std::forward<Vs>(vs))...));
            return visit_return_type_check<Expected, Actual>::invoke(
                std::forward<F>(f),
                access::base::get_alt<Is>(std::forward<Vs>(vs))...);
          }

          template <std::size_t... Is>
          inline static constexpr auto impl(std::index_sequence<Is...>) {
            return &dispatch<Is...>;
          }

          template <typename Is, std::size_t... Js, typename... Ls>
          inline static constexpr auto impl(Is,
                                            std::index_sequence<Js...>,
                                            Ls... ls) {
            return make_farray(impl(lib::push_back_t<Is, Js>{}, ls...)...);
          }
        };

        template <typename F, typename... Vs>
        inline static constexpr auto make_fmatrix() {
          return make_fmatrix_impl<F, Vs...>::impl(
              std::index_sequence<>{},
              lib::make_index_sequence<std::decay_t<Vs>::size()>{}...);
        }

        template <typename F, typename... Vs>
        struct make_fdiagonal_impl {
          template <std::size_t I>
          inline static constexpr dispatch_result_t<F, Vs...> dispatch(
              F &&f, Vs &&... vs) {
            using Expected = dispatch_result_t<F, Vs...>;
            using Actual = decltype(
                invoke(std::forward<F>(f),
                            access::base::get_alt<I>(std::forward<Vs>(vs))...));
            return visit_return_type_check<Expected, Actual>::invoke(
                std::forward<F>(f),
                access::base::get_alt<I>(std::forward<Vs>(vs))...);
          }

          template <std::size_t... Is>
          inline static constexpr auto impl(std::index_sequence<Is...>) {
            return make_farray(&dispatch<Is>...);
          }
        };

        template <typename F, typename V, typename... Vs>
        inline static constexpr auto make_fdiagonal()
            -> decltype(make_fdiagonal_impl<F, V, Vs...>::impl(
                lib::make_index_sequence<std::decay_t<V>::size()>{})) {
          static_assert(detail_variant::all<(std::decay_t<V>::size() ==
                                  std::decay_t<Vs>::size())...>::value,
                        "all of the variants must be the same size.");
          return make_fdiagonal_impl<F, V, Vs...>::impl(
              lib::make_index_sequence<std::decay_t<V>::size()>{});
        }
#endif
      };

#if !defined(GUL14_VARIANT_SWITCH_VISIT) && \
    (!defined(_MSC_VER) || _MSC_VER >= 1910)
      template <typename F, typename... Vs>
      using fmatrix_t = decltype(base::make_fmatrix<F, Vs...>());

      template <typename F, typename... Vs>
      struct fmatrix {
        static constexpr fmatrix_t<F, Vs...> value =
            base::make_fmatrix<F, Vs...>();
      };

      template <typename F, typename... Vs>
      constexpr fmatrix_t<F, Vs...> fmatrix<F, Vs...>::value;

      template <typename F, typename... Vs>
      using fdiagonal_t = decltype(base::make_fdiagonal<F, Vs...>());

      template <typename F, typename... Vs>
      struct fdiagonal {
        static constexpr fdiagonal_t<F, Vs...> value =
            base::make_fdiagonal<F, Vs...>();
      };

      template <typename F, typename... Vs>
      constexpr fdiagonal_t<F, Vs...> fdiagonal<F, Vs...>::value;
#endif

      struct alt {
        template <typename Visitor, typename... Vs>
        inline static constexpr decltype(auto) visit_alt(Visitor &&visitor,
                                                        Vs &&... vs)
#ifdef GUL14_VARIANT_SWITCH_VISIT
          DECLTYPE_AUTO_RETURN(
              base::dispatcher<
                  true,
                  base::dispatch_result_t<Visitor,
                                          decltype(as_base(
                                              std::forward<Vs>(vs)))...>>::
                  template dispatch<0>(std::forward<Visitor>(visitor),
                                       as_base(std::forward<Vs>(vs))...))
#elif !defined(_MSC_VER) || _MSC_VER >= 1910
          DECLTYPE_AUTO_RETURN(base::at(
              fmatrix<Visitor &&,
                      decltype(as_base(std::forward<Vs>(vs)))...>::value,
              vs.index()...)(std::forward<Visitor>(visitor),
                             as_base(std::forward<Vs>(vs))...))
#else
          DECLTYPE_AUTO_RETURN(base::at(
              base::make_fmatrix<Visitor &&,
                      decltype(as_base(std::forward<Vs>(vs)))...>(),
              vs.index()...)(std::forward<Visitor>(visitor),
                             as_base(std::forward<Vs>(vs))...))
#endif

        template <typename Visitor, typename... Vs>
        inline static constexpr decltype(auto) visit_alt_at(std::size_t index,
                                                           Visitor &&visitor,
                                                           Vs &&... vs)
#ifdef GUL14_VARIANT_SWITCH_VISIT
          DECLTYPE_AUTO_RETURN(
              base::dispatcher<
                  true,
                  base::dispatch_result_t<Visitor,
                                          decltype(as_base(
                                              std::forward<Vs>(vs)))...>>::
                  template dispatch_at<0>(index,
                                          std::forward<Visitor>(visitor),
                                          as_base(std::forward<Vs>(vs))...))
#elif !defined(_MSC_VER) || _MSC_VER >= 1910
          DECLTYPE_AUTO_RETURN(base::at(
              fdiagonal<Visitor &&,
                        decltype(as_base(std::forward<Vs>(vs)))...>::value,
              index)(std::forward<Visitor>(visitor),
                     as_base(std::forward<Vs>(vs))...))
#else
          DECLTYPE_AUTO_RETURN(base::at(
              base::make_fdiagonal<Visitor &&,
                        decltype(as_base(std::forward<Vs>(vs)))...>(),
              index)(std::forward<Visitor>(visitor),
                     as_base(std::forward<Vs>(vs))...))
#endif
      };

      struct variant {
        private:
        template <typename Visitor>
        struct visitor {
          template <typename... Values>
          inline static constexpr bool does_not_handle() {
            return detail_variant::is_invocable<Visitor, Values...>::value;
          }
        };

        template <typename Visitor, typename... Values>
        struct visit_exhaustiveness_check {
          static_assert(visitor<Visitor>::template does_not_handle<Values...>(),
                        "`visit` requires the visitor to be exhaustive.");

          inline static constexpr decltype(auto) invoke(Visitor &&visitor,
                                                       Values &&... values)
            DECLTYPE_AUTO_RETURN(invoke(std::forward<Visitor>(visitor),
                                             std::forward<Values>(values)...))
        };

        template <typename Visitor>
        struct value_visitor {
          Visitor &&visitor_;

          template <typename... Alts>
          inline constexpr decltype(auto) operator()(Alts &&... alts) const
            DECLTYPE_AUTO_RETURN(
                visit_exhaustiveness_check<
                    Visitor,
                    decltype((std::forward<Alts>(alts).value))...>::
                    invoke(std::forward<Visitor>(visitor_),
                           std::forward<Alts>(alts).value...))
        };

        template <typename Visitor>
        inline static constexpr auto make_value_visitor(Visitor &&visitor) {
          return value_visitor<Visitor>{std::forward<Visitor>(visitor)};
        }

        public:
        template <typename Visitor, typename... Vs>
        inline static constexpr decltype(auto) visit_alt(Visitor &&visitor,
                                                        Vs &&... vs)
          DECLTYPE_AUTO_RETURN(alt::visit_alt(std::forward<Visitor>(visitor),
                                              std::forward<Vs>(vs).impl_...))

        template <typename Visitor, typename... Vs>
        inline static constexpr decltype(auto) visit_alt_at(std::size_t index,
                                                           Visitor &&visitor,
                                                           Vs &&... vs)
          DECLTYPE_AUTO_RETURN(
              alt::visit_alt_at(index,
                                std::forward<Visitor>(visitor),
                                std::forward<Vs>(vs).impl_...))

        template <typename Visitor, typename... Vs>
        inline static constexpr decltype(auto) visit_value(Visitor &&visitor,
                                                          Vs &&... vs)
          DECLTYPE_AUTO_RETURN(
              visit_alt(make_value_visitor(std::forward<Visitor>(visitor)),
                        std::forward<Vs>(vs)...))

        template <typename Visitor, typename... Vs>
        inline static constexpr decltype(auto) visit_value_at(std::size_t index,
                                                             Visitor &&visitor,
                                                             Vs &&... vs)
          DECLTYPE_AUTO_RETURN(
              visit_alt_at(index,
                           make_value_visitor(std::forward<Visitor>(visitor)),
                           std::forward<Vs>(vs)...))
      };

    }  // namespace visitation

    template <std::size_t Index, typename T>
    struct alt {
      using value_type = T;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#endif
      template <typename... Args>
      inline explicit constexpr alt(in_place_t, Args &&... args)
          : value(std::forward<Args>(args)...) {}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

      T value;
    };

    template <Trait DestructibleTrait, std::size_t Index, typename... Ts>
    union recursive_union;

    template <Trait DestructibleTrait, std::size_t Index>
    union recursive_union<DestructibleTrait, Index> {};

#define GUL14_VARIANT_RECURSIVE_UNION(destructible_trait, destructor)      \
  template <std::size_t Index, typename T, typename... Ts>                 \
  union recursive_union<destructible_trait, Index, T, Ts...> {             \
    public:                                                                \
    inline explicit constexpr recursive_union(valueless_t) noexcept        \
        : dummy_{} {}                                                      \
                                                                           \
    template <typename... Args>                                            \
    inline explicit constexpr recursive_union(in_place_index_t<0>,         \
                                              Args &&... args)             \
        : head_(in_place_t{}, std::forward<Args>(args)...) {}              \
                                                                           \
    template <std::size_t I, typename... Args>                             \
    inline explicit constexpr recursive_union(in_place_index_t<I>,         \
                                              Args &&... args)             \
        : tail_(in_place_index_t<I - 1>{}, std::forward<Args>(args)...) {} \
                                                                           \
    recursive_union(const recursive_union &) = default;                    \
    recursive_union(recursive_union &&) = default;                         \
                                                                           \
    destructor                                                             \
                                                                           \
    recursive_union &operator=(const recursive_union &) = default;         \
    recursive_union &operator=(recursive_union &&) = default;              \
                                                                           \
    private:                                                               \
    char dummy_;                                                           \
    alt<Index, T> head_;                                                   \
    recursive_union<destructible_trait, Index + 1, Ts...> tail_;           \
                                                                           \
    friend struct access::recursive_union;                                 \
  }

    GUL14_VARIANT_RECURSIVE_UNION(Trait::TriviallyAvailable,
                                  ~recursive_union() = default;);
    GUL14_VARIANT_RECURSIVE_UNION(Trait::Available,
                                  ~recursive_union() {});
    GUL14_VARIANT_RECURSIVE_UNION(Trait::Unavailable,
                                  ~recursive_union() = delete;);

#undef GUL14_VARIANT_RECURSIVE_UNION

    template <typename... Ts>
    using index_t = typename std::conditional<
            sizeof...(Ts) < (std::numeric_limits<unsigned char>::max)(),
            unsigned char,
            typename std::conditional<
                sizeof...(Ts) < (std::numeric_limits<unsigned short>::max)(),
                unsigned short,
                unsigned int>::type
            >::type;

    template <Trait DestructibleTrait, typename... Ts>
    class base {
      public:
      inline explicit constexpr base(valueless_t tag) noexcept
          : data_(tag), index_(static_cast<index_t<Ts...>>(-1)) {}

      template <std::size_t I, typename... Args>
      inline explicit constexpr base(in_place_index_t<I>, Args &&... args)
          : data_(in_place_index_t<I>{}, std::forward<Args>(args)...),
            index_(I) {}

      inline constexpr bool valueless_by_exception() const noexcept {
        return index_ == static_cast<index_t<Ts...>>(-1);
      }

      inline constexpr std::size_t index() const noexcept {
        return valueless_by_exception() ? variant_npos : index_;
      }

      protected:
      using data_t = recursive_union<DestructibleTrait, 0, Ts...>;

      friend inline constexpr base &as_base(base &b) { return b; }
      friend inline constexpr const base &as_base(const base &b) { return b; }
      friend inline constexpr base &&as_base(base &&b) { return std::move(b); }
      friend inline constexpr const base &&as_base(const base &&b) { return std::move(b); }

      friend inline constexpr data_t &data(base &b) { return b.data_; }
      friend inline constexpr const data_t &data(const base &b) { return b.data_; }
      friend inline constexpr data_t &&data(base &&b) { return std::move(b).data_; }
      friend inline constexpr const data_t &&data(const base &&b) { return std::move(b).data_; }

      inline static constexpr std::size_t size() { return sizeof...(Ts); }

      data_t data_;
      index_t<Ts...> index_;

      friend struct access::base;
      friend struct visitation::base;
    };

    struct dtor {
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100)
#endif
      template <typename Alt>
      inline void operator()(Alt &alt) const noexcept { alt.~Alt(); }
#ifdef _MSC_VER
#pragma warning(pop)
#endif
    };

#if !defined(_MSC_VER) || _MSC_VER >= 1910
#define GUL14_INHERITING_CTOR(type, base) using base::base;
#else
#define GUL14_INHERITING_CTOR(type, base)         \
  template <typename... Args>                     \
  inline explicit constexpr type(Args &&... args) \
      : base(std::forward<Args>(args)...) {}
#endif

    template <typename Traits, Trait = Traits::destructible_trait>
    class destructor;

#define GUL14_VARIANT_DESTRUCTOR(destructible_trait, definition, destroy) \
  template <typename... Ts>                                               \
  class destructor<traits<Ts...>, destructible_trait>                     \
      : public base<destructible_trait, Ts...> {                          \
    using super = base<destructible_trait, Ts...>;                        \
                                                                          \
    public:                                                               \
    GUL14_INHERITING_CTOR(destructor, super)                              \
    using super::operator=;                                               \
                                                                          \
    destructor(const destructor &) = default;                             \
    destructor(destructor &&) = default;                                  \
    definition                                                            \
    destructor &operator=(const destructor &) = default;                  \
    destructor &operator=(destructor &&) = default;                       \
                                                                          \
    protected:                                                            \
    destroy                                                               \
  }

    GUL14_VARIANT_DESTRUCTOR(
        Trait::TriviallyAvailable,
        ~destructor() = default;,
        inline void destroy() noexcept {
          this->index_ = static_cast<index_t<Ts...>>(-1);
        });

    GUL14_VARIANT_DESTRUCTOR(
        Trait::Available,
        ~destructor() { destroy(); },
        inline void destroy() noexcept {
          if (!this->valueless_by_exception()) {
            visitation::alt::visit_alt(dtor{}, *this);
          }
          this->index_ = static_cast<index_t<Ts...>>(-1);
        });

    GUL14_VARIANT_DESTRUCTOR(
        Trait::Unavailable,
        ~destructor() = delete;,
        inline void destroy() noexcept = delete;);

#undef GUL14_VARIANT_DESTRUCTOR

    template <typename Traits>
    class constructor : public destructor<Traits> {
      using super = destructor<Traits>;

      public:
      GUL14_INHERITING_CTOR(constructor, super)
      using super::operator=;

      protected:
      template <std::size_t I, typename T, typename... Args>
      inline static T &construct_alt(alt<I, T> &a, Args &&... args) {
        auto *result = ::new (static_cast<void *>(std::addressof(a)))
            alt<I, T>(in_place_t{}, std::forward<Args>(args)...);
        return result->value;
      }

      template <typename Rhs>
      inline static void generic_construct(constructor &lhs, Rhs &&rhs) {
        lhs.destroy();
        if (!rhs.valueless_by_exception()) {
          visitation::alt::visit_alt_at(
              rhs.index(),
              [](auto &lhs_alt, auto &&rhs_alt) {
                constructor::construct_alt(
                    lhs_alt, std::forward<decltype(rhs_alt)>(rhs_alt).value);
              }
              ,
              lhs,
              std::forward<Rhs>(rhs));
          lhs.index_ = rhs.index_;
        }
      }
    };

    template <typename Traits, Trait = Traits::move_constructible_trait>
    class move_constructor;

#define GUL14_VARIANT_MOVE_CONSTRUCTOR(move_constructible_trait, definition) \
  template <typename... Ts>                                                  \
  class move_constructor<traits<Ts...>, move_constructible_trait>            \
      : public constructor<traits<Ts...>> {                                  \
    using super = constructor<traits<Ts...>>;                                \
                                                                             \
    public:                                                                  \
    GUL14_INHERITING_CTOR(move_constructor, super)                           \
    using super::operator=;                                                  \
                                                                             \
    move_constructor(const move_constructor &) = default;                    \
    definition                                                               \
    ~move_constructor() = default;                                           \
    move_constructor &operator=(const move_constructor &) = default;         \
    move_constructor &operator=(move_constructor &&) = default;              \
  }

    GUL14_VARIANT_MOVE_CONSTRUCTOR(
        Trait::TriviallyAvailable,
        move_constructor(move_constructor &&that) = default;);

    GUL14_VARIANT_MOVE_CONSTRUCTOR(
        Trait::Available,
        move_constructor(move_constructor &&that) noexcept(
            detail_variant::all<std::is_nothrow_move_constructible<Ts>::value...>::value)
            : move_constructor(valueless_t{}) {
          this->generic_construct(*this, std::move(that));
        });

    GUL14_VARIANT_MOVE_CONSTRUCTOR(
        Trait::Unavailable,
        move_constructor(move_constructor &&) = delete;);

#undef GUL14_VARIANT_MOVE_CONSTRUCTOR

    template <typename Traits, Trait = Traits::copy_constructible_trait>
    class copy_constructor;

#define GUL14_VARIANT_COPY_CONSTRUCTOR(copy_constructible_trait, definition) \
  template <typename... Ts>                                                  \
  class copy_constructor<traits<Ts...>, copy_constructible_trait>            \
      : public move_constructor<traits<Ts...>> {                             \
    using super = move_constructor<traits<Ts...>>;                           \
                                                                             \
    public:                                                                  \
    GUL14_INHERITING_CTOR(copy_constructor, super)                           \
    using super::operator=;                                                  \
                                                                             \
    definition                                                               \
    copy_constructor(copy_constructor &&) = default;                         \
    ~copy_constructor() = default;                                           \
    copy_constructor &operator=(const copy_constructor &) = default;         \
    copy_constructor &operator=(copy_constructor &&) = default;              \
  }

    GUL14_VARIANT_COPY_CONSTRUCTOR(
        Trait::TriviallyAvailable,
        copy_constructor(const copy_constructor &that) = default;);

    GUL14_VARIANT_COPY_CONSTRUCTOR(
        Trait::Available,
        copy_constructor(const copy_constructor &that)
            : copy_constructor(valueless_t{}) {
          this->generic_construct(*this, that);
        });

    GUL14_VARIANT_COPY_CONSTRUCTOR(
        Trait::Unavailable,
        copy_constructor(const copy_constructor &) = delete;);

#undef GUL14_VARIANT_COPY_CONSTRUCTOR

    template <typename Traits>
    class assignment : public copy_constructor<Traits> {
      using super = copy_constructor<Traits>;

      public:
      GUL14_INHERITING_CTOR(assignment, super)
      using super::operator=;

      template <std::size_t I, typename... Args>
      inline /* auto & */ auto emplace(Args &&... args)
          -> decltype(this->construct_alt(access::base::get_alt<I>(*this),
                                          std::forward<Args>(args)...)) {
        this->destroy();
        auto &result = this->construct_alt(access::base::get_alt<I>(*this),
                                           std::forward<Args>(args)...);
        this->index_ = I;
        return result;
      }

      protected:
      template <std::size_t I, typename T, typename Arg>
      inline void assign_alt(alt<I, T> &a, Arg &&arg) {
        if (this->index() == I) {
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#endif
          a.value = std::forward<Arg>(arg);
#ifdef _MSC_VER
#pragma warning(pop)
#endif
        } else {
          struct {
            void operator()(std::true_type) const {
              this_->emplace<I>(std::forward<Arg>(arg_));
            }
            void operator()(std::false_type) const {
              this_->emplace<I>(T(std::forward<Arg>(arg_)));
            }
            assignment *this_;
            Arg &&arg_;
          } impl{this, std::forward<Arg>(arg)};
          impl(std::integral_constant<bool,
                   std::is_nothrow_constructible<T, Arg>::value ||
                   !std::is_nothrow_move_constructible<T>::value>{});
        }
      }

      template <typename That>
      inline void generic_assign(That &&that) {
        if (this->valueless_by_exception() && that.valueless_by_exception()) {
          // do nothing.
        } else if (that.valueless_by_exception()) {
          this->destroy();
        } else {
          visitation::alt::visit_alt_at(
              that.index(),
              [this](auto &this_alt, auto &&that_alt) {
                this->assign_alt(
                    this_alt, std::forward<decltype(that_alt)>(that_alt).value);
              },
              *this,
              std::forward<That>(that));
        }
      }
    };

    template <typename Traits, Trait = Traits::move_assignable_trait>
    class move_assignment;

#define GUL14_VARIANT_MOVE_ASSIGNMENT(move_assignable_trait, definition) \
  template <typename... Ts>                                              \
  class move_assignment<traits<Ts...>, move_assignable_trait>            \
      : public assignment<traits<Ts...>> {                               \
    using super = assignment<traits<Ts...>>;                             \
                                                                         \
    public:                                                              \
    GUL14_INHERITING_CTOR(move_assignment, super)                        \
    using super::operator=;                                              \
                                                                         \
    move_assignment(const move_assignment &) = default;                  \
    move_assignment(move_assignment &&) = default;                       \
    ~move_assignment() = default;                                        \
    move_assignment &operator=(const move_assignment &) = default;       \
    definition                                                           \
  }

    GUL14_VARIANT_MOVE_ASSIGNMENT(
        Trait::TriviallyAvailable,
        move_assignment &operator=(move_assignment &&that) = default;);

    GUL14_VARIANT_MOVE_ASSIGNMENT(
        Trait::Available,
        move_assignment &
        operator=(move_assignment &&that) noexcept(
            detail_variant::all<(std::is_nothrow_move_constructible<Ts>::value &&
                      std::is_nothrow_move_assignable<Ts>::value)...>::value) {
          this->generic_assign(std::move(that));
          return *this;
        });

    GUL14_VARIANT_MOVE_ASSIGNMENT(
        Trait::Unavailable,
        move_assignment &operator=(move_assignment &&) = delete;);

#undef GUL14_VARIANT_MOVE_ASSIGNMENT

    template <typename Traits, Trait = Traits::copy_assignable_trait>
    class copy_assignment;

#define GUL14_VARIANT_COPY_ASSIGNMENT(copy_assignable_trait, definition) \
  template <typename... Ts>                                              \
  class copy_assignment<traits<Ts...>, copy_assignable_trait>            \
      : public move_assignment<traits<Ts...>> {                          \
    using super = move_assignment<traits<Ts...>>;                        \
                                                                         \
    public:                                                              \
    GUL14_INHERITING_CTOR(copy_assignment, super)                        \
    using super::operator=;                                              \
                                                                         \
    copy_assignment(const copy_assignment &) = default;                  \
    copy_assignment(copy_assignment &&) = default;                       \
    ~copy_assignment() = default;                                        \
    definition                                                           \
    copy_assignment &operator=(copy_assignment &&) = default;            \
  }

    GUL14_VARIANT_COPY_ASSIGNMENT(
        Trait::TriviallyAvailable,
        copy_assignment &operator=(const copy_assignment &that) = default;);

    GUL14_VARIANT_COPY_ASSIGNMENT(
        Trait::Available,
        copy_assignment &operator=(const copy_assignment &that) {
          this->generic_assign(that);
          return *this;
        });

    GUL14_VARIANT_COPY_ASSIGNMENT(
        Trait::Unavailable,
        copy_assignment &operator=(const copy_assignment &) = delete;);

#undef GUL14_VARIANT_COPY_ASSIGNMENT

    template <typename... Ts>
    class impl : public copy_assignment<traits<Ts...>> {
      using super = copy_assignment<traits<Ts...>>;

      public:
      GUL14_INHERITING_CTOR(impl, super)
      using super::operator=;

      impl(const impl&) = default;
      impl(impl&&) = default;
      ~impl() = default;
      impl &operator=(const impl &) = default;
      impl &operator=(impl &&) = default;

      template <std::size_t I, typename Arg>
      inline void assign(Arg &&arg) {
        this->assign_alt(access::base::get_alt<I>(*this),
                         std::forward<Arg>(arg));
      }

      inline void swap(impl &that) {
        if (this->valueless_by_exception() && that.valueless_by_exception()) {
          // do nothing.
        } else if (this->index() == that.index()) {
          visitation::alt::visit_alt_at(this->index(),
                                        [](auto &this_alt, auto &that_alt) {
                                          using std::swap;
                                          swap(this_alt.value,
                                               that_alt.value);
                                        },
                                        *this,
                                        that);
        } else {
          impl *lhs = this;
          impl *rhs = std::addressof(that);
          if (lhs->move_nothrow() && !rhs->move_nothrow()) {
            std::swap(lhs, rhs);
          }
          impl tmp(std::move(*rhs));
          // EXTENSION: When the move construction of `lhs` into `rhs` throws
          // and `tmp` is nothrow move constructible then we move `tmp` back
          // into `rhs` and provide the strong exception safety guarantee.
          try {
            this->generic_construct(*rhs, std::move(*lhs));
          } catch (...) {
            if (tmp.move_nothrow()) {
              this->generic_construct(*rhs, std::move(tmp));
            }
            throw;
          }
          this->generic_construct(*lhs, std::move(tmp));
        }
      }

      private:
      inline constexpr bool move_nothrow() const {
        return this->valueless_by_exception() ||
               detail_variant::array<bool, sizeof...(Ts)>{
                   {std::is_nothrow_move_constructible<Ts>::value...}
               }[this->index()];
      }
    };

#undef GUL14_INHERITING_CTOR

    template <typename From, typename To>
    struct is_non_narrowing_convertible {
      template <typename T>
      static std::true_type test(T(&&)[1]);

      template <typename T>
      static auto impl(int) -> decltype(test<T>({std::declval<From>()}));

      template <typename>
      static auto impl(...) -> std::false_type;

      static constexpr bool value = decltype(impl<To>(0))::value;
    };

    template <typename Arg,
              std::size_t I,
              typename T,
              bool = std::is_arithmetic<T>::value,
              typename = void>
    struct overload_leaf {};

    template <typename Arg, std::size_t I, typename T>
    struct overload_leaf<Arg, I, T, false> {
      using impl = std::integral_constant<std::size_t, I> (*)(T);
      operator impl() const { return nullptr; };
    };

    template <typename Arg, std::size_t I, typename T>
    struct overload_leaf<
        Arg,
        I,
        T,
        true
#if defined(__clang__) || !defined(__GNUC__) || __GNUC__ >= 5
        ,
        std::enable_if_t<
            std::is_same<remove_cvref_t<T>, bool>::value
                ? std::is_same<remove_cvref_t<Arg>, bool>::value
                : is_non_narrowing_convertible<Arg, T>::value>
#endif
        > {
      using impl = std::integral_constant<std::size_t, I> (*)(T);
      operator impl() const { return nullptr; };
    };

    template <typename Arg, typename... Ts>
    struct overload_impl {
      private:
      template <typename>
      struct impl;

      template <std::size_t... Is>
      struct impl<std::index_sequence<Is...>> : overload_leaf<Arg, Is, Ts>... {};

      public:
      using type = impl<std::index_sequence_for<Ts...>>;
    };

    template <typename Arg, typename... Ts>
    using overload = typename overload_impl<Arg, Ts...>::type;

    template <typename Arg, typename... Ts>
    using best_match = invoke_result_t<overload<Arg, Ts...>, Arg>;

    template <typename T>
    struct is_in_place_index : std::false_type {};

    template <std::size_t I>
    struct is_in_place_index<in_place_index_t<I>> : std::true_type {};

    template <typename T>
    struct is_in_place_type : std::false_type {};

    template <typename T>
    struct is_in_place_type<in_place_type_t<T>> : std::true_type {};

  }  // detail_variant

  template <typename... Ts>
  class variant {
    static_assert(0 < sizeof...(Ts),
                  "variant must consist of at least one alternative.");

    static_assert(detail_variant::all<!std::is_array<Ts>::value...>::value,
                  "variant can not have an array type as an alternative.");

    static_assert(detail_variant::all<!std::is_reference<Ts>::value...>::value,
                  "variant can not have a reference type as an alternative.");

    static_assert(detail_variant::all<!std::is_void<Ts>::value...>::value,
                  "variant can not have a void type as an alternative.");

    public:
    template <
        typename Front = detail_variant::type_pack_element_t<0, Ts...>,
        std::enable_if_t<std::is_default_constructible<Front>::value, int> = 0>
    inline constexpr variant() noexcept(
        std::is_nothrow_default_constructible<Front>::value)
        : impl_(in_place_index_t<0>{}) {}

    variant(const variant &) = default;
    variant(variant &&) = default;

    template <
        typename Arg,
        typename Decayed = std::decay_t<Arg>,
        std::enable_if_t<!std::is_same<Decayed, variant>::value, int> = 0,
        std::enable_if_t<!detail_variant::is_in_place_index<Decayed>::value, int> = 0,
        std::enable_if_t<!detail_variant::is_in_place_type<Decayed>::value, int> = 0,
        std::size_t I = detail_variant::best_match<Arg, Ts...>::value,
        typename T = detail_variant::type_pack_element_t<I, Ts...>,
        std::enable_if_t<std::is_constructible<T, Arg>::value, int> = 0>
    inline constexpr variant(Arg &&arg) noexcept(
        std::is_nothrow_constructible<T, Arg>::value)
        : impl_(in_place_index_t<I>{}, std::forward<Arg>(arg)) {}

    template <
        std::size_t I,
        typename... Args,
        typename T = detail_variant::type_pack_element_t<I, Ts...>,
        std::enable_if_t<std::is_constructible<T, Args...>::value, int> = 0>
    inline explicit constexpr variant(
        in_place_index_t<I>,
        Args &&... args) noexcept(std::is_nothrow_constructible<T,
                                                                Args...>::value)
        : impl_(in_place_index_t<I>{}, std::forward<Args>(args)...) {}

    template <
        std::size_t I,
        typename Up,
        typename... Args,
        typename T = detail_variant::type_pack_element_t<I, Ts...>,
        std::enable_if_t<std::is_constructible<T,
                                               std::initializer_list<Up> &,
                                               Args...>::value,
                         int> = 0>
    inline explicit constexpr variant(
        in_place_index_t<I>,
        std::initializer_list<Up> il,
        Args &&... args) noexcept(std::
                                      is_nothrow_constructible<
                                          T,
                                          std::initializer_list<Up> &,
                                          Args...>::value)
        : impl_(in_place_index_t<I>{}, il, std::forward<Args>(args)...) {}

    template <
        typename T,
        typename... Args,
        std::size_t I = detail_variant::find_index_sfinae<T, Ts...>::value,
        std::enable_if_t<std::is_constructible<T, Args...>::value, int> = 0>
    inline explicit constexpr variant(
        in_place_type_t<T>,
        Args &&... args) noexcept(std::is_nothrow_constructible<T,
                                                                Args...>::value)
        : impl_(in_place_index_t<I>{}, std::forward<Args>(args)...) {}

    template <
        typename T,
        typename Up,
        typename... Args,
        std::size_t I = detail_variant::find_index_sfinae<T, Ts...>::value,
        std::enable_if_t<std::is_constructible<T,
                                               std::initializer_list<Up> &,
                                               Args...>::value,
                         int> = 0>
    inline explicit constexpr variant(
        in_place_type_t<T>,
        std::initializer_list<Up> il,
        Args &&... args) noexcept(std::
                                      is_nothrow_constructible<
                                          T,
                                          std::initializer_list<Up> &,
                                          Args...>::value)
        : impl_(in_place_index_t<I>{}, il, std::forward<Args>(args)...) {}

    ~variant() = default;

    variant &operator=(const variant &) = default;
    variant &operator=(variant &&) = default;

    template <typename Arg,
              std::enable_if_t<!std::is_same<std::decay_t<Arg>, variant>::value,
                               int> = 0,
              std::size_t I = detail_variant::best_match<Arg, Ts...>::value,
              typename T = detail_variant::type_pack_element_t<I, Ts...>,
              std::enable_if_t<(std::is_assignable<T &, Arg>::value &&
                                std::is_constructible<T, Arg>::value),
                               int> = 0>
    inline variant &operator=(Arg &&arg) noexcept(
        (std::is_nothrow_assignable<T &, Arg>::value &&
         std::is_nothrow_constructible<T, Arg>::value)) {
      impl_.template assign<I>(std::forward<Arg>(arg));
      return *this;
    }

    template <
        std::size_t I,
        typename... Args,
        typename T = detail_variant::type_pack_element_t<I, Ts...>,
        std::enable_if_t<std::is_constructible<T, Args...>::value, int> = 0>
    inline T &emplace(Args &&... args) {
      return impl_.template emplace<I>(std::forward<Args>(args)...);
    }

    template <
        std::size_t I,
        typename Up,
        typename... Args,
        typename T = detail_variant::type_pack_element_t<I, Ts...>,
        std::enable_if_t<std::is_constructible<T,
                                               std::initializer_list<Up> &,
                                               Args...>::value,
                         int> = 0>
    inline T &emplace(std::initializer_list<Up> il, Args &&... args) {
      return impl_.template emplace<I>(il, std::forward<Args>(args)...);
    }

    template <
        typename T,
        typename... Args,
        std::size_t I = detail_variant::find_index_sfinae<T, Ts...>::value,
        std::enable_if_t<std::is_constructible<T, Args...>::value, int> = 0>
    inline T &emplace(Args &&... args) {
      return impl_.template emplace<I>(std::forward<Args>(args)...);
    }

    template <
        typename T,
        typename Up,
        typename... Args,
        std::size_t I = detail_variant::find_index_sfinae<T, Ts...>::value,
        std::enable_if_t<std::is_constructible<T,
                                               std::initializer_list<Up> &,
                                               Args...>::value,
                         int> = 0>
    inline T &emplace(std::initializer_list<Up> il, Args &&... args) {
      return impl_.template emplace<I>(il, std::forward<Args>(args)...);
    }

    inline constexpr bool valueless_by_exception() const noexcept {
      return impl_.valueless_by_exception();
    }

    inline constexpr std::size_t index() const noexcept {
      return impl_.index();
    }

    template <bool Dummy = true,
              std::enable_if_t<
                  detail_variant::all<Dummy,
                           (detail_variant::dependent_type<std::is_move_constructible<Ts>,
                                                Dummy>::value &&
                            detail_variant::dependent_type<detail_variant::is_swappable<Ts>,
                                                Dummy>::value)...>::value,
                  int> = 0>
    inline void swap(variant &that) noexcept(
        detail_variant::all<(std::is_nothrow_move_constructible<Ts>::value &&
                  detail_variant::is_nothrow_swappable<Ts>::value)...>::value) {
      impl_.swap(that.impl_);
    }

    private:
    detail_variant::impl<Ts...> impl_;

    friend struct detail_variant::access::variant;
    friend struct detail_variant::visitation::variant;
  };

  template <std::size_t I, typename... Ts>
  inline constexpr bool holds_alternative(const variant<Ts...> &v) noexcept {
    return v.index() == I;
  }

  template <typename T, typename... Ts>
  inline constexpr bool holds_alternative(const variant<Ts...> &v) noexcept {
    return holds_alternative<detail_variant::find_index_checked<T, Ts...>::value>(v);
  }

  namespace detail_variant {
    template <std::size_t I, typename V>
    struct generic_get_impl {
      constexpr generic_get_impl(int) noexcept {}

      constexpr auto&& operator()(V &&v) const
        AUTO_REFREF_RETURN(
            access::variant::get_alt<I>(std::forward<V>(v)).value)
    };

    template <std::size_t I, typename V>
    inline constexpr auto&& generic_get(V &&v)
      AUTO_REFREF_RETURN(generic_get_impl<I, V>(
          holds_alternative<I>(v) ? 0 : (throw_bad_variant_access(), 0))(
          std::forward<V>(v)))
  }  // namespace detail_variant

  template <std::size_t I, typename... Ts>
  inline constexpr variant_alternative_t<I, variant<Ts...>> &get(
      variant<Ts...> &v) {
    return detail_variant::generic_get<I>(v);
  }

  template <std::size_t I, typename... Ts>
  inline constexpr variant_alternative_t<I, variant<Ts...>> &&get(
      variant<Ts...> &&v) {
    return detail_variant::generic_get<I>(std::move(v));
  }

  template <std::size_t I, typename... Ts>
  inline constexpr const variant_alternative_t<I, variant<Ts...>> &get(
      const variant<Ts...> &v) {
    return detail_variant::generic_get<I>(v);
  }

  template <std::size_t I, typename... Ts>
  inline constexpr const variant_alternative_t<I, variant<Ts...>> &&get(
      const variant<Ts...> &&v) {
    return detail_variant::generic_get<I>(std::move(v));
  }

  template <typename T, typename... Ts>
  inline constexpr T &get(variant<Ts...> &v) {
    return get<detail_variant::find_index_checked<T, Ts...>::value>(v);
  }

  template <typename T, typename... Ts>
  inline constexpr T &&get(variant<Ts...> &&v) {
    return get<detail_variant::find_index_checked<T, Ts...>::value>(std::move(v));
  }

  template <typename T, typename... Ts>
  inline constexpr const T &get(const variant<Ts...> &v) {
    return get<detail_variant::find_index_checked<T, Ts...>::value>(v);
  }

  template <typename T, typename... Ts>
  inline constexpr const T &&get(const variant<Ts...> &&v) {
    return get<detail_variant::find_index_checked<T, Ts...>::value>(std::move(v));
  }

  namespace detail_variant {

    template <std::size_t I, typename V>
    inline constexpr auto* generic_get_if(V* v) noexcept {
      return v && holds_alternative<I>(*v)
              ? std::addressof(access::variant::get_alt<I>(*v).value)
              : nullptr;
    }

  }  // namespace detail_variant

  template <std::size_t I, typename... Ts>
  inline constexpr std::add_pointer_t<variant_alternative_t<I, variant<Ts...>>>
  get_if(variant<Ts...> *v) noexcept {
    return detail_variant::generic_get_if<I>(v);
  }

  template <std::size_t I, typename... Ts>
  inline constexpr std::add_pointer_t<
      const variant_alternative_t<I, variant<Ts...>>>
  get_if(const variant<Ts...> *v) noexcept {
    return detail_variant::generic_get_if<I>(v);
  }

  template <typename T, typename... Ts>
  inline constexpr std::add_pointer_t<T>
  get_if(variant<Ts...> *v) noexcept {
    return get_if<detail_variant::find_index_checked<T, Ts...>::value>(v);
  }

  template <typename T, typename... Ts>
  inline constexpr std::add_pointer_t<const T>
  get_if(const variant<Ts...> *v) noexcept {
    return get_if<detail_variant::find_index_checked<T, Ts...>::value>(v);
  }

  namespace detail_variant {
    template <typename RelOp>
    struct convert_to_bool {
      template <typename Lhs, typename Rhs>
      inline constexpr bool operator()(Lhs &&lhs, Rhs &&rhs) const {
        static_assert(std::is_convertible<invoke_result_t<RelOp, Lhs, Rhs>,
                                          bool>::value,
                      "relational operators must return a type"
                      " implicitly convertible to bool");
        return invoke(
            RelOp{}, std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
      }
    };
  }  // namespace detail_variant

  template <typename... Ts>
  inline constexpr bool operator==(const variant<Ts...> &lhs,
                                   const variant<Ts...> &rhs) {
    using detail_variant::visitation::variant;
    using equal_to = detail_variant::convert_to_bool<std::equal_to<>>;
    if (lhs.index() != rhs.index()) return false;
    if (lhs.valueless_by_exception()) return true;
    return variant::visit_value_at(lhs.index(), equal_to{}, lhs, rhs);
  }

  template <typename... Ts>
  inline constexpr bool operator!=(const variant<Ts...> &lhs,
                                   const variant<Ts...> &rhs) {
    using detail_variant::visitation::variant;
    using not_equal_to = detail_variant::convert_to_bool<std::not_equal_to<>>;
    if (lhs.index() != rhs.index()) return true;
    if (lhs.valueless_by_exception()) return false;
    return variant::visit_value_at(lhs.index(), not_equal_to{}, lhs, rhs);
  }

  template <typename... Ts>
  inline constexpr bool operator<(const variant<Ts...> &lhs,
                                  const variant<Ts...> &rhs) {
    using detail_variant::visitation::variant;
    using less = detail_variant::convert_to_bool<std::less<>>;
    if (rhs.valueless_by_exception()) return false;
    if (lhs.valueless_by_exception()) return true;
    if (lhs.index() < rhs.index()) return true;
    if (lhs.index() > rhs.index()) return false;
    return variant::visit_value_at(lhs.index(), less{}, lhs, rhs);
  }

  template <typename... Ts>
  inline constexpr bool operator>(const variant<Ts...> &lhs,
                                  const variant<Ts...> &rhs) {
    using detail_variant::visitation::variant;
    using greater = detail_variant::convert_to_bool<std::greater<>>;
    if (lhs.valueless_by_exception()) return false;
    if (rhs.valueless_by_exception()) return true;
    if (lhs.index() > rhs.index()) return true;
    if (lhs.index() < rhs.index()) return false;
    return variant::visit_value_at(lhs.index(), greater{}, lhs, rhs);
  }

  template <typename... Ts>
  inline constexpr bool operator<=(const variant<Ts...> &lhs,
                                   const variant<Ts...> &rhs) {
    using detail_variant::visitation::variant;
    using less_equal = detail_variant::convert_to_bool<std::less_equal<>>;
    if (lhs.valueless_by_exception()) return true;
    if (rhs.valueless_by_exception()) return false;
    if (lhs.index() < rhs.index()) return true;
    if (lhs.index() > rhs.index()) return false;
    return variant::visit_value_at(lhs.index(), less_equal{}, lhs, rhs);
  }

  template <typename... Ts>
  inline constexpr bool operator>=(const variant<Ts...> &lhs,
                                   const variant<Ts...> &rhs) {
    using detail_variant::visitation::variant;
    using greater_equal = detail_variant::convert_to_bool<std::greater_equal<>>;
    if (rhs.valueless_by_exception()) return true;
    if (lhs.valueless_by_exception()) return false;
    if (lhs.index() > rhs.index()) return true;
    if (lhs.index() < rhs.index()) return false;
    return variant::visit_value_at(lhs.index(), greater_equal{}, lhs, rhs);
  }

  namespace detail_variant {

    inline constexpr bool any(std::initializer_list<bool> bs) {
      for (bool b : bs) {
        if (b) {
          return true;
        }
      }
      return false;
    }

  }  // namespace detail_variant

  template <typename Visitor, typename... Vs>
  inline constexpr decltype(auto) visit(Visitor &&visitor, Vs &&... vs) {
    return (!detail_variant::any({vs.valueless_by_exception()...})
                ? (void)0
                : throw_bad_variant_access()),
           detail_variant::visitation::variant::visit_value(
               std::forward<Visitor>(visitor), std::forward<Vs>(vs)...);
  }

  template <typename... Ts>
  inline auto swap(variant<Ts...> &lhs,
                   variant<Ts...> &rhs) noexcept(noexcept(lhs.swap(rhs)))
      -> decltype(lhs.swap(rhs)) {
    lhs.swap(rhs);
  }

  namespace detail_variant {

    template <typename T, typename...>
    using enabled_type = T;

    namespace hash {

      template <typename H, typename K>
      constexpr bool meets_requirements() noexcept {
        return std::is_copy_constructible<H>::value &&
               std::is_move_constructible<H>::value &&
               detail_variant::is_invocable_r<std::size_t, H, const K &>::value;
      }

      template <typename K>
      constexpr bool is_enabled() noexcept {
        using H = std::hash<K>;
        return meets_requirements<H, K>() &&
               std::is_default_constructible<H>::value &&
               std::is_copy_assignable<H>::value &&
               std::is_move_assignable<H>::value;
      }

    } // namespace hash

  } // namespace detail_variant

#undef AUTO_REFREF_RETURN
#undef DECLTYPE_AUTO_RETURN
#undef GUL14_RETURN

} // namespace gul14

namespace std {

template <typename... Ts>
struct hash<gul14::detail_variant::enabled_type<
    gul14::variant<Ts...>,
    std::enable_if_t<gul14::detail_variant::all<gul14::detail_variant::hash::is_enabled<
        std::remove_const_t<Ts>>()...>::value>>> {
  using argument_type = gul14::variant<Ts...>;
  using result_type = std::size_t;

  inline result_type operator()(const argument_type &v) const {
    using gul14::detail_variant::visitation::variant;
    std::size_t result =
        v.valueless_by_exception()
            ? 299792458  // Random value chosen by the universe upon creation
            : variant::visit_alt(
                  [](const auto &alt) {
                    using alt_type = std::decay_t<decltype(alt)>;
                    using value_type = std::remove_const_t<
                        typename alt_type::value_type>;
                    return hash<value_type>{}(alt.value);
                  },
                  v);
    return hash_combine(result, hash<std::size_t>{}(v.index()));
  }

  private:
  static std::size_t hash_combine(std::size_t lhs, std::size_t rhs) {
    return lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
  }
};

/// @}

} // namespace std

/// \endcond

#endif
