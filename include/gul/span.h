/**
 * \file   span.h
 * \author Tristan Brindle
 * \brief  Provides a gul::span that mimicks C++20's std::span as closely as possible.
 *
 * \copyright
 * Copyright Tristan Brindle 2018.
 * Distributed under the Boost Software License, Version 1.0. (See \ref license_boost_1_0
 * or http://www.boost.org/LICENSE_1_0.txt)
 *
 * For more information, see http://www.boost.org
 *
 * This is an implementation of C++20's std::span
 *   http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/n4820.pdf
 *
 * Modified November 2019 for GUL (L. Froehlich)
 */

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include <type_traits>

namespace gul {

/// \cond HIDE_SYMBOLS

// Various feature test macros
#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#define GUL_SPAN_HAVE_CPP17
#endif

#if defined(GUL_SPAN_HAVE_CPP17) || defined(__cpp_inline_variables)
#define GUL_SPAN_INLINE_VAR inline
#else
#define GUL_SPAN_INLINE_VAR
#endif

#if (!defined(_MSC_VER) || _MSC_VER > 1900)
#define GUL_SPAN_CONSTEXPR_ASSIGN constexpr
#else
#define GUL_SPAN_CONSTEXPR_ASSIGN
#endif

#if defined(GUL_SPAN_HAVE_CPP17) || defined(__cpp_deduction_guides)
#define GUL_SPAN_HAVE_DEDUCTION_GUIDES
#endif

#if defined(GUL_SPAN_HAVE_CPP17) || defined(__cpp_lib_byte)
#define GUL_SPAN_HAVE_STD_BYTE
#endif

#if defined(GUL_SPAN_HAVE_CPP17) || defined(__cpp_lib_array_constexpr)
#define GUL_SPAN_HAVE_CONSTEXPR_STD_ARRAY_ETC
#endif

#if defined(GUL_SPAN_HAVE_CONSTEXPR_STD_ARRAY_ETC)
#define GUL_SPAN_ARRAY_CONSTEXPR constexpr
#else
#define GUL_SPAN_ARRAY_CONSTEXPR
#endif

#ifdef GUL_SPAN_HAVE_STD_BYTE
using byte = std::byte;
#else
using byte = unsigned char;
#endif

#if defined(GUL_SPAN_HAVE_CPP17)
#define GUL_SPAN_NODISCARD [[nodiscard]]
#else
#define GUL_SPAN_NODISCARD
#endif

GUL_SPAN_INLINE_VAR constexpr std::size_t dynamic_extent = SIZE_MAX;

template <typename ElementType, std::size_t Extent = dynamic_extent>
class span;

namespace detail {

template <typename E, std::size_t S>
struct span_storage {
    constexpr span_storage() noexcept = default;

    constexpr span_storage(E* pointer, std::size_t /*unused*/) noexcept : ptr(pointer)
    {}

    E* ptr = nullptr;
    static constexpr std::size_t size = S;
};

template <typename E>
struct span_storage<E, dynamic_extent> {
    constexpr span_storage() noexcept = default;

    constexpr span_storage(E* pointer, std::size_t sz) noexcept
        : ptr(pointer), size(sz)
    {}

    E* ptr = nullptr;
    std::size_t size = 0;
};

// Reimplementation of C++17 std::size() and std::data()
#if defined(GUL_SPAN_HAVE_CPP17) || defined(__cpp_lib_nonmember_container_access)
using std::data;
using std::size;
#else
template <class C>
constexpr auto size(const C& c) -> decltype(c.size())
{
    return c.size();
}

template <class T, std::size_t N>
constexpr std::size_t size(const T (&)[N]) noexcept
{
    return N;
}

template <class C>
constexpr auto data(C& c) -> decltype(c.data())
{
    return c.data();
}

template <class C>
constexpr auto data(const C& c) -> decltype(c.data())
{
    return c.data();
}

template <class T, std::size_t N>
constexpr T* data(T (&array)[N]) noexcept
{
    return array;
}

template <class E>
constexpr const E* data(std::initializer_list<E> il) noexcept
{
    return il.begin();
}
#endif // GUL_SPAN_HAVE_CPP17

#if defined(GUL_SPAN_HAVE_CPP17) || defined(__cpp_lib_void_t)
using std::void_t;
#else
template <typename...>
using void_t = void;
#endif

template <typename T>
using uncvref_t =
    typename std::remove_cv<typename std::remove_reference<T>::type>::type;

template <typename>
struct is_span : std::false_type {};

template <typename T, std::size_t S>
struct is_span<span<T, S>> : std::true_type {};

template <typename>
struct is_std_array : std::false_type {};

template <typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

template <typename, typename = void>
struct has_size_and_data : std::false_type {};

template <typename T>
struct has_size_and_data<T, void_t<decltype(detail::size(std::declval<T>())),
                                   decltype(detail::data(std::declval<T>()))>>
    : std::true_type {};

template <typename C, typename U = uncvref_t<C>>
struct is_container {
    static constexpr bool value =
        !is_span<U>::value && !is_std_array<U>::value &&
        !std::is_array<U>::value && has_size_and_data<C>::value;
};

template <typename T>
using remove_pointer_t = typename std::remove_pointer<T>::type;

template <typename, typename, typename = void>
struct is_container_element_type_compatible : std::false_type {};

template <typename T, typename E>
struct is_container_element_type_compatible<
    T, E, void_t<decltype(detail::data(std::declval<T>()))>>
    : std::is_convertible<
          remove_pointer_t<decltype(detail::data(std::declval<T>()))> (*)[],
          E (*)[]> {};

template <typename, typename = size_t>
struct is_complete : std::false_type {};

template <typename T>
struct is_complete<T, decltype(sizeof(T))> : std::true_type {};

} // namespace detail


/// \endcond


/**
 * A view to a contiguous sequence of objects. This is a backport of
 * [std::span](https://en.cppreference.com/w/cpp/container/span) from C++20.
 *
 * \since GUL version 1.9
 */
template <typename ElementType, std::size_t Extent>
class span {
    static_assert(std::is_object<ElementType>::value,
                  "A span's ElementType must be an object type (not a "
                  "reference type or void)");
    static_assert(detail::is_complete<ElementType>::value,
                  "A span's ElementType must be a complete type (not a forward "
                  "declaration)");
    static_assert(!std::is_abstract<ElementType>::value,
                  "A span's ElementType cannot be an abstract class type");

    using storage_type = detail::span_storage<ElementType, Extent>;

public:

/// \{

    // constants and types
    using element_type = ElementType;
    using value_type = typename std::remove_cv<ElementType>::type;
    using index_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = element_type*;
    using const_pointer = const element_type*;
    using reference = element_type&;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    static constexpr index_type extent = Extent;

    // [span.cons], span constructors, copy, assignment, and destructor
    template <
        std::size_t E = Extent,
        typename std::enable_if<(E == dynamic_extent || E <= 0), int>::type = 0>
    constexpr span() noexcept
    {}

    constexpr span(pointer ptr, index_type count)
        : storage_(ptr, count)
    {}

    constexpr span(pointer first_elem, pointer last_elem)
        : storage_(first_elem, last_elem - first_elem)
    {}

    template <std::size_t N, std::size_t E = Extent,
              typename std::enable_if<
                  (E == dynamic_extent || N == E) &&
                      detail::is_container_element_type_compatible<
                          element_type (&)[N], ElementType>::value,
                  int>::type = 0>
    constexpr span(element_type (&arr)[N]) noexcept : storage_(arr, N)
    {}

    template <std::size_t N, std::size_t E = Extent,
              typename std::enable_if<
                  (E == dynamic_extent || N == E) &&
                      detail::is_container_element_type_compatible<
                          std::array<value_type, N>&, ElementType>::value,
                  int>::type = 0>
    GUL_SPAN_ARRAY_CONSTEXPR span(std::array<value_type, N>& arr) noexcept
        : storage_(arr.data(), N)
    {}

    template <std::size_t N, std::size_t E = Extent,
              typename std::enable_if<
                  (E == dynamic_extent || N == E) &&
                      detail::is_container_element_type_compatible<
                          const std::array<value_type, N>&, ElementType>::value,
                  int>::type = 0>
    GUL_SPAN_ARRAY_CONSTEXPR span(const std::array<value_type, N>& arr) noexcept
        : storage_(arr.data(), N)
    {}

    template <
        typename Container, std::size_t E = Extent,
        typename std::enable_if<
            E == dynamic_extent && detail::is_container<Container>::value &&
                detail::is_container_element_type_compatible<
                    Container&, ElementType>::value,
            int>::type = 0>
    constexpr span(Container& cont)
        : storage_(detail::data(cont), detail::size(cont))
    {}

    template <
        typename Container, std::size_t E = Extent,
        typename std::enable_if<
            E == dynamic_extent && detail::is_container<Container>::value &&
                detail::is_container_element_type_compatible<
                    const Container&, ElementType>::value,
            int>::type = 0>
    constexpr span(const Container& cont)
        : storage_(detail::data(cont), detail::size(cont))
    {}

    constexpr span(const span& other) noexcept = default;

    template <typename OtherElementType, std::size_t OtherExtent,
              typename std::enable_if<
                  (Extent == OtherExtent || Extent == dynamic_extent) &&
                      std::is_convertible<OtherElementType (*)[],
                                          ElementType (*)[]>::value,
                  int>::type = 0>
    constexpr span(const span<OtherElementType, OtherExtent>& other) noexcept
        : storage_(other.data(), other.size())
    {}

    ~span() noexcept = default;

    GUL_SPAN_CONSTEXPR_ASSIGN span&
    operator=(const span& other) noexcept = default;

    // [span.sub], span subviews
    template <std::size_t Count>
    constexpr span<element_type, Count> first() const
    {
        return {data(), Count};
    }

    template <std::size_t Count>
    constexpr span<element_type, Count> last() const
    {
        return {data() + (size() - Count), Count};
    }

    template <std::size_t Offset, std::size_t Count = dynamic_extent>
    using subspan_return_t =
        span<ElementType, Count != dynamic_extent
                              ? Count
                              : (Extent != dynamic_extent ? Extent - Offset
                                                          : dynamic_extent)>;

    template <std::size_t Offset, std::size_t Count = dynamic_extent>
    constexpr subspan_return_t<Offset, Count> subspan() const
    {
        return {data() + Offset,
                Count != dynamic_extent ? Count : size() - Offset};
    }

    constexpr span<element_type, dynamic_extent>
    first(index_type count) const
    {
        return {data(), count};
    }

    constexpr span<element_type, dynamic_extent>
    last(index_type count) const
    {
        return {data() + (size() - count), count};
    }

    constexpr span<element_type, dynamic_extent>
    subspan(index_type offset, index_type count = dynamic_extent) const
    {
        return {data() + offset,
                count == dynamic_extent ? size() - offset : count};
    }

    // [span.obs], span observers
    constexpr index_type size() const noexcept { return storage_.size; }

    constexpr index_type size_bytes() const noexcept
    {
        return size() * sizeof(element_type);
    }

    GUL_SPAN_NODISCARD constexpr bool empty() const noexcept
    {
        return size() == 0;
    }

    // [span.elem], span element access
    constexpr reference operator[](index_type idx) const
    {
        return *(data() + idx);
    }

    constexpr reference front() const
    {
        return *data();
    }

    constexpr reference back() const
    {
        return *(data() + (size() - 1));
    }

    constexpr pointer data() const noexcept { return storage_.ptr; }

    // [span.iterators], span iterator support
    constexpr iterator begin() const noexcept { return data(); }

    constexpr iterator end() const noexcept { return data() + size(); }

    constexpr const_iterator cbegin() const noexcept { return begin(); }

    constexpr const_iterator cend() const noexcept { return end(); }

    GUL_SPAN_ARRAY_CONSTEXPR reverse_iterator rbegin() const noexcept
    {
        return reverse_iterator(end());
    }

    GUL_SPAN_ARRAY_CONSTEXPR reverse_iterator rend() const noexcept
    {
        return reverse_iterator(begin());
    }

    GUL_SPAN_ARRAY_CONSTEXPR const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(cend());
    }

    GUL_SPAN_ARRAY_CONSTEXPR const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(cbegin());
    }

    friend constexpr iterator begin(span s) noexcept { return s.begin(); }

    friend constexpr iterator end(span s) noexcept { return s.end(); }

private:
    storage_type storage_{};

/// \}

};

/// \cond HIDE_SYMBOLS

#ifdef GUL_SPAN_HAVE_DEDUCTION_GUIDES

/* Deduction Guides */
template <class T, size_t N>
span(T (&)[N])->span<T, N>;

template <class T, size_t N>
span(std::array<T, N>&)->span<T, N>;

template <class T, size_t N>
span(const std::array<T, N>&)->span<const T, N>;

template <class Container>
span(Container&)->span<typename Container::value_type>;

template <class Container>
span(const Container&)->span<const typename Container::value_type>;

#endif // GUL_HAVE_DEDUCTION_GUIDES

/// \endcond

/**
 * Create a span from another span.
 *
 * This is a backport from the C++20 standard library, see:
 * https://en.cppreference.com/w/cpp/container/span/as_bytes
 */
template <typename ElementType, std::size_t Extent>
constexpr span<ElementType, Extent>
make_span(span<ElementType, Extent> s) noexcept
{
    return s;
}

/**
 * Create a span from a fixed-size array.
 *
 * This is a backport from the C++20 standard library, see:
 * https://en.cppreference.com/w/cpp/container/span/as_bytes
 */
template <typename T, std::size_t N>
constexpr span<T, N> make_span(T (&arr)[N]) noexcept
{
    return {arr};
}

/**
 * Create a span from a std::array.
 *
 * This is a backport from the C++20 standard library, see:
 * https://en.cppreference.com/w/cpp/container/span/as_bytes
 */
template <typename T, std::size_t N>
GUL_SPAN_ARRAY_CONSTEXPR span<T, N> make_span(std::array<T, N>& arr) noexcept
{
    return {arr};
}

/**
 * Create a span from a const std::array.
 *
 * This is a backport from the C++20 standard library, see:
 * https://en.cppreference.com/w/cpp/container/span/as_bytes
 */
template <typename T, std::size_t N>
GUL_SPAN_ARRAY_CONSTEXPR span<const T, N>
make_span(const std::array<T, N>& arr) noexcept
{
    return {arr};
}

/**
 * Create a span from a container.
 *
 * This is a backport from the C++20 standard library, see:
 * https://en.cppreference.com/w/cpp/container/span/as_bytes
 */
template <typename Container>
constexpr span<typename Container::value_type> make_span(Container& cont)
{
    return {cont};
}

/**
 * Create a span from a const container.
 *
 * This is a backport from the C++20 standard library, see:
 * https://en.cppreference.com/w/cpp/container/span/as_bytes
 */
template <typename Container>
constexpr span<const typename Container::value_type>
make_span(const Container& cont)
{
    return {cont};
}

/// \cond HIDE_SYMBOLS

/* Comparison operators */
// Implementation note: the implementations of == and < are equivalent to
// 4-legged std::equal and std::lexicographical_compare respectively

template <typename T, std::size_t X, typename U, std::size_t Y>
constexpr bool operator==(span<T, X> lhs, span<U, Y> rhs)
{
    if (lhs.size() != rhs.size()) {
        return false;
    }

    for (std::size_t i = 0; i < lhs.size(); i++) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }

    return true;
}

template <typename T, std::size_t X, typename U, std::size_t Y>
constexpr bool operator!=(span<T, X> lhs, span<U, Y> rhs)
{
    return !(lhs == rhs);
}

template <typename T, std::size_t X, typename U, std::size_t Y>
constexpr bool operator<(span<T, X> lhs, span<U, Y> rhs)
{
    // No std::min to avoid dragging in <algorithm>
    const std::size_t size = lhs.size() < rhs.size() ? lhs.size() : rhs.size();

    for (std::size_t i = 0; i < size; i++) {
        if (lhs[i] < rhs[i]) {
            return true;
        }
        if (lhs[i] > rhs[i]) {
            return false;
        }
    }
    return lhs.size() < rhs.size();
}

template <typename T, std::size_t X, typename U, std::size_t Y>
constexpr bool operator<=(span<T, X> lhs, span<U, Y> rhs)
{
    return !(rhs < lhs);
}

template <typename T, std::size_t X, typename U, std::size_t Y>
constexpr bool operator>(span<T, X> lhs, span<U, Y> rhs)
{
    return rhs < lhs;
}

template <typename T, std::size_t X, typename U, std::size_t Y>
constexpr bool operator>=(span<T, X> lhs, span<U, Y> rhs)
{
    return !(lhs < rhs);
}

/// \endcond

/**
 * Return a constant view to the byte representation of the elements of a given span.
 * 
 * This is a backport from the C++20 standard library, see:
 * https://en.cppreference.com/w/cpp/container/span/as_bytes
 */
template <typename ElementType, std::size_t Extent>
span<const byte, ((Extent == dynamic_extent) ? dynamic_extent
                                             : sizeof(ElementType) * Extent)>
as_bytes(span<ElementType, Extent> s) noexcept
{
    return {reinterpret_cast<const byte*>(s.data()), s.size_bytes()};
}

/**
 * Return a writable view to the byte representation of the elements of a given span.
 *
 * This is a backport from the C++20 standard library, see:
 * https://en.cppreference.com/w/cpp/container/span/as_bytes
 */
template <
    class ElementType, size_t Extent,
    typename std::enable_if<!std::is_const<ElementType>::value, int>::type = 0>
span<byte, ((Extent == dynamic_extent) ? dynamic_extent
                                       : sizeof(ElementType) * Extent)>
as_writable_bytes(span<ElementType, Extent> s) noexcept
{
    return {reinterpret_cast<byte*>(s.data()), s.size_bytes()};
}

/**
 * Return a reference to the Nth element of a given span.
 *
 * This is a backport from the C++20 standard library, see:
 * https://en.cppreference.com/w/cpp/container/span/get
 */
template <std::size_t N, typename E, std::size_t S>
constexpr auto get(span<E, S> s) -> decltype(s[N])
{
    return s[N];
}

} // namespace gul


namespace std {

template <typename ElementType, size_t Extent>
class tuple_size<gul::span<ElementType, Extent>>
    : public integral_constant<size_t, Extent> {};

template <typename ElementType>
class tuple_size<gul::span<ElementType, gul::dynamic_extent>>; // not defined

template <size_t I, typename ElementType, size_t Extent>
class tuple_element<I, gul::span<ElementType, Extent>> {
public:
    static_assert(Extent != gul::dynamic_extent && I < Extent, "");
    using type = ElementType;
};

} // end namespace std

// vi:ts=4:sw=4:sts=4:et
