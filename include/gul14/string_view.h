/**
 * \file   string_view.h
 * \author Marshall Clow, Beman Dawes, \ref contributors
 * \brief  Provides a gul14::string_view that is fully compatible with C++17's
 *         std::string_view.
 *
 * \copyright
 * Copyright Marshall Clow 2012-2015
 * Copyright Beman Dawes 2015
 * Copyright \ref contributors 2018-2022 (modifications for GUL)
 *
 * Distributed under the Boost Software License, Version 1.0. (See \ref license_boost_1_0
 * or http://www.boost.org/LICENSE_1_0.txt)
 *
 * For more information, see http://www.boost.org
 *
 * Based on the StringRef implementation in LLVM (http://llvm.org) and
 * N3422 by Jeffrey Yasskin
 *   http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3442.html
 * Updated July 2015 to reflect the Library Fundamentals TS
 *   http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4480.html
 */

#ifndef GUL14_STRING_VIEW_H_
#define GUL14_STRING_VIEW_H_

#include <cstddef>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <string>
#include <cstring>
#include <iosfwd>
#include "gul14/internal.h"

namespace gul14 {

namespace detail {

//  A helper functor because sometimes we don't have lambdas
template <typename charT, typename traits>
class string_view_traits_eq {
public:
    string_view_traits_eq ( charT ch ) : ch_(ch) {}
    bool operator()( charT val ) const { return traits::eq (ch_, val); }
    charT ch_;
};

} // namespace detail

// We use custom traits to allow constexpr operations on string_view
template <typename charT, typename BaseT = std::char_traits<charT>>
struct char_traits : public BaseT
{
    using char_type = typename BaseT::char_type;
    using int_type = typename BaseT::int_type;
    using pos_type = typename BaseT::pos_type;
    using off_type = typename BaseT::off_type;
    using state_type = typename BaseT::state_type;

    using base_traits = BaseT;

    using BaseT::lt;
    using BaseT::eq;

    static constexpr void assign(char_type& c1, const char_type& c2) noexcept
    {
        c1 = c2;
    }

    static constexpr int compare(const char_type* s1, const char_type* s2, size_t n) noexcept
    {
        for (;n > 0; --n, ++s1, ++s2) {
            if (lt(*s1, *s2))
                return -1;
            else if (lt(*s2, *s1))
                return 1;
        }
        return 0;
    }

    static constexpr size_t length(const char_type* s) noexcept
    {
        auto len = std::size_t{ };
        while (!eq(s[len], char_type{ }))
            ++len;
        return len;
    }

    static constexpr const char_type* find(const char_type* s, std::size_t n, const char_type& a) noexcept
    {
        for (;n > 0; --n, ++s) {
            if (eq(*s, a))
                return s;
        }
        return nullptr;
    }
};


/**
 * A view to a contiguous sequence of chars or char-like objects. This is a backport of
 * [std::string_view](https://en.cppreference.com/w/cpp/string/basic_string_view)
 * from libc++ for C++17.
 */
template<typename charT, typename traits = gul14::char_traits<charT>>
class basic_string_view {
public:
    // types
    typedef traits                                traits_type;
    typedef charT                                 value_type;
    typedef charT*                                pointer;
    typedef const charT*                          const_pointer;
    typedef charT&                                reference;
    typedef const charT&                          const_reference;
    typedef const_pointer                         const_iterator; // impl-defined
    typedef const_iterator                        iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef const_reverse_iterator                reverse_iterator;
    typedef std::size_t                           size_type;
    typedef std::ptrdiff_t                        difference_type;
    static constexpr size_type     npos = size_type(-1);

    // construct/copy
    constexpr basic_string_view() noexcept
            : ptr_(NULL), len_(0) {}

    // by defaulting these functions, basic_string_ref becomes
    //  trivially copy/move constructible.
    constexpr basic_string_view(const basic_string_view &rhs) noexcept = default;

    basic_string_view& operator=(const basic_string_view &rhs) noexcept = default;

    template<typename Allocator, typename Traits>
    basic_string_view(const std::basic_string<charT, Traits,
            Allocator>& str) noexcept
            : ptr_(str.data()), len_(str.length()) {}

    constexpr basic_string_view(const charT* str)
            : ptr_(str), len_(traits::length(str)) {}

    constexpr basic_string_view(const charT* str, size_type len)
            : ptr_(str), len_(len) {}

    // iterators
    constexpr const_iterator   begin() const noexcept { return ptr_; }
    constexpr const_iterator  cbegin() const noexcept { return ptr_; }
    constexpr const_iterator     end() const noexcept { return ptr_ + len_; }
    constexpr const_iterator    cend() const noexcept { return ptr_ + len_; }
    const_reverse_iterator  rbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator    rend() const noexcept { return const_reverse_iterator(begin()); }
    const_reverse_iterator   crend() const noexcept { return const_reverse_iterator(begin()); }

    // capacity
    constexpr size_type size()     const noexcept { return len_; }
    constexpr size_type length()   const noexcept { return len_; }
    constexpr size_type max_size() const noexcept { return len_; }
    constexpr bool empty()         const noexcept { return len_ == 0; }

    // element access
    constexpr const_reference operator[](size_type pos) const noexcept { return ptr_[pos]; }

    constexpr const_reference at(size_t pos) const {
        return pos >= len_ ?  throw std::out_of_range("gul14::string_view::at") : ptr_[pos];
    }

    constexpr const_reference front() const                { return ptr_[0]; }
    constexpr const_reference back()  const                { return ptr_[len_-1]; }
    constexpr const_pointer data()    const noexcept { return ptr_; }

    // modifiers
    constexpr void remove_prefix(size_type n) {
        if ( n > len_ )
            n = len_;
        ptr_ += n;
        len_ -= n;
    }

    constexpr void remove_suffix(size_type n) {
        if ( n > len_ )
            n = len_;
        len_ -= n;
    }

    constexpr void swap(basic_string_view& s) noexcept {
        std::swap(ptr_, s.ptr_);
        std::swap(len_, s.len_);
    }

    // basic_string_view string operations
    template<typename Allocator>
    explicit operator std::basic_string<charT, typename traits::base_traits, Allocator>() const {
        return std::basic_string<charT, typename traits::base_traits, Allocator>(begin(), end());
    }

    size_type copy(charT* s, size_type n, size_type pos=0) const {
        if (pos > size())
            throw std::out_of_range("string_view::copy" );
        size_type rlen = (std::min)(n, len_ - pos);
        // use std::copy(begin() + pos, begin() + pos + rlen, s) rather than
        // std::copy_n(begin() + pos, rlen, s) to support pre-C++11 standard libraries
        std::copy(begin() + pos, begin() + pos + rlen, s);
        return rlen;
    }

    constexpr basic_string_view substr(size_type pos, size_type n=npos) const {
        if ( pos > size())
            throw std::out_of_range ( "string_view::substr" );
        if (n == npos || pos + n > size())
            n = size () - pos;
        return basic_string_view(data() + pos, n);
    }

    constexpr int compare(basic_string_view x) const noexcept {
        const int cmp = traits::compare(ptr_, x.ptr_, (std::min)(len_, x.len_));
        return cmp != 0 ? cmp : (len_ == x.len_ ? 0 : len_ < x.len_ ? -1 : 1);
    }

    constexpr int compare(size_type pos1, size_type n1, basic_string_view x)
    const noexcept {
        return substr(pos1, n1).compare(x);
    }

    constexpr int compare(size_type pos1, size_type n1,
                                      basic_string_view x, size_type pos2, size_type n2) const {
        return substr(pos1, n1).compare(x.substr(pos2, n2));
    }

    constexpr int compare(const charT* x) const {
        return compare(basic_string_view(x));
    }

    constexpr int compare(size_type pos1, size_type n1, const charT* x) const {
        return substr(pos1, n1).compare(basic_string_view(x));
    }

    constexpr int compare(size_type pos1, size_type n1,
                                      const charT* x, size_type n2) const {
        return substr(pos1, n1).compare(basic_string_view(x, n2));
    }

    //  find
    constexpr size_type find(basic_string_view s, size_type pos = 0) const noexcept {
        if (pos > size())
            return npos;
        if (s.empty())
            return pos;
        if (len_ < s.len_)
            return npos;
        auto const end = len_ - s.len_;
        for (auto cur = pos; cur <= end; ++cur) {
            if (traits::compare(ptr_ + cur, s.ptr_, s.len_) == 0)
                return cur;
        }
        return npos;
    }
    constexpr size_type find(charT c, size_type pos = 0) const noexcept
    { return find(basic_string_view(&c, 1), pos); }
    constexpr size_type find(const charT* s, size_type pos, size_type n) const noexcept
    { return find(basic_string_view(s, n), pos); }
    constexpr size_type find(const charT* s, size_type pos = 0) const noexcept
    { return find(basic_string_view(s), pos); }

    //  rfind
    constexpr size_type rfind(basic_string_view s, size_type pos = npos) const noexcept {
        if (len_ < s.len_)
            return npos;
        if (pos > len_ - s.len_)
            pos = len_ - s.len_;
        if (s.len_ == 0u)     // an empty string is always found
            return pos;
        for (const charT* cur = ptr_ + pos; ; --cur) {
            if (traits::compare(cur, s.ptr_, s.len_) == 0)
                return cur - ptr_;
            if (cur == ptr_)
                return npos;
        };
    }
    constexpr size_type rfind(charT c, size_type pos = npos) const noexcept
    { return rfind(basic_string_view(&c, 1), pos); }
    constexpr size_type rfind(const charT* s, size_type pos, size_type n) const noexcept
    { return rfind(basic_string_view(s, n), pos); }
    constexpr size_type rfind(const charT* s, size_type pos = npos) const noexcept
    { return rfind(basic_string_view(s), pos); }

    //  find_first_of
    constexpr size_type find_first_of(basic_string_view s, size_type pos = 0) const noexcept {
        if (pos >= len_ || s.len_ == 0)
            return npos;
        const_iterator iter = std::find_first_of
                (this->cbegin () + pos, this->cend (), s.cbegin (), s.cend (), traits::eq);
        return iter == this->cend () ? npos : std::distance ( this->cbegin (), iter );
    }
    constexpr size_type find_first_of(charT c, size_type pos = 0) const noexcept
    { return find_first_of(basic_string_view(&c, 1), pos); }
    constexpr size_type find_first_of(const charT* s, size_type pos, size_type n) const noexcept
    { return find_first_of(basic_string_view(s, n), pos); }
    constexpr size_type find_first_of(const charT* s, size_type pos = 0) const noexcept
    { return find_first_of(basic_string_view(s), pos); }

    //  find_last_of
    constexpr size_type find_last_of(basic_string_view s, size_type pos = npos) const noexcept {
        if (s.len_ == 0u)
            return npos;
        if (pos >= len_)
            pos = 0;
        else
            pos = len_ - (pos+1);
        const_reverse_iterator iter = std::find_first_of
                ( this->crbegin () + pos, this->crend (), s.cbegin (), s.cend (), traits::eq );
        return iter == this->crend () ? npos : reverse_distance ( this->crbegin (), iter);
    }
    constexpr size_type find_last_of(charT c, size_type pos = npos) const noexcept
    { return find_last_of(basic_string_view(&c, 1), pos); }
    constexpr size_type find_last_of(const charT* s, size_type pos, size_type n) const noexcept
    { return find_last_of(basic_string_view(s, n), pos); }
    constexpr size_type find_last_of(const charT* s, size_type pos = npos) const noexcept
    { return find_last_of(basic_string_view(s), pos); }

    //  find_first_not_of
    constexpr size_type find_first_not_of(basic_string_view s, size_type pos = 0) const noexcept {
        if (pos >= len_)
            return npos;
        if (s.len_ == 0)
            return pos;
        const_iterator iter = find_not_of ( this->cbegin () + pos, this->cend (), s );
        return iter == this->cend () ? npos : std::distance ( this->cbegin (), iter );
    }
    constexpr size_type find_first_not_of(charT c, size_type pos = 0) const noexcept
    { return find_first_not_of(basic_string_view(&c, 1), pos); }
    constexpr size_type find_first_not_of(const charT* s, size_type pos, size_type n) const noexcept
    { return find_first_not_of(basic_string_view(s, n), pos); }
    constexpr size_type find_first_not_of(const charT* s, size_type pos = 0) const noexcept
    { return find_first_not_of(basic_string_view(s), pos); }

    //  find_last_not_of
    constexpr size_type find_last_not_of(basic_string_view s, size_type pos = npos) const noexcept {
        if (pos >= len_)
            pos = len_ - 1;
        if (s.len_ == 0u)
            return pos;
        pos = len_ - (pos+1);
        const_reverse_iterator iter = find_not_of ( this->crbegin () + pos, this->crend (), s );
        return iter == this->crend () ? npos : reverse_distance ( this->crbegin (), iter );
    }
    constexpr size_type find_last_not_of(charT c, size_type pos = npos) const noexcept
    { return find_last_not_of(basic_string_view(&c, 1), pos); }
    constexpr size_type find_last_not_of(const charT* s, size_type pos, size_type n) const noexcept
    { return find_last_not_of(basic_string_view(s, n), pos); }
    constexpr size_type find_last_not_of(const charT* s, size_type pos = npos) const noexcept
    { return find_last_not_of(basic_string_view(s), pos); }

private:
    template <typename r_iter>
    size_type reverse_distance(r_iter first, r_iter last) const noexcept {
        // Portability note here: std::distance is not NOEXCEPT, but calling it with a string_view::reverse_iterator will not throw.
        return len_ - 1 - std::distance ( first, last );
    }

    template <typename Iterator>
    Iterator find_not_of(Iterator first, Iterator last, basic_string_view s) const noexcept {
        for (; first != last ; ++first)
            if ( 0 == traits::find(s.ptr_, s.len_, *first))
                return first;
        return last;
    }

    const charT *ptr_;
    std::size_t len_;
};

/// \cond HIDE_SYMBOLS

//  Comparison operators
//  Equality
template<typename charT, typename traits>
inline bool operator==(basic_string_view<charT, traits> x,
                       basic_string_view<charT, traits> y) noexcept {
    if (x.size () != y.size ()) return false;
    return x.compare(y) == 0;
}

//  Inequality
template<typename charT, typename traits>
inline bool operator!=(basic_string_view<charT, traits> x,
                       basic_string_view<charT, traits> y) noexcept {
    if ( x.size () != y.size ()) return true;
    return x.compare(y) != 0;
}

//  Less than
template<typename charT, typename traits>
inline bool operator<(basic_string_view<charT, traits> x,
                      basic_string_view<charT, traits> y) noexcept {
    return x.compare(y) < 0;
}

//  Greater than
template<typename charT, typename traits>
inline bool operator>(basic_string_view<charT, traits> x,
                      basic_string_view<charT, traits> y) noexcept {
    return x.compare(y) > 0;
}

//  Less than or equal to
template<typename charT, typename traits>
inline bool operator<=(basic_string_view<charT, traits> x,
                       basic_string_view<charT, traits> y) noexcept {
    return x.compare(y) <= 0;
}

//  Greater than or equal to
template<typename charT, typename traits>
inline bool operator>=(basic_string_view<charT, traits> x,
                       basic_string_view<charT, traits> y) noexcept {
    return x.compare(y) >= 0;
}

// "sufficient additional overloads of comparison functions"
template<typename charT, typename traits, typename Allocator>
inline bool operator==(basic_string_view<charT, traits> x,
                       const std::basic_string<charT, typename char_traits<charT>::base_traits, Allocator> & y) noexcept {
    return x == basic_string_view<charT, traits>(y);
}

template<typename charT, typename traits, typename Allocator>
inline bool operator==(const std::basic_string<charT, typename char_traits<charT>::base_traits, Allocator> & x,
                       basic_string_view<charT, traits> y) noexcept {
    return basic_string_view<charT, traits>(x) == y;
}

template<typename charT, typename traits>
inline bool operator==(basic_string_view<charT, traits> x,
                       const charT * y) noexcept {
    return x == basic_string_view<charT, traits>(y);
}

template<typename charT, typename traits>
inline bool operator==(const charT * x,
                       basic_string_view<charT, traits> y) noexcept {
    return basic_string_view<charT, traits>(x) == y;
}

template<typename charT, typename traits, typename Allocator>
inline bool operator!=(basic_string_view<charT, traits> x,
                       const std::basic_string<charT, typename char_traits<charT>::base_traits, Allocator> & y) noexcept {
    return x != basic_string_view<charT, traits>(y);
}

template<typename charT, typename traits, typename Allocator>
inline bool operator!=(const std::basic_string<charT, typename char_traits<charT>::base_traits, Allocator> & x,
                       basic_string_view<charT, traits> y) noexcept {
    return basic_string_view<charT, traits>(x) != y;
}

template<typename charT, typename traits>
inline bool operator!=(basic_string_view<charT, traits> x,
                       const charT * y) noexcept {
    return x != basic_string_view<charT, traits>(y);
}

template<typename charT, typename traits>
inline bool operator!=(const charT * x,
                       basic_string_view<charT, traits> y) noexcept {
    return basic_string_view<charT, traits>(x) != y;
}

template<typename charT, typename traits, typename Allocator>
inline bool operator<(basic_string_view<charT, traits> x,
                      const std::basic_string<charT, typename char_traits<charT>::base_traits, Allocator> & y) noexcept {
    return x < basic_string_view<charT, traits>(y);
}

template<typename charT, typename traits, typename Allocator>
inline bool operator<(const std::basic_string<charT, typename char_traits<charT>::base_traits, Allocator> & x,
                      basic_string_view<charT, traits> y) noexcept {
    return basic_string_view<charT, traits>(x) < y;
}

template<typename charT, typename traits>
inline bool operator<(basic_string_view<charT, traits> x,
                      const charT * y) noexcept {
    return x < basic_string_view<charT, traits>(y);
}

template<typename charT, typename traits>
inline bool operator<(const charT * x,
                      basic_string_view<charT, traits> y) noexcept {
    return basic_string_view<charT, traits>(x) < y;
}

template<typename charT, typename traits, typename Allocator>
inline bool operator>(basic_string_view<charT, traits> x,
                      const std::basic_string<charT, typename char_traits<charT>::base_traits, Allocator> & y) noexcept {
    return x > basic_string_view<charT, traits>(y);
}

template<typename charT, typename traits, typename Allocator>
inline bool operator>(const std::basic_string<charT, typename char_traits<charT>::base_traits, Allocator> & x,
                      basic_string_view<charT, traits> y) noexcept {
    return basic_string_view<charT, traits>(x) > y;
}

template<typename charT, typename traits>
inline bool operator>(basic_string_view<charT, traits> x,
                      const charT * y) noexcept {
    return x > basic_string_view<charT, traits>(y);
}

template<typename charT, typename traits>
inline bool operator>(const charT * x,
                      basic_string_view<charT, traits> y) noexcept {
    return basic_string_view<charT, traits>(x) > y;
}

template<typename charT, typename traits, typename Allocator>
inline bool operator<=(basic_string_view<charT, traits> x,
                       const std::basic_string<charT, typename char_traits<charT>::base_traits, Allocator> & y) noexcept {
    return x <= basic_string_view<charT, traits>(y);
}

template<typename charT, typename traits, typename Allocator>
inline bool operator<=(const std::basic_string<charT, typename char_traits<charT>::base_traits, Allocator> & x,
                       basic_string_view<charT, traits> y) noexcept {
    return basic_string_view<charT, traits>(x) <= y;
}

template<typename charT, typename traits>
inline bool operator<=(basic_string_view<charT, traits> x,
                       const charT * y) noexcept {
    return x <= basic_string_view<charT, traits>(y);
}

template<typename charT, typename traits>
inline bool operator<=(const charT * x,
                       basic_string_view<charT, traits> y) noexcept {
    return basic_string_view<charT, traits>(x) <= y;
}

template<typename charT, typename traits, typename Allocator>
inline bool operator>=(basic_string_view<charT, traits> x,
                       const std::basic_string<charT, typename char_traits<charT>::base_traits, Allocator> & y) noexcept {
    return x >= basic_string_view<charT, traits>(y);
}

template<typename charT, typename traits, typename Allocator>
inline bool operator>=(const std::basic_string<charT, typename char_traits<charT>::base_traits, Allocator> & x,
                       basic_string_view<charT, traits> y) noexcept {
    return basic_string_view<charT, traits>(x) >= y;
}

template<typename charT, typename traits>
inline bool operator>=(basic_string_view<charT, traits> x,
                       const charT * y) noexcept {
    return x >= basic_string_view<charT, traits>(y);
}

template<typename charT, typename traits>
inline bool operator>=(const charT * x,
                       basic_string_view<charT, traits> y) noexcept {
    return basic_string_view<charT, traits>(x) >= y;
}

namespace detail {

template<class charT, class traits>
inline void sv_insert_fill_chars(std::basic_ostream<charT, traits>& os, std::size_t n) {
    enum { chunk_size = 8 };
    charT fill_chars[chunk_size];
    std::fill_n(fill_chars, static_cast< std::size_t >(chunk_size), os.fill());
    for (; n >= chunk_size && os.good(); n -= chunk_size)
        os.write(fill_chars, static_cast< std::size_t >(chunk_size));
    if (n > 0 && os.good())
        os.write(fill_chars, n);
}

template<class charT, class traits>
void sv_insert_aligned(std::basic_ostream<charT, typename char_traits<charT>::base_traits>& os, const basic_string_view<charT,traits>& str) {
    const std::size_t size = str.size();
    const std::size_t alignment_size = static_cast< std::size_t >(os.width()) - size;
    const bool align_left = (os.flags() & std::basic_ostream<charT, typename char_traits<charT>::base_traits>::adjustfield)
            == std::basic_ostream<charT, typename char_traits<charT>::base_traits>::left;
    if (!align_left) {
        detail::sv_insert_fill_chars(os, alignment_size);
        if (os.good())
            os.write(str.data(), size);
    }
    else {
        os.write(str.data(), size);
        if (os.good())
            detail::sv_insert_fill_chars(os, alignment_size);
    }
}

} // namespace detail

// Inserter
template<class charT, class traits>
inline std::basic_ostream<charT, typename char_traits<charT>::base_traits>&
operator<<(std::basic_ostream<charT, typename char_traits<charT>::base_traits>& os,
           const basic_string_view<charT,traits>& str) {
    if (os.good()) {
        const std::size_t size = str.size();
        const std::size_t w = static_cast< std::size_t >(os.width());
        if (w <= size)
            os.write(str.data(), size);
        else
            detail::sv_insert_aligned(os, str);
        os.width(0);
    }
    return os;
}

/// \endcond


/**
 * A view to a contiguous sequence of chars. This is a backport of
 * [std::string_view](https://en.cppreference.com/w/cpp/string/basic_string_view)
 * from libc++ for C++17.
 */
using string_view = basic_string_view<char>;

/**
 * A view to a contiguous sequence of char16_ts. This is a backport of
 * [std::string_view](https://en.cppreference.com/w/cpp/string/basic_string_view)
 * from libc++ for C++17.
 */
using u16string_view = basic_string_view<char16_t>;

/**
 * A view to a contiguous sequence of char32_ts. This is a backport of
 * [std::string_view](https://en.cppreference.com/w/cpp/string/basic_string_view)
 * from libc++ for C++17.
 */
using u32string_view = basic_string_view<char32_t>;

/**
 * A view to a contiguous sequence of wchar_ts. This is a backport of
 * [std::string_view](https://en.cppreference.com/w/cpp/string/basic_string_view)
 * from libc++ for C++17.
 */
using wstring_view = basic_string_view<wchar_t>;



// Custom extensions (Lars Froehlich)

/// \cond HIDE_SYMBOLS

inline std::string &operator+=(std::string &a, string_view b)
{
    a.append(b.begin(), b.end());
    return a;
}

inline std::string operator+(std::string a, string_view b)
{
    a += b;
    return a;
}

inline std::string operator+(string_view a, std::string b)
{
    b.insert(b.begin(), a.begin(), a.end());
    return b;
}

/// \endcond

} // namespace gul14

#endif
