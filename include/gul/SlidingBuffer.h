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

#include <iostream>
#include <array>

namespace gul {

/**
 * A simple data buffer with a (semi) fixed size to use as sliding window on a data stream
 *
 * The SlidingBuffer augments a std::array with very few extra functions to make it possible
 * to use it as sliding window on some incoming data.
 *
 * The SlidingBuffer shares some properties with ring buffers: It has a fixed maximum size
 * and incoming elements are easily appended. But there is no way to pop elements out of
 * the buffer. They will drop out automatically if the capacity is not sufficient to hold
 * all encountered incoming elements.
 *
 * This buffer is not intended for producer-consumer problems. If the elements access is
 * atomic it is threadsafe though.
 *
 * Differing from arrays this buffer will start empty (with a size of zero).
 * Once the buffer has grown to its designated size, it will never change in size again,
 * except when 'clear'ed.
 *
 * A typical application would be to analyze an incoming stream of elements in a finite
 * impulse response filter.
 *
 * The following lists the differences to the std::array interface. All interface
 * items not mentioned here access the underlying std::array interface.
 *
 * Member types:
 *   value_type          Type of the elements
 *   BaseT               Type of the underlying atd::array (i.e. std::array<value_type, ..>)
 *
 * Member functions:
 *   Element access:
 *     push_back         Insert an element into the buffer
 *     operator[]        Access element relative to most recent element in buffer, bounds corrected, read only
 *     at                Note: Not overloaded, gives access to underlying std:array
 *     front             Access first (next to be pushed out) element
 *     back              Acces last (most recently pushed in) element
 *   Iterators:
 *     end, cend         Returns iterator to end of used space in std::array
 *     rbegin, crbegin   Returns reverse iterator to beginning of used space in std::array
 *   Capacity:
 *     size              Returns number of used elements
 *     capacity          Returns maximum number of elements
 *     filled            Checks whether the buffer is completely filled
 *   Modifiers:
 *     clear             Empty the buffer
 *
 * Non-member functions:
 *   operator<<          Dump the raw data of the buffer to an ostream
 *
 */
template<typename ElementT, std::size_t BufferSize, typename = std::enable_if_t<(BufferSize >= 1u)>>
class SlidingBuffer : public std::array<ElementT, BufferSize> {
private:
    std::size_t next_element_{ 0u };
    bool full_{ false };

public:
    using BaseT = std::array<ElementT, BufferSize>;
    using value_type = ElementT;

    /**
     * Insert one element item into the buffer.
     *
     * Think of this as appending to the end. Probably an element from the front is
     * dropped to make room in the fixed size buffer.
     */
    auto push_back(ElementT&& in) -> void
    {
        BaseT::operator[](next_element_) = std::move(in);
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
        const std::size_t idx = next_element_ + BufferSize - (i % BufferSize) - 1;
        // If the element has ever been filled or not is ignored. A default
        // constructed ELEMENT will be returned on unset elements
        return BaseT::operator[](idx % BufferSize);
    }

    /**
     * Return the oldest ('left most') Element in the buffer.
     *
     * This is a read-only operation.
     */
    auto front() const noexcept -> const ElementT&
    {
        return BaseT::operator[](next_element_);
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
    auto end() noexcept -> decltype(BaseT::end())
    {
        if (full_)
            return BaseT::end();
        return BaseT::begin() + next_element_;
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
    auto cend() const noexcept -> decltype(BaseT::cend())
    {
        if (full_)
            return BaseT::cend();
        return BaseT::cbegin() + next_element_;
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
    auto rbegin() noexcept -> decltype(BaseT::rbegin())
    {
        if (not full_ and next_element_ == 0)
            return BaseT::rend();
        return BaseT::rbegin() + BufferSize - next_element_;
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
    auto crbegin() const noexcept -> decltype (BaseT::crbegin())
    {
        if (not full_ and next_element_ == 0)
            return BaseT::rcend();
        return BaseT::crbegin() + BufferSize - next_element_;
    }

    /**
     * Return the number of elements in the container, i.e. std::distance(begin(), end()).
     *
     * In the startup phase it can be 0 and up to the BufferSize, after startup (filled() == true)
     * it will always return BufferSize.
     */
    auto size() const noexcept -> std::size_t
    {
        if (full_)
            return BufferSize;
        return next_element_;
    }

    /**
     * Return the maximum possible number of elements in the container (BufferSize).
     */
    auto constexpr capacity() const noexcept -> std::size_t
    {
        return BufferSize;
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

};

} // namespace gul

// vi:ts=4:sw=4:et
