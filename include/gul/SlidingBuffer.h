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

#include <algorithm>
#include <array>
#include <vector>

namespace gul {

/**
 * A circular data buffer of (semi-)fixed capacity to which elements can be added at the
 * front or at the back.
 *
 * A SlidingBuffer is a flat array containing zero to capacity() elements of an arbitrary
 * type. The capacity can either be specified at compile time via a template parameter or
 * at runtime via resize(). In the former case, the internal buffer is embedded in the
 * object (std::array), in the latter case it is dynamically allocated (std::vector).
 *
 * The SlidingBuffer shares many characteristics with traditional ring buffers: It has a
 * fixed maximum size and new elements are added with push_front() or push_back().
 * However, there is no way to *pop* elements out of the buffer. They drop out
 * automatically at the other end of the sliding window if the capacity is reached:
 *
 * \code
 * SlidingBuffer<int, 2> buf; // Create a buffer with up to 2 entries
 *
 * buf.push_back(1);
 * buf.push_back(2);
 * buf.push_back(3);
 *
 * std::cout << buf[0] << ", " << buf[1] << "\n";
 * // prints "2, 3"
 *
 * buf.push_front(1);
 * std::cout << buf[0] << ", " << buf[1] << "\n";
 * // prints "1, 2"
 * \endcode
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
 * This container uses an accompanying iterator class called SlidingBufferIterator.
 * See SlidingBufferExposed for a variant with a different (more performant) iterator
 * interface.
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
 *     push_back         Insert an element at the back of the buffer
 *     push_front        Insert an element at the front of the buffer
 *     operator[]        Access element by index, unchecked
 *     at                Access element by index with bounds checking
 *     front             Access foremost element (i.e. [0])
 *     back              Access backmost element (i.e. [size() - 1])
 *   Iterators:
 *     begin, cbegin     Return an iterator to the first element of the container
 *     end, cend         Return an iterator to the element following the last element of the container
 *     rbegin, crbegin   Return an iterator to the first element of the reversed container
 *     rend, crend       Return an iterator to the element following the last element of the reversed container
 *   Capacity:
 *     size              Return number of used elements
 *     capacity          Return maximum number of elements
 *     filled            Check whether the buffer is completely filled
 *     empty             Check whether the buffer is empty
 *     resize            Change the maximum number of elements (only if fixed_size==0)
 *     reserve           Change the maximum number of elements (only if fixed_size==0)
 *   Modifiers:
 *     clear             Empty the buffer
 *
 * Non-member functions:
 *   operator<<          Dump the raw data of the buffer to an ostream
 * \endcode
 *
 * The sliding buffer can be instantiated in two slightly different versions:
 * * If the size is known at compile time, it can be specified as the `fixed_capacity`
 *   template parameter. The elements are stored within the sliding buffer as in a
 *   std::array.
 * * If a flexible capacity is desired, `fixed_capacity` can be omitted. It defaults to
 *   zero, and space for elements can subsequently be allocated dynamically as in a
 *   std::vector. You need to use a constructor that sets a certain capacity or set the
 *   capacity afterwards with resize(). As long as the capacity is zero, the buffer is
 *   unusable and most operations result in undefined behavior.
 *
 * If clear() is to be used, `ElementT` must be default constructible.
 *
 * \tparam ElementT       Type of elements in the buffer
 * \tparam fixed_capacity Maximum number of elements in the buffer (capacity), zero if
 *                        unspecified/dynamic
 * \tparam Container      Type of the underlying container, usually not specified
 */
template<typename ElementT, std::size_t fixed_capacity = 0u,
    typename Container = typename std::conditional_t<(fixed_capacity >= 1u),
        std::array<ElementT, fixed_capacity>,
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
    /// Pointer to a constant element
    using const_pointer = typename Container::const_pointer;
    /// Iterator to an element
    using iterator = SlidingBufferIterator<SlidingBuffer<ElementT, fixed_capacity, Container>*>;
    /// Iterator to a const element
    using const_iterator = SlidingBufferIterator<SlidingBuffer<ElementT, fixed_capacity, Container> const*>;
    /// Iterator to an element in reversed container
    using reverse_iterator = std::reverse_iterator<iterator>;
    /// Iterator to a const element in reversed container
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    /**
     * Construct an empty sliding buffer.
     *
     * The capacity of the buffer is given by the fixed_capacity template parameter.
     * If that template argument is not zero, a std::array based SlidingBuffer
     * with that (unchangeable) capacity is created.
     *
     * If the template argument is zero, a SlidingBuffer based on std::vector with a
     * capacity of zero elements is generated. Use the \ref SlidingBuffer(size_type)
     * constructor or call the resize() function afterwards to get a SlidingBuffer
     * based on std::vector with nonzero capacity.
     */
    SlidingBuffer() = default;
    /// Default copy constructor.
    SlidingBuffer(SlidingBuffer const&) = default;
    /// Default move constructor.
    SlidingBuffer(SlidingBuffer&&) noexcept = default;
    /// Default copy assignment operator.
    SlidingBuffer& operator=(SlidingBuffer const&) = default;
    /// Default move assignment operator.
    SlidingBuffer& operator=(SlidingBuffer &&) noexcept = default;
    /// Default Destructor.
    virtual ~SlidingBuffer() = default;

    /**
     * \overload
     *
     * Only available for sliding buffers based on std::vector.
     *
     * Constructs a sliding buffer with a specified capacity.
     *
     * For std::array based sliding buffers the capacity is specified by the
     * fixed_capacity template parameter.
     */
    SlidingBuffer(size_type count) : storage_(count) {}

protected:

    /// Index of the first SlidingBuffer element in the underlying container (the one
    /// retrieved by SlidingBuffer::front().
    size_type idx_begin_{ 0u };
    /// Index pointing to the element in the underlying container that will be written to
    /// by the next call to push_back().
    size_type idx_end_{ 0u };
    /// Indicates if the buffer is completely filled with elements.
    bool full_{ false };
    /// Actual data is stored here, the underlying container.
    Container storage_{ };

public:

    /**
     * Insert one element at the end of the buffer; if it is full, an element at the front
     * is dropped to make room.
     *
     * Iterator end() is invalidated. Iterator begin() is only invalidated if the call
     * causes the size of the buffer to increase (i.e. if it was not full yet).
     * All other iterators still point to the same logical element, while the contents of
     * all logical elements is shifted.
     *
     * \warning
     * Calling push_back() on a SlidingBuffer with zero capacity results in undefined
     * behavior.
     */
    auto push_back(const value_type& in) -> void
    {
        storage_[idx_end_] = in;

        increase_idx(idx_end_);
        if (full_)
            increase_idx(idx_begin_);

        if (idx_end_ == idx_begin_)
            full_ = true;
    }

    /**
     * \overload
     */
    auto push_back(value_type&& in) -> void
    {
        storage_[idx_end_] = std::move(in);

        increase_idx(idx_end_);
        if (full_)
            increase_idx(idx_begin_);

        if (idx_end_ == idx_begin_)
            full_ = true;
    }

    /**
     * Insert one element at the front of the buffer; if it is full, an element at the
     * back is dropped to make room.
     *
     * Iterator begin() is invalidated. Iterator end() is only invalidated if the call
     * causes the size of the buffer to increase (i.e. if it was not full yet).
     * All other iterators still point to the same logical element, while the contents of
     * all logical elements is shifted.
     *
     * \warning
     * Calling push_front() on a SlidingBuffer with zero capacity results in undefined
     * behavior.
     */
    auto push_front(const value_type& in) -> void
    {
        decrease_idx(idx_begin_);

        if (full_)
            decrease_idx(idx_end_);

        storage_[idx_begin_] = in;

        if (idx_end_ == idx_begin_)
            full_ = true;
    }

    /**
     * \overload
     */
    auto push_front(value_type&& in) -> void
    {
        decrease_idx(idx_begin_);

        if (full_)
            decrease_idx(idx_end_);

        storage_[idx_begin_] = std::move(in);

        if (idx_end_ == idx_begin_)
            full_ = true;
    }

    /**
     * Access an element in the buffer by index without bounds checking.
     *
     * Index 0 is the foremost element, `size() - 1` the backmost one. Access to elements
     * outside the capacity is not allowed and results in undefined behavior. Access to
     * elements inside the capacity is always allowed; a default-constructed element is
     * returned if `idx >= size()`.
     *
     * \returns a reference to the requested element.
     */
    auto operator[](size_type idx) noexcept -> reference
    {
        idx += idx_begin_;

        return (idx >= capacity()) ? storage_[idx - capacity()] : storage_[idx];
    }

    /**
     * \overload
     */
    auto operator[](size_type idx) const noexcept -> const_reference
    {
        idx += idx_begin_;

        return (idx >= capacity()) ? storage_[idx - capacity()] : storage_[idx];
    }

    /**
     * Access an element in the buffer by index with bounds checking.
     *
     * Index 0 is the foremost element, `size() - 1` the backmost one. Access to elements
     * beyond the last one causes an exception to be thrown.
     *
     * \param idx   Index of the element to return
     *
     * \returns a reference to the requested element.
     * \exception std::out_of_range is thrown if `idx >= size()`.
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
     * Return the foremost element (the one with index 0).
     * This call does not check if an element has ever been pushed into the buffer, so it
     * might return a default-constructed element. In the case of a SlidingBuffer with
     * zero capacity, calling front() results in undefined behavior.
     */
    auto front() noexcept -> reference
    {
        return storage_[idx_begin_];
    }

    /**
     * \overload
     */
    auto front() const noexcept -> const_reference
    {
        return storage_[idx_begin_];
    }

    /**
     * Return the backmost element (the one with the highest valid index).
     * This call does not check if an element has ever been pushed into the buffer, so it
     * might return a default-constructed element. In the case of a SlidingBuffer with
     * zero capacity, calling back() results in undefined behavior.
     */
    auto back() noexcept -> reference
    {
        if (idx_end_ == 0)
            return storage_[capacity() - 1];
        else
            return storage_[idx_end_ - 1];
    }

    /**
     * \overload
     */
    auto back() const noexcept -> const_reference
    {
        if (idx_end_ == 0)
            return storage_[capacity() - 1];
        else
            return storage_[idx_end_ - 1];
    }

    /**
     * Return the number of elements in the container, i.e. std::distance(begin(), end()).
     *
     * In the startup phase it can be 0 and up to the fixed_capacity or capacity(),
     * after startup (filled() == true) it will always return fixed_capacity resp.
     * capacity().
     */
    auto size() const noexcept -> size_type
    {
        if (full_)
            return capacity();
        
        if (idx_end_ >= idx_begin_)
            return idx_end_ - idx_begin_;
        else
            return idx_end_ + capacity() - idx_begin_;
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
        return (fixed_capacity > 0) ? fixed_capacity : storage_.size();
    }

    /**
     * Return true if the buffer is completely filled with elements.
     *
     * If the buffer is used in filter contexts this means the filter is fully
     * initialized and working.
     *
     * If the buffer has zero capacity it will be filled after one element (that
     * is not memorized) has been pushed into it.
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
        idx_begin_ = 0u;
        idx_end_ = 0u;

        // Fill with new empty elements to possibly trigger RAII in the elements
        std::fill(storage_.begin(), storage_.end(), value_type{});
    }

    /**
     * Resize the container.
     *
     * Only possible if the underlying container is a std::vector.
     *
     * Shrinking: The excess elements with the highest indices are dropped.
     * Growing: The capacity changes, but the (used) size does not. It will grow
     * gradually when elements are pushed, as in the startup phase.
     *
     * \param new_capacity  New capacity (maximum size) of the sliding buffer.
     */
    auto resize(size_type new_capacity) -> void
    {
        static_assert(fixed_capacity == 0u,
            "resize() only possible if the underlying container is resizable");
        auto const old_capacity = capacity();
        auto const old_size = size();

        //////
        // No change
        if (new_capacity == old_capacity)
            return;

        //////
        // Vanishing
        if (new_capacity == 0) {
            storage_.resize(0);
            idx_begin_ = 0;
            idx_end_ = 0;
            full_ = false;
            return;
        }

        // For growing or shrinking, make SlidingBuffer indices equal to those of the
        // underlying container
        std::rotate(storage_.begin(), storage_.begin() + idx_begin_, storage_.end());
        idx_begin_ = 0;

        //////
        // Growing
        if (new_capacity > old_capacity) {
            storage_.resize(new_capacity);
            full_ = false;
            idx_end_ = old_size;
            return;
        }

        //////
        // Shrinking
        storage_.resize(new_capacity);
        if (old_size < new_capacity) {
            full_ = false;
            idx_end_ = old_size;
        }
        else {
            full_ = true;
            idx_end_ = 0;
        }
    }

    /**
     * Resize the container.
     *
     * Only possible if the underlying container is a std::vector.
     *
     * This just calls resize(). See further explanations at resize().
     *
     * \param size   New capacity (maximum size) of the sliding buffer.
     */
    auto reserve(size_type size) -> void
    {
        static_assert(fixed_capacity == 0u,
            "reserve() only possible if the underlying container is resizable");
        resize(size);
    }

    /**
     * Check if the buffer contains no elements, i.e. whether begin() == end().
     *
     * \returns true if the container is empty, false otherwise.
     */
    auto empty() const noexcept -> bool
    {
        return (not full_) and (idx_begin_ == idx_end_);
    }

    /**
     * Dump all buffer elements.
     *
     * Shown on the left is front(), on the right back().
     *
     * Needs the elements to be dump-able to an ostream.
     */
    auto friend operator<< (std::ostream& s,
            const SlidingBuffer<value_type, fixed_capacity, container_type>& buffer) -> std::ostream&
    {
        auto const size = buffer.size();
        // We can not use range-for here, because of SlidingBufferExposed<>'s un-wrapped iterators
        for (auto i = size_type{ 0 }; i < size; ++i)
            s << buffer[i] << "  ";
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
     * * Only begin() invalidated on size increase by push_front()
     * * Only end() invalidated on size increase by push_back()
     *
     * An iterator always points to the same logical slot in the SlidingBuffer.
     *
     * \tparam BufferPointer Type of the pointer used to access the SlidingBuffer
     */
    template <typename BufferPointer>
    struct SlidingBufferIterator : std::iterator<std::bidirectional_iterator_tag, value_type> {
    protected:
        /// This is the logical index we are currently pointing at.
        size_type position_{ 0 };
    private:
        /// A pointer to the container holding the actual data.
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
        SlidingBufferIterator(SlidingBufferIterator&&) noexcept = default;
        /// Default copy assignment operator.
        SlidingBufferIterator& operator=(SlidingBufferIterator const&) = default;
        /// Default move assignment operator.
        SlidingBufferIterator& operator=(SlidingBufferIterator &&) noexcept = default;
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
    void decrease_idx(size_t &idx) noexcept
    {
        if (idx == 0)
            idx = capacity() - 1;
        else
            --idx;
    }

    void increase_idx(size_t &idx) noexcept
    {
        ++idx;

        if (idx == capacity())
            idx = 0;
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
 * \tparam fixed_capacity Maximum number of elements in the buffer (i.e. capacity), zero if unspecified/dynamic
 * \tparam Container      Type of the underlying container, usually not specified
 *
 */
template<typename ElementT, std::size_t fixed_capacity = 0u,
    typename Container = typename std::conditional_t<(fixed_capacity >= 1u),
        std::array<ElementT, fixed_capacity>,
        std::vector<ElementT>>
    >
class SlidingBufferExposed : public SlidingBuffer<ElementT, fixed_capacity, Container> {
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
    using typename SlidingBuffer<ElementT, fixed_capacity, Container>::container_type;
    using typename SlidingBuffer<ElementT, fixed_capacity, Container>::value_type;
    using typename SlidingBuffer<ElementT, fixed_capacity, Container>::size_type;
    using typename SlidingBuffer<ElementT, fixed_capacity, Container>::difference_type;
    using typename SlidingBuffer<ElementT, fixed_capacity, Container>::reference;
    using typename SlidingBuffer<ElementT, fixed_capacity, Container>::const_reference;
    using typename SlidingBuffer<ElementT, fixed_capacity, Container>::pointer;
    using typename SlidingBuffer<ElementT, fixed_capacity, Container>::const_pointer;

    // Inherit constructors
    using SlidingBuffer<ElementT, fixed_capacity, Container>::SlidingBuffer;

    // Inherit members
    using SlidingBuffer<ElementT, fixed_capacity, Container>::storage_;
    using SlidingBuffer<ElementT, fixed_capacity, Container>::idx_end_;
    using SlidingBuffer<ElementT, fixed_capacity, Container>::full_;
    using SlidingBuffer<ElementT, fixed_capacity, Container>::capacity;

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
        return storage_.begin() + idx_end_;
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
        return storage_.cbegin() + idx_end_;
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
