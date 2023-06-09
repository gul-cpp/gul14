/**
 * \file    SmallVector.h
 * \authors \ref contributors
 * \date    Created on August 17, 2020
 * \brief   Definition of the SmallVector class template.
 *
 * \copyright Copyright 2020-2023 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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

#ifndef GUL14_SMALLVECTOR_H_
#define GUL14_SMALLVECTOR_H_

#include <algorithm>
#include <array>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <stdexcept>
#include <type_traits>

#include "gul14/internal.h"
#include "gul14/cat.h"

namespace gul14 {

/**
 * \addtogroup SmallVector_h gul14/SmallVector.h
 * \brief A vector with small-buffer optimization.
 * @{
 */

/**
 * A resizable container with contiguous storage that can hold a specified number of
 * elements without allocating memory on the heap.
 *
 * A SmallVector is very similar to a std::vector, but it can store a small number of
 * elements within the object itself, similar to a std::array. If the number of elements
 * exceeds this <em>inner capacity</em> (size() > inner_capacity()), the elements are
 * stored on the heap as in a conventional std::vector.
 *
 * SmallVector mimicks the API of std::vector closely, but it does not support allocators.
 * It also uses 32-bit integers for storing the number of elements and capacity. On 64-bit
 * systems, this makes the object slightly more memory efficient and lets it store less
 * elements.
 *
 * \code
 * // Create a buffer which can store up to 3 entries without allocating
 * SmallVector<int, 3> buf;
 *
 * buf.push_back(1);
 * buf.push_back(2);
 * buf.push_back(3); // So far, no heap allocation has taken place
 *
 * buf.push_back(4); // Moves all elements into newly allocated memory
 * \endcode
 *
 * \tparam ElementT     Type of the elements to be stored in the container
 * \tparam in_capacity  The number of elements that can be stored directly in the object
 *                      without allocating (the "inner capacity")
 *
 * \since GUL version 2.5
 *
 * <h3>Member function overview</h3>
 *
 * <table style="border-style: none;">
 * <tr>
 *     <th colspan="2">Construction &amp; destruction</th>
 * </tr><tr>
 *     <td>SmallVector()</td>
 *     <td>Construct an empty SmallVector</td>
 * </tr><tr>
 *     <td>SmallVector(SizeType)</td>
 *     <td>Construct a SmallVector that is filled with a certain number of
 *     default-initialized elements</td>
 * </tr><tr>
 *     <td>SmallVector(SizeType, const ValueType&)</td>
 *     <td>Construct a SmallVector that is filled with a certain number of copies of the
 *     given value</td>
 * </tr><tr>
 *     <td>SmallVector(InputIterator, InputIterator)</td>
 *     <td>Construct a SmallVector that is filled with copies of elements from the given
 *     range</td>
 * </tr><tr>
 *     <td>SmallVector(const SmallVector&)</td>
 *     <td>Copy constructor</td>
 * </tr><tr>
 *     <td>SmallVector(SmallVector&&)</td>
 *     <td>Move constructor</td>
 * </tr><tr>
 *     <td>SmallVector(std::initializer_list<ValueType>)</td>
 *     <td>Construct a SmallVector from an initializer list</td>
 * </tr><tr>
 *     <td>~SmallVector()</td>
 *     <td>Destructor</td>
 * </tr><tr>
 *     <th colspan="2">Assignment</th>
 * </tr><tr>
 *     <td>assign(SizeType, const ValueType&)</td>
 *     <td>Assign a certain number of copies of the given value</td>
 * </tr><tr>
 *     <td>assign(InputIterator, InputIterator)</td>
 *     <td>Assign copies of the elements from the given range</td>
 * </tr><tr>
 *     <td>assign(std::initializer_list<ValueType>)</td>
 *     <td>Assign the contents of an initializer list</td>
 * </tr><tr>
 *     <td>operator=(SmallVector const&)</td>
 *     <td>Copy assignment</td>
 * </tr><tr>
 *     <td>operator=(SmallVector&&)</td>
 *     <td>Move assignment</td>
 * </tr><tr>
 *     <td>operator=(std::initializer_list<ValueType>)</td>
 *     <td>Assign the contents of an initializer list</td>
 * </tr><tr>
 *     <th colspan="2">Element access</th>
 * </tr><tr>
 *     <td>operator[](SizeType)</td>
 *     <td>Access an element by index without bounds-checking</td>
 * </tr><tr>
 *     <td>at(SizeType)</td>
 *     <td>Access an element by index with bounds-checking</td>
 * </tr><tr>
 *     <td>front(), back()</td>
 *     <td>Access the first/last element</td>
 * </tr><tr>
 *     <td>data()</td>
 *     <td>Return a pointer to the contiguous data storage</td>
 * </tr><tr>
 *     <th colspan="2">Iterators</th>
 * </tr><tr>
 *     <td>begin(), cbegin()</td>
 *     <td>Return an iterator to the first element</td>
 * </tr><tr>
 *     <td>end(), cend()</td>
 *     <td>Return an iterator past the last element</td>
 * </tr><tr>
 *     <td>rbegin(), crbegin()</td>
 *     <td>Return an iterator to the first element of the reversed vector</td>
 * </tr><tr>
 *     <td>rend(), crend()</td>
 *     <td>Return an iterator past the last element of the reversed vector</td>
 * </tr><tr>
 *     <th colspan="2">Size &amp; capacity</th>
 * </tr><tr>
 *     <td>capacity()</td>
 *     <td>Return the number of elements that can currently be stored without allocating
 *     more memory</td>
 * </tr><tr>
 *     <td>empty()</td>
 *     <td>Determine if the vector is empty</td>
 * </tr><tr>
 *     <td>inner_capacity()</td>
 *     <td>Return the number of elements that can be stored internally without allocating
 *     heap memory</td>
 * </tr><tr>
 *     <td>max_size()</td>
 *     <td>Return the maximum number of elements that can theoretically be stored</td>
 * </tr><tr>
 *     <td>reserve(SizeType)</td>
 *     <td>Increase the capacity of the vector to the specified size</td>
 * </tr><tr>
 *     <td>resize(SizeType)</td>
 *     <td>Change the number of elements, filling up with default-constructed elements if
 *     necessary</td>
 * </tr><tr>
 *     <td>resize(SizeType, const ValueType&)</td>
 *     <td>Change the number of elements, filling up with copy-constructed elements if
 *     necessary</td>
 * </tr><tr>
 *     <td>shrink_to_fit()</td>
 *     <td>Reduce the capacity as far as possible while retaining all stored elements</td>
 * </tr><tr>
 *     <td>size()</td>
 *     <td>Return the number of elements that are currently stored</td>
 * </tr><tr>
 *     <th colspan="2">Content modification</th>
 * </tr><tr>
 *     <td>clear()</td>
 *     <td>Erase all elements without changing capacity</td>
 * </tr><tr>
 *     <td>\ref emplace "emplace(ConstIterator, ArgumentTypes&&...)"</td>
 *     <td>Construct an additional element at an arbitrary position</td>
 * </tr><tr>
 *     <td>\ref emplace_back "emplace_back(ArgumentTypes&&...)"</td>
 *     <td>Construct an additional element at the end</td>
 * </tr><tr>
 *     <td>erase(ConstIterator)</td>
 *     <td>Erase a single element</td>
 * </tr><tr>
 *     <td>erase(ConstIterator, ConstIterator)</td>
 *     <td>Erase a range of elements</td>
 * </tr><tr>
 *     <td>insert(ConstIterator, const ValueType&)</td>
 *     <td>Copy-insert a single element</td>
 * </tr><tr>
 *     <td>insert(ConstIterator, ValueType&&)</td>
 *     <td>Move-insert a single element</td>
 * </tr><tr>
 *     <td>insert(ConstIterator, SizeType, const ValueType&)</td>
 *     <td>Insert a number of copies of a given value</td>
 * </tr><tr>
 *     <td>insert(ConstIterator, InputIterator, InputIterator)</td>
 *     <td>Insert a range of values</td>
 * </tr><tr>
 *     <td>insert(ConstIterator, std::initializer_list<ValueType>)</td>
 *     <td>Insert elements from an initializer list</td>
 * </tr><tr>
 *     <td>pop_back()</td>
 *     <td>Remove the last element</td>
 * </tr><tr>
 *     <td>push_back(const ValueType&)</td>
 *     <td>Copy-insert one element at the end</td>
 * </tr><tr>
 *     <td>push_back(ValueType&&)</td>
 *     <td>Move-insert one element at the end</td>
 * </tr><tr>
 *     <td>swap(SmallVector&)</td>
 *     <td>Exchange the contents of this SmallVector with those of another one</td>
 * </tr>
 * </table>
 *
 * <h3>Associated free functions</h3>
 *
 * <table style="border-style: none;">
 * <tr>
 *     <td>operator==(const SmallVector&, const SmallVector&)</td>
 *     <td>Return true if both vectors have the same size() and the same elements</td>
 * </tr><tr>
 *     <td>operator!=(const SmallVector&, const SmallVector&)</td>
 *     <td>Return true if both vectors have a different size() or at least one different
 *     element</td>
 * </tr><tr>
 *     <td>swap(SmallVector&, SmallVector&)</td>
 *     <td>Exchange the contents two SmallVectors</td>
 * </tr>
 * </table>
 *
 * <h3>Member type overview</h3>
 *
 * <table style="border-style: none;">
 * <tr><td colspan="3">Member types are available in both CamelCase and snake_case.</td></tr>
 * <tr><td>\ref ValueType           </td><td>\ref value_type            </td><td>Type of the elements in the underlying container</td></tr>
 * <tr><td>\ref SizeType            </td><td>\ref size_type             </td><td>Unsigned integer type for indexing, number of elements, capacity</td></tr>
 * <tr><td>\ref DifferenceType      </td><td>\ref difference_type       </td><td>Signed integer type for storing the difference of two iterators</td></tr>
 * <tr><td>\ref Reference           </td><td>\ref reference             </td><td>Reference to an element</td></tr>
 * <tr><td>\ref ConstReference      </td><td>\ref const_reference       </td><td>Reference to a const element</td></tr>
 * <tr><td>\ref Iterator            </td><td>\ref iterator              </td><td>Iterator to an element</td></tr>
 * <tr><td>\ref ConstIterator       </td><td>\ref const_iterator        </td><td>Iterator to a const element</td></tr>
 * <tr><td>\ref ReverseIterator     </td><td>\ref reverse_iterator      </td><td>Iterator to an element in reversed container</td></tr>
 * <tr><td>\ref ConstReverseIterator</td><td>\ref const_reverse_iterator</td><td>Iterator to a const element in reversed container</td></tr>
 * </table>
 */
template <typename ElementT, size_t in_capacity>
class SmallVector
{
public:
    /// Type of the elements in the underlying container
    using ValueType = ElementT;
    /// \copydoc ValueType
    using value_type = ValueType;
    /// Unsigned integer type for indexing, number of elements, capacity
    using SizeType = uint32_t;
    /// \copydoc SizeType
    using size_type = SizeType;
    /// Signed integer type for the difference of two iterators
    using DifferenceType = std::ptrdiff_t;
    /// \copydoc DifferenceType
    using difference_type = DifferenceType;
    /// Reference to an element
    using Reference = ValueType&;
    /// \copydoc Reference
    using reference = Reference;
    /// Reference to a const element
    using ConstReference = const ValueType&;
    /// \copydoc ConstReference
    using const_reference = ConstReference;
    /// Iterator to an element
    using Iterator = ValueType*;
    /// \copydoc Iterator
    using iterator = Iterator;
    /// Iterator to a const element
    using ConstIterator = const ValueType*;
    /// \copydoc ConstIterator
    using const_iterator = ConstIterator;
    /// Iterator to an element in reversed container
    using ReverseIterator = std::reverse_iterator<Iterator>;
    /// \copydoc ReverseIterator
    using reverse_iterator = ReverseIterator;
    /// Iterator to a const element in reversed container
    using ConstReverseIterator = std::reverse_iterator<ConstIterator>;
    /// \copydoc ConstReverseIterator
    using const_reverse_iterator = ConstReverseIterator;

    /**
     * Construct an empty SmallVector.
     *
     * If the template argument is zero, a SmallVector based on std::vector with a
     * capacity of zero elements is generated. Use the \ref SmallVector(size_type)
     * constructor or call the resize() function afterwards to get a SmallVector
     * based on std::vector with nonzero capacity.
     */
    SmallVector() noexcept = default;

    /**
     * Construct a SmallVector that is filled with a certain number of default-initialized
     * elements.
     *
     * \param num_elements  The number of initial elements
     */
    explicit SmallVector(SizeType num_elements)
    {
        static_assert(std::is_default_constructible<ValueType>::value,
            "SmallVector: Element type is not default-constructible");

        reserve(num_elements);
        for (SizeType i = 0u; i != num_elements; ++i)
            ::new(static_cast<void*>(data() + i)) ValueType{};
        size_ = num_elements;
    }

    /**
     * Construct a SmallVector that is filled with a certain number of copies of the given
     * value.
     *
     * \param num_elements  The number of initial elements
     * \param value         The value to be copied to the initial elements
     */
    SmallVector(SizeType num_elements, const ValueType& value)
    {
        fill_empty_vector_with_copied_value(num_elements, value);
    }

    /**
     * Construct a SmallVector that is filled with copies of elements from the given
     * range.
     *
     * This constructor is not available if the iterators are of integral (and
     * non-pointer) type, in order to avoid confusion with
     * SmallVector(SizeType num_elements, const ValueType& value).
     *
     * \tparam InputIterator  An input iterator type
     *
     * \param first  Iterator to the first element to be copied
     * \param last   Iterator past the last element to be copied
     *
     * \warning The behavior is undefined if the number of input elements exceeds
     *          max_size().
     */
    template<class InputIterator,
             typename = std::enable_if_t<not std::is_integral<InputIterator>::value>>
    SmallVector(InputIterator first, InputIterator last)
    {
        fill_empty_vector_with_copied_range(first, last);
    }

    /**
     * Create a copy of another SmallVector with the same inner capacity.
     * The new vector is filled with copies of the original data elements. It has the same
     * size() as the original, but its capacity() might differ.
     *
     * \note The copy constructor cannot be used if the element type itself is not
     *       copyable.
     */
    SmallVector(const SmallVector& other)
        noexcept(std::is_nothrow_copy_constructible<ValueType>::value)
    {
        static_assert(std::is_copy_constructible<ValueType>::value == true,
            "SmallVector: Element type is not copy-constructible");
        fill_empty_vector_with_copied_range(other.cbegin(), other.cend());
    }

    /**
     * Move constructor: Create a SmallVector from the contents of another one with the
     * same inner capacity using move semantics. The new vector has the same size() as
     * the original, but its capacity() might differ. The original vector is empty() after
     * the operation.
     *
     * If the other vector has allocated storage, it is efficiently moved in en-bloc.
     * If it uses the internal storage, the behavior depends on the availability of a
     * non-throwing move constructor. If such a \c noexcept move constructor is available,
     * elements are moved in one-by-one. Otherwise, elements are copied in.
     *
     * If no exception is thrown, the other vector is guaranteed to be empty after the
     * call. If an exception is thrown, it is guaranteed to be unchanged if ValueType
     * is either copy-constructible or nothrow-move-constructible.
     */
    SmallVector(SmallVector&& other)
        noexcept(std::is_nothrow_move_constructible<ValueType>::value)
    {
        move_or_copy_all_elements_from(std::move(other));
    }

    /**
     * Construct a SmallVector that is filled with copies of the elements from a given
     * initializer list.
     *
     * \warning The behavior is undefined if the number of elements in the initializer
     *          list exceeds max_size().
     */
    SmallVector(std::initializer_list<ValueType> init)
    {
        fill_empty_vector_with_copied_range(init.begin(), init.end());
    }

    /// Destructor: Destroys all stored elements and frees all allocated memory.
    ~SmallVector()
    {
        clear();

        if (is_storage_allocated())
            delete[] data_ptr_;
    }

    /**
     * Fill the vector with a certain number of copies of the given value after clearing
     * all previous contents.
     *
     * \param num_elements  The number of initial elements
     * \param value         The value to be copied into the vector
     */
    void assign(SizeType num_elements, const ValueType& value)
    {
        clear();
        fill_empty_vector_with_copied_value(num_elements, value);
    }

    /**
     * Fill the vector with copies of elements from the given range.
     *
     * This overload is not available if the iterators are of integral (and non-pointer)
     * type, in order to avoid confusion with
     * assign(SizeType num_elements, const ValueType& value).
     *
     * \tparam InputIterator  An input iterator type
     *
     * \param first  Iterator to the first element to be copied
     * \param last   Iterator past the last element to be copied
     *
     * \note
     * Unlike the corresponding function from `std::vector`, `SmallVector::assign()` does
     * not allow narrowing conversions.
     *
     * \warning
     * The behavior is undefined if the number of input elements exceeds max_size().
     */
    template<class InputIterator,
        typename = std::enable_if_t<not std::is_integral<InputIterator>::value>>
    void assign(InputIterator first, InputIterator last)
    {
        clear();
        fill_empty_vector_with_copied_range(first, last);
    }

    /**
     * Assign the elements of an initializer list to this vector after clearing all
     * previous contents.
     *
     * \warning The behavior is undefined if the number of elements in the initializer
     *          list exceeds max_size().
     */
    void assign(std::initializer_list<ValueType> init)
    {
        const auto num_elements = static_cast<SizeType>(init.size());

        clear();
        reserve(num_elements);
        std::uninitialized_copy(init.begin(), init.end(), data());
        size_ = num_elements;
    }

    /**
     * Return a reference to the element at the specified index with bounds-checking.
     * \exception std::out_of_range is thrown if idx >= size()
     */
    constexpr Reference at(SizeType idx)
    {
        if (idx >= size_)
            throw std::out_of_range(cat("Index out of range: ", idx, " >= ", size_));
        return *(data() + idx);
    }

    /// Return a const reference to the element at the specified index.
    constexpr ConstReference at(SizeType idx) const
    {
        if (idx >= size_)
            throw std::out_of_range(cat("Index out of range: ", idx, " >= ", size_));
        return *(data() + idx);
    }

    /**
     * Return a reference to the last element in the vector.
     * The behavior is undefined if the vector is empty.
     */
    constexpr Reference back() noexcept
    {
        return *(data_end() - 1);
    }

    /**
     * Return a const reference to the last element in the vector.
     * The behavior is undefined if the vector is empty.
     */
    constexpr ConstReference back() const noexcept
    {
        return *(data_end() - 1);
    }

    /**
     * Return an iterator to the first element of the vector.
     * If the vector is empty, this iterator cannot be dereferenced.
     */
    constexpr Iterator begin() noexcept
    {
        return data();
    }

    /**
     * Return a const iterator to the first element of the vector.
     * If the vector is empty, this iterator cannot be dereferenced.
     */
    constexpr ConstIterator begin() const noexcept
    {
        return data();
    }

    /**
     * Return the number of elements that can currently be stored in this vector without
     * having to allocate more memory.
     */
    constexpr SizeType capacity() const noexcept { return capacity_; }

    /**
     * Return a const iterator to the first element of the vector.
     * If the vector is empty, this iterator cannot be dereferenced.
     */
    constexpr ConstIterator cbegin() const noexcept
    {
        return begin();
    }

    /**
     * Return a const iterator pointing past the last element of the vector.
     * This iterator cannot be dereferenced.
     */
    constexpr ConstIterator cend() const noexcept
    {
        return end();
    }

    /**
     * Erase all elements from the container without changing its capacity.
     * The size() is zero after this call.
     */
    void clear() noexcept
    {
        destroy_range(data(), data_end());
        size_ = 0;
    }

    /**
     * Return a const reverse iterator to the first element of the reversed vector (which
     * is the last element of the actual vector).
     *
     * The iterator cannot be dereferenced if the vector is empty.
     */
    ConstReverseIterator crbegin() noexcept
    {
        return std::make_reverse_iterator(end());
    }

    /**
     * Return a const reverse iterator pointing past the last element of the reversed
     * vector.
     *
     * The iterator cannot be dereferenced if the vector is empty.
     */
    ConstReverseIterator crend() noexcept
    {
        return std::make_reverse_iterator(begin());
    }

    /**
     * Return a pointer to the contiguous data storage of the vector.
     * If the vector is empty, the pointer is not dereferencable.
     */
    constexpr ValueType* data() noexcept
    {
        return reinterpret_cast<ValueType*>(data_ptr_);
    }

    /**
     * Return a pointer to the contiguous data storage of the vector.
     * If the vector is empty, the pointer is not dereferencable.
     */
    constexpr const ValueType* data() const noexcept
    {
        return reinterpret_cast<const ValueType*>(data_ptr_);
    }

    /**
     * Construct an additional element at an arbitrary position in the vector.
     *
     * The new element is inserted at the position indicated by pos; pre-existing
     * elements at and after this position are moved backwards. Other parameters
     * of emplace() are passed to the element constructor via perfect forwarding.
     *
     * If pos == end(), the new element is constructed in-place at the end of the vector.
     * Otherwise, it is first constructed on the stack and then move-assigned into the
     * vector. If the constructor of the new element throws, the contents of the vector
     * are not changed, but the capacity might differ.
     *
     * All existing iterators are invalidated if the new size() exceeds the original
     * capacity(). Otherwise, only the iterators to elements at or after pos are
     * invalidated.
     *
     * \param pos        Constant iterator to the position at which the new element should
     *                   be constructed.
     * \param arguments  List of arguments for the element constructor.
     *
     * \returns an iterator to the new element.
     *
     * \exception std::length_error is thrown if the maximum capacity of the container is
     *            reached and cannot be increased.
     */
    template <typename... ArgumentTypes>
    Iterator emplace(ConstIterator pos, ArgumentTypes&&... arguments)
    {
        if (pos == cend())
        {
            emplace_back(std::forward<ArgumentTypes>(arguments)...);
            return data_end() - 1;
        }

        ValueType v(std::forward<ArgumentTypes>(arguments)...);

        return insert_single_value(pos, std::move(v));
    }

    /**
     * Construct an additional element at the end of the buffer.
     *
     * The parameters of emplace_back() are passed to the element constructor via
     * perfect forwarding. If the constructor of the new element throws, the contents of
     * the vector are not changed, but the capacity might differ.
     *
     * All existing iterators are invalidated if the new size() exceeds the original
     * capacity(). Otherwise, only the end() iterator is invalidated.
     *
     * \param arguments  List of arguments for the element constructor.
     *
     * \returns a reference to the new element.
     *
     * \exception std::length_error is thrown if the maximum capacity of the container is
     *            reached and cannot be increased.
     */
    template <typename... ArgumentTypes>
    Reference emplace_back(ArgumentTypes&&... arguments)
    {
        if (size_ == capacity_)
            grow();

        ::new(static_cast<void*>(data_end()))
            ValueType(std::forward<ArgumentTypes>(arguments)...);

        ++size_;

        return back();
    }

    /// Determine if the vector is empty.
    constexpr bool empty() const noexcept { return size_ == 0u; }

    /**
     * Return an iterator pointing past the last element of the vector.
     * This iterator cannot be dereferenced.
     */
    constexpr Iterator end() noexcept
    {
        return data_end();
    }

    /**
     * Return a const iterator pointing past the last element of the vector.
     * This iterator cannot be dereferenced.
     */
    constexpr ConstIterator end() const noexcept
    {
        return data_end();
    }

    /**
     * Erase a single element from the vector, moving elements behind it forward.
     * This call does not change the capacity. Iterators pointing at or after the deleted
     * element are invalidated.
     *
     * \param pos  Iterator to the element to be erased. This iterator must be
     *             dereferenceable, so end() is not allowed.
     *
     * \returns an iterator to the element following the erased one or end() if
     *          the element at the end of the vector was deleted.
     */
    Iterator erase(ConstIterator pos)
    {
        return erase(pos, pos + 1);
    }

    /**
     * Erase a range of elements from the vector, moving elements behind the range
     * forward.
     * This call does not change the capacity. Iterators pointing at or after the deleted
     * range are invalidated.
     *
     * \param first  Iterator to the first element to be erased.
     * \param last   Iterator past the last element to be erased.
     *
     * \returns an iterator to the element following the last erased one or end() if
     *          elements at the end of the vector were deleted. If the range to be
     *          erased was empty, \c last is returned.
     */
    Iterator erase(ConstIterator first, ConstIterator last)
    {
        auto range_begin = const_cast<Iterator>(first);
        auto range_end = const_cast<Iterator>(last);
        auto num_elements = range_end - range_begin;

        std::move(range_end, end(), range_begin);
        destroy_range(end() - num_elements, end());

        size_ -= static_cast<SizeType>(num_elements);

        return range_begin;
    }

    /**
     * Return a reference to the first element in the vector.
     * The behavior is undefined if the vector is empty.
     */
    constexpr Reference front() noexcept
    {
        return *data();
    }

    /**
     * Return a const reference to the first element in the vector.
     * The behavior is undefined if the vector is empty.
     */
    constexpr ConstReference front() const noexcept
    {
        return *data();
    }

    /**
     * Return the number of elements this SmallVector can hold internally without having
     * to allocate storage.
     */
    constexpr SizeType inner_capacity() const noexcept { return in_capacity; }

    /**
     * Insert a single element before the indicated position.
     * This increases the size of the container by 1. All elements starting from the
     * indicated position are moved backwards.
     * \param pos    Iterator to the existing element in front of which the new value
     *               should be inserted.
     * \param value  The value to be copied into the container.
     * \returns an iterator to the newly inserted element.
     */
    Iterator insert(ConstIterator pos, const ValueType& value)
    {
        return insert_single_value(pos, value);
    }

    /**
     * Insert a single element before the indicated position.
     * This increases the size of the container by 1. All elements starting from the
     * indicated position are moved backwards.
     * \param pos    Iterator to the existing element in front of which the new value
     *               should be inserted.
     * \param value  The value to be moved into the container.
     * \returns an iterator to the newly inserted element.
     */
    Iterator insert(ConstIterator pos, ValueType&& value)
    {
        return insert_single_value(pos, std::move(value));
    }

    /**
     * Insert a number of copies of the given value before the indicated position.
     * \param pos           Iterator to the existing element in front of which the new
     *                      value should be inserted.
     * \param num_elements  The number of elements to be inserted.
     * \param value         The value to be moved into the container.
     * \returns an iterator to the first of the inserted elements or pos if
     *          num_elements == 0.
     */
    Iterator insert(ConstIterator pos, SizeType num_elements, const ValueType& value)
    {
        const auto idx = static_cast<SizeType>(pos - begin());
        if (num_elements < 1)
            return begin() + idx;

        const SizeType num_assignable = make_space_at_idx_for_n_elements(idx, num_elements);
        Iterator insert_pos = begin() + idx;

        // Copy-assign to moved-from elements
        std::fill_n(insert_pos, num_assignable, value);

        // Copy the remaining values into uninitialized cells
        copy_value_into_uninitialized_cells(size_, num_elements - num_assignable, value);

        size_ += num_elements;

        return insert_pos;
    }

    /**
     * Insert a range of values before the indicated position.
     * This overload is not available if the iterators are of integral (and non-pointer)
     * type, in order to avoid confusion with
     * insert(ConstIterator pos, SizeType num_elements, const ValueType& value).
     *
     * \tparam InputIterator  An input iterator type.
     *
     * \param pos    Iterator to the existing element in front of which the new values
     *               should be inserted.
     * \param first  Iterator to the first element to be copied.
     * \param last   Iterator pointing past the last element to be copied.
     *
     * \returns an iterator to the first of the inserted elements or pos if the range
     *          is empty.
     *
     * \warning The behavior is undefined if the total number of elements after insertion
     *          would exceed max_size().
     */
    template<class InputIterator,
             typename = std::enable_if_t<not std::is_integral<InputIterator>::value>>
    Iterator insert(ConstIterator pos, InputIterator first, InputIterator last)
    {
        const auto idx = static_cast<SizeType>(pos - begin());
        const auto num_elements = static_cast<SizeType>(std::distance(first, last));
        if (num_elements < 1)
            return begin() + idx;

        const SizeType num_assignable = make_space_at_idx_for_n_elements(idx, num_elements);
        Iterator insert_pos = begin() + idx;

        // Copy-assign to moved-from elements
        std::copy_n(first, num_assignable, insert_pos);

        // Copy the remaining values into uninitialized cells
        copy_range_into_uninitialized_cells(size_, first + num_assignable, last);

        size_ += num_elements;

        return insert_pos;
    }

    /**
     * Insert elements from an initializer list before the indicated position.
     *
     * \param pos   Iterator to the existing element in front of which the new values
     *              should be inserted.
     * \param init  An initializer list with the values to be copied.
     *
     * \returns an iterator to the first of the inserted elements or pos if the range
     *          is empty.
     *
     * \warning The behavior is undefined if the total number of elements after insertion
     *          would exceed max_size().
     */
    Iterator insert(ConstIterator pos, std::initializer_list<ValueType> init)
    {
        return insert(pos, init.begin(), init.end());
    }

    /**
     * Return the maximum number of elements that this vector can theoretically hold.
     * This value reflects only limitations of the implementation. It is possible that
     * the size of the container is effectively limited to a much smaller value because of
     * limited available memory.
     */
    constexpr SizeType max_size() const noexcept
    {
        return std::numeric_limits<SizeType>::max();
    }

    /**
     * Copy assignment operator: Copy all elements from another SmallVector after clearing
     * all previous contents.
     *
     * \note
     * Copy assignment is only possible if the element type itself is copyable.
     */
    SmallVector& operator=(const SmallVector& other)
        noexcept(std::is_nothrow_copy_assignable<ValueType>::value)
    {
        if (&other != this)
        {
            clear();
            reserve(other.size());
            std::uninitialized_copy(other.cbegin(), other.cend(), data());
            size_ = other.size();
        }

        return *this;
    }

    /**
     * Move assignment operator: Assign all of the elements from another vector to this
     * one using move semantics after clearing all previous contents.
     *
     * If the other vector has allocated storage, it is efficiently moved in en-bloc.
     * If it uses the internal storage, the behavior depends on the availability of a
     * non-throwing move constructor. If such a \c noexcept move constructor is available,
     * elements are moved in one-by-one. Otherwise, elements are copied in.
     *
     * If no exception is thrown, the other vector is guaranteed to be empty after the
     * call. If an exception is thrown, it is guaranteed to be unchanged if ValueType is
     * either copy-constructible or nothrow-move-constructible.
     */
    SmallVector& operator=(SmallVector&& other)
        noexcept(std::is_nothrow_move_constructible<ValueType>::value)
    {
        if (&other != this)
        {
            clear();
            if (is_storage_allocated())
                delete[] data_ptr_;
            move_or_copy_all_elements_from(std::move(other));
        }

        return *this;
    }

    /**
     * Assign the elements of an initializer list to this vector after clearing all
     * previous contents.
     */
    SmallVector& operator=(std::initializer_list<ValueType> init)
    {
        assign(init);
        return *this;
    }

    /**
     * Equality operator: Return true if both vectors have the same size() and the same
     * elements.
     * \param lhs  First vector.
     * \param rhs  Second vector.
     * \returns true if both vectors are equal, false otherwise.
     */
    friend bool operator==(const SmallVector& lhs, const SmallVector& rhs)
    {
        return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

    /**
     * Inequality operator: Return true if both vectors have a different size() or at
     * least one different element.
     * \param lhs  First vector.
     * \param rhs  Second vector.
     * \returns false if both vectors are equal, true otherwise.
     */
    friend bool operator!=(const SmallVector& lhs, const SmallVector& rhs)
    {
        return not (lhs == rhs);
    }

    /// Return a reference to the element at the specified index.
    constexpr Reference operator[](SizeType idx)
    {
        return *(data() + idx);
    }

    /// Return a const reference to the element at the specified index.
    constexpr ConstReference operator[](SizeType idx) const
    {
        return *(data() + idx);
    }

    /**
     * Remove the last element from the vector.
     * Calling pop_back() on an empty vector results in undefined behavior.
     */
    void pop_back()
    {
        --size_;
        auto p = data_end();
        p->~ValueType();
    }

    /**
     * Copy one element to the end of the buffer.
     *
     * All existing iterators are invalidated if the new size() exceeds the original
     * capacity(). Otherwise, only the end() iterator is invalidated.
     *
     * \exception std::length_error is thrown if the maximum capacity of the container is
     *            reached and cannot be increased.
     */
    void push_back(const ValueType& value)
    {
        if (size_ == capacity_)
            grow();

        ::new(static_cast<void*>(data_end())) ValueType(value);

        ++size_;
    }

    /**
     * Move one element to the end of the buffer.
     *
     * This invalidates all existing iterators.
     *
     * \exception std::length_error is thrown if the maximum capacity of the container is
     *            reached and cannot be increased.
     */
    void push_back(ValueType&& value)
    {
        if (size_ == capacity_)
            grow();

        ::new(static_cast<void*>(data_end())) ValueType(std::move(value));

        ++size_;
    }

    /**
     * Return a reverse iterator to the first element of the reversed vector (which is the
     * last element of the actual vector).
     *
     * The iterator cannot be dereferenced if the vector is empty.
     */
    ReverseIterator rbegin() noexcept
    {
        return std::make_reverse_iterator(end());
    }

    /**
     * Return a reverse iterator pointing past the last element of the reversed vector.
     *
     * The iterator cannot be dereferenced if the vector is empty.
     */
    ReverseIterator rend() noexcept
    {
        return std::make_reverse_iterator(begin());
    }

    /**
     * Increase the capacity of the vector to the specified size.
     * This call can only enlarge the container. If new_capacity < capacity(), the call
     * does nothing.
     * \param new_capacity  The new capacity of the vector
     *
     * \see shrink_to_fit() can be used to shrink the vector
     */
    void reserve(SizeType new_capacity)
    {
        if (new_capacity <= capacity_)
            return;

        auto new_data = std::make_unique<AlignedStorage[]>(new_capacity);

        const auto d_end = data_end();

        uninitialized_move_or_copy(data(), d_end, reinterpret_cast<ValueType*>(new_data.get()));
        destroy_range(data(), d_end);

        if (is_storage_allocated())
            delete[] data_ptr_;

        data_ptr_ = new_data.release();
        capacity_ = new_capacity;
    }

    /**
     * Change the number of elements in the container.
     *
     * If the number of elements is decreased, superfluous elements are removed from the
     * end of the container and the capacity is not changed. If the number of elements is
     * increased, default-constructed elements are added at the end of the container and
     * the capacity can grow.
     *
     * \param num_elements  The desired number of elements after resizing
     *
     * \note
     * Exception guarantee: If the element default constructor throws while enlarging the
     * vector, the size and content of the container are not changed. However, the
     * capacity might change, and therefore iterators can be invalidated.
     */
    void resize(SizeType num_elements)
    {
        static_assert(std::is_default_constructible<ValueType>::value,
            "SmallVector: For using resize(), element type must be default-constructible");

        if (num_elements < size_)
        {
            destroy_range(data() + num_elements, data_end());
        }
        else if (num_elements > size_)
        {
            reserve(num_elements);
            fill_uninitialized_cells_with_default_constructed_elements(size_,
                num_elements - size_);
        }

        size_ = num_elements;
    }

    /**
     * Change the number of elements in the container.
     *
     * If the number of elements is decreased, superfluous elements are removed from the
     * end of the container and the capacity is not changed. If the number of elements is
     * increased, copies of the specified element are added at the end of the container
     * and the capacity can grow.
     *
     * \param num_elements  The desired number of elements after resizing
     * \param element       The element to be copied into the new cells when enlarging
     */
    void resize(SizeType num_elements, const ValueType& element)
    {
        if (num_elements < size_)
        {
            destroy_range(data() + num_elements, data_end());
        }
        else if (num_elements > size_)
        {
            reserve(num_elements);
            copy_value_into_uninitialized_cells(size_, num_elements - size_, element);
        }

        size_ = num_elements;
    }

    /**
     * Reduce the capacity as far as possible while retaining all stored elements.
     * This might free up some space on the heap, and it invalidates existing iterators.
     */
    void shrink_to_fit()
    {
        const SizeType new_capacity = std::max(inner_capacity(), size_);

        // No shrinking required?
        if (new_capacity == capacity_)
            return;

        AlignedStorage* new_data;
        auto new_memory = std::unique_ptr<AlignedStorage[]>{};

        if (new_capacity == inner_capacity()) {
            new_data = internal_array_.data();
        } else {
            new_memory = std::make_unique<AlignedStorage[]>(new_capacity);
            new_data = new_memory.get();
        }

        const auto d_end = data_end();

        uninitialized_move_or_copy(data(), d_end, reinterpret_cast<ValueType*>(new_data));
        destroy_range(data(), d_end);

        if (is_storage_allocated())
            delete[] data_ptr_;

        data_ptr_ = new_memory ? new_memory.release() : new_data;
        capacity_ = new_capacity;
    }

    /// Return the number of elements that are currently stored.
    constexpr SizeType size() const noexcept { return size_; }

    /**
     * Exchange the contents of this SmallVector with those of another one.
     *
     * If either this or the other vector have internally stored elements
     * (capacity() <= inner_capacity()), this function falls back to element-wise
     * swapping. Otherwise, the heap-allocated buffers are swapped directly.
     */
    void swap(SmallVector& other)
    {
        if (is_storage_allocated())
        {
            if (other.is_storage_allocated())
                swap_heap_with_heap(*this, other);
            else
                swap_heap_with_internal(*this, other);
        }
        else
        {
            if (other.is_storage_allocated())
                swap_heap_with_internal(other, *this);
            else
                swap_internal_with_internal(*this, other);
        }
    }

private:
    using AlignedStorage =
        typename std::aligned_storage<sizeof(ValueType), alignof(ValueType)>::type;

    /**
     * This union either holds the storage for the "internal" elements or, if the vector
     * has grown beyond that size, a pointer to storage on the heap.
     */
    std::array<AlignedStorage, in_capacity> internal_array_;

    /// Pointer to internal or external contiguous data storage.
    AlignedStorage* data_ptr_{ internal_array_.data() };

    /// Capacity of the vector (i.e. number of elements that can be stored without
    /// enlarging the container)
    SizeType capacity_{ in_capacity };
    /// Number of elements stored in the container.
    SizeType size_{ 0u };

    /**
     * Copy a range of values into uninitialized cells.
     *
     * If the element copy constructor throws an exception, all elements copied until this
     * point are destroyed.
     *
     * \tparam InputIterator  An input iterator type
     *
     * \param pos    The index of the first cell to be filled
     * \param first  Iterator to the first element to be copied
     * \param last   Iterator past the last element to be copied
     */
    template<class InputIterator>
    void copy_range_into_uninitialized_cells(SizeType pos, InputIterator first,
                                             InputIterator last)
    {
        auto data_ptr = data() + pos;

        try
        {
            for (auto it = first; it != last; ++it)
            {
                ::new(static_cast<void*>(data_ptr)) ValueType{ *it };
                ++data_ptr;
            }
        }
        catch (...)
        {
            for (auto p = data() + pos; p != data_ptr; ++p)
                p->~ValueType();

            throw;
        }
    }

    /**
     * Fill a range of uninitialized cells with copies of the given element.
     * \param pos           The index of the first cell to be filled.
     * \param num_elements  The number of cells to be filled.
     * \param value         The value to be copied into the range.
     */
    void copy_value_into_uninitialized_cells(SizeType pos, SizeType num_elements,
                                             const ValueType& value)
    {
        const auto start_ptr = data() + pos;
        const auto end_ptr = start_ptr + num_elements;
        for (auto p = start_ptr; p != end_ptr; ++p)
            ::new(static_cast<void*>(p)) ValueType(value);
    }

    /// Return a non-dereferencable pointer past the last element.
    constexpr ValueType* data_end() noexcept
    {
        return reinterpret_cast<ValueType*>(data_ptr_) + size_;
    }

    /// Return a non-dereferencable pointer past the last element.
    constexpr const ValueType* data_end() const noexcept
    {
        return reinterpret_cast<const ValueType*>(data_ptr_) + size_;
    }

    /**
     * Call the destructor on all elements in the given range, turning the occupied space
     * into uninitialized memory.
     * \param it_start  Iterator to the first element to be destroyed
     * \param it_end    Iterator past the last element to be destroyed
     *
     * \note Can be replaced with std::destroy() from C++17 onwards.
     */
    static void destroy_range(Iterator it_start, Iterator it_end) noexcept
    {
        for (auto it = it_start; it != it_end; ++it)
            it->~ValueType();
    }

    /**
     * Fill the vector with copies of elements from the given range.
     * The vector must be empty when this function is called.
     *
     * \tparam InputIterator  An input iterator type
     *
     * \param first  Iterator to the first element to be copied
     * \param last   Iterator past the last element to be copied
     *
     * \warning
     * The vector must be empty when this function is called.
     *
     * \pre `size() == 0`
     */
    template<typename InputIterator>
    void fill_empty_vector_with_copied_range(InputIterator first, InputIterator last)
    {
        const auto num_elements = static_cast<SizeType>(std::distance(first, last));

        reserve(num_elements);
        copy_range_into_uninitialized_cells(0u, first, last);
        size_ = num_elements;
    }

    /**
     * Fill the vector with num_elements copies of the given value.
     *
     * \warning
     * The vector must be empty when this function is called.
     *
     * \pre `size() == 0`
     */
    void fill_empty_vector_with_copied_value(SizeType num_elements, const ValueType& value)
    {
        reserve(num_elements);
        copy_value_into_uninitialized_cells(0u, num_elements, value);
        size_ = num_elements;
    }

    /**
     * Fill a range of uninitialized cells with default-constructed elements.
     * \param pos           The index of the first cell to be filled.
     * \param num_elements  The number of cells to be filled.
     *
     * \note Strong exception guarantee: If the element default constructor throws at any
     *       point, all previously uninitialized cells are left uninitialized.
     */
    void fill_uninitialized_cells_with_default_constructed_elements(SizeType pos,
        SizeType num_elements)
    {
        const auto start_ptr = data() + pos;
        const auto end_ptr = start_ptr + num_elements;
        auto ptr = start_ptr;

        try
        {
            for (; ptr != end_ptr; ++ptr)
                ::new(static_cast<void*>(ptr)) ValueType{};
        }
        catch (...)
        {
            destroy_range(start_ptr, ptr);
            throw;
        }
    }

    /**
     * Increase the capacity of the vector, typically by ~50%.
     *
     * If this call succeeds without throwing an exception, it is guaranteed that the
     * capacity has been increased at least by 1.
     *
     * \exception std::length_error is thrown if the maximum capacity of the container is
     *            reached and cannot be increased.
     */
    void grow()
    {
        const auto remaining_space = std::numeric_limits<SizeType>::max() - capacity_;

        if (remaining_space == 0u)
            throw std::length_error("Max. capacity reached");

        SizeType increase = capacity_ / 2u;
        if (increase > remaining_space)
            increase = remaining_space;
        else if (increase == 0u)
            increase = 1u;

        reserve(capacity_ + increase);
    }

    /// Insert a single value before the given position.
    template <typename T>
    Iterator insert_single_value(ConstIterator pos, T&& value)
    {
        if (pos == end())
        {
            push_back(std::forward<T>(value));
            return begin() + size_ - 1;
        }

        const auto idx = static_cast<SizeType>(pos - begin());

        if (size_ == capacity_)
            grow();

        Iterator insert_pos = begin() + idx;

        auto data_ptr = data();

        // Construct new back() element by moving from the old back()
        ::new(static_cast<void*>(data_ptr + size_)) ValueType(std::move(*(data_ptr + size_ - 1)));

        std::move_backward(insert_pos, data_ptr + size_ - 1, data_ptr + size_);
        ++size_;

        *insert_pos = std::forward<T>(value);

        return insert_pos;
    }

    /// Determine if this vector is using allocated external storage.
    bool is_storage_allocated() const noexcept
    {
        return capacity_ > inner_capacity();
    }

    /**
     * Reserve space to hold num_elements new elements and create space for them at
     * the position idx, moving existing elements backwards if necessary.
     * The newly created "hole" consists of
     * - some cells with moved-from values whose number is returned. These cells can be
     *   assigned to directly.
     * - some uninitialized cells which require placement new to be filled.
     * \returns the number of moved-from elements in the new space that can be assigned
     *          to directly.
     */
    SizeType make_space_at_idx_for_n_elements(SizeType idx, SizeType num_elements)
    {
        const auto new_size = size_ + num_elements;

        if (new_size > capacity_)
            reserve(new_size);

        auto data_ptr = data();

        // We have a total number of elements that need to be shifted backwards,
        // of which some need to be move-initialized at the end of the vector
        // and some simply need to be moved from one element to another.
        const SizeType num_shifted = size_ - idx;
        const SizeType num_move_initialized = std::min(num_shifted, num_elements);
        const SizeType num_moved = num_shifted - num_move_initialized;

        // Move-initialize elements in uninitialized space at the back of the container
        const auto from_ptr = data_ptr + size_ - num_move_initialized;
        const auto to_ptr = from_ptr + num_elements;
        for (SizeType i = 0; i != num_move_initialized; ++i)
            ::new(static_cast<void*>(to_ptr + i)) ValueType(std::move(*(from_ptr + i)));

        // Move elements backwards within old vector size
        std::move_backward(data_ptr + idx, data_ptr + idx + num_moved, to_ptr);

        return num_move_initialized;
    }

    /**
     * Steal all elements from another SmallVector with move semantics.
     *
     * If the other vector has allocated storage, it is efficiently moved in en-bloc.
     * If it uses the internal storage, the behavior depends on the availability of a
     * non-throwing move constructor. If such a \c noexcept move constructor is available,
     * elements are moved in one-by-one. Otherwise, elements are copied in.
     *
     * If no exception is thrown, other is guaranteed to be empty after the call. If an
     * exception is thrown, other is guaranteed to be unchanged if ValueType is either
     * copy-constructible or nothrow-move-constructible.
     *
     * \warning
     * This function does not clear the vector before moving the other elements in and
     * performs no check for self-assignment.
     *
     * \pre `size() == 0 and is_allocated() == false and &other != this`
     */
    void move_or_copy_all_elements_from(SmallVector&& other)
        noexcept(std::is_nothrow_move_constructible<ValueType>::value)
    {
        // Can we simply steal the complete allocated storage?
        if (other.is_storage_allocated())
        {
            data_ptr_ = other.data_ptr_;    other.data_ptr_ = other.internal_array_.data();
            capacity_ = other.capacity_;    other.capacity_ = other.inner_capacity();
            size_ = other.size_;            other.size_ = 0u;
        }
        else // otherwise fall back to moving (or at least copying) all elements
        {
            data_ptr_ = internal_array_.data();
            capacity_ = in_capacity;
            uninitialized_move_or_copy(other.begin(), other.end(), data());
            size_ = other.size();
            other.clear();
        }
    }

    /**
     * Swap two SmallVectors, assuming that both have elements on the heap.
     *
     * \pre `a.is_storage_allocated() == true and b.is_storage_allocated() == true`
     */
    static void swap_heap_with_heap(SmallVector &a, SmallVector &b) noexcept
    {
        std::swap(a.data_ptr_, b.data_ptr_);
        std::swap(a.capacity_, b.capacity_);
        std::swap(a.size_, b.size_);
    }

    /**
     * Swap two SmallVectors, assuming that the first has elements on the heap and the
     * second only internally stored ones.
     *
     * \pre `a.is_storage_allocated() == true and b.is_storage_allocated() == false`
     */
    static void swap_heap_with_internal(SmallVector &a, SmallVector &b)
    {
        uninitialized_move_or_copy(b.begin(), b.end(),
            reinterpret_cast<ValueType*>(a.internal_array_.data()));
        destroy_range(b.begin(), b.end());

        b.data_ptr_ = a.data_ptr_;
        a.data_ptr_ = a.internal_array_.data();

        std::swap(a.capacity_, b.capacity_);
        std::swap(a.size_, b.size_);
    }

    /**
     * Swap two SmallVectors, assuming that both have only internally stored elements.
     *
     * \pre `a.is_storage_allocated() == false and b.is_storage_allocated() == false`
     */
    static void swap_internal_with_internal(SmallVector &a, SmallVector &b)
    {
        if (a.size_ <= b.size_)
        {
            for (SmallVector::SizeType i = 0; i != a.size_; ++i)
                std::swap(a.data()[i], b.data()[i]);

            uninitialized_move_or_copy(b.begin() + a.size_, b.end(), a.begin() + a.size_);
            destroy_range(b.begin() + a.size_, b.end());
        }
        else
        {
            for (SizeType i = 0; i != b.size_; ++i)
                std::swap(a.data()[i], b.data()[i]);

            uninitialized_move_or_copy(a.begin() + b.size_, a.end(), b.begin() + b.size_);
            destroy_range(a.begin() + b.size_, a.end());
        }

        std::swap(a.size_, b.size_);
    }

    /**
     * A custom replacement for std::uninitialized move from C++17, specialized for types
     * that can throw on move.
     */
    template <typename T>
    static typename std::enable_if_t<not std::is_nothrow_move_constructible<T>::value>
    uninitialized_move(T* src_begin, T* src_end, T* dest_begin)
    {
        auto src = src_begin;
        auto dest = dest_begin;

        try
        {
            while (src != src_end)
            {
                ::new (static_cast<void*>(dest)) ValueType(std::move(*src));
                ++src;
                ++dest;
            }
        }
        catch (...)
        {
            for (auto p = dest_begin; p != dest; ++p)
                p->~ValueType();

            throw;
        }
    }

    /**
     * A custom replacement for std::uninitialized move from C++17, specialized for types
     * that cannot throw on move.
     */
    template <typename T>
    static typename std::enable_if_t<std::is_nothrow_move_constructible<T>::value>
    uninitialized_move(T* src_begin, T* src_end, T* dest_begin) noexcept
    {
        auto src = src_begin;
        auto dest = dest_begin;

        while (src != src_end)
        {
            ::new (static_cast<void*>(dest)) ValueType(std::move(*src));
            ++src;
            ++dest;
        }
    }

    /// Uninitialized-move-or-copy: Becomes uninitialized-move for types that cannot throw
    /// on move.
    template <typename T>
    static typename std::enable_if_t<std::is_nothrow_move_constructible<T>::value>
    uninitialized_move_or_copy(T* src_begin, T* src_end, T* dest_begin) noexcept
    {
        uninitialized_move(src_begin, src_end, dest_begin);
    }

    /// Uninitialized-move-or-copy: Becomes uninitialized-copy for types that can throw on
    /// move and are copy-constructible.
    template <typename T>
    static typename std::enable_if_t<not std::is_nothrow_move_constructible<T>::value and
                                     std::is_copy_constructible<T>::value>
    uninitialized_move_or_copy(T* src_begin, T* src_end, T* dest_begin)
    {
        std::uninitialized_copy(src_begin, src_end, dest_begin);
    }

    /// Uninitialized-move-or-copy: Becomes uninitialized-move for move-only types that
    /// can throw on move.
    template <typename T>
    static typename std::enable_if_t<not std::is_nothrow_move_constructible<T>::value
                                     and not std::is_copy_constructible<T>::value>
    uninitialized_move_or_copy(T* src_begin, T* src_end, T* dest_begin)
    {
        uninitialized_move(src_begin, src_end, dest_begin);
    }
};


/**
 * Exchange the contents of one SmallVector with those of another one.
 *
 * If either this or the other vector have internally stored elements
 * (capacity() <= inner_capacity()), this function falls back to element-wise swapping.
 * Otherwise, the heap-allocated buffers are swapped directly.
 */
template<typename ElementT, size_t in_capacity>
void swap(SmallVector<ElementT,in_capacity>& a, SmallVector<ElementT, in_capacity>& b)
{
    a.swap(b);
}

/// @}

} // namespace gul14

#endif

// vi:ts=4:sw=4:sts=4:et
