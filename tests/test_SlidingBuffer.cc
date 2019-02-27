/**
 * \file   test_SlidingBuffer.cc
 * \author \ref contributors
 * \date   Created on Feb 7, 2019
 * \brief  Test suite for the SlidingBuffer{}
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
#include <random>

auto random_double() -> double
{
    using bit_type = int64_t;

    union convert {
        decltype(random_double()) f;
        bit_type i;
    };
    auto converter = convert{};

    static_assert(sizeof(decltype(std::declval<convert>().f))
                <= sizeof(decltype(std::declval<convert>().i)),
                "Integer random buffer too small");
    static_assert(offsetof(convert, f) == offsetof(convert, i),
                "Alignment problem in conversion union");

    static std::random_device rd;
    auto static gen = std::mt19937{ rd() };
    auto static dis = std::uniform_int_distribution<bit_type>{ };

    do
        converter.i = dis(gen);
    while (std::isnan(converter.f)); // discard/retry if random number is NaN

    return converter.f;
}

template <typename DataT, typename StateT>
struct TestElement {
    using has_state = std::true_type;
    using data_type = DataT;
    using state_type = StateT;
    DataT val{ std::numeric_limits<DataT>::quiet_NaN() };
    StateT sta{ 0 };

    auto friend operator<< (std::ostream& s, const TestElement<DataT, StateT>& e) -> std::ostream&
    {
        return s << e.val;
    }
};

TEST_CASE("SlidingBuffer test", "[sliding]")
{
    SECTION("queueing tests") {
        auto const input_size = 100u;
        auto const buffer_size = 12u;
        static_assert(input_size % buffer_size != 0,
                "We want to have unaligned sizes");

        auto nums = std::vector<double>(input_size);
        std::transform(nums.begin(), nums.end(), nums.begin(),
                [](auto) { return random_double(); } );

        auto buff = gul::SlidingBuffer<TestElement<double, unsigned int>, buffer_size>{};
        for (auto el : nums)
            buff.push_back({el,0});

        // Check last put element
        REQUIRE(buff.back().val == nums.at(nums.size() - 1));

        // Stuff Elements in and see if outcoming elements are as expected
        auto const signature = 1.234E5;
        for (auto i = 0u; i < buffer_size; ++i) {
            REQUIRE(buff.front().val == nums.at(nums.size() - buffer_size + i));
            buff.push_back({signature, 0u});
            REQUIRE(buff.back().val == signature);
        }
        // Now zero elements should come out...
        static_assert(buffer_size > 3, "Fifo too small for test case");
        REQUIRE(buff.front().val == signature);
        buff.push_back({0.0, 0u});
        REQUIRE(buff.front().val == signature);
        buff.push_back({0.0, 0u});
        REQUIRE(buff.front().val == signature);
        buff.push_back({0.0, 0u});
    }
    SECTION("dumping tests") {
        auto buff = gul::SlidingBuffer<TestElement<double, unsigned int>, 10>{};
        std::stringstream s1{ };
        s1 << buff;
        REQUIRE_THAT(s1.str(), Catch::Matchers::StartsWith(
                    "nan* nan  nan  nan  nan  nan  nan  nan  nan  nan",
                    Catch::CaseSensitive::No));

        for (auto i = 0; i < 15; ++i)
            buff.push_back({ static_cast<double>(i)*10 + static_cast<double>(i)/10, static_cast<unsigned int>(i) });

        std::stringstream s2{ };
        s2 << buff;
        REQUIRE_THAT(s2.str(), Catch::Matchers::StartsWith(
                    "101  111.1  121.2  131.3  141.4  50.5* 60.6  70.7  80.8  90.9"));
    }
}

// vi:ts=4:sw=4:et
