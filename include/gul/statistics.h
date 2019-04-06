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

#include <numeric>
#include <type_traits>

namespace gul {

namespace {

    //////
    //
    // Until we have concepts ;-)
    //

    // Helper to identify containers, i.e. types that have
    // cbegin(), cend(), value_type
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

template <typename ElementT>
auto ElementAccessor() -> std::enable_if_t<std::is_fundamental<ElementT>::value,
                                           ElementT(*)(const ElementT&)>
{
        return [](const ElementT& el) { return el; };
}

template <typename DataT, typename = void, typename = std::enable_if<std::is_arithmetic<DataT>::value>>
struct MinMax {
    DataT min{ std::numeric_limits<DataT>::max() };
    DataT max{ std::numeric_limits<DataT>::lowest() };
};

template <typename DataT>
struct MinMax<DataT, std::enable_if_t<std::is_floating_point<DataT>::value>> {
    DataT min{ NAN };
    DataT max{ NAN };
};

template <typename ContainerT,
          typename ElementT = typename ContainerT::value_type,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = typename std::result_of_t<Accessor(ElementT)>,
          typename = std::enable_if_t<IsContainerLike<ContainerT>::value>
         >
auto mean(const ContainerT& container, Accessor accessor = ElementAccessor<ElementT>()) -> DataT
{
    auto const sum = std::accumulate(
            container.cbegin(), container.cend(),
            DataT{ },
            [accessor] (const DataT& accu, const ElementT& el) {
                return accu + accessor(el); } );
    return sum / container.size();
}

template <typename ContainerT,
          typename ElementT = typename ContainerT::value_type,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = typename std::result_of_t<Accessor(ElementT)>,
          typename = std::enable_if_t<IsContainerLike<ContainerT>::value>
         >
auto rms(const ContainerT& container, Accessor accessor = ElementAccessor<ElementT>()) -> DataT
{
    auto const sum = std::accumulate(
            container.cbegin(), container.cend(),
            DataT{ },
            [accessor] (const DataT& accu, const ElementT& el) {
                return accu + std::pow(accessor(el), 2); } );
    return std::sqrt(sum / container.size());
}

template <typename ContainerT,
          typename ElementT = typename ContainerT::value_type,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = typename std::result_of_t<Accessor(ElementT)>,
          typename = std::enable_if_t<IsContainerLike<ContainerT>::value>
         >
auto median(const ContainerT& container, Accessor accessor = ElementAccessor<ElementT>()) -> DataT
{
    auto const len = container.size();
    if (len == 0)
        return std::numeric_limits<DataT>::quiet_NaN();

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
    auto median = *middle;

    // If we have an even number of elements we need to do more:
    // We calculate the mean value of the two 'middle' elements
    if (0 == len % 2) {
        std::nth_element(data_copy.begin(), middle - 1, data_copy.end());
        median = (median / 2) + (*(middle - 1) / 2);
    }

    return median;

}

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

template <typename ContainerT,
          typename ElementT = typename ContainerT::value_type,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = typename std::result_of_t<Accessor(ElementT)>,
          typename = std::enable_if_t<IsContainerLike<ContainerT>::value>
         >
auto standard_deviation(const ContainerT& container, Accessor accessor = ElementAccessor<ElementT>()) -> DataT
{
    auto const len = container.size();

    if (len == 0)
        return std::numeric_limits<DataT>::quiet_NaN();
    auto mean_val = mean(container, accessor);
    if (len == 1)
        return mean_val;

    auto sum = std::accumulate(container.cbegin(), container.cend(),
        DataT{ },
        [mean_val, accessor] (const DataT& accu, const ElementT& el)
        { return accu + std::pow(accessor(el) - mean_val, 2); });

    sum /= container.size() - 1;

    return std::sqrt(sum);
}

template <typename ContainerT,
          typename ElementT = typename ContainerT::value_type,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = typename std::result_of_t<Accessor(ElementT)>,
          typename OpClosure,
          typename = std::enable_if_t<IsContainerLike<ContainerT>::value>
         >
auto accumulate(const ContainerT& container, OpClosure op, Accessor accessor = ElementAccessor<ElementT>()) -> DataT
{
    auto const sum = std::accumulate(
            container.cbegin(), container.cend(),
            DataT{ },
            [accessor, op] (const DataT& accu, const ElementT& el) {
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

template <typename IteratorT,
          typename ElementT = std::decay_t<decltype(*std::declval<IteratorT>())>,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = std::result_of_t<Accessor(ElementT)>>
auto mean(const IteratorT& begin, const IteratorT& end,
        Accessor accessor = ElementAccessor<ElementT>()) -> DataT
{
    return mean(make_view(begin, end), accessor);
}

template <typename IteratorT,
          typename ElementT = std::decay_t<decltype(*std::declval<IteratorT>())>,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = std::result_of_t<Accessor(ElementT)>>
auto rms(const IteratorT& begin, const IteratorT& end,
        Accessor accessor = ElementAccessor<ElementT>()) -> DataT
{
    return rms(make_view(begin, end), accessor);
}

template <typename IteratorT,
          typename ElementT = std::decay_t<decltype(*std::declval<IteratorT>())>,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = std::result_of_t<Accessor(ElementT)>>
auto median(const IteratorT& begin, const IteratorT& end,
        Accessor accessor = ElementAccessor<ElementT>()) -> DataT
{
    return median(make_view(begin, end), accessor);
}

template <typename IteratorT,
          typename ElementT = std::decay_t<decltype(*std::declval<IteratorT>())>,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = std::result_of_t<Accessor(ElementT)>>
auto min_max(const IteratorT& begin, const IteratorT& end,
        Accessor accessor = ElementAccessor<ElementT>()) -> MinMax<DataT>
{
    return min_max(make_view(begin, end), accessor);
}

template <typename IteratorT,
          typename ElementT = std::decay_t<decltype(*std::declval<IteratorT>())>,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = std::result_of_t<Accessor(ElementT)>>
auto remove_outliers(const IteratorT& begin, const IteratorT& end,
        std::size_t outliers, Accessor accessor = ElementAccessor<ElementT>()) -> std::vector<ElementT>
{
    return remove_outliers(make_view(begin, end), outliers, accessor);
}

template <typename IteratorT,
          typename ElementT = std::decay_t<decltype(*std::declval<IteratorT>())>,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = std::result_of_t<Accessor(ElementT)>>
auto standard_deviation(const IteratorT& begin, const IteratorT& end,
        Accessor accessor = ElementAccessor<ElementT>()) -> DataT
{
    return standard_deviation(make_view(begin, end), accessor);
}

template <typename IteratorT,
          typename ElementT = std::decay_t<decltype(*std::declval<IteratorT>())>,
          typename Accessor = std::result_of_t<decltype(ElementAccessor<ElementT>())(ElementT)>(*)(const ElementT&),
          typename DataT = std::result_of_t<Accessor(ElementT)>,
          typename OpClosure>
auto accumulate(const IteratorT& begin, const IteratorT& end, OpClosure op,
        Accessor accessor = ElementAccessor<ElementT>()) -> DataT
{
    return accumulate(make_view(begin, end), op, accessor);
}

} // namespace gul

// vi:ts=4:sw=4:et
