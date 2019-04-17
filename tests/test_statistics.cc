/**
 * \file   test_statistics.cc
 * \author \ref contributors
 * \date   Created on Feb 7, 2019
 * \brief  Test suite for statistics functions.
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

#include "catch.h"
#include <gul.h>
#include <sstream>

template <typename DataT, typename StateT = void>
struct StatisticsElement {
    using has_state = std::true_type;
    using data_type = DataT;
    using state_type = StateT;
    DataT val{ std::numeric_limits<DataT>::quiet_NaN() };
    StateT sta{ 0 };

    auto value() -> DataT
    {
        return val;
    }

    auto friend operator<< (std::ostream& s, const StatisticsElement<DataT, StateT>& e) -> std::ostream&
    {
        return s << e.val;
    }
};

template <typename DataT>
struct StatisticsElement<DataT, void> {
    using has_state = std::false_type;
    using data_type = DataT;
    DataT val{ std::numeric_limits<DataT>::quiet_NaN() };

    auto friend operator<< (std::ostream& s, const StatisticsElement<DataT, void>& e) -> std::ostream&
    {
        return s << e.val;
    }
};

template <typename ContainerT,
          typename ElementT = typename ContainerT::value_type,
          typename DataT = typename ElementT::data_type>
auto StatisticsElementAccessor() -> DataT(*)(const ElementT&)
{
    return [](const ElementT& el) { return el.val; };
}

auto bit(int bit) -> unsigned int
{
    return 1u << bit;
}

TEST_CASE("Container Statistics Tests", "[statistics]")
{
    using namespace gul;
    auto op_max = [](const auto a, const auto b) { return std::max(a, b); };
    auto op_or = [](const auto a, const auto b) { return a | b; };
    auto acc_state = [](const auto& e) { return e.sta; };

    SECTION("data analysis tests on std::deque") {
        auto fifo = std::deque<StatisticsElement<double, unsigned int>>{};
        auto accessor = StatisticsElementAccessor<decltype(fifo)>();
        // No data -> return NAN
        REQUIRE(std::isnan(mean(fifo, accessor)));
        REQUIRE(std::isnan(rms(fifo, accessor)));
        REQUIRE(std::isnan(median(fifo, accessor)));
        REQUIRE(accumulate<unsigned int>(fifo, op_max, acc_state) == 0u);
        REQUIRE(std::isnan(min_max(fifo, accessor).min));
        REQUIRE(std::isnan(min_max(fifo, accessor).max));
        REQUIRE(std::isnan(standard_deviation(fifo, accessor).sigma()));

        auto const value1 = 10.0;
        auto const state1 = bit(1);
        fifo.push_back({value1, state1});
        REQUIRE(mean(fifo, accessor) == value1);
        REQUIRE(rms(fifo, accessor) == value1);
        REQUIRE(median(fifo, accessor) == value1);
        REQUIRE(accumulate<unsigned int>(fifo, op_max, acc_state) == state1);
        REQUIRE(min_max(fifo, accessor).min == value1);
        REQUIRE(min_max(fifo, accessor).max == value1);

        auto const value2 = 7.7;
        auto const state2 = bit(8);
        fifo.push_back({value2, state2});
        REQUIRE(mean(fifo, accessor) == Approx((value1 + value2) / 2.0));
        auto rmsval = std::sqrt((value1 * value1 + value2 * value2) / 2.0);
        REQUIRE(rms(fifo, accessor) ==
                Approx(std::sqrt((value1 * value1 + value2 * value2) / 2.0)));
        REQUIRE(median(fifo, accessor) == (value1 + value2) / 2.0);
        REQUIRE(accumulate<unsigned int>(fifo, op_max, acc_state) == state2);
        REQUIRE(min_max(fifo, accessor).min == value2);
        REQUIRE(min_max(fifo, accessor).max == value1);

        auto const value3 = 9.1;
        auto const state3 = bit(3);
        auto elem = typename decltype(fifo)::value_type{ };
        elem.val = value3;
        elem.sta = state3;
        fifo.push_back(elem);
        REQUIRE(mean(fifo, accessor) == Approx((value1 + value2 + value3) / 3.0));
        REQUIRE(mean(fifo.begin(), fifo.end(), accessor) ==
                Approx((value1 + value2 + value3) / 3.0));
        rmsval = std::sqrt((value1 * value1 + value2 * value2 + value3 * value3) / 3.0);
        REQUIRE(rms(fifo, accessor) == Approx(rmsval));
        REQUIRE(rms(fifo.begin(), fifo.end(), accessor) == Approx(rmsval));
        REQUIRE(median(fifo, accessor) == value3);
        REQUIRE(median(fifo.cbegin(), fifo.cend(), accessor) == value3);
        REQUIRE(accumulate<unsigned int>(fifo, op_max, acc_state) == state2);
        REQUIRE(min_max(fifo, accessor).min == value2);
        REQUIRE(min_max(fifo, accessor).max == value1);
        REQUIRE(min_max(fifo.rbegin(), fifo.rend(), accessor).min == value2);
        REQUIRE(min_max(fifo.rbegin(), fifo.rend(), accessor).max == value1);
        REQUIRE(fifo.size() == 3);

        // Add two more values, one above and one below value3,
        // keeping it the median
        auto const value4 = 7.9;
        auto const state4 = bit(4);
        fifo.push_back({value4, state4});
        auto const value5 = 9.3;
        auto const state5 = bit(3);
        fifo.push_back({value5, state5});
        REQUIRE(mean(fifo, accessor) ==
                Approx((value1 + value2 + value3 + value4 + value5) / 5.0));
        rmsval = std::sqrt((value1 * value1 + value2 * value2 + value3 * value3 + value4 * value4 + value5 * value5) / 5.0);
        REQUIRE(rms(fifo, accessor) == Approx(rmsval));
        REQUIRE(median(fifo, accessor) == value3);
        REQUIRE(accumulate<unsigned int>(fifo, op_max, acc_state) == state2);
        // For some reason stuff from standard header <numeric> seem to be in our namespace,
        // so we need to specify that we want to use GUL's accumulate rather than std::accumulate
        REQUIRE(gul::accumulate<unsigned int>(fifo.begin(), fifo.end(), op_max, acc_state) == state2);
        REQUIRE(accumulate<unsigned int>(fifo, op_or, acc_state) == (state1 | state2 | state3 | state4 | state5));
        REQUIRE(min_max(fifo, accessor).min == value2);
        REQUIRE(min_max(fifo, accessor).max == value1);

        REQUIRE_THAT(standard_deviation(fifo, accessor).sigma(),
                Catch::Matchers::WithinAbs(0.975, 0.001));
        // check cast operator
        float standard_dev1 = standard_deviation(fifo, accessor);
        REQUIRE_THAT(standard_dev1, Catch::Matchers::WithinAbs(0.975, 0.001));
        double standard_dev2 = standard_deviation(fifo, accessor);
        REQUIRE_THAT(standard_dev2, Catch::Matchers::WithinAbs(0.975, 0.001));

        REQUIRE_THAT(standard_deviation(remove_outliers(fifo, 1, accessor), accessor).sigma(),
                Catch::Matchers::WithinAbs(0.816, 0.001));
        REQUIRE_THAT(standard_deviation(fifo.begin(), fifo.end(), accessor).sigma(),
                Catch::Matchers::WithinAbs(0.975, 0.001));
    }

    SECTION("data analysis tests on std::vector") {
        auto vec = std::vector<double>{{ 1.2, 3.4, 5.6, 7.4, 9.1 }};
        REQUIRE_THAT(mean(vec), Catch::Matchers::WithinULP(5.34, 3));
        REQUIRE_THAT(mean(vec.begin(), vec.end()), Catch::Matchers::WithinULP(5.34, 3));
        REQUIRE_THAT(rms(vec), Catch::Matchers::WithinAbs(6.032, 0.001));
        REQUIRE_THAT(rms(vec.begin(), vec.end()), Catch::Matchers::WithinAbs(6.032, 0.001));
        REQUIRE(median(vec) == 5.6); // exactly
        REQUIRE(median(vec.begin(), vec.end()) == 5.6);
        REQUIRE(min_max(vec).min == 1.2);
        REQUIRE(min_max(vec.begin(), vec.end()).min == 1.2);
        REQUIRE_THAT(standard_deviation(vec).sigma(),
                Catch::Matchers::WithinAbs(3.136, 0.001));
        REQUIRE_THAT(standard_deviation(vec.begin(), vec.end()).sigma(),
                Catch::Matchers::WithinAbs(3.136, 0.001));

        // Copy version of remove_outliers
        auto vec2 = remove_outliers(vec, 1);
        REQUIRE(vec == std::vector<double>{{ 1.2, 3.4, 5.6, 7.4, 9.1 }});
        REQUIRE(vec2 == std::vector<double>{{ 3.4, 5.6, 7.4, 9.1 }});

        // In-place version of remove_outliers
        vec2.insert(vec2.begin(), 8.0);
        REQUIRE(vec2 == std::vector<double>{{ 8.0, 3.4, 5.6, 7.4, 9.1 }});
        auto vec3 = remove_outliers(std::move(vec2), 1);
        REQUIRE(vec2 == std::vector<double>{{ 8.0, 5.6, 7.4, 9.1 }});

        vec3.insert(vec3.begin(), 3.3);
        vec3 = remove_outliers(std::move(vec3), 2);
        REQUIRE(vec3 == std::vector<double>{{ 8.0, 7.4, 9.1 }});
        vec3 = remove_outliers(std::move(vec3), 5);
        REQUIRE(vec3 == std::vector<double>{ });
    }

    SECTION("data analysis examples") {
        { // simple example
            std::vector<float> my_data = { 3.0f, 2.1f, 7.5f, 9.9f, 5.2f };

            auto durchschnitt = mean(my_data);

            REQUIRE_THAT(durchschnitt, Catch::Matchers::WithinULP(5.54000f, 3));
        }
        { // example with accessors
            struct measurement {
                float intensity;
                float charge;
            };

            std::vector<measurement> my_data = { { 3.1f, 1102.2f }, { 2.2f, 1023.4f }, { 2.9f, 1077.3f } };

            auto durchschnitt_i = mean(my_data, [](const auto& e){ return e.intensity; });
            auto durchschnitt_c = mean(my_data, [](const auto& e){ return e.charge; } );

            REQUIRE_THAT(durchschnitt_i, Catch::Matchers::WithinULP(2.73333333f, 3));
            REQUIRE_THAT(durchschnitt_c, Catch::Matchers::WithinULP(1067.63333f, 3));

            auto durchschnitt_i2 = mean<int>(my_data, [](const auto& e){ return e.intensity; });
            auto durchschnitt_c2 = mean<int>(my_data, [](const auto& e){ return e.charge; } );

            REQUIRE(durchschnitt_i2 == 2);
            REQUIRE(durchschnitt_c2 == 1067);
        }
        { // small custom struct
            using LaserType = unsigned int;
            struct chargedata {
                float val;
                LaserType laser;
            };

            std::vector<chargedata> my_data = { { 1102.2f, 3u }, { 1023.4f, 2u }, { 1077.3f, 5u } };

            auto durchschnitt_c = mean(my_data, [](chargedata const& el) { return el.val; });

            REQUIRE_THAT(durchschnitt_c, Catch::Matchers::WithinULP(1067.63333f, 3));
        }
        { // embedding one struct into another
            using LaserType = unsigned int;
            struct measurement {
                float intensity;
                float charge;
            };
            struct chargedata {
                measurement meas;
                LaserType laser;
            };

            std::vector<chargedata> my_data = { { { 3.1f , 1102.2f }, 2u }, { { 2.2f, 1023.4f }, 2u } };

            auto durchschnitt_c = mean(my_data, [](const auto& e){ return e.meas.charge; } );

            REQUIRE_THAT(durchschnitt_c, Catch::Matchers::WithinULP(1062.80000f, 3));
        }
        { // convoluted expressions
            using LaserMode = int;
            using TrainIdT = unsigned long long int;
            struct chargedata {
                float val;
                LaserMode laser;
            };
            struct train {
                TrainIdT id;
                std::vector<chargedata> meas;
            };

            std::vector<train> my_trains = { // comment: hand calculated median
                { 123ul, { { 1969.1f,  3 }, { 1643.2f,  9 }, { 1306.1f,  1 }, { 1616.0f,  5 } } }, // 1629.6
                { 124ul, { { 1862.2f, 15 }, { 1742.6f,  2 }, { 1033.5f,  3 }, { 1360.9f,  1 } } }, // 1551.75
                { 125ul, { { 1358.1f,  4 }, { 1577.3f,  2 }, { 1222.4f, 17 }, { 1275.5f,  6 } } }, // 1316.8
                { 126ul, { { 1832.6f,  8 }, { 1983.9f, 17 }, { 1324.8f,  6 }, { 1521.6f,  8 } } }, // 1677.1
                { 127ul, { { 1663.7f, 16 }, { 1050.8f,  5 }, { 1826.9f,  9 }, { 1786.6f, 15 } } }, // 1725.15
                { 128ul, { { 1660.9f,  3 }, { 1974.3f,  4 }, { 1595.3f,  4 }, { 1771.7f,  4 } } } }; // 1716.3

            auto trainrange = min_max(my_trains, [](const auto& e){ return e.id; } );
            auto mean_of_median_c = mean(my_trains, [](const auto& e){ return median(e.meas,
                                                        [] (const auto& e){ return e.val;}); } );
            auto lasermax = min_max(my_trains, [](const auto& e){ return min_max(e.meas,
                                                    [](const auto& e){ return e.laser; }).max; } );

            REQUIRE(trainrange.min == 123ul);
            REQUIRE(trainrange.max == 128ul);
            REQUIRE_THAT(mean_of_median_c, Catch::Matchers::WithinULP(1602.78333f, 5)); // 5 because many operations
            REQUIRE(lasermax.max == 17u);
        }
        { // Show how even character sequences work: Outlier magic
            std::string digits = { "8e7r6846209463768276894209524" };
            auto mm = min_max(digits); // std::minmax() also useful in simple cases like these
            REQUIRE(mm.min == '0');
            REQUIRE(mm.max == 'r');

            auto digits_clustered = remove_outliers(digits, 2);
            auto clusterlimits = min_max(digits_clustered);
            REQUIRE(clusterlimits.min == '0');
            REQUIRE(clusterlimits.max == '9');
        }
    }

}

// vi:ts=4:sw=4:et
