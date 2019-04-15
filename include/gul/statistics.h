/**
 * \file    statistics.h
 * \brief   Declarations of statistical utility functions/objects for the General Utility Library.
 * \authors \ref contributors
 * \date    Created on 7 Feb 2019
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

namespace {

    using statistics_result_type = double; ///< Type used to return statistic properties

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
template <typename DataT, typename = void, typename = std::enable_if<std::is_arithmetic<DataT>::value>>
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
 * Object that holds two values: standard_deviation and mean
 *
 * DataT must be an arithmetic type.
 *
 * Default constructed the members have these possibly useful values:
 * * sigma: Not-a-Number or zero
 * * mean: Not-a-Number or zero
 *
 * The object can be cast to DataT in which case it is results in the sigma value.
 *
 * \tparam DataT     Type of the contained values
 */
template <typename DataT, typename = void, typename = std::enable_if<std::is_arithmetic<DataT>::value>>
struct StandardDeviation {
    DataT sigma{ 0 }; ///< The standard deviation (sigma) value
    DataT mean{ 0 }; ///< The mean value

    operator DataT() { ///< Cast to DataT results in the sigma member
        return sigma;
    }
};

template <typename DataT>
struct StandardDeviation<DataT, std::enable_if_t<std::is_floating_point<DataT>::value>> {
    DataT sigma{ NAN };
    DataT mean{ NAN };

    operator DataT() {
        return sigma;
    }
};

/////////// Main statistics functions following

/**
 * Calculate arithmetic mean of all elements in a container.
 *
 * The mean value is calculated by dividing the sum of all elements by the number
 * of elements: ``mean -> sum 0..n-1 (element i) / n``
 *
 * \param container    Container of the elements to examine
 * \param accessor     Helper function to access the numeric value of one container element
 * \returns            Arithmetic mean value
 *
 * \tparam ResultT     Type of the result value
 * \tparam ContainerT  Type of the container to examine
 * \tparam ElementT    Type of an element in the container, i.e. ContainerT::value_type
 * \tparam Accessor    Type of the accessor function
 * \tparam DataT       Type returned by the accessor, i.e. numeric value of ElementT
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
 * Calculate root mean square of all elements in a container.
 *
 * The mean value is calculated by taking the square root of the the sum of all
 * squared elements divided by the number of elements:
 * ``rms -> sqrt (sum 0..n-1 (element i * element i) / n)``
 *
 * \param container    Container of the elements to examine
 * \param accessor     Helper function to access the numeric value of one container element
 * \returns            RMS value
 *
 * \tparam ResultT     Type of the result value
 * \tparam ContainerT  Type of the container to examine
 * \tparam ElementT    Type of an element in the container, i.e. ContainerT::value_type
 * \tparam Accessor    Type of the accessor function
 * \tparam DataT       Type returned by the accessor, i.e. numeric value of ElementT
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
 * The median element is the element that has an equal number of elements higher and
 * lower in value.
 * If the container has an even number of elements there can be no 'middle' element. In
 * this case the two 'middlemost' elements are taken and the arithmetic mean of the two
 * is returned.
 *
 * Because we need to sort all elements the function works with a temporary container that
 * holds all numerical values of the original container.
 *
 * \param container    Container of the elements to examine
 * \param accessor     Helper function to access the numeric value of one container element
 * \returns            Median value or arithmetic mean of the two middlemost (in value) elements
 *
 * \tparam ResultT     Type of the result value
 * \tparam ContainerT  Type of the container to examine
 * \tparam ElementT    Type of an element in the container, i.e. ContainerT::value_type
 * \tparam Accessor    Type of the accessor function
 * \tparam DataT       Type returned by the accessor, i.e. numeric value of ElementT
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
 * Find the minimum and maximum elements of a container.
 *
 * \param container    Container of the elements to examine
 * \param accessor     Helper function to access the numeric value of one container element
 * \returns            Minimum and maximum values as MinMax object
 *
 * The elements are returned by value.
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
auto min_max(const ContainerT& container, Accessor accessor = ElementAccessor<ElementT>()) -> MinMax<DataT>
{
    using MinMaxT = MinMax<DataT>;
    auto const sum = std::accumulate(
            container.cbegin(), container.cend(),
            MinMaxT{ },
            [accessor] (const MinMaxT& accu, const ElementT& el) -> MinMaxT {
                auto out{ accu };
                auto const val = accessor(el);
                if (not std::isnan(val)) {
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
 * The element which's value differs the most from the arithmetic mean of all
 * elements is removed. This process is repeated if more than one outlier is
 * to be removed - specifically the mean is again calculated from the remaining
 * elements.
 *
 * The original container is modified.
 * The container needs to be modifiable and have the ``erase()`` member function.
 *
 * \param cont         Container of the elements to examine
 * \param outliers     How many outliers shall be removed
 * \param accessor     Helper function to access the numeric value of one container element
 * \returns            Container without outliers
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
        Accessor accessor = ElementAccessor<ElementT>()) -> ContainerT
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
 * A copy of the original container with the outerlier elements removed is returned.
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
 * The corrected sample standard deviation is calculated, i.e.
 * ``std_dev -> sqrt (sum 0..n-1 ((element i - mean) * (element i - mean)) / (n - 1))``
 *
 * \param container    Container of the elements to examine
 * \param accessor     Helper function to access the numeric value of one container element
 * \returns            The standard deviation and mean values as StandardDeviation object
 *
 * \tparam ResultT     Type of the result value
 * \tparam ContainerT  Type of the container to examine
 * \tparam ElementT    Type of an element in the container, i.e. ContainerT::value_type
 * \tparam Accessor    Type of the accessor function
 * \tparam DataT       Type returned by the accessor, i.e. numeric value of ElementT
 */
template <typename ResultT = statistics_result_type,
          typename ContainerT,
          typename ElementT = typename ContainerT::value_type,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = typename std::result_of_t<Accessor(ElementT)>,
          typename = std::enable_if_t<IsContainerLike<ContainerT>::value>
         >
auto standard_deviation(const ContainerT& container, Accessor accessor = ElementAccessor<ElementT>()) -> StandardDeviation<ResultT>
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
 * * Works on a whole container
 * * Accesses the value of the container elements through a accessor function
 * * Applies the binary operator op to a running 'sum' and each element's value
 * * The initial value of the sum is just the default constructed value
 *
 * \param container    Container of the elements to examine
 * \param op           Binary operator to aggregate two values into one value
 * \param accessor     Helper function to access the numeric value of one container element
 * \returns            The aggregate value
 *
 * \tparam ResultT     Type of the result value
 * \tparam ContainerT  Type of the container to examine
 * \tparam ElementT    Type of an element in the container, i.e. ContainerT::value_type
 * \tparam Accessor    Type of the accessor function
 * \tparam DataT       Type returned by the accessor, i.e. numeric value of ElementT
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
 * \param begin        Iterator to first elements to examine in the container
 * \param end          Iterator past the last element to examine in the container
 * \param accessor     Helper function to access the numeric value of one container element
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
 * \param begin        Iterator to first elements to examine in the container
 * \param end          Iterator past the last element to examine in the container
 * \param accessor     Helper function to access the numeric value of one container element
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
 * \param begin        Iterator to first elements to examine in the container
 * \param end          Iterator past the last element to examine in the container
 * \param accessor     Helper function to access the numeric value of one container element
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
 * \param begin        Iterator to first elements to examine in the container
 * \param end          Iterator past the last element to examine in the container
 * \param accessor     Helper function to access the numeric value of one container element
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
 * \param begin        Iterator to first elements to examine in the container
 * \param end          Iterator past the last element to examine in the container
 * \param accessor     Helper function to access the numeric value of one container element
 */
template <typename ResultT = statistics_result_type,
          typename IteratorT,
          typename ElementT = std::decay_t<decltype(*std::declval<IteratorT>())>,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = std::result_of_t<Accessor(ElementT)>>
auto standard_deviation(const IteratorT& begin, const IteratorT& end,
        Accessor accessor = ElementAccessor<ElementT>()) -> StandardDeviation<ResultT>
{
    return standard_deviation<ResultT>(make_view(begin, end), accessor);
}

/**
 * \overload
 *
 * \param begin        Iterator to first elements to examine in the container
 * \param end          Iterator past the last element to examine in the container
 * \param op           Binary operator to aggregate two values into one value
 * \param accessor     Helper function to access the numeric value of one container element
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
