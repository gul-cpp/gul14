/**
 * \file   test_num_utils.cc
 * \author \ref contributors
 * \date   Created on 7 Feb 2019
 * \brief  Unit tests for withinOrders() and withinAbs().
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

#include <limits>
#include "gul/catch.h"
#include "gul.h"

TEST_CASE("test withinOrders()", "[numerics]")
{
    REQUIRE(gul::withinOrders(1.0, 101.0, 2) == false);
    REQUIRE(gul::withinOrders(101.0, 1.0, 2) == false);
    REQUIRE(gul::withinOrders(1.01l, 1.0201l, 2) == true);
    REQUIRE(gul::withinOrders(1.01l, 1.0202l, 2) == false);
    REQUIRE(gul::withinOrders(1.01, 1.02, 2) == true);
    REQUIRE(gul::withinOrders(1.01, 1.002, 2) == true);

    // crossing zero
    REQUIRE(gul::withinOrders(1.0, -2.0, 2) == false);
    REQUIRE(gul::withinOrders(-1.01, 1.02, 2) == false);
    REQUIRE(gul::withinOrders(-1.01, -1.02, 2) == true);

    // negative orders
    REQUIRE(gul::withinOrders(1.0, 100.0, -2) == true);
    REQUIRE(gul::withinOrders(1.0, 101.0, -2) == false);

    // equal digits marked:                12345
    REQUIRE(gul::withinOrders(0.6482831, 0.6482843, 2) == true);
    REQUIRE(gul::withinOrders(0.6482831, 0.6482843, 3) == true);
    REQUIRE(gul::withinOrders(0.6482831, 0.6482843, 4) == true);
    REQUIRE(gul::withinOrders(0.6482831, 0.6482843, 5) == true);
    REQUIRE(gul::withinOrders(0.6482831, 0.6482843, 6) == false);
    REQUIRE(gul::withinOrders(0.6482831, 0.6482843, 7) == false);
    REQUIRE(gul::withinOrders(0.6482831, 0.6482843, 8) == false);

    // equal digits marked:              1234 56
    REQUIRE(gul::withinOrders(3.6482831, 3.6482843, 2) == true);
    REQUIRE(gul::withinOrders(3.6482831, 3.6482843, 3) == true);
    REQUIRE(gul::withinOrders(3.6482831, 3.6482843, 4) == true);
    REQUIRE(gul::withinOrders(3.6482831, 3.6482843, 5) == true);
    REQUIRE(gul::withinOrders(3.6482831, 3.6482843, 6) == true);
    REQUIRE(gul::withinOrders(3.6482831, 3.6482843, 7) == false);
    REQUIRE(gul::withinOrders(3.6482831, 3.6482843, 8) == false);

    // equal digits marked:              1234 56
    REQUIRE(gul::withinOrders(3.6482831, 3.6482820, 2) == true);
    REQUIRE(gul::withinOrders(3.6482831, 3.6482820, 3) == true);
    REQUIRE(gul::withinOrders(3.6482831, 3.6482820, 4) == true);
    REQUIRE(gul::withinOrders(3.6482831, 3.6482820, 5) == true);
    REQUIRE(gul::withinOrders(3.6482831, 3.6482820, 6) == true);
    REQUIRE(gul::withinOrders(3.6482831, 3.6482820, 7) == false);
    REQUIRE(gul::withinOrders(3.6482831, 3.6482820, 8) == false);

    // equal digits marked:              1234 56
    REQUIRE(gul::withinOrders(3648.2831, 3648.2843, 2) == true);
    REQUIRE(gul::withinOrders(3648.2831, 3648.2843, 3) == true);
    REQUIRE(gul::withinOrders(3648.2831, 3648.2843, 4) == true);
    REQUIRE(gul::withinOrders(3648.2831, 3648.2843, 5) == true);
    REQUIRE(gul::withinOrders(3648.2831, 3648.2843, 6) == true);
    REQUIRE(gul::withinOrders(3648.2831, 3648.2843, 7) == false);
    REQUIRE(gul::withinOrders(3648.2831, 3648.2843, 8) == false);

    // equal digits marked:              1234 5
    REQUIRE(gul::withinOrders(3648.2831, 3648.2743, 2) == true);
    REQUIRE(gul::withinOrders(3648.2831, 3648.2743, 3) == true);
    REQUIRE(gul::withinOrders(3648.2831, 3648.2743, 4) == true);
    REQUIRE(gul::withinOrders(3648.2831, 3648.2743, 5) == true);
    REQUIRE(gul::withinOrders(3648.2831, 3648.2743, 6) == false);
    REQUIRE(gul::withinOrders(3648.2831, 3648.2743, 7) == false);
    REQUIRE(gul::withinOrders(3648.2831, 3648.2743, 8) == false);

    // equal digits marked:              1234
    REQUIRE(gul::withinOrders(3648.2831, 3648.3843, 2) == true);
    REQUIRE(gul::withinOrders(3648.2831, 3648.3843, 3) == true);
    REQUIRE(gul::withinOrders(3648.2831, 3648.3843, 4) == true);
    REQUIRE(gul::withinOrders(3648.2831, 3648.3843, 5) == false);
    REQUIRE(gul::withinOrders(3648.2831, 3648.3843, 6) == false);
    REQUIRE(gul::withinOrders(3648.2831, 3648.3843, 7) == false);
    REQUIRE(gul::withinOrders(3648.2831, 3648.3843, 8) == false);

    // equal digits marked:              123
    REQUIRE(gul::withinOrders(3648.2831, 3649.2843, 2) == true);
    REQUIRE(gul::withinOrders(3648.2831, 3649.2843, 3) == true);
    REQUIRE(gul::withinOrders(3648.2831, 3649.2843, 4) == false);
    REQUIRE(gul::withinOrders(3648.2831, 3649.2843, 5) == false);
    REQUIRE(gul::withinOrders(3648.2831, 3649.2843, 6) == false);
    REQUIRE(gul::withinOrders(3648.2831, 3649.2843, 7) == false);
    REQUIRE(gul::withinOrders(3648.2831, 3649.2843, 8) == false);

    // equal digits marked:              12
    REQUIRE(gul::withinOrders(3648.2831, 3638.2843, 2) == true);
    REQUIRE(gul::withinOrders(3648.2831, 3638.2843, 3) == false);
    REQUIRE(gul::withinOrders(3648.2831, 3638.2843, 4) == false);
    REQUIRE(gul::withinOrders(3648.2831, 3638.2843, 5) == false);
    REQUIRE(gul::withinOrders(3648.2831, 3638.2843, 6) == false);
    REQUIRE(gul::withinOrders(3648.2831, 3638.2843, 7) == false);
    REQUIRE(gul::withinOrders(3648.2831, 3638.2843, 8) == false);

    // equal digits marked:              1
    REQUIRE(gul::withinOrders(3648.2831, 3748.2843, 1) == true);
    REQUIRE(gul::withinOrders(3648.2831, 3748.2843, 2) == false);
    REQUIRE(gul::withinOrders(3648.2831, 3748.2843, 3) == false);
    REQUIRE(gul::withinOrders(3648.2831, 3748.2843, 4) == false);
    REQUIRE(gul::withinOrders(3648.2831, 3748.2843, 5) == false);
    REQUIRE(gul::withinOrders(3648.2831, 3748.2843, 6) == false);
    REQUIRE(gul::withinOrders(3648.2831, 3748.2843, 7) == false);
    REQUIRE(gul::withinOrders(3648.2831, 3748.2843, 8) == false);

    REQUIRE(gul::withinOrders(1.01, 1.010, 2) == true);
    REQUIRE(gul::withinOrders(1.01, 1.011, 2) == true);
    REQUIRE(gul::withinOrders(1.01, 1.012, 2) == true);
    REQUIRE(gul::withinOrders(1.01, 1.013, 2) == true); // 0.3 % off
    REQUIRE(gul::withinOrders(1.01, 1.024, 2) == false); // 1.4 % off

    // floating point special values must always fail
    REQUIRE(gul::withinOrders(std::nan(""), 1.0, 2) == false);
    REQUIRE(gul::withinOrders(1.0, std::nan(""), 2) == false);
    REQUIRE(gul::withinOrders(std::numeric_limits<double>::infinity(), 1.0, 2) == false);
    REQUIRE(gul::withinOrders(1.0, std::numeric_limits<double>::infinity(), 2) == false);
    REQUIRE(gul::withinOrders(-1E10, 1.0, 2) == false);
    REQUIRE(gul::withinOrders(-std::numeric_limits<double>::infinity(), 1.0, 2) == false);
    REQUIRE(gul::withinOrders(1.0, -std::numeric_limits<double>::infinity(), 2) == false);

    // floating point special values must always fail, except when they don't :->
    REQUIRE(gul::withinOrders(1.0, 1.01, std::nan("")) == false);
    REQUIRE(gul::withinOrders(1.0, 1.01, std::numeric_limits<double>::infinity()) == false);
    REQUIRE(gul::withinOrders(1.0, 1.01, -std::numeric_limits<double>::infinity()) == true);
}

TEST_CASE("test withinAbs()", "[numerics]")
{
    REQUIRE(gul::withinAbs(1.0, 101.0, 2.0) == false);
    REQUIRE(gul::withinAbs(101.0, 1.0, 2.0) == false);
    REQUIRE(gul::withinAbs(1.01, 1.02, 0.010001) == true);
    REQUIRE(gul::withinAbs(1.01, 1.002, 0.010001) == true);

    // equal digits marked:             12345
    REQUIRE(gul::withinAbs(0.6482831, 0.6482843, 0.01) == true);
    REQUIRE(gul::withinAbs(0.6482831, 0.6482843, 0.001) == true);
    REQUIRE(gul::withinAbs(0.6482831, 0.6482843, 0.0001) == true);
    REQUIRE(gul::withinAbs(0.6482831, 0.6482843, 0.00001) == true);
    REQUIRE(gul::withinAbs(0.6482831, 0.6482843, 0.000001) == false);
    REQUIRE(gul::withinAbs(0.6482831, 0.6482843, 0.0000001) == false);
    REQUIRE(gul::withinAbs(0.6482831, 0.6482843, 0.00000001) == false);

    // equal digits marked:           1234 56
    REQUIRE(gul::withinAbs(3.6482831, 3.6482843, 0.01) == true);
    REQUIRE(gul::withinAbs(3.6482831, 3.6482843, 0.001) == true);
    REQUIRE(gul::withinAbs(3.6482831, 3.6482843, 0.0001) == true);
    REQUIRE(gul::withinAbs(3.6482831, 3.6482843, 0.00001) == true);
    REQUIRE(gul::withinAbs(3.6482831, 3.6482843, 0.000001) == false);
    REQUIRE(gul::withinAbs(3.6482831, 3.6482843, 0.0000001) == false);
    REQUIRE(gul::withinAbs(3.6482831, 3.6482843, 0.00000001) == false);

    // equal digits marked:           1234 56
    REQUIRE(gul::withinAbs(3.6482831, 3.6482820, 0.01) == true);
    REQUIRE(gul::withinAbs(3.6482831, 3.6482820, 0.001) == true);
    REQUIRE(gul::withinAbs(3.6482831, 3.6482820, 0.0001) == true);
    REQUIRE(gul::withinAbs(3.6482831, 3.6482820, 0.00001) == true);
    REQUIRE(gul::withinAbs(3.6482831, 3.6482820, 0.000001) == false);
    REQUIRE(gul::withinAbs(3.6482831, 3.6482820, 0.0000001) == false);
    REQUIRE(gul::withinAbs(3.6482831, 3.6482820, 0.00000001) == false);

    // equal digits marked:           1234 56
    REQUIRE(gul::withinAbs(3648.2831, 3648.2843, 1.0) == true);
    REQUIRE(gul::withinAbs(3648.2831, 3648.2843, 0.1) == true);
    REQUIRE(gul::withinAbs(3648.2831, 3648.2843, 0.01) == true);
    REQUIRE(gul::withinAbs(3648.2831, 3648.2843, 0.001) == false);
    REQUIRE(gul::withinAbs(3648.2831, 3648.2843, 0.0001) == false);
    REQUIRE(gul::withinAbs(3648.2831, 3648.2843, 0.00001) == false);
    REQUIRE(gul::withinAbs(3648.2831, 3648.2843, 0.000001) == false);

    // equal digits marked:           1234 5
    REQUIRE(gul::withinAbs(3648.2831, 3648.2743, 1.0) == true);
    REQUIRE(gul::withinAbs(3648.2831, 3648.2743, 0.1) == true);
    REQUIRE(gul::withinAbs(3648.2831, 3648.2743, 0.01) == true);
    REQUIRE(gul::withinAbs(3648.2831, 3648.2743, 0.001) == false);
    REQUIRE(gul::withinAbs(3648.2831, 3648.2743, 0.0001) == false);
    REQUIRE(gul::withinAbs(3648.2831, 3648.2743, 0.00001) == false);
    REQUIRE(gul::withinAbs(3648.2831, 3648.2743, 0.000001) == false);

    // equal digits marked:           1234
    REQUIRE(gul::withinAbs(3648.2831, 3648.3843, 1.0) == true);
    REQUIRE(gul::withinAbs(3648.2831, 3648.3843, 0.1) == false);
    REQUIRE(gul::withinAbs(3648.2831, 3648.3843, 0.01) == false);
    REQUIRE(gul::withinAbs(3648.2831, 3648.3843, 0.001) == false);
    REQUIRE(gul::withinAbs(3648.2831, 3648.3843, 0.0001) == false);
    REQUIRE(gul::withinAbs(3648.2831, 3648.3843, 0.00001) == false);
    REQUIRE(gul::withinAbs(3648.2831, 3648.3843, 0.000001) == false);

    REQUIRE(gul::withinAbs(3648.2831, 3649.2843, 1.0) == false);

    // equal digits marked:           12
    REQUIRE(gul::withinAbs(3648.2831, 3638.2843, 1000.0) == true);
    REQUIRE(gul::withinAbs(3648.2831, 3638.2843, 100.0) == true);
    REQUIRE(gul::withinAbs(3648.2831, 3638.2843, 10.0) == true);
    REQUIRE(gul::withinAbs(3648.2831, 3638.2823, 10.0) == false);
    REQUIRE(gul::withinAbs(3648.2831, 3638.2843, 1.0) == false);

    // equal digits marked:           1
    REQUIRE(gul::withinAbs(3648.2831, 3748.2843, 1000.0) == true);
    REQUIRE(gul::withinAbs(3648.2831, 3748.2843, 100.0) == false);
    REQUIRE(gul::withinAbs(3648.2831, 3748.2843, 10.0) == false);
    REQUIRE(gul::withinAbs(3648.2831, 3748.2843, 1.0) == false);

    // diff smaller than epsilon()
    REQUIRE(gul::withinAbs(1.23E77, 1.23E77, 1.0) == true);

    // floating point special values must always fail
    REQUIRE(gul::withinAbs(std::nan(""), 1.0, 1.0) == false);
    REQUIRE(gul::withinAbs(1.0, std::nan(""), 1.0) == false);
    REQUIRE(gul::withinAbs(std::numeric_limits<double>::infinity(), 1.0, 1.0) == false);
    REQUIRE(gul::withinAbs(1.0, std::numeric_limits<double>::infinity(), 1.0) == false);
    REQUIRE(gul::withinAbs(-std::numeric_limits<double>::infinity(), 1.0, 1.0) == false);
    REQUIRE(gul::withinAbs(1.0, -std::numeric_limits<double>::infinity(), 1.0) == false);

    // floating point special values must always fail, except when they don't :->
    REQUIRE(gul::withinAbs(1.0, 2.0, std::nan("")) == false);
    REQUIRE(gul::withinAbs(1.0, 2.0, std::numeric_limits<double>::infinity()) == true);

    // including boundaries
    REQUIRE(gul::withinAbs(7.0, 8.0, 1.0) == true);
    REQUIRE(gul::withinAbs(8.0, 7.0, 1.0) == true);

    // only very few tests on integers
    REQUIRE(gul::withinAbs(7, 8, 1) == true);
    REQUIRE(gul::withinAbs(8, 7, 1) == true);
    REQUIRE(gul::withinAbs(7, 9, 1) == false);
    REQUIRE(gul::withinAbs(9, 7, 1) == false);
    REQUIRE(gul::withinAbs(7u, 8u, 1u) == true);
    REQUIRE(gul::withinAbs(8u, 7u, 1u) == true);
    REQUIRE(gul::withinAbs(7u, 9u, 1u) == false);
    REQUIRE(gul::withinAbs(9u, 7u, 1u) == false);

    // integer wraparound test
    auto i1 = std::numeric_limits<int>::min() + 10;
    auto i2 = i1 - 1;
    auto i3 = 60;
    REQUIRE(gul::withinAbs(i1, i2, i3) == true);
}

TEST_CASE("test withinULP()", "[numerics]")
{
    REQUIRE(gul::withinULP(1.0, 1.0 + 1 * std::numeric_limits<double>::epsilon(), 0) == false);
    REQUIRE(gul::withinULP(1.0, 1.0 + 1 * std::numeric_limits<double>::epsilon(), 1) == true);
    REQUIRE(gul::withinULP(1.0, 1.0 - 1 * std::numeric_limits<double>::epsilon(), 0) == false);
    REQUIRE(gul::withinULP(1.0, 1.0 - 1 * std::numeric_limits<double>::epsilon(), 1) == true);

    REQUIRE(gul::withinULP(1.0, 1.0 + 3 * std::numeric_limits<double>::epsilon(), 1) == false);
    REQUIRE(gul::withinULP(1.0, 1.0 + 3 * std::numeric_limits<double>::epsilon(), 2) == false);
    REQUIRE(gul::withinULP(1.0, 1.0 + 3 * std::numeric_limits<double>::epsilon(), 3) == true);
    REQUIRE(gul::withinULP(1.0, 1.0 - 3 * std::numeric_limits<double>::epsilon(), 1) == false);
    REQUIRE(gul::withinULP(1.0, 1.0 - 3 * std::numeric_limits<double>::epsilon(), 2) == false);
    REQUIRE(gul::withinULP(1.0, 1.0 - 3 * std::numeric_limits<double>::epsilon(), 3) == true);

    // Typical 4 byte float values allow ~6-7 meaningfull digits, more digits are lost to ULP
    static_assert(sizeof(float) == 4, "Test needs redesign because float resolution changed");
    REQUIRE(gul::withinULP(543.0f, 543.001f, 3) == false);
    REQUIRE(gul::withinULP(543.0f, 543.0001f, 3) == true);
}

// vi:ts=4:sw=4:et
