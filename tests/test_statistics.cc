/**
 * \file   test_statistics.cc
 * \author \ref contributors
 * \date   Created on Feb 7, 2019
 * \brief  Test suite for Fifo
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
        // No data -> return NAN
        REQUIRE(std::isnan(mean(fifo)));
        REQUIRE(std::isnan(median(fifo)));
        REQUIRE(accumulate(fifo, op_max, acc_state) == 0u);
        REQUIRE(std::isnan(min_max(fifo).min));
        REQUIRE(std::isnan(min_max(fifo).max));
        REQUIRE(std::isnan(standard_deviation(fifo)));

        auto const value1 = 10.0;
        auto const state1 = bit(1);
        fifo.push_back({value1, state1});
        REQUIRE(mean(fifo) == value1);
        REQUIRE(median(fifo) == value1);
        REQUIRE(accumulate(fifo, op_max, acc_state) == state1);
        REQUIRE(min_max(fifo).min == value1);
        REQUIRE(min_max(fifo).max == value1);

        auto const value2 = 7.7;
        auto const state2 = bit(8);
        fifo.push_back({value2, state2});
        REQUIRE(mean(fifo) == (value1 + value2) / 2.0);
        REQUIRE(median(fifo) == (value1 + value2) / 2.0);
        REQUIRE(accumulate(fifo, op_max, acc_state) == state2);
        REQUIRE(min_max(fifo).min == value2);
        REQUIRE(min_max(fifo).max == value1);

        auto const value3 = 9.1;
        auto const state3 = bit(3);
        auto elem = typename decltype(fifo)::value_type{ };
        elem.val = value3;
        elem.sta = state3;
        fifo.push_back(elem);
        REQUIRE(mean(fifo) == (value1 + value2 + value3) / 3.0);
        REQUIRE(mean_it(fifo.begin(), fifo.end()) == (value1 + value2 + value3) / 3.0);
        REQUIRE(median(fifo) == value3);
        REQUIRE(median_it(fifo.cbegin(), fifo.cend()) == value3);
        REQUIRE(accumulate(fifo, op_max, acc_state) == state2);
        REQUIRE(min_max(fifo).min == value2);
        REQUIRE(min_max(fifo).max == value1);
        REQUIRE(min_max_it(fifo.rbegin(), fifo.rend()).min == value2);
        REQUIRE(min_max_it(fifo.rbegin(), fifo.rend()).max == value1);
        REQUIRE(fifo.size() == 3);

        // Add two more values, one above and one below value3,
        // keeping it the median
        auto const value4 = 7.9;
        auto const state4 = bit(4);
        fifo.push_back({value4, state4});
        auto const value5 = 9.3;
        auto const state5 = bit(3);
        fifo.push_back({value5, state5});
        REQUIRE(mean(fifo) == (value1 + value2 + value3 + value4 + value5) / 5.0);
        REQUIRE(median(fifo) == value3);
        REQUIRE(accumulate(fifo, op_max, acc_state) == state2);
        REQUIRE(accumulate_it(fifo.begin(), fifo.end(), op_max, acc_state) == state2);
        REQUIRE(accumulate(fifo, op_or, acc_state) == (state1 | state2 | state3 | state4 | state5));
        REQUIRE(min_max(fifo).min == value2);
        REQUIRE(min_max(fifo).max == value1);
        REQUIRE_THAT(standard_deviation(fifo), Catch::Matchers::WithinAbs(0.975, 0.001));
        REQUIRE_THAT(standard_deviation(remove_outliers(fifo, 1)), Catch::Matchers::WithinAbs(0.816, 0.001));
        REQUIRE_THAT(standard_deviation_it(fifo.begin(), fifo.end()), Catch::Matchers::WithinAbs(0.975, 0.001));

    }

    SECTION("data analysis tests on std::vector") {
        auto vec = std::vector<double>{{ 1.2, 3.4, 5.6, 7.4, 9.1 }};
        REQUIRE_THAT(mean(vec), Catch::Matchers::WithinAbs(5.34, 0.001));
        REQUIRE_THAT(mean_it(vec.begin(), vec.end()), Catch::Matchers::WithinAbs(5.34, 0.001));
        REQUIRE(median(vec) == 5.6); // exactly
        REQUIRE(median_it(vec.begin(), vec.end()) == 5.6);
        REQUIRE(min_max(vec).min == 1.2);
        REQUIRE(min_max_it(vec.begin(), vec.end()).min == 1.2);
        REQUIRE_THAT(standard_deviation(vec), Catch::Matchers::WithinAbs(3.136, 0.001));
        REQUIRE_THAT(standard_deviation_it(vec.begin(), vec.end()), Catch::Matchers::WithinAbs(3.136, 0.001));

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

            REQUIRE_THAT(durchschnitt, Catch::Matchers::WithinAbs(5.54000f, 0.00001));
        }
        { // example with accessors
            struct measurement {
                float intensity;
                float charge;
            };

            std::vector<measurement> my_data = { { 3.1f, 1102.2f }, { 2.2f, 1023.4f }, { 2.9f, 1077.3f } };

            auto durchschnitt_i = mean(my_data, [](const auto& e){ return e.intensity; });
            auto durchschnitt_c = mean(my_data, [](const auto& e){ return e.charge; } );

            REQUIRE_THAT(durchschnitt_i, Catch::Matchers::WithinAbs(2.73333f, 0.00001));
            REQUIRE_THAT(durchschnitt_c, Catch::Matchers::WithinAbs(1067.63342f, 0.00001));
        }
        { // lazy custom struct (using member val)
            using LaserType = unsigned int;
            struct chargedata {
                float val;
                LaserType laser;
            };

            std::vector<chargedata> my_data = { { 1102.2f, 3u }, { 1023.4f, 2u }, { 1077.3f, 5u } };

            auto durchschnitt_c = mean(my_data);

            REQUIRE_THAT(durchschnitt_c, Catch::Matchers::WithinAbs(1067.63342f, 0.00001));
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

            REQUIRE_THAT(durchschnitt_c, Catch::Matchers::WithinAbs(1062.80005f, 0.00001));
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

            std::vector<train> my_trains = {
                { 123ul, { { 1969.1f,  3 }, { 1643.2f,  9 }, { 1306.1f,  1 }, { 1616.0f,  5 } } },
                { 124ul, { { 1862.2f, 15 }, { 1742.6f,  2 }, { 1033.5f,  3 }, { 1360.9f,  1 } } },
                { 125ul, { { 1358.1f,  4 }, { 1577.3f,  2 }, { 1222.4f, 17 }, { 1275.5f,  6 } } },
                { 126ul, { { 1832.6f,  8 }, { 1983.9f, 17 }, { 1324.8f,  6 }, { 1521.6f,  8 } } },
                { 127ul, { { 1663.7f, 16 }, { 1050.8f,  5 }, { 1826.9f,  9 }, { 1786.6f, 15 } } },
                { 128ul, { { 1660.9f,  3 }, { 1974.3f,  4 }, { 1595.3f,  4 }, { 1771.7f,  4 } } } };

            auto trainrange = min_max(my_trains, [](const auto& e){ return e.id; } );
            auto mean_of_median_c = mean(my_trains, [](const auto& e){ return median(e.meas); } );
            auto lasermax = min_max(my_trains, [](const auto& e){ return min_max(e.meas,
                                                    [](const auto& e){ return e.laser; }).max; } );

            REQUIRE(trainrange.min == 123ul);
            REQUIRE(trainrange.max == 128ul);
            REQUIRE_THAT(mean_of_median_c, Catch::Matchers::WithinAbs(1602.78333f, 0.00001));
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
