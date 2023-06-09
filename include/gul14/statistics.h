/**
 * \file    statistics.h
 * \brief   Declaration of statistical utility functions and classes for the General
 *          Utility Library.
 * \authors \ref contributors
 * \date    Created on 7 February 2019
 *
 * \copyright Copyright 2019-2022 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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

#ifndef GUL14_STATISTICS_H_
#define GUL14_STATISTICS_H_

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <type_traits>
#include <vector>

#include "gul14/internal.h"
#include "gul14/traits.h"

namespace gul14 {

using statistics_result_type = double; ///< Type used to return statistic properties

/**
 * Return a mock element accessor for containers.
 *
 * All functions in statistics.h access the elements of the containers they
 * work on through accessor functions. If the container is simple, i.e. contains
 * just the types we want to work on (like double of std::vector<double>) we can
 * automate the generation of the accessor function, so that the user does not need
 * to specify it.
 *
 * \tparam ElementT   Type of the elements in the container
 * \returns           Pointer to accessor function
 */
template <typename ElementT>
auto ElementAccessor()
{
    return [](ElementT const& el) -> ElementT const&
        { return el; };
}

/**
 * Object that is designed to holds two values: minimum and maximum of something
 *
 * DataT must be an arithmetic type.
 *
 * Default constructed the members have these possibly useful values:
 * * min: Not-a-Number or maximum value representable with DataT
 * * max: Not-a-Number or minimum value representable with DataT
 *
 * \tparam DataT     Type of the contained values
 */
template <typename DataT, typename = void, typename = std::enable_if_t<std::is_arithmetic<DataT>::value>>
struct MinMax {
    DataT min{ std::numeric_limits<DataT>::max() }; ///< Minimum value
    DataT max{ std::numeric_limits<DataT>::lowest() }; ///< Maximum value
};

template <typename DataT>
struct MinMax<DataT, std::enable_if_t<std::is_floating_point<DataT>::value>> {
    DataT min{ NAN };
    DataT max{ NAN };
};

/**
 * A struct holding a standard deviation and a mean value.
 *
 * DataT must be a floating point type.
 *
 * Default constructed the contained values are Not-a-Number.
 *
 * The object can be cast to DataT in which case it is results in the sigma value.
 * DataT is usually statistics_result_type.
 *
 * The data members are public to allow structured bindings.
 *
 * \tparam DataT     Type of the contained values
 *
 * \see standard_deviation()
 */
template <typename DataT, typename = std::enable_if_t<std::is_floating_point<DataT>::value>>
class StandardDeviationMean {
public:
    DataT sigma_{ NAN }; ///< The standard deviation (sigma) value
    DataT mean_{ NAN }; ///< The mean value

    /**
     * Cast to DataT results in standard deviation value
     *
     * The conversion operator is not explicit, so any implicit conversion
     * from DataT to the desired type is also conducted.
     *
     * For example the following works:
     * \code
     * auto sdm = StandardDeviationMean<float>{ 3.0, 4.0 };
     * long double sigma = sdm; // implicit conversion from float to long double
     * \endcode
     */
    operator DataT() const noexcept {
        return sigma_;
    }
    /// Get the standard deviation value
    auto sigma() const noexcept -> DataT {
        return sigma_;
    }
    /// Get the arithmetic mean value
    auto mean() const noexcept -> DataT {
        return mean_;
    }
};

/////////// Main statistics functions following

/**
 * Calculate the arithmetic mean value of all elements in a container.
 *
 * The mean value is calculated by dividing the sum of all elements by the number
 * of elements:
 * ``mean -> sum 0..n-1 (element i) / n``
 *
 * \param container    Container of the elements to examine
 * \param accessor     Helper function to access the numeric value of one container
 *                     element
 * \returns            the arithmetic mean value.
 *
 * \tparam ResultT     Type of the result (this is also the type used for holding the sum
 *                     of all elements and for the division by the number of elements, so
 *                     make sure it can hold numbers that are big enough)
 * \tparam ContainerT  Type of the container to examine
 * \tparam ElementT    Type of an element in the container, i.e. ContainerT::value_type
 * \tparam Accessor    Type of the accessor function
 * \tparam DataT       Type returned by the accessor, i.e. numeric value of ElementT
 *
 * \see mean(IteratorT const&, IteratorT const&, Accessor) accepts two iterators instead
 *      of a container.
 */
template <typename ResultT = statistics_result_type,
          typename ContainerT,
          typename ElementT = typename ContainerT::value_type,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(ElementT const&),
          typename DataT = typename std::decay_t<std::result_of_t<Accessor(ElementT)>>,
          typename = std::enable_if_t<IsContainerLike<ContainerT>::value>
         >
auto mean(ContainerT const& container, Accessor accessor = ElementAccessor<ElementT>()) -> ResultT
{
    auto const sum = std::accumulate(
            container.cbegin(), container.cend(),
            ResultT{ },
            [accessor] (ResultT const& accu, ElementT const& el) {
                return accu + static_cast<ResultT>(accessor(el)); } );
    return sum / static_cast<ResultT>(container.size());
}

/**
 * Calculate the root mean square of all elements in a container.
 *
 * The rms value is calculated as the square root of the sum of all squared elements
 * divided by the number of elements:
 * ``rms -> sqrt (sum 0..n-1 (element i * element i) / n)``
 *
 * \param container    Container of the elements to examine
 * \param accessor     Helper function to access the numeric value of one container element
 * \returns            the rms value.
 *
 * \tparam ResultT     Type of the result value
 * \tparam ContainerT  Type of the container to examine
 * \tparam ElementT    Type of an element in the container, i.e. ContainerT::value_type
 * \tparam Accessor    Type of the accessor function
 * \tparam DataT       Type returned by the accessor, i.e. numeric value of ElementT
 *
 * \see rms(IteratorT const&, IteratorT const&, Accessor) accepts two iterators instead of
 *      a container.

 */
template <typename ResultT = statistics_result_type,
          typename ContainerT,
          typename ElementT = typename ContainerT::value_type,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(ElementT const&),
          typename DataT = typename std::decay_t<std::result_of_t<Accessor(ElementT)>>,
          typename = std::enable_if_t<IsContainerLike<ContainerT>::value>
         >
auto rms(ContainerT const& container, Accessor accessor = ElementAccessor<ElementT>()) -> ResultT
{
    auto const sum = std::accumulate(
            container.cbegin(), container.cend(),
            ResultT{ },
            [accessor] (ResultT const& accu, ElementT const& el) {
                return accu + std::pow(static_cast<ResultT>(accessor(el)), 2); } );
    return std::sqrt(sum / static_cast<ResultT>(container.size()));
}

/**
 * Find the median of all elements in a container.
 *
 * The median element is the element that has an equal number of elements higher and lower
 * in value. If the container has an even number of elements there can be no 'middle'
 * element. In this case the two 'middlemost' elements are taken and the arithmetic mean
 * of the two is returned.
 *
 * Because all elements need to be sorted, the function works with a temporary copy of the
 * original container.
 *
 * \param container    Container of the elements to examine
 * \param accessor     Helper function to access the numeric value of one container element
 * \returns            the median value.
 *
 * \tparam ResultT     Type of the result value
 * \tparam ContainerT  Type of the container to examine
 * \tparam ElementT    Type of an element in the container, i.e. ContainerT::value_type
 * \tparam Accessor    Type of the accessor function
 * \tparam DataT       Type returned by the accessor, i.e. numeric value of ElementT
 *
 * \see median(IteratorT const&, IteratorT const&, Accessor) accepts two iterators instead
 *      of a container.
 */
template <typename ResultT = statistics_result_type,
          typename ContainerT,
          typename ElementT = typename ContainerT::value_type,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(ElementT const&),
          typename DataT = typename std::decay_t<std::result_of_t<Accessor(ElementT)>>,
          typename = std::enable_if_t<IsContainerLike<ContainerT>::value>
         >
auto median(ContainerT const& container, Accessor accessor = ElementAccessor<ElementT>()) -> ResultT
{
    auto const len = container.size();
    if (len == 0)
        return std::numeric_limits<ResultT>::quiet_NaN();

    // work with a copy of the data
    // because nth_element() partially sorts the input data
    auto data_copy = std::vector<DataT>{ };
    data_copy.resize(len);
    auto data_copy_it = data_copy.begin();
    auto data_it = container.cbegin();
    auto const data_end = container.cend();
    for (; data_it != data_end; ++data_it) {
        *(data_copy_it++) = accessor(*data_it);
    }

    // What is the middle element?
    auto middle = data_copy.begin() + (len / 2);
    std::nth_element(data_copy.begin(), middle, data_copy.end());
    auto median = static_cast<ResultT>(*middle);

    // If we have an even number of elements we need to do more:
    // We calculate the mean value of the two 'middle' elements
    if (0 == len % 2) {
        std::nth_element(data_copy.begin(), middle - 1, data_copy.end());
        median = (median / 2) + (static_cast<ResultT>(*(middle - 1)) / 2);
    }

    return median;
}

/**
 * Return the maximum element value in a container.
 *
 * The value of each element is determined through the accessor function. The maximum
 * value is returned. Its type (DataT, the return type of the accessor) must provide
 * `operator==()` and `operator<=()`. If DataT supports not-a-number (NaN) values, such
 * values are ignored. If the container is empty, the return value is NaN (if supported)
 * or \c std::numeric_limits<DataT>::lowest().
 *
 * Hint: If looking for an iterator to the maximum element instead of its value, use
 * `std::max_element()` from the STL algorithm collection.
 *
 * \param container    Container of the elements to examine
 * \param accessor     Helper function to access the numeric value of one container
 *                     element.
 * \returns            the maximum value.
 *
 * \tparam ContainerT  Type of the container to examine
 * \tparam ElementT    Type of an element in the container, i.e. ContainerT::value_type
 * \tparam Accessor    Type of the accessor function
 * \tparam DataT       Type returned by the accessor, i.e. numeric value of ElementT
 *
 * \see
 * maximum(IteratorT const&, IteratorT const&, Accessor) accepts two iterators instead of
 * a container.\n
 * minimum() returns the minimum value, min_max() returns both the minimum and the
 * maximum.
 *
 * \since GUL version 2.2
 */
template <typename ContainerT,
    typename ElementT = typename ContainerT::value_type,
    typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(ElementT const&),
    typename DataT = typename std::decay_t<std::result_of_t<Accessor(ElementT)>>,
    typename = std::enable_if_t<IsContainerLike<ContainerT>::value>
>
auto maximum(ContainerT const& container, Accessor accessor = ElementAccessor<ElementT>()) -> DataT
{
    constexpr auto initial_value = std::numeric_limits<DataT>::has_quiet_NaN ?
        std::numeric_limits<DataT>::quiet_NaN() : std::numeric_limits<DataT>::lowest();

    return std::accumulate(
        container.cbegin(), container.cend(), initial_value,
        [&accessor](DataT const& accu, ElementT const& el) -> DataT {
            auto const val = accessor(el);
            // Test portably for not-NAN (some compilers do not have std::isnan() for
            // integral types)
            if (val == val) {
                if (not (val <= accu)) // inverted logic to handle NAN correctly
                    return val;
            }
            return accu; });
}

/**
 * Return the minimum element value in a container.
 *
 * The value of each element is determined through the accessor function. The minimum
 * value is returned. Its type (DataT, the return type of the accessor) must provide
 * `operator==()` and `operator>=()`. If DataT supports not-a-number (NaN) values, such
 * values are ignored. If the container is empty, the return value is NaN (if supported)
 * or \c std::numeric_limits<DataT>::max().
 *
 * Hint: If looking for an iterator to the minimum element instead of its value, use
 * `std::min_element()` from the STL algorithm collection.
 *
 * \param container    Container of the elements to examine
 * \param accessor     Helper function to access the numeric value of one container
 *                     element.
 * \returns            the minimum value.
 *
 * \tparam ContainerT  Type of the container to examine
 * \tparam ElementT    Type of an element in the container, i.e. ContainerT::value_type
 * \tparam Accessor    Type of the accessor function
 * \tparam DataT       Type returned by the accessor, i.e. numeric value of ElementT
 *
 * \see
 * minimum(IteratorT const&, IteratorT const&, Accessor) accepts two iterators instead of
 * a container.\n
 * maximum() returns the maximum value, min_max() returns both the minimum and the
 * maximum.
 *
 * \since GUL version 2.2
 */
template <typename ContainerT,
    typename ElementT = typename ContainerT::value_type,
    typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(ElementT const&),
    typename DataT = typename std::decay_t<std::result_of_t<Accessor(ElementT)>>,
    typename = std::enable_if_t<IsContainerLike<ContainerT>::value>
>
auto minimum(ContainerT const& container, Accessor accessor = ElementAccessor<ElementT>()) -> DataT
{
    constexpr auto initial_value = std::numeric_limits<DataT>::has_quiet_NaN ?
        std::numeric_limits<DataT>::quiet_NaN() : std::numeric_limits<DataT>::max();

    return std::accumulate(
        container.cbegin(), container.cend(), initial_value,
        [&accessor](DataT const& accu, ElementT const& el) -> DataT {
            auto const val = accessor(el);
            // Test portably for not-NAN (some compilers do not have std::isnan() for
            // integral types)
            if (val == val) {
                if (not (val >= accu)) // inverted logic to handle NAN correctly
                    return val;
            }
            return accu; });
}

/**
 * Find the minimum and maximum element values in a container.
 *
 * The value of each element is determined through the accessor function. The minimum and
 * the maximum of these values are returned. Their type (DataT, the return type of the
 * accessor) must provide `operator==(), `operator<=()`, and `operator>=()`. If DataT
 * supports not-a-number (NaN) values, such values are ignored. If the container is empty,
 * the return values are NaN (if supported) or \c std::numeric_limits<DataT>::max() for the
 * minimum and \c std::numeric_limits<DataT>::lowest() for the maximum.
 *
 * This behaves like (symbolic code):
 * \code
 * std::minmax_element(std::transform(mycontainer, accessor))
 * \endcode
 * without constructing a temporary container and without the ownership problems that
 * would arise from minmax_element usage on a temporary container.
 *
 * Hint: If looking for iterators to the minimum and maximum element instead of their
 * values, use `std::minmax_element()` from the STL algorithm collection.
 *
 * \param container    Container of the elements to examine
 * \param accessor     Helper function to access the numeric value of one container element
 * \returns            the minimum and maximum values stored in a MinMax<DataT> object.
 *
 * \tparam ContainerT  Type of the container to examine
 * \tparam ElementT    Type of an element in the container, i.e. ContainerT::value_type
 * \tparam Accessor    Type of the accessor function
 * \tparam DataT       Type returned by the accessor, i.e. numeric value of ElementT
 *
 *
 * \see
 * min_max(IteratorT const&, IteratorT const&, Accessor) accepts two iterators instead of
 * a container.\n
 * minimum() returns only the minimum value, maximum() only the maximum value.
 */
template <typename ContainerT,
          typename ElementT = typename ContainerT::value_type,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(ElementT const&),
          typename DataT = typename std::decay_t<std::result_of_t<Accessor(ElementT)>>,
          typename = std::enable_if_t<IsContainerLike<ContainerT>::value>
         >
auto min_max(ContainerT const& container, Accessor accessor = ElementAccessor<ElementT>()) -> MinMax<DataT>
{
    using MinMaxT = MinMax<DataT>;
    auto const sum = std::accumulate(
            container.cbegin(), container.cend(),
            MinMaxT{ },
            [accessor] (MinMaxT const& accu, ElementT const& el) -> MinMaxT {
                auto out{ accu };
                auto const val = accessor(el);
                // Test portably for not-NAN (some compilers do not have std::isnan() for
                // integral types)
                if (val == val) {
                    // (a >= NAN) and (a <= NAN) always false for all a
                    if (not (val >= out.min))
                        out.min = val;
                    if (not (val <= out.max))
                        out.max = val;
                }
                return out; });
    return sum;
}

/**
 * Remove elements that are far away from other elements.
 *
 * The element whose value differs the most from the arithmetic mean of all elements is
 * removed. This process is repeated if more than one outlier is to be removed;
 * specifically, the mean is recalculated from the remaining elements.
 *
 * The original container is modified.
 * The container needs to be modifiable and have the ``erase()`` member function.
 *
 * \param cont         Container of the elements to examine
 * \param outliers     How many outliers shall be removed
 * \param accessor     Helper function to access the numeric value of one container element
 * \returns            the container passed in as `cont` after removal of outliers.
 *
 * \tparam ContainerT  Type of the container to examine
 * \tparam ElementT    Type of an element in the container, i.e. ContainerT::value_type
 * \tparam Accessor    Type of the accessor function
 * \tparam DataT       Type returned by the accessor, i.e. numeric value of ElementT
 */
template <typename ContainerT,
          typename ElementT = typename ContainerT::value_type,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(ElementT const&),
          typename DataT = typename std::decay_t<std::result_of_t<Accessor(ElementT)>>,
          typename = std::enable_if_t<IsContainerLike<ContainerT>::value>
         >
auto remove_outliers(ContainerT&& cont, std::size_t outliers,
        Accessor accessor = ElementAccessor<ElementT>()) -> ContainerT&
{
    while (outliers-- > 0 and cont.size() > 0) {
        auto max_distant = std::max_element(cont.begin(), cont.end(),
                [mean = mean(cont, accessor), accessor] (ElementT const& a, ElementT const& b)
                { return std::abs(accessor(a) - mean) < std::abs(accessor(b) - mean); });
        cont.erase(max_distant);
    }
    return cont;
}

/**
 * \overload
 *
 * The original container is not modified.
 * A copy of the original container without the outlier elements is returned.
 */
template <typename ContainerT,
          typename ElementT = typename ContainerT::value_type,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(ElementT const&),
          typename DataT = typename std::decay_t<std::result_of_t<Accessor(ElementT)>>,
          typename = std::enable_if_t<IsContainerLike<ContainerT>::value>
         >
auto remove_outliers(ContainerT const& cont, std::size_t outliers,
        Accessor accessor = ElementAccessor<ElementT>()) -> std::vector<ElementT>
{
    auto c = std::vector<ElementT>(cont.size());
    std::copy(cont.cbegin(), cont.cend(), c.begin());
    return remove_outliers(std::move(c), outliers, accessor);
}

/**
 * Calculate the standard deviation of all elements in a container.
 *
 * The corrected sample standard deviation is calculated:
 * ``standard_deviation -> sqrt (sum 0..n-1 ((element i - mean) * (element i - mean)) / (n - 1))``
 *
 * The returned StandardDeviationMean object can be used like this:
 * \code
 * const StandardDeviationMean std_and_mean = standard_deviation(something);
 * const double std = std_and_mean.sigma();
 * const double mean = std_and_mean.mean();
 *
 * // The cast operator is for people that want to ignore the complicated stuff like this
 * const double std = standard_deviation(something);
 * const float std = standard_deviation(something); // implicit conversions possible
 *
 * // Structured binding (C++17)
 * const auto [std, mean] = standard_deviation(something);
 * \endcode
 *
 * If the container is empty, not-a-number is returned for both the standard deviation and
 * the mean value. If it contains only a single element, not-a-number is returned for the
 * standard deviation and the mean value is the value of the element.
 *
 * \param container Container of the elements to examine
 * \param accessor  Helper function to access the numeric value of one container element
 * \returns         the standard deviation and mean values as a StandardDeviationMean
 *                  object.
 *
 * \tparam ResultT     Type of the result value
 * \tparam ContainerT  Type of the container to examine
 * \tparam ElementT    Type of an element in the container, i.e. ContainerT::value_type
 * \tparam Accessor    Type of the accessor function
 * \tparam DataT       Type returned by the accessor, i.e. numeric value of ElementT
 *
 * \see standard_deviation(IteratorT const&, IteratorT const&, Accessor) accepts two
 *      iterators instead of a container.
 */
template <typename ResultT = statistics_result_type,
          typename ContainerT,
          typename ElementT = typename ContainerT::value_type,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(ElementT const&),
          typename DataT = typename std::decay_t<std::result_of_t<Accessor(ElementT)>>,
          typename = std::enable_if_t<IsContainerLike<ContainerT>::value>
         >
auto standard_deviation(ContainerT const& container, Accessor accessor = ElementAccessor<ElementT>()) -> StandardDeviationMean<ResultT>
{
    auto const len = container.size();

    if (len == 0)
        return { };

    auto mean_val = mean<ResultT>(container, accessor);

    if (len == 1)
        return { std::numeric_limits<ResultT>::quiet_NaN(), mean_val };

    auto sum = std::accumulate(container.cbegin(), container.cend(),
        ResultT{ },
        [mean_val, accessor] (ResultT const& accu, ElementT const& el)
        { return accu + std::pow(static_cast<ResultT>(accessor(el)) - mean_val, 2); });

    sum /= static_cast<ResultT>(container.size() - 1);

    return { std::sqrt(sum), mean_val };
}

/**
 * Calculate some aggregate value from all elements of a container.
 *
 * This is similar to std::accumulate, but
 * * works on a whole container,
 * * accesses the container elements through an accessor function,
 * * applies the binary operator `op` to a running sum and each element value,
 * * the sum's type is in the first template parameter, and
 * * the initial value of the sum is its default constructed value.
 *
 * \param container    Container of the elements to examine
 * \param op           Binary operator to aggregate two values into one value
 * \param accessor     Helper function to access the numeric value of one container element
 * \returns            the aggregate value.
 *
 * \tparam ResultT     Type of the result value / accumulator
 * \tparam ContainerT  Type of the container to examine
 * \tparam ElementT    Type of an element in the container, i.e. ContainerT::value_type
 * \tparam Accessor    Type of the accessor function
 * \tparam DataT       Type returned by the accessor, i.e. numeric value of ElementT
 *
 * \see accumulate(IteratorT const&, IteratorT const&, OpClosure, Accessor) accepts two
 *      iterators instead of a container.
 */
template <typename ResultT = statistics_result_type,
          typename ContainerT,
          typename ElementT = typename ContainerT::value_type,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(ElementT const&),
          typename DataT = typename std::decay_t<std::result_of_t<Accessor(ElementT)>>,
          typename OpClosure,
          typename = std::enable_if_t<IsContainerLike<ContainerT>::value>
         >
auto accumulate(ContainerT const& container, OpClosure op, Accessor accessor = ElementAccessor<ElementT>()) -> ResultT
{
    auto const sum = std::accumulate(
            container.cbegin(), container.cend(),
            ResultT{ },
            [accessor, op] (ResultT const& accu, ElementT const& el) {
                return op(accu, accessor(el)); } );
    return sum;
}

namespace {

// The following stuff is only there to have a two iterator interface

    template <typename IteratorT>
    struct ContainerView {
        IteratorT const& begin_;
        IteratorT const& end_;
        using value_type = std::decay_t<decltype(*begin_)>;

        ContainerView(IteratorT const& i1, IteratorT const& i2)
        : begin_{ i1 }
        , end_{ i2 }
        {
        }

        // Just implement the member functions that we use here

        auto cbegin() const noexcept -> IteratorT const&
        {
            return begin_;
        }
        auto cend() const noexcept -> IteratorT const&
        {
            return end_;
        }

        auto size() const noexcept -> std::size_t
        {
            return std::distance(begin_, end_);
        }
    };

    template<typename IteratorT>
    auto make_view(IteratorT const& cbegin, IteratorT const& cend) -> ContainerView<IteratorT> const
    {
        return ContainerView<IteratorT>{ cbegin, cend };
    }

} // namespace anonymous

/**
 * \overload
 *
 * \param begin     Iterator to first elements to examine in the container
 * \param end       Iterator past the last element to examine in the container
 * \param accessor  Helper function to access the numeric value of one container element
 *
 * \see mean(ContainerT const&, Accessor) accepts a container instead of iterators.
 */
template <typename ResultT = statistics_result_type,
          typename IteratorT,
          typename ElementT = std::decay_t<decltype(*std::declval<IteratorT>())>,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(ElementT const&),
          typename DataT = std::decay_t<std::result_of_t<Accessor(ElementT)>>>
auto mean(IteratorT const& begin, IteratorT const& end,
        Accessor accessor = ElementAccessor<ElementT>()) -> ResultT
{
    return mean<ResultT>(make_view(begin, end), accessor);
}

/**
 * \overload
 *
 * \param begin     Iterator to first elements to examine in the container
 * \param end       Iterator past the last element to examine in the container
 * \param accessor  Helper function to access the numeric value of one container element
 *
 * \see rms(ContainerT const&, Accessor) accepts a container instead of iterators.
 */
template <typename ResultT = statistics_result_type,
          typename IteratorT,
          typename ElementT = std::decay_t<decltype(*std::declval<IteratorT>())>,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(ElementT const&),
          typename DataT = std::decay_t<std::result_of_t<Accessor(ElementT)>>>
auto rms(IteratorT const& begin, IteratorT const& end,
        Accessor accessor = ElementAccessor<ElementT>()) -> ResultT
{
    return rms<ResultT>(make_view(begin, end), accessor);
}

/**
 * \overload
 *
 * \param begin     Iterator to first elements to examine in the container
 * \param end       Iterator past the last element to examine in the container
 * \param accessor  Helper function to access the numeric value of one container element
 *
 * \see median(ContainerT const&, Accessor) accepts a container instead of iterators.
 */
template <typename ResultT = statistics_result_type,
          typename IteratorT,
          typename ElementT = std::decay_t<decltype(*std::declval<IteratorT>())>,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(ElementT const&),
          typename DataT = std::decay_t<std::result_of_t<Accessor(ElementT)>>>
auto median(IteratorT const& begin, IteratorT const& end,
        Accessor accessor = ElementAccessor<ElementT>()) -> ResultT
{
    return median<ResultT>(make_view(begin, end), accessor);
}

/**
 * \overload
 *
 * \param begin     Iterator to first elements to examine in the container
 * \param end       Iterator past the last element to examine in the container
 * \param accessor  Helper function to access the numeric value of one container element
 *
 * \see max(ContainerT const&, Accessor) accepts a container instead of iterators.
 */
template <typename IteratorT,
    typename ElementT = std::decay_t<decltype(*std::declval<IteratorT>())>,
    typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(ElementT const&),
    typename DataT = std::decay_t<std::result_of_t<Accessor(ElementT)>>>
    auto maximum(IteratorT const& begin, IteratorT const& end,
        Accessor accessor = ElementAccessor<ElementT>()) -> DataT
{
    return maximum(make_view(begin, end), accessor);
}

/**
 * \overload
 *
 * \param begin     Iterator to first elements to examine in the container
 * \param end       Iterator past the last element to examine in the container
 * \param accessor  Helper function to access the numeric value of one container element
 *
 * \see max(ContainerT const&, Accessor) accepts a container instead of iterators.
 */
template <typename IteratorT,
    typename ElementT = std::decay_t<decltype(*std::declval<IteratorT>())>,
    typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(ElementT const&),
    typename DataT = std::decay_t<std::result_of_t<Accessor(ElementT)>>>
    auto minimum(IteratorT const& begin, IteratorT const& end,
        Accessor accessor = ElementAccessor<ElementT>()) -> DataT
{
    return minimum(make_view(begin, end), accessor);
}

/**
 * \overload
 *
 * \param begin     Iterator to first elements to examine in the container
 * \param end       Iterator past the last element to examine in the container
 * \param accessor  Helper function to access the numeric value of one container element
 *
 * \see min_max(ContainerT const&, Accessor) accepts a container instead of iterators.
 */
template <typename IteratorT,
          typename ElementT = std::decay_t<decltype(*std::declval<IteratorT>())>,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(ElementT const&),
          typename DataT = std::decay_t<std::result_of_t<Accessor(ElementT)>>>
auto min_max(IteratorT const& begin, IteratorT const& end,
        Accessor accessor = ElementAccessor<ElementT>()) -> MinMax<DataT>
{
    return min_max(make_view(begin, end), accessor);
}

/**
 * \overload
 *
 * The original container is not modified.
 * A copy of the original container with the outerlier elements removed is returned.
 *
 * \param begin        Iterator to first elements to examine in the container
 * \param end          Iterator past the last element to examine in the container
 * \param outliers     How many outliers shall be removed
 * \param accessor     Helper function to access the numeric value of one container element
 */
template <typename IteratorT,
          typename ElementT = std::decay_t<decltype(*std::declval<IteratorT>())>,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(ElementT const&),
          typename DataT = std::decay_t<std::result_of_t<Accessor(ElementT)>>>
auto remove_outliers(IteratorT const& begin, IteratorT const& end,
        std::size_t outliers, Accessor accessor = ElementAccessor<ElementT>()) -> std::vector<ElementT>
{
    return remove_outliers(make_view(begin, end), outliers, accessor);
}

/**
 * \overload
 *
 * \param begin     Iterator to first elements to examine in the container
 * \param end       Iterator past the last element to examine in the container
 * \param accessor  Helper function to access the numeric value of one container element
 *
 * \see standard_deviation(ContainerT const&, Accessor) accepts a container instead of
 *      iterators.
 */
template <typename ResultT = statistics_result_type,
          typename IteratorT,
          typename ElementT = std::decay_t<decltype(*std::declval<IteratorT>())>,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(ElementT const&),
          typename DataT = std::decay_t<std::result_of_t<Accessor(ElementT)>>>
auto standard_deviation(IteratorT const& begin, IteratorT const& end,
        Accessor accessor = ElementAccessor<ElementT>()) -> StandardDeviationMean<ResultT>
{
    return standard_deviation<ResultT>(make_view(begin, end), accessor);
}

/**
 * \overload
 *
 * \param begin     Iterator to first elements to examine in the container
 * \param end       Iterator past the last element to examine in the container
 * \param op        Binary operator to aggregate two values into one value
 * \param accessor  Helper function to access the numeric value of one container element
 *
 * \see accumulate(ContainerT const&, OpClosure, Accessor) accepts a container instead of
 *      iterators.
 */
template <typename ResultT = statistics_result_type,
          typename IteratorT,
          typename ElementT = std::decay_t<decltype(*std::declval<IteratorT>())>,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(ElementT const&),
          typename DataT = std::decay_t<std::result_of_t<Accessor(ElementT)>>,
          typename OpClosure>
auto accumulate(IteratorT const& begin, IteratorT const& end, OpClosure op,
        Accessor accessor = ElementAccessor<ElementT>()) -> ResultT
{
    return accumulate<ResultT>(make_view(begin, end), op, accessor);
}

} // namespace gul14

#endif

// vi:ts=4:sw=4:sts=4:et
