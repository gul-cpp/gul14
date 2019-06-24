/**
 * \file    statistics.h
 * \brief   Declaration of statistical utility functions and classes for the General
 *          Utility Library.
 * \authors \ref contributors
 * \date    Created on 7 February 2019
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
#include <cmath>
#include <limits>
#include <numeric>
#include <type_traits>
#include <vector>

namespace gul {

    using statistics_result_type = double; ///< Type used to return statistic properties

namespace {
    /**
     * Helper type trait object to determine of a type is a container.
     *
     * A container is identified by the presense of ``cbegin()`` and ``cend()``
     * member functions and a ``value_type`` type alias.
     *
     * If the type specified as template parameter has the required members
     * std::true_type is returned, otherwise std::false_type.
     *
     * \tparam T    Type to check
     * \returns     std::true_type if the type is probably a container
     */
    template <typename T, typename = int>
    struct IsContainerLike : std::false_type { };

    template <typename T>
    struct IsContainerLike <T,
        typename std::enable_if_t<true,
            decltype(std::declval<T>().cbegin(),
                std::declval<T>().cend(),
                std::declval<typename T::value_type>(),
                0)
        >>
        : std::true_type { };

} // namespace anonymous

/**
 * Return a mock element accessor for containers of fundamental types.
 *
 * All functions in statistics.h access the elements of the containers they
 * work on through accessor functions. If the container is simple, i.e. contains
 * just fundamental types (like std::vector<double>) we can automate the
 * generation of the accessor function, so that the user does not need to specify
 * it.
 *
 * The type returned from the accessor should be an fundamental and arithmetic type.
 * As this is expected the element's content is returned by value and not be reference.
 *
 * \tparam ElementT   Type of the elements in the container
 * \returns           Pointer to accessor function
 */
template <typename ElementT>
auto ElementAccessor() -> std::enable_if_t<std::is_fundamental<ElementT>::value,
                                           ElementT(*)(const ElementT&)>
{
    return [](const ElementT& el) { return el; };
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
 * \param accessor     Helper function to access the numeric value of one container element
 * \returns            the arithmetic mean value.
 *
 * \tparam ResultT     Type of the result value
 * \tparam ContainerT  Type of the container to examine
 * \tparam ElementT    Type of an element in the container, i.e. ContainerT::value_type
 * \tparam Accessor    Type of the accessor function
 * \tparam DataT       Type returned by the accessor, i.e. numeric value of ElementT
 *
 * \see mean(const IteratorT&, const IteratorT&, Accessor) accepts two iterators instead
 *      of a container.
 */
template <typename ResultT = statistics_result_type,
          typename ContainerT,
          typename ElementT = typename ContainerT::value_type,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = typename std::result_of_t<Accessor(ElementT)>,
          typename = std::enable_if_t<IsContainerLike<ContainerT>::value>
         >
auto mean(const ContainerT& container, Accessor accessor = ElementAccessor<ElementT>()) -> ResultT
{
    auto const sum = std::accumulate(
            container.cbegin(), container.cend(),
            ResultT{ },
            [accessor] (const ResultT& accu, const ElementT& el) {
                return accu + accessor(el); } );
    return sum / container.size();
}

/**
 * Calculate the root mean square of all elements in a container.
 *
 * The rms value is calculated as the square root of the the sum of all squared elements
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
 * \see rms(const IteratorT&, const IteratorT&, Accessor) accepts two iterators instead of
 *      a container.

 */
template <typename ResultT = statistics_result_type,
          typename ContainerT,
          typename ElementT = typename ContainerT::value_type,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = typename std::result_of_t<Accessor(ElementT)>,
          typename = std::enable_if_t<IsContainerLike<ContainerT>::value>
         >
auto rms(const ContainerT& container, Accessor accessor = ElementAccessor<ElementT>()) -> ResultT
{
    auto const sum = std::accumulate(
            container.cbegin(), container.cend(),
            ResultT{ },
            [accessor] (const ResultT& accu, const ElementT& el) {
                return accu + std::pow(static_cast<ResultT>(accessor(el)), 2); } );
    return std::sqrt(sum / container.size());
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
 * \see median(const IteratorT&, const IteratorT&, Accessor) accepts two iterators instead
 *      of a container.
 */
template <typename ResultT = statistics_result_type,
          typename ContainerT,
          typename ElementT = typename ContainerT::value_type,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = typename std::result_of_t<Accessor(ElementT)>,
          typename = std::enable_if_t<IsContainerLike<ContainerT>::value>
         >
auto median(const ContainerT& container, Accessor accessor = ElementAccessor<ElementT>()) -> ResultT
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
    auto median = ResultT{ *middle };

    // If we have an even number of elements we need to do more:
    // We calculate the mean value of the two 'middle' elements
    if (0 == len % 2) {
        std::nth_element(data_copy.begin(), middle - 1, data_copy.end());
        median = (median / 2) + (static_cast<ResultT>(*(middle - 1)) / 2);
    }

    return median;

}

/**
 * Find the minimum and maximum element values in a container.
 *
 * The value of each element is determined through the accessor function. The minimum and
 * the maximum of these values are returned. The element type (i.e. the return type of the
 * accessor) must provide `operator<()`.
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
 * \see min_max(const IteratorT&, const IteratorT&, Accessor) accepts two iterators
 *      instead of a container.
 */
template <typename ContainerT,
          typename ElementT = typename ContainerT::value_type,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = typename std::result_of_t<Accessor(ElementT)>,
          typename = std::enable_if_t<IsContainerLike<ContainerT>::value>
         >
auto min_max(const ContainerT& container, Accessor accessor = ElementAccessor<ElementT>()) -> MinMax<DataT>
{
    using MinMaxT = MinMax<DataT>;
    auto const sum = std::accumulate(
            container.cbegin(), container.cend(),
            MinMaxT{ },
            [accessor] (const MinMaxT& accu, const ElementT& el) -> MinMaxT {
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
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = typename std::result_of_t<Accessor(ElementT)>,
          typename = std::enable_if_t<IsContainerLike<ContainerT>::value>
         >
auto remove_outliers(ContainerT&& cont, std::size_t outliers,
        Accessor accessor = ElementAccessor<ElementT>()) -> ContainerT&
{
    while (outliers-- > 0 and cont.size() > 0) {
        auto max_distant = std::max_element(cont.begin(), cont.end(),
                [mean = mean(cont, accessor), accessor] (const ElementT& a, const ElementT& b)
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
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = typename std::result_of_t<Accessor(ElementT)>,
          typename = std::enable_if_t<IsContainerLike<ContainerT>::value>
         >
auto remove_outliers(const ContainerT& cont, std::size_t outliers,
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
 * ``std_dev -> sqrt (sum 0..n-1 ((element i - mean) * (element i - mean)) / (n - 1))``
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
 * \param container    Container of the elements to examine
 * \param accessor     Helper function to access the numeric value of one container element
 * \returns            the standard deviation and mean values as a StandardDeviationMean object.
 *
 * \tparam ResultT     Type of the result value
 * \tparam ContainerT  Type of the container to examine
 * \tparam ElementT    Type of an element in the container, i.e. ContainerT::value_type
 * \tparam Accessor    Type of the accessor function
 * \tparam DataT       Type returned by the accessor, i.e. numeric value of ElementT
 *
 * \see standard_deviation(const IteratorT&, const IteratorT&, Accessor) accepts two
 *      iterators instead of a container.
 */
template <typename ResultT = statistics_result_type,
          typename ContainerT,
          typename ElementT = typename ContainerT::value_type,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = typename std::result_of_t<Accessor(ElementT)>,
          typename = std::enable_if_t<IsContainerLike<ContainerT>::value>
         >
auto standard_deviation(const ContainerT& container, Accessor accessor = ElementAccessor<ElementT>()) -> StandardDeviationMean<ResultT>
{
    auto const len = container.size();

    if (len == 0)
        return { };
    auto mean_val = mean<ResultT>(container, accessor);
    if (len == 1)
        return { mean_val, mean_val };

    auto sum = std::accumulate(container.cbegin(), container.cend(),
        ResultT{ },
        [mean_val, accessor] (const ResultT& accu, const ElementT& el)
        { return accu + std::pow(accessor(el) - mean_val, 2); });

    sum /= container.size() - 1;

    return { std::sqrt(sum), mean_val };
}

/**
 * Calculate some aggregate value from all elements of a container.
 *
 * This is similar to std::accumulate, but
 * * works on a whole container,
 * * accesses the container elements through an accessor function,
 * * applies the binary operator `op` to a running sum and each element value, and
 * * the initial value of the sum is its default constructed value.
 *
 * \param container    Container of the elements to examine
 * \param op           Binary operator to aggregate two values into one value
 * \param accessor     Helper function to access the numeric value of one container element
 * \returns            the aggregate value.
 *
 * \tparam ResultT     Type of the result value
 * \tparam ContainerT  Type of the container to examine
 * \tparam ElementT    Type of an element in the container, i.e. ContainerT::value_type
 * \tparam Accessor    Type of the accessor function
 * \tparam DataT       Type returned by the accessor, i.e. numeric value of ElementT
 *
 * \see accumulate(const IteratorT&, const IteratorT&, OpClosure, Accessor) accepts two
 *      iterators instead of a container.
 */
template <typename ResultT = statistics_result_type,
          typename ContainerT,
          typename ElementT = typename ContainerT::value_type,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = typename std::result_of_t<Accessor(ElementT)>,
          typename OpClosure,
          typename = std::enable_if_t<IsContainerLike<ContainerT>::value>
         >
auto accumulate(const ContainerT& container, OpClosure op, Accessor accessor = ElementAccessor<ElementT>()) -> ResultT
{
    auto const sum = std::accumulate(
            container.cbegin(), container.cend(),
            ResultT{ },
            [accessor, op] (const ResultT& accu, const ElementT& el) {
                return op(accu, accessor(el)); } );
    return sum;
}

namespace {

// The following stuff is only there to have a two iterator interface

    template <typename IteratorT>
    struct ContainerView {
        const IteratorT& begin_;
        const IteratorT& end_;
        using value_type = std::decay_t<decltype(*begin_)>;

        ContainerView(const IteratorT& i1, const IteratorT& i2)
        : begin_{ i1 }
        , end_{ i2 }
        {
        }

        // Just implement the member functions that we use here

        auto cbegin() const noexcept -> const IteratorT&
        {
            return begin_;
        }
        auto cend() const noexcept -> const IteratorT&
        {
            return end_;
        }

        auto size() const noexcept -> std::size_t
        {
            return std::distance(begin_, end_);
        }
    };

    template<typename IteratorT>
    auto make_view(const IteratorT& cbegin, const IteratorT& cend) -> const ContainerView<IteratorT>
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
 * \see mean(const ContainerT&, Accessor) accepts a container instead of iterators.
 */
template <typename ResultT = statistics_result_type,
          typename IteratorT,
          typename ElementT = std::decay_t<decltype(*std::declval<IteratorT>())>,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = std::result_of_t<Accessor(ElementT)>>
auto mean(const IteratorT& begin, const IteratorT& end,
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
 * \see rms(const ContainerT&, Accessor) accepts a container instead of iterators.
 */
template <typename ResultT = statistics_result_type,
          typename IteratorT,
          typename ElementT = std::decay_t<decltype(*std::declval<IteratorT>())>,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = std::result_of_t<Accessor(ElementT)>>
auto rms(const IteratorT& begin, const IteratorT& end,
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
 * \see median(const ContainerT&, Accessor) accepts a container instead of iterators.
 */
template <typename ResultT = statistics_result_type,
          typename IteratorT,
          typename ElementT = std::decay_t<decltype(*std::declval<IteratorT>())>,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = std::result_of_t<Accessor(ElementT)>>
auto median(const IteratorT& begin, const IteratorT& end,
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
 * \see min_max(const ContainerT&, Accessor) accepts a container instead of iterators.
 */
template <typename IteratorT,
          typename ElementT = std::decay_t<decltype(*std::declval<IteratorT>())>,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = std::result_of_t<Accessor(ElementT)>>
auto min_max(const IteratorT& begin, const IteratorT& end,
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
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = std::result_of_t<Accessor(ElementT)>>
auto remove_outliers(const IteratorT& begin, const IteratorT& end,
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
 * \see standard_deviation(const ContainerT&, Accessor) accepts a container instead of
 *      iterators.
 */
template <typename ResultT = statistics_result_type,
          typename IteratorT,
          typename ElementT = std::decay_t<decltype(*std::declval<IteratorT>())>,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = std::result_of_t<Accessor(ElementT)>>
auto standard_deviation(const IteratorT& begin, const IteratorT& end,
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
 * \see accumulate(const ContainerT&, OpClosure, Accessor) accepts a container instead of
 *      iterators.
 */
template <typename ResultT = statistics_result_type,
          typename IteratorT,
          typename ElementT = std::decay_t<decltype(*std::declval<IteratorT>())>,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = std::result_of_t<Accessor(ElementT)>,
          typename OpClosure>
auto accumulate(const IteratorT& begin, const IteratorT& end, OpClosure op,
        Accessor accessor = ElementAccessor<ElementT>()) -> ResultT
{
    return accumulate<ResultT>(make_view(begin, end), op, accessor);
}

} // namespace gul

// vi:ts=4:sw=4:et
