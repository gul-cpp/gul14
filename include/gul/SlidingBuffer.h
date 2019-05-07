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
 * There is the SlidingBufferIterator to use. See SlidingBufferExposed for a variant with
 * a different (more direct) iterator interface.
 *
 * \code
 * Iterator invalidation:
 *   All read only operations    None
 *   clear                       All iterators except begin()
 *   reserve, resize             If shrunk: All behind the new end (incl end()).
 *   push_front                  If size increased end()
 *
 * Member types:
 *   value_type                  Type of the elements
 *   container_type              Type of the underlying container (i.e. std::array<value_type, ..>)
 *   size_type                   Unsigned integer type (usually std::size_t)
 *   difference_type             Signed integer type (usually std::ptrdiff_t)
 *   reference                   value_type&
 *   const_reference             value_type const&
 *   pointer                     value_type*
 *   const_pointer               value_type const*
 *   iterator                    SlidingBufferIterator
 *   const_iterator              SlidingBufferIterator const
 *   reverse_iterator            SlidingBufferIterator
 *   const_reverse_iterator      SlidingBufferIterator const
 *
 * Member functions:
 *     SlidingBuffer     Constructor
 *   Element access:
 *     push_front        Insert an element into the buffer
 *     operator[]        Access element relative to most recent element in buffer
 *     at                Access element relative to most recent element in buffer, with bounds checking
 *     front             Access first (most recently pushed in) element (i.e. [0])
 *     back              Access last (next to be pushed out) element (i.e. [size() - 1])
 *   Iterators:
 *     begin, cbegin     Returns an iterator to the first element of the container
 *     end, cend         Returns an iterator to the element following the last element of the container
 *     rbegin, crbegin   Returns an iterator to the first element of the reversed container
 *     rend, crend       Returns an iterator to the element following the last element of the reversed container
 *   Capacity:
 *     size              Returns number of used elements
 *     capacity          Returns maximum number of elements
 *     filled            Checks whether the buffer is completely filled
 *     empty             Checks whether the buffer is empty
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
 * * If the size is known at compile time, instantiate it with that number as BufferSize.
 *   The underlying container will be a std::array.
 * * If a flexible size is desired, omit the template parameter BufferSize. It will be
 *   defaulted to zero (0). The underlying container will then be a std::vector. You need
 *   to use a constructor that sets a certain capacity or set the capacity afterwards with
 *   resize(). If the size is really 0 elements, the buffer is unusable.
 * Apart from the ability of the latter to resize()/reserve() all functionality is
 * identical.
 *
 * The elements (ElementT) must be default constructible, if clear() will be used.
 *
 * \tparam ElementT      Type of elements in the buffer
 * \tparam BufferSize    Maximum number of elements in the buffer, zero if unspecified
 * \tparam Container     Type of the underlying container, usually not specified
 */
template<typename ElementT, std::size_t BufferSize = 0u,
    typename Container = typename std::conditional_t<(BufferSize >= 1u),
        std::array<ElementT, BufferSize>,
        std::vector<ElementT>>
    >
class SlidingBuffer {
public:
    template <typename>
    struct SlidingBufferIterator;
    /// Type of the underlying container (e.g. std::array<value_type, ..>)
    using container_type = Container;
    /// Type of the elements in the underlying container
    using value_type = ElementT;
    /// Unsigned integer type (usually std::size_t)
    using size_type = typename Container::size_type;
    /// Signed integer type (usually std::ptrdiff_t)
    using difference_type = typename Container::difference_type;
    /// Reference to an element
    using reference = typename Container::reference;
    /// Reference to a constant element
    using const_reference = typename Container::const_reference;
    /// Pointer to an element
    using pointer = typename Container::pointer;
    /// Pointer to a constant elemenet
    using const_pointer = typename Container::const_pointer;
    /// Iterator to an element
    using iterator = SlidingBufferIterator<SlidingBuffer<ElementT, BufferSize, Container>*>;
    /// Iterator to a const element
    using const_iterator = SlidingBufferIterator<SlidingBuffer<ElementT, BufferSize, Container> const*>;
    /// Iterator to an element in reversed container
    using reverse_iterator = std::reverse_iterator<iterator>;
    /// Iterator to a const element in reversed container
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    /**
     * Constructs a new sliding buffer.
     *
     * The buffer is initially empty.
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
    /// Default copy constructor.
    SlidingBuffer(SlidingBuffer const&) = default;
    /// Default move constructor.
    SlidingBuffer(SlidingBuffer&&) = default;
    /// Default copy assignment operator.
    SlidingBuffer& operator=(SlidingBuffer const&) = default;
    /// Default move assignment operator.
    SlidingBuffer& operator=(SlidingBuffer &&) = default;
    /// Default Destructor.
    virtual ~SlidingBuffer() = default;

    /**
     * \overload
     *
     * Only available for sliding buffers based on std::vector.
     *
     * Constructs a sliding buffer with a specified capacity.
     *
     * For std::array based sliding buffers the capacity is specified by the BufferSize template parameter.
     */
    SlidingBuffer(size_type count) : storage_(count) {}

protected:

    /// Index of the element in the underlying container that will be written to next.
    size_type next_element_{ 0u };
    /// Indicates if the buffer is completely filled with elements
    /// (i.e. elements with indices higher than next_element_ are valid).
    bool full_{ false };
    /// Actual data is stored here, the underlying container.
    Container storage_{ };

public:

    /**
     * Insert one element item into the buffer.
     *
     * Think of this as inserting in the front. Probably an element at the back is
     * dropped to make room in the fixed size buffer.
     *
     * Iterator end() is invalidated if the size of the buffer increaded (i.e.
     * in the startup phase where filled() == false).
     * All other iterators still point to the same logical element, while the
     * contents of all logical elements is shifted.
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
     * Access one element in the buffer, relative to the most recently `push`ed
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
     * \returns     Reference to the requested element
     */
    auto operator[](const size_type idx) -> reference
    {
        if (next_element_ <= idx)
            return storage_[next_element_ - idx - 1 + capacity()];
        else
            return storage_[next_element_ - idx - 1];
    }

    /**
     * \overload
     */
    auto operator[](const size_type idx) const -> const_reference
    {
        if (next_element_ <= idx)
            return storage_[next_element_ - idx - 1 + capacity()];
        else
            return storage_[next_element_ - idx - 1];
    }

    /**
     * Access one element in the buffer, relative to the most recently `push`ed
     * element, with bounds checking.
     *
     * The index 0 is the most recent element, 1 is the element before that
     * and so on.
     *
     * If idx is not within the range of the container,
     * an exception of type std::out_of_range is thrown.
     *
     * \param idx   Index of the element to return
     * \returns     Reference to the requested element
     */
    auto at(const size_type idx) noexcept(false) -> reference
    {
        auto const s = size();
        if (idx >= s) {
            throw std::out_of_range(gul::cat("SlidingBuffer::", __func__,
                ": idx (which is ", idx, ") >= this->size() (which is ", s, ")"));
        }
        return operator[](idx);
    }

    /**
     * \overload
     */
    auto at(const size_type idx) const noexcept(false) -> const_reference
    {
        auto const s = size();
        if (idx >= s) {
            throw std::out_of_range(gul::cat("SlidingBuffer::", __func__,
                ": idx (which is ", idx, ") >= this->size() (which is ", s, ")"));
        }
        return operator[](idx);
    }

    /**
     * Return the oldest ('right most') Element in the buffer.
     *
     * This is a read-only operation.
     */
    auto front() const noexcept -> const_reference
    {
        return operator[](0);
    }

    /**
     * Return the oldest ('right most') Element in the buffer.
     *
     * This is a read-only operation.
     */
    auto back() const noexcept -> const_reference
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
        storage_.fill(value_type{});
    }

    /**
     * Resize the container.
     *
     * Only possible if the underlying container is a std::vector.
     *
     * Shrinking: The oldest excess elements are just dropped instantly.
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
     * Checks if the buffer has no elements, i.e. whether begin() == end().
     *
     * \returns true if the container is empty, false otherwise
     */
    auto empty() noexcept -> bool
    {
        return (not full_) and (next_element_ == 0);
    }

    /**
     * Dump all (also unfilled) buffer elements and which element is to be replaced next.
     *
     * Needs the Elements to be dump-able to an ostream.
     */
    auto friend operator<< (std::ostream& s,
            const SlidingBuffer<value_type, BufferSize, container_type>& buffer) -> std::ostream&
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

    /**
     * Iterator of the SlidingBuffer.
     *
     * This is a bidirectional iterator.
     *
     * It has the following guarantees:
     * * No invalidation on any read
     * * No invalidation after push_front() after container filled
     * * Only end() invalidated on size increase
     * * Only all iterators pointing past the new end() invalidated on size decrease
     *
     * An iterator always points to the same logical slot in the SlidingBuffer.
     *
     * If the buffer grows by one element the iterator end() now (often) points
     * to the oldest element, because that has been pushed right in the logical
     * slots. After this end() has to be re-aquired. Other iterators still
     * point to the same slots.
     *
     * \tparam BufferPointer Type of the pointer used to access the SlidingBuffer
     */
    template <typename BufferPointer>
    struct SlidingBufferIterator : std::iterator<std::bidirectional_iterator_tag, value_type> {
    protected:
        /// This is the logical index we are currently pointing at.
        size_type position_{ 0 };
    private:
        /// A reference to the container holding the actual data.
        BufferPointer buffer_;

    public:
        /**
         * Create an iterator pointing into a SlidingBuffer.
         *
         * \param buff Reference to the SlidingBuffer the iterator points into.
         * \param num  Index of the element the iterator points to.
         */
        explicit SlidingBufferIterator(BufferPointer buff, size_type num = 0)
            : position_{ num }
            , buffer_{ buff }
        {
        }

        // No default constructor
        /// Default copy constructor.
        SlidingBufferIterator(SlidingBufferIterator const&) = default;
        /// Default move constructor.
        SlidingBufferIterator(SlidingBufferIterator&&) = default;
        /// Default copy assignment operator.
        SlidingBufferIterator& operator=(SlidingBufferIterator const&) = default;
        /// Default move assignment operator.
        SlidingBufferIterator& operator=(SlidingBufferIterator &&) = default;
        /// Default Destructor.
        virtual ~SlidingBufferIterator() = default;

        /// Pre-increment iterator by one position
        auto operator++() noexcept -> SlidingBufferIterator&
        {
            ++position_;
            return *this;
        }

        /// Post-increment iterator by one position
        auto operator++(int) noexcept -> SlidingBufferIterator
        {
            auto previous = *this;
            ++(*this);
            return previous;
        }

        /// Pre-decrement iterator by one position
        auto operator--() noexcept -> SlidingBufferIterator&
        {
            --position_;
            return *this;
        }

        /// Post-decrement iterator by one position
        auto operator--(int) noexcept -> SlidingBufferIterator
        {
            auto previous = *this;
            --(*this);
            return previous;
        }

        /// Access element pointed to by the iterator
        auto operator*() const -> typename std::conditional_t<
            std::is_const<std::remove_pointer_t<BufferPointer>>::value,
            const_reference, reference>
        {
            return (*buffer_)[position_];
        }

        /// Access member of element pointed to by the iterator
        auto operator->() const -> typename std::conditional_t<
            std::is_const<std::remove_pointer_t<BufferPointer>>::value,
            const_pointer, pointer>
        {
            return &(*buffer_)[position_];
        }

        /// Compare two iterators for equality.
        ///
        /// Both iterators must be from the same container, or the result is
        /// undefined.
        auto operator==(SlidingBufferIterator other) const noexcept -> bool
        {
            return position_ == other.position_;
        }

        /// Compare two iterators for inequality.
        ///
        /// Both iterators must be from the same container, or the result is
        /// undefined.
        auto operator!=(SlidingBufferIterator other) const noexcept -> bool
        {
            return not (*this == other);
        }
    };

    /**
     * Returns an iterator to the first element of the container.
     *
     * If the container is empty, the returned iterator will be equal to end()
     */
    auto begin() noexcept -> iterator
    {
        return iterator{ this, 0 };
    }

    /**
     * Returns an iterator to the first element of the reversed container.
     *
     * If the container is empty, the returned iterator will be equal to end()
     */
    auto rbegin() noexcept -> reverse_iterator
    {
        return std::make_reverse_iterator(end());
    }

    /**
     * Returns an iterator to the element following the last element of the container.
     *
     * This element acts as a placeholder; attempting to access it results in undefined behavior.
     */
    auto end() noexcept -> iterator
    {
        return iterator{ this, size() };
    }

    /**
     * Returns an iterator to the element following the last element of the reversed container.
     *
     * This element acts as a placeholder; attempting to access it results in undefined behavior.
     */
    auto rend() noexcept -> reverse_iterator
    {
        return std::make_reverse_iterator(begin());
    }

    /**
     * Returns a read only iterator to the first element of the container.
     *
     * If the container is empty, the returned iterator will be equal to cend()
     */
    auto cbegin() const noexcept -> const_iterator
    {
        return const_iterator{ this, 0 };
    }

    /**
     * Returns a read only iterator to the first element of the reversed container.
     *
     * If the container is empty, the returned iterator will be equal to cend()
     */
    auto crbegin() const noexcept -> const_reverse_iterator
    {
        return std::make_reverse_iterator(cend());
    }

    /**
     * Returns a read only iterator to the element following the last element of the container.
     *
     * This element acts as a placeholder; attempting to access it results in undefined behavior.
     */
    auto cend() const noexcept -> const_iterator
    {
        return const_iterator{ this, size() };
    }

    /**
     * Returns a read only iterator to the element following the last element of the reversed container.
     *
     * This element acts as a placeholder; attempting to access it results in undefined behavior.
     */
    auto crend() const noexcept -> const_reverse_iterator
    {
        return std::make_reverse_iterator(cbegin());
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

/**
 * A simple data buffer with a (semi) fixed size to use as sliding window on a data stream
 *
 * This is a variant of SlidingBuffer that exposes the underlying container through its
 * iterator interface. For a complete description see SlidingBuffer; here are the differences
 * only:
 *
 * \code
 * Iterator invalidation:
 *   All read only operations    None
 *   clear                       All iterators except begin()
 *   reserve, resize             If shrank: All behind the new end (incl end()); if grown: all
 *   push_front                  If size incresed end()
 *
 * Member types:
 *   iterator                    container_type::iterator
 *   const_iterator              container_type::const_iterator
 *   reverse_iterator            container_type::reverse_iterator
 *   const_reverse_iterator      container_type::const_reverse_iterator
 *
 * Member functions:
 *   Iterators:
 *     begin, cbegin     Returns an iterator to the first element of the container
 *     end, cend         Returns an iterator to the element following the last element of the container
 *     rbegin, crbegin   Returns an iterator to the first element of the reversed container
 *     rend, crend       Returns an iterator to the element following the last element of the reversed container
 *
 * Non-member functions:
 *   operator<<          Dump the raw data of the buffer to an ostream
 * \endcode
 *
 * \tparam ElementT       Type of elements in the buffer
 * \tparam BufferSize    Maximum number of elements in the buffer, zero if unspecified
 * \tparam Container     Type of the underlying container, usually not specified
 *
 */
template<typename ElementT, std::size_t BufferSize = 0u,
    typename Container = typename std::conditional_t<(BufferSize >= 1u),
        std::array<ElementT, BufferSize>,
        std::vector<ElementT>>
    >
class SlidingBufferExposed : public SlidingBuffer<ElementT, BufferSize, Container> {
public:
    /// Iterator to an element
    using iterator = typename Container::iterator;
    /// Iterator to a const element
    using const_iterator = typename Container::const_iterator;
    /// Iterator to an element in reversed container
    using reverse_iterator = typename Container::reverse_iterator;
    /// Iterator to a const element in reversed container
    using const_reverse_iterator = typename Container::const_reverse_iterator;

    // Inherit member types
    using typename SlidingBuffer<ElementT, BufferSize, Container>::container_type;
    using typename SlidingBuffer<ElementT, BufferSize, Container>::value_type;
    using typename SlidingBuffer<ElementT, BufferSize, Container>::size_type;
    using typename SlidingBuffer<ElementT, BufferSize, Container>::difference_type;
    using typename SlidingBuffer<ElementT, BufferSize, Container>::reference;
    using typename SlidingBuffer<ElementT, BufferSize, Container>::const_reference;
    using typename SlidingBuffer<ElementT, BufferSize, Container>::pointer;
    using typename SlidingBuffer<ElementT, BufferSize, Container>::const_pointer;

    // Inherit constructors
    using SlidingBuffer<ElementT, BufferSize, Container>::SlidingBuffer;

    // Inherit members
    using SlidingBuffer<ElementT, BufferSize, Container>::storage_;
    using SlidingBuffer<ElementT, BufferSize, Container>::next_element_;
    using SlidingBuffer<ElementT, BufferSize, Container>::full_;
    using SlidingBuffer<ElementT, BufferSize, Container>::capacity;

    /**
     * Return an iterator to the first element of the underlying container.
     *
     * This accesses the underlying container in its order. The iterators do not know
     * where the sliding starts and ends. Use the iterators only if you want to access
     * all elements in unknown order.
     *
     * If the container is empty, the returned iterator will be equal to end()
     */
    auto begin() noexcept -> iterator
    {
        return storage_.begin();
    }

    /**
     * Return a constant iterator to the first element of the
     * underlying container.
     *
     * This accesses the underlying container in its order. The iterators do not know
     * where the sliding starts and ends. Use the iterators only if you want to access
     * all elements in unknown order.
     *
     * If the container is empty, the returned iterator will be equal to cend()
     */
    auto cbegin() const noexcept -> const_iterator
    {
        return storage_.cbegin();
    }

    /**
     * Returns an iterator to the element following the last element in the used
     * space of the underlying container.
     *
     * This element acts as a placeholder; attempting to access it results in undefined behavior.
     *
     * This accesses the underlying container in its order. The iterators do not know
     * where the sliding starts and ends. Use the iterators only if you want to access
     * all elements in unknown order.
     *
     * It does, however, take not yet filled buffers into account and returns iterators
     * only to elements really filled.
     */
    auto end() noexcept -> iterator
    {
        if (full_)
            return storage_.end();
        return storage_.begin() + next_element_;
    }

    /**
     * Return a constant iterator to the element following the last element in the
     * used space of the underlying container.
     *
     * This element acts as a placeholder; attempting to access it results in undefined behavior.
     *
     * This accesses the underlying container in its order. The iterators do not know
     * where the sliding starts and ends. Use the iterators only if you want to access
     * all elements in unknown order.
     *
     * It does, however, take not yet filled buffers into account and returns iterators
     * only to elements really filled.
     */
    auto cend() const noexcept -> const_iterator
    {
        if (full_)
            return storage_.cend();
        return storage_.cbegin() + next_element_;
    }

    /**
     * Return a reverse iterator to the first used element of the reversed underlying container.
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
     *
     * If the container is empty, the returned iterator will be equal to end()
     */
    auto rbegin() noexcept -> reverse_iterator
    {
        return std::make_reverse_iterator(end());
    }

    /**
     * Return a constant reverse iterator to the first used element of the reversed
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
     *
     * If the container is empty, the returned iterator will be equal to cend()
     */
    auto crbegin() const noexcept -> const_reverse_iterator
    {
        return std::make_reverse_iterator(cend());
    }

    /**
     * Return an iterator to the last element of the reversed underlying container.
     *
     * This accesses the underlying container in its order. The iterators do not know
     * where the sliding starts and ends. Use the iterators only if you want to access
     * all elements in unknown order.
     */
    auto rend() noexcept -> reverse_iterator
    {
        return std::make_reverse_iterator(begin());
    }

    /**
     * Return a constant iterator to the last element of the reversed
     * underlying container.
     *
     * This accesses the underlying container in its order. The iterators do not know
     * where the sliding starts and ends. Use the iterators only if you want to access
     * all elements in unknown order.
     */
    auto crend() const noexcept -> const_reverse_iterator
    {
        return std::make_reverse_iterator(cbegin());
    }
};

} // namespace gul

// vi:ts=4:sw=4:sts=4:et
