/**
 * \file    SlidingBuffer.h
 * \authors \ref contributors
 * \date    Created on Feb 7, 2019
 * \brief   Declaration of the SlidingBuffer class for the General Utility Library.
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

#include <array>
#include <vector>

namespace gul {

/**
 * A simple data buffer with a (semi) fixed size to use as sliding window on a data stream
 *
 * The SlidingBuffer augments a std::array (or a std::vector) with very few extra functions
 * to make it possible to use it as sliding window on some incoming data.
 *
 * The SlidingBuffer shares some properties with ring buffers: It has a fixed maximum size
 * and incoming elements are easily appended. But there is no way to pop elements out of
 * the buffer. They will drop out automatically if the capacity is not sufficient to hold
 * all encountered incoming elements.
 *
 * This buffer is not intended for producer-consumer problems. If the elements access is
 * atomic it is thread-safe though. All auxiliary functions that mutate the data are not
 * thread safe.
 *
 * Differing from arrays this buffer will start empty (with a size of zero).
 * Once the buffer has grown to its designated size, it will never change in size again,
 * except when 'clear'ed.
 *
 * A typical application would be to analyze an incoming stream of elements in a finite
 * impulse response filter.
 *
 * The following lists the differences to the std::array interface. All interface
 * items not mentioned here access the underlying std::array resp. std::vector
 * interface. Be careful if you use members not mentioned here.
 *
 * \code
 * Member types:
 *   value_type          Type of the elements
 *   container_type      Type of the underlying container (i.e. std::array<value_type, ..>)
 *   size_type           Unsigned integer type (usually std::size_t)
 *
 * Member functions:
 *     SlidingBuffer     Constructor
 *   Element access:
 *     push_front        Insert an element into the buffer
 *     operator[]        Access element relative to most recent element in buffer, bounds corrected, read only
 *     front             Access first (most recently pushed in) element (i.e. [0]
 *     back              Access last (next to be pushed out) element (i.e. [size() - 1]
 *   Iterators:
 *     begin, cbegin     Returns iterator to first element in underlying container
 *     end, cend         Returns iterator to end of used space in the underlying container
 *     rbegin, crbegin   Returns reverse iterator to beginning of used space in the reversed underlying container
 *     rend, crend       Returns reverse iterator to the end of the reversed underlying container
 *   Capacity:
 *     size              Returns number of used elements
 *     capacity          Returns maximum number of elements
 *     filled            Checks whether the buffer is completely filled
 *     resize            Change the maximum number of elements (only for std::vector based buffers)
 *     reserve           Change the maximum number of elements (only for std::vector based buffers)
 *   Modifiers:
 *     clear             Empty the buffer
 *
 * Non-member functions:
 *   operator<<          Dump the raw data of the buffer to an ostream
 * \endcode
 *
 * Only the member functions ``operator[]()``, ``front()``, and ``back()`` are aware of the
 * wrapping nature of the sliding buffer and access elements relative to when they were
 * pushed in.
 * The other member functions access the underlying container without reordering, so the
 * elements are accessed in unknown order by
 * * all of the iterators ``begin()``, ``end()``, ``rbegin()``, ``rend()``, ...
 * * range based ``for`` (as this uses ``begin()`` and ``end()``)
 *
 * The sliding buffer can be instantiated in two different underlying container versions:
 * * If the size is known at compile time, instanziate it with that number as BufferSize. The
 *   underlying container will be a std::array.
 * * If a flexible size is desired, omit the template parameter BufferSize. It will be
 *   defaulted to zero (0). The underlying container will then
 *   be a std::vector. You need to use a constructor that sets a certain capacity or set the
 *   capacity afterwards with resize(). If the size is really 0 elements the buffer is unusable.
 * Apart from the ability of the later to resize()/reserve() all
 * is identical.
 *
 * \tparam ElemenT       Type of elements in the buffer
 * \tparam BufferSize    Maximum number of elements in the buffer, zero if unspecified
 * \tparam Container     Type of the underlying container, usually not specified
 *
 */
template<typename ElementT, std::size_t BufferSize = 0u,
    typename Container = typename std::conditional<(BufferSize >= 1u),
        std::array<ElementT, BufferSize>,
        std::vector<ElementT>>::type
>
class SlidingBuffer {
public:
    /// Type of the underlying container (e.g. std::array<value_type, ..>)
    using container_type = Container;
    /// Type of the elements in the underlying container
    using value_type = ElementT;
    /// Unsigned integer type (usually std::size_t)
    using size_type = typename Container::size_type;

    /**
     * Constructs a new sliding buffer.
     *
     * The buffer is initially empty
     * The capacity is given by the BufferSize template parameter.
     *
     * If that template argument is not zero a std::array based SlidingBuffer
     * with that (unchangeable) capacity is created.
     *
     * If it is zero a std::vector based SlidingBuffer is generated with a capacity
     * that is zero elements. Use the other constructor or resize() afterwards to
     * get a std::vector bases SlidingBuffer with capacity.
     */
    SlidingBuffer() = default;

    /**
     * \overload
     *
     * Only available for sliding buffers that base on std::vector.
     *
     * Constructs a sliding buffer with a specified capacity.
     *
     * For std::array based sliding buffers the capacity is specified by the BufferSize template parameter.
     */
    SlidingBuffer(size_type count) : storage_(count) {}

private:
    size_type next_element_{ 0u };
    bool full_{ false };
protected:
    Container storage_{ }; ///< Actual data is stored here, the underlying container.

public:

    /**
     * Insert one element item into the buffer.
     *
     * Think of this as inserting in the front. Probably an element at the back is
     * dropped to make room in the fixed size buffer.
     */
    auto push_front(value_type&& in) -> void
    {
        storage_[next_element_] = std::move(in);
        ++next_element_;
        if (next_element_ >= capacity()) {
            next_element_ = 0;
            full_ = true;
        }
    }

    /**
     * \overload
     */
    auto push_front(const value_type& in) -> void
    {
        auto in_copy = in;
        push_back(std::move(in_copy));
    }

    /**
     * Access (read) one element in the buffer, relative to the most recently `push`ed
     * element.
     *
     * The index 0 is the most recent element, 1 is the element before that
     * and so on.
     *
     * If the buffer is not yet full it may be possible that the function has nothing to
     * return and so a default constructed Element is returned.
     *
     * Access to elements outside the capacity is not allowed and results in undefined
     * behavior.
     *
     */
    auto operator[] (size_type i) const -> const value_type&
    {
        ++i;
        auto offset = next_element_;
        if (offset < i)
            offset += capacity();
        offset -= i;

        return storage_[offset];
    }

    /**
     * Access (read) one element in the buffer, relative to the most recently `push`ed
     * element.
     *
     * The index 0 is the most recent element, 1 is the element before that
     * and so on.
     *
     * `i` is coerced to be inside the size of the buffer, wrapping around by the buffer size.
     *
     * If the buffer is not yet full it may be possible that the function has nothing to
     * return and so a default constructed Element is returned.
     */
    auto at(const size_type i) const -> const value_type&
    {
        const auto capa = capacity();
        const size_type idx = next_element_ - 1 + capa - i;
        // If the element has ever been filled or not is ignored. A default
        // constructed ELEMENT will be returned on unset elements
        return storage_[idx % capa];
    }

    /**
     * Return the youngest / most recent ('left most') Element in the buffer.
     *
     * This is a read-only operation.
     */
    auto front() const noexcept -> const value_type&
    {
        return operator[](0);
    }

    /**
     * Return the oldest ('right most') Element in the buffer.
     *
     * This is a read-only operation.
     */
    auto back() const noexcept -> const value_type&
    {
        return storage_[next_element_];
    }

    /**
     * Return the number of elements in the container, i.e. std::distance(begin(), end()).
     *
     * In the startup phase it can be 0 and up to the BufferSize, after startup (filled() == true)
     * it will always return BufferSize (capacity()).
     */
    auto size() const noexcept -> size_type
    {
        if (full_)
            return capacity();
        return next_element_;
    }

    /**
     * Return the maximum possible number of elements in the container.
     *
     * If the underlying type is std::vector, its size is returned, not
     * its capacity, because only the active elements (i.e. size) participate
     * in the sliding buffer.
     */
    auto constexpr capacity() const noexcept -> size_type
    {
        return BufferSize ? BufferSize : storage_.size();
    }

    /**
     * Return true if the buffer is completely filled with elements.
     *
     * If the buffer is used in filter contexts this means the filter is fully
     * initialized and working.
     */
    auto filled() const noexcept -> bool
    {
        return full_;
    }

    /**
     * Empty the buffer.
     *
     * Its size() will be zero afterwards.
     */
    auto clear() -> void
    {
        full_ = false;
        next_element_ = 0u;
        // Fill with new empty elements to possibly trigger RAII in the elements
        fill(value_type{});
    }

    /**
     * Resize the container.
     *
     * Only possible if the underlying container is a std::vector.
     *
     * Shrinking: The oldes excess elements are just dropped instantly.
     * Growing: The capacity changes, but the (used) size not. It will grow
     * gradually when elements are pushed, as in the startup phase.
     *
     * Shrinking to less than one element is not possible.
     *
     * \param count    New maximum size / capacity of the sliding buffer
     * \param value    Will be ignored if specified
     */
    template <typename = std::enable_if<(BufferSize == 0u)>>
    auto resize(size_type count, value_type const& value = {}) -> void
    {
        count = std::max(count, size_type{ 1 });
        static_cast<void>(value); // Ignore, but we want to have it named for Doxygen
        auto const old_count = capacity();
        if (count == old_count)
            return;

        //////
        // Growing
        if (count > old_count) {
            storage_.resize(count);
            if (not full_) {
                return;
            }
            // transform the completely filled state into an initial filling state (oldest = leftmost)
            make_indices_trivial(old_count);
            full_ = false;
            return;
        }

        //////
        // Shrinking
        if (not full_ and next_element_ <= count) {
            storage_.resize(count);
            if (next_element_ > 0) {
                next_element_ %= count;
                full_ = (next_element_ == 0);
            }
            return;
        }
        // transform current filling state to completely filled state
        if (full_ and count < old_count) {
            make_indices_trivial(old_count);
            next_element_ = 0;
        }
        std::move(storage_.begin() + (old_count - count),
                storage_.end(),
                storage_.begin());
        next_element_ = 0;
        storage_.resize(count);
    }

    /**
     * Resize the container.
     *
     * Only possible if the underlying container is a std::vector.
     *
     * This just calls resize(). See further explanations at resize().
     *
     * \param size   New maximum size / capacity of the sliding buffer
     */
    template <typename = std::enable_if<(BufferSize == 0u)>>
    auto reserve(size_type size) -> void
    {
        resize(size);
    }

    /**
     * Dump all (also unfilled) buffer elements and which element is to be replaced next.
     *
     * Needs the Elements to be dump-able to an ostream.
     */
    auto friend operator<< (std::ostream& s, const SlidingBuffer<value_type, BufferSize>& buffer) -> std::ostream&
    {
        auto const len = buffer.capacity();
        for (size_type i{0}; i < len; ++i) {
            s << buffer.storage_.at(i);
            if (i == buffer.next_element_)
                s << "* ";
            else
                s << "  ";
        }
        return s << '\n';
    }

    struct iterator {
        /// Blah blah blah
        using iterator_category = std::forward_iterator_tag;
        /// The type "pointed to" by the iterator.
        using value_type = ElementT;
        /// Distance between iterators is represented as this type.
        using difference_type = std::ptrdiff_t;
        /// This type represents a pointer-to-value_type.
        using pointer = value_type const*;
        /// This type represents a reference-to-value_type.
        using reference = value_type const&;

    private:
        /// This is to be documented
        typename container_type::const_iterator internal_it_;
        typename container_type::const_iterator begin_;
        typename container_type::const_iterator end_;
        bool skip_;

    public:
        explicit iterator(SlidingBuffer<ElementT, BufferSize, Container> const* buff,
                typename container_type::const_iterator it,
                bool skip = false)
            : internal_it_{ std::move(it) }
            , begin_{ buff->storage_.cbegin() }
            , end_{ buff->storage_.cend() }
            , skip_{ skip }
        {
        }

        iterator& operator++()
        {
            if (internal_it_ == begin_) {
                skip_ = false;
                internal_it_ = end_;
            }
            --internal_it_;
            return *this;
        }

        reference operator*() const { return *internal_it_; }
        pointer operator->() const { return &*internal_it_; }

        iterator operator++(int) { auto previous = *this; ++(*this); return previous; }
        bool operator==(iterator other) const { return internal_it_ == other.internal_it_ and skip_ == other.skip_; }
        bool operator!=(iterator other) const { return !(*this == other); }
    };

    iterator begin() const {
        if (next_element_ != 0)
            return iterator{ this, storage_.cbegin() + next_element_ -1, filled() };
        if (filled())
            return iterator{ this, storage_.cbegin() + storage_.size() - 1, filled() };
        return iterator{ this, storage_.cbegin(), filled() };
    }
    iterator end() const {
        if (not filled())
            return iterator{ this, storage_.cend() - 1 };
        if (next_element_ == 0)
            return iterator{ this, storage_.cbegin() + storage_.size() - 1 };
        return iterator{ this, storage_.cbegin() + next_element_ - 1 };
    }

private:
    // Shuffle elements so that we have the most trivial representation
    // next_element_ has to be possibly corrected afterwards
    auto make_indices_trivial(const size_type count) -> void
    {
        auto const limit = count - 1;
        for (auto i = decltype(limit){0}; i <= limit; i++) {
            auto const j = std::min(limit, next_element_ + i);
            std::swap(storage_[i], storage_[j]);
        }
        next_element_ = count;
    }
};

} // namespace gul

// vi:ts=4:sw=4:et
