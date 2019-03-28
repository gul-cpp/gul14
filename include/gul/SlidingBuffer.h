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
#include <iostream>
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
 *
 * Member functions:
 *   Element access:
 *     push_back         Insert an element into the buffer
 *     operator[]        Access element relative to most recent element in buffer, bounds corrected, read only
 *     at                Note: Not overloaded, gives access to underlying container
 *     front             Access first (next to be pushed out) element
 *     back              Acces last (most recently pushed in) element
 *   Iterators:
 *     end, cend         Returns iterator to end of used space in the container
 *     rbegin, crbegin   Returns reverse iterator to beginning of used space in the container
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
 * The sliding buffer can be instantiated in two different underlying container versions:
 * * If the size is known at compile time, instanziate it with that number as BufferSize. The
 *   underlying container will be a std::array.
 * * If a flexible size is desired, specify BufferSize zero (0). The underlying container will
 *   be a std::vector. You need to set the capacity afterwards with resize(). The default size
 *   is just 1 element.
 * Apart from the ability of the later to resize()/reserve() all
 * is identical.
 *
 * \tparam ElemenT       Type of elements in the buffer
 * \tparam BufferSize    Maximum number of elements in the buffer
 * \tparam Container     Type of the underlying container, usually not specified
 *
 */
template<typename ElementT, std::size_t BufferSize,
    typename Container = typename std::conditional<(BufferSize >= 1u),
        std::array<ElementT, BufferSize>,
        std::vector<ElementT>>::type
>
class SlidingBuffer : public Container {
private:
    std::size_t next_element_{ 0u };
    bool full_{ false };

public:
    /// Type of the underlying container (e.g. std::array<value_type, ..>)
    using container_type = Container;
    /// Type of the elements in the underlying container
    using value_type = ElementT;

    /**
     * Insert one element item into the buffer.
     *
     * Think of this as appending to the end. Probably an element from the front is
     * dropped to make room in the fixed size buffer.
     */
    auto push_back(ElementT&& in) -> void
    {
        container_type::operator[](next_element_) = std::move(in);
        ++next_element_;
        if (next_element_ >= capacity()) {
            next_element_ = 0;
            full_ = true;
        }
    }

    /**
     * \overload
     */
    auto push_back(const ElementT& in) -> void
    {
        auto in_copy = in;
        push_back(std::move(in_copy));
    }

    /**
     * Access (read) one element in the buffer, relative to the most recently `push`ed
     * element.
     *
     * The index 0 is the most recent element, 1 is the element before that
     * and so on. One can think of i as being implicitly interpreted as always being
     * negative.
     * i := >=0, but works on the i-th previous element
     *
     * i in coerced to be inside the size of the buffer, wrapping around by the buffer size.
     *
     * If the buffer is not yet full it may be possible that the function has nothing to
     * return and so a default constructed Element is returned.
     */
    auto operator[] (const std::size_t i) const -> const ElementT&
    {
        const std::size_t idx = next_element_ + capacity() - (i % capacity()) - 1;
        // If the element has ever been filled or not is ignored. A default
        // constructed ELEMENT will be returned on unset elements
        return container_type::operator[](idx % capacity());
    }

    /**
     * Return the oldest ('left most') Element in the buffer.
     *
     * This is a read-only operation.
     */
    auto front() const noexcept -> const ElementT&
    {
        return container_type::operator[](next_element_);
    }

    /**
     * Return the youngest / most recent ('right most') Element in the buffer.
     *
     * This is a read-only operation.
     */
    auto back() const noexcept -> const ElementT&
    {
        return operator[](0);
    }

    /**
     * Return an iterator to the element following the last element of the underlying
     * container.
     *
     * This accesses the underlying container in its order. The iterators do not know
     * where the sliding starts and ends. Use the iterators only if you want to access
     * all elements in unknown order.
     *
     * It does, however, take not yet filled buffers into account and returns iterators
     * only to elements really filled.
     */
    auto end() noexcept -> decltype(container_type::end())
    {
        if (full_)
            return container_type::end();
        return container_type::begin() + next_element_;
    }

    /**
     * Return a constant iterator to the element following the last element of the
     * underlying container.
     *
     * This accesses the underlying container in its order. The iterators do not know
     * where the sliding starts and ends. Use the iterators only if you want to access
     * all elements in unknown order.
     *
     * It does, however, take not yet filled buffers into account and returns iterators
     * only to elements really filled.
     */
    auto cend() const noexcept -> decltype(container_type::cend())
    {
        if (full_)
            return container_type::cend();
        return container_type::cbegin() + next_element_;
    }

    /**
     * Return a reverse iterator to the first element of the reversed underlying container.
     *
     * It corresponds to the last element of the non-reversed container.
     * If the container is empty, the returned iterator is equal to rend().
     *
     * This accesses the underlying container in its order. The iterators do not know
     * where the sliding starts and ends. Use the iterators only if you want to access
     * all elements in unknown order.
     *
     * It does, however, take not yet filled buffers into account and returns iterators
     * only to elements really filled.
     */
    auto rbegin() noexcept -> decltype(container_type::rbegin())
    {
        if (not full_ and next_element_ == 0)
            return container_type::rend();
        return container_type::rbegin() + capacity() - next_element_;
    }

    /**
     * Return a const reverse iterator to the first element of the reversed
     * underlying container.
     *
     * It corresponds to the last element of the non-reversed container.
     * If the container is empty, the returned iterator is equal to rend().
     *
     * This accesses the underlying container in its order. The iterators do not know
     * where the sliding starts and ends. Use the iterators only if you want to access
     * all elements in unknown order.
     *
     * It does, however, take not yet filled buffers into account and returns iterators
     * only to elements really filled.
     */
    auto crbegin() const noexcept -> decltype (container_type::crbegin())
    {
        if (not full_ and next_element_ == 0)
            return container_type::rcend();
        return container_type::crbegin() + capacity() - next_element_;
    }

    /**
     * Return the number of elements in the container, i.e. std::distance(begin(), end()).
     *
     * In the startup phase it can be 0 and up to the BufferSize, after startup (filled() == true)
     * it will always return BufferSize (capacity()).
     */
    auto size() const noexcept -> std::size_t
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
    auto constexpr capacity() const noexcept -> std::size_t
    {
        return BufferSize ? BufferSize : Container::size();
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
        // Need to fill with 'empty' data so that operator[] etc work as expected
        fill(ElementT{});
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
     *
     */
    template <typename = std::enable_if<(BufferSize == 0u)>>
    auto resize(std::size_t count, ElementT const& value = {}) -> void
    {
        count = std::max(count, std::size_t{ 1 });
        static_cast<void>(value); // Ignore, but we want to have it named for Doxygen
        auto const old_count = capacity();
        if (count == old_count)
            return;

        //////
        // Growing
        if (count > old_count) {
            Container::resize(count);
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
            Container::resize(count);
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
        std::move(container_type::begin() + (old_count - count),
                container_type::end(),
                container_type::begin());
        next_element_ = 0;
        Container::resize(count);
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
     * Needs the Elements to be dumpable to an ostream.
     */
    auto friend operator<< (std::ostream& s, const SlidingBuffer<ElementT, BufferSize>& buffer) -> std::ostream&
    {
        auto const len = buffer.capacity();
        for (std::size_t i{0}; i < len; ++i) {
            s << buffer.at(i);
            if (i == buffer.next_element_)
                s << "* ";
            else
                s << "  ";
        }
        return s << '\n';
    }

private:
    // Shuffle elements so that we have the most trivial representation
    // next_element_ has to be possibly corrected afterwards
    auto make_indices_trivial(const std::size_t count) -> void
    {
        auto const limit = count - 1;
        for (auto i = decltype(limit){0}; i <= limit; i++) {
            auto const j = std::min(limit, next_element_ + i);
            std::swap(container_type::operator[](i), container_type::operator[](j));
        }
        next_element_ = count;
    }

};

} // namespace gul

// vi:ts=4:sw=4:et
