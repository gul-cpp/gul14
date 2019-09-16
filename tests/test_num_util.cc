/**
 * \file   test_num_utils.cc
 * \author \ref contributors
 * \date   Created on 7 Feb 2019
 * \brief  Unit tests for within_orders(), within_abs(), and within_ulp().
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
#include "gul/num_util.h"

TEST_CASE("test within_orders()", "[num_util]")
{
    REQUIRE(gul::within_orders(1.0, 101.0, 2) == false);
    REQUIRE(gul::within_orders(101.0, 1.0, 2) == false);
    REQUIRE(gul::within_orders(1.01l, 1.0201l, 2) == true);
    REQUIRE(gul::within_orders(1.01l, 1.0902l, 2) == false);
    REQUIRE(gul::within_orders(1.01, 1.02, 2) == true);
    REQUIRE(gul::within_orders(1.01, 1.002, 2) == true);

    // crossing zero
    REQUIRE(gul::within_orders(1.0, -2.0, 2) == false);
    REQUIRE(gul::within_orders(-1.01, 1.02, 2) == false);
    REQUIRE(gul::within_orders(-1.01, -1.02, 2) == true);

    // negative orders
    REQUIRE(gul::within_orders(1.0, 100.0, -2) == true);

    // equal digits marked:                 12345
    REQUIRE(gul::within_orders(0.6482831, 0.6482843, 2) == true);
    REQUIRE(gul::within_orders(0.6482831, 0.6482843, 3) == true);
    REQUIRE(gul::within_orders(0.6482831, 0.6482843, 4) == true);
    REQUIRE(gul::within_orders(0.6482831, 0.6482843, 5) == true);
    REQUIRE(gul::within_orders(0.6482831, 0.6482843, 6) == false);
    REQUIRE(gul::within_orders(0.6482831, 0.6482843, 7) == false);
    REQUIRE(gul::within_orders(0.6482831, 0.6482843, 8) == false);

    // equal digits marked:               1 23456
    REQUIRE(gul::within_orders(3.6482831, 3.6482843, 2) == true);
    REQUIRE(gul::within_orders(3.6482831, 3.6482843, 3) == true);
    REQUIRE(gul::within_orders(3.6482831, 3.6482843, 4) == true);
    REQUIRE(gul::within_orders(3.6482831, 3.6482843, 5) == true);
    REQUIRE(gul::within_orders(3.6482831, 3.6482843, 6) == true);
    REQUIRE(gul::within_orders(3.6482831, 3.6482843, 7) == false);
    REQUIRE(gul::within_orders(3.6482831, 3.6482843, 8) == false);

    // equal digits marked:               1 23456
    REQUIRE(gul::within_orders(3.6482831, 3.6482820, 2) == true);
    REQUIRE(gul::within_orders(3.6482831, 3.6482820, 3) == true);
    REQUIRE(gul::within_orders(3.6482831, 3.6482820, 4) == true);
    REQUIRE(gul::within_orders(3.6482831, 3.6482820, 5) == true);
    REQUIRE(gul::within_orders(3.6482831, 3.6482820, 6) == true);
    REQUIRE(gul::within_orders(3.6482831, 3.6482820, 7) == false);
    REQUIRE(gul::within_orders(3.6482831, 3.6482820, 8) == false);

    // equal digits marked:               1234 56
    REQUIRE(gul::within_orders(3648.2831, 3648.2843, 2) == true);
    REQUIRE(gul::within_orders(3648.2831, 3648.2843, 3) == true);
    REQUIRE(gul::within_orders(3648.2831, 3648.2843, 4) == true);
    REQUIRE(gul::within_orders(3648.2831, 3648.2843, 5) == true);
    REQUIRE(gul::within_orders(3648.2831, 3648.2843, 6) == true);
    REQUIRE(gul::within_orders(3648.2831, 3648.2843, 7) == false);
    REQUIRE(gul::within_orders(3648.2831, 3648.2843, 8) == false);

    // equal digits marked:               1234 5
    REQUIRE(gul::within_orders(3648.2831, 3648.2743, 2) == true);
    REQUIRE(gul::within_orders(3648.2831, 3648.2743, 3) == true);
    REQUIRE(gul::within_orders(3648.2831, 3648.2743, 4) == true);
    REQUIRE(gul::within_orders(3648.2831, 3648.2743, 5) == true);
    REQUIRE(gul::within_orders(3648.2831, 3648.2743, 6) == false);
    REQUIRE(gul::within_orders(3648.2831, 3648.2743, 7) == false);
    REQUIRE(gul::within_orders(3648.2831, 3648.2743, 8) == false);

    // equal digits marked:               1234
    REQUIRE(gul::within_orders(3648.2831, 3648.3843, 2) == true);
    REQUIRE(gul::within_orders(3648.2831, 3648.3843, 3) == true);
    REQUIRE(gul::within_orders(3648.2831, 3648.3843, 4) == true);
    REQUIRE(gul::within_orders(3648.2831, 3648.3843, 5) == false);
    REQUIRE(gul::within_orders(3648.2831, 3648.3843, 6) == false);
    REQUIRE(gul::within_orders(3648.2831, 3648.3843, 7) == false);
    REQUIRE(gul::within_orders(3648.2831, 3648.3843, 8) == false);

    // equal digits marked:               123
    REQUIRE(gul::within_orders(3648.2831, 3649.2843, 2) == true);
    REQUIRE(gul::within_orders(3648.2831, 3649.2843, 3) == true);
    REQUIRE(gul::within_orders(3648.2831, 3649.2843, 4) == false);
    REQUIRE(gul::within_orders(3648.2831, 3649.2843, 5) == false);
    REQUIRE(gul::within_orders(3648.2831, 3649.2843, 6) == false);
    REQUIRE(gul::within_orders(3648.2831, 3649.2843, 7) == false);
    REQUIRE(gul::within_orders(3648.2831, 3649.2843, 8) == false);

    // equal digits marked:               12
    REQUIRE(gul::within_orders(3648.2831, 3638.2843, 2) == true);
    REQUIRE(gul::within_orders(3648.2831, 3638.2843, 3) == false);
    REQUIRE(gul::within_orders(3648.2831, 3638.2843, 4) == false);
    REQUIRE(gul::within_orders(3648.2831, 3638.2843, 5) == false);
    REQUIRE(gul::within_orders(3648.2831, 3638.2843, 6) == false);
    REQUIRE(gul::within_orders(3648.2831, 3638.2843, 7) == false);
    REQUIRE(gul::within_orders(3648.2831, 3638.2843, 8) == false);

    // equal digits marked:               1
    REQUIRE(gul::within_orders(3648.2831, 3748.2843, 1) == true);
    REQUIRE(gul::within_orders(3648.2831, 3748.2843, 2) == false);
    REQUIRE(gul::within_orders(3648.2831, 3748.2843, 3) == false);
    REQUIRE(gul::within_orders(3648.2831, 3748.2843, 4) == false);
    REQUIRE(gul::within_orders(3648.2831, 3748.2843, 5) == false);
    REQUIRE(gul::within_orders(3648.2831, 3748.2843, 6) == false);
    REQUIRE(gul::within_orders(3648.2831, 3748.2843, 7) == false);
    REQUIRE(gul::within_orders(3648.2831, 3748.2843, 8) == false);

    REQUIRE(gul::within_orders(1.01, 1.010, 2) == true);
    REQUIRE(gul::within_orders(1.01, 1.011, 2) == true);
    REQUIRE(gul::within_orders(1.01, 1.012, 2) == true);
    REQUIRE(gul::within_orders(1.01, 1.013, 2) == true); // 0.3 % off
    REQUIRE(gul::within_orders(1.01, 1.024, 2) == false); // 1.4 % off

    // floating point special values must always fail
    REQUIRE(gul::within_orders(std::nan(""), 1.0, 2) == false);
    REQUIRE(gul::within_orders(1.0, std::nan(""), 2) == false);
    REQUIRE(gul::within_orders(std::numeric_limits<double>::infinity(), 1.0, 2) == false);
    REQUIRE(gul::within_orders(1.0, std::numeric_limits<double>::infinity(), 2) == false);
    REQUIRE(gul::within_orders(-1E10, 1.0, 2) == false);
    REQUIRE(gul::within_orders(-std::numeric_limits<double>::infinity(), 1.0, 2) == false);
    REQUIRE(gul::within_orders(1.0, -std::numeric_limits<double>::infinity(), 2) == false);

    // floating point special values must always fail, except when they don't :->
    REQUIRE(gul::within_orders(1.0, 1.01, std::nan("")) == false);
    REQUIRE(gul::within_orders(1.0, 1.01, std::numeric_limits<double>::infinity()) == false);
    REQUIRE(gul::within_orders(1.0, 1.01, -std::numeric_limits<double>::infinity()) == true);
    REQUIRE(gul::within_orders(1.0, 0.0, 1) == false);
    REQUIRE(gul::within_orders(1.0, 0.0, 0) == false);

    // From the Doxygen example with integers
    REQUIRE(gul::within_orders(23736384, 23736228, 5) == true);
    REQUIRE(gul::within_orders(23736384, 23735384, 5) == false);
}

TEST_CASE("test within_abs()", "[num_util]")
{
    REQUIRE(gul::within_abs(1.0, 101.0, 2.0) == false);
    REQUIRE(gul::within_abs(101.0, 1.0, 2.0) == false);
    REQUIRE(gul::within_abs(1.01, 1.02, 0.010001) == true);
    REQUIRE(gul::within_abs(1.01, 1.002, 0.010001) == true);

    // crossing zero
    REQUIRE(gul::within_abs(2.0, -2.0, 0.1) == false);
    REQUIRE(gul::within_abs(-1.01, 1.02, 0.1) == false);
    REQUIRE(gul::within_abs(-1.01, -1.02, 0.02) == true);
    REQUIRE(gul::within_abs(-1.01, -1.02, -0.02) == true);

    // equal digits marked:              12345
    REQUIRE(gul::within_abs(0.6482831, 0.6482843, 0.01) == true);
    REQUIRE(gul::within_abs(0.6482831, 0.6482843, 0.001) == true);
    REQUIRE(gul::within_abs(0.6482831, 0.6482843, 0.0001) == true);
    REQUIRE(gul::within_abs(0.6482831, 0.6482843, 0.00001) == true);
    REQUIRE(gul::within_abs(0.6482831, 0.6482843, 0.000001) == false);
    REQUIRE(gul::within_abs(0.6482831, 0.6482843, 0.0000001) == false);
    REQUIRE(gul::within_abs(0.6482831, 0.6482843, 0.00000001) == false);

    // equal digits marked:            1 23456
    REQUIRE(gul::within_abs(3.6482831, 3.6482843, 0.01) == true);
    REQUIRE(gul::within_abs(3.6482831, 3.6482843, 0.001) == true);
    REQUIRE(gul::within_abs(3.6482831, 3.6482843, 0.0001) == true);
    REQUIRE(gul::within_abs(3.6482831, 3.6482843, 0.00001) == true);
    REQUIRE(gul::within_abs(3.6482831, 3.6482843, 0.000001) == false);
    REQUIRE(gul::within_abs(3.6482831, 3.6482843, 0.0000001) == false);
    REQUIRE(gul::within_abs(3.6482831, 3.6482843, 0.00000001) == false);

    // equal digits marked:            1 23456
    REQUIRE(gul::within_abs(3.6482831, 3.6482820, 0.01) == true);
    REQUIRE(gul::within_abs(3.6482831, 3.6482820, 0.001) == true);
    REQUIRE(gul::within_abs(3.6482831, 3.6482820, 0.0001) == true);
    REQUIRE(gul::within_abs(3.6482831, 3.6482820, 0.00001) == true);
    REQUIRE(gul::within_abs(3.6482831, 3.6482820, 0.000001) == false);
    REQUIRE(gul::within_abs(3.6482831, 3.6482820, 0.0000001) == false);
    REQUIRE(gul::within_abs(3.6482831, 3.6482820, 0.00000001) == false);

    // equal digits marked:            1234 56
    REQUIRE(gul::within_abs(3648.2831, 3648.2843, 1.0) == true);
    REQUIRE(gul::within_abs(3648.2831, 3648.2843, 0.1) == true);
    REQUIRE(gul::within_abs(3648.2831, 3648.2843, 0.01) == true);
    REQUIRE(gul::within_abs(3648.2831, 3648.2843, 0.001) == false);
    REQUIRE(gul::within_abs(3648.2831, 3648.2843, 0.0001) == false);
    REQUIRE(gul::within_abs(3648.2831, 3648.2843, 0.00001) == false);
    REQUIRE(gul::within_abs(3648.2831, 3648.2843, 0.000001) == false);

    // equal digits marked:            1234 5
    REQUIRE(gul::within_abs(3648.2831, 3648.2743, 1.0) == true);
    REQUIRE(gul::within_abs(3648.2831, 3648.2743, 0.1) == true);
    REQUIRE(gul::within_abs(3648.2831, 3648.2743, 0.01) == true);
    REQUIRE(gul::within_abs(3648.2831, 3648.2743, 0.001) == false);
    REQUIRE(gul::within_abs(3648.2831, 3648.2743, 0.0001) == false);
    REQUIRE(gul::within_abs(3648.2831, 3648.2743, 0.00001) == false);
    REQUIRE(gul::within_abs(3648.2831, 3648.2743, 0.000001) == false);

    // equal digits marked:            1234
    REQUIRE(gul::within_abs(3648.2831, 3648.3843, 1.0) == true);
    REQUIRE(gul::within_abs(3648.2831, 3648.3843, 0.1) == false);
    REQUIRE(gul::within_abs(3648.2831, 3648.3843, 0.01) == false);
    REQUIRE(gul::within_abs(3648.2831, 3648.3843, 0.001) == false);
    REQUIRE(gul::within_abs(3648.2831, 3648.3843, 0.0001) == false);
    REQUIRE(gul::within_abs(3648.2831, 3648.3843, 0.00001) == false);
    REQUIRE(gul::within_abs(3648.2831, 3648.3843, 0.000001) == false);

    REQUIRE(gul::within_abs(3648.2831, 3649.2843, 1.0) == false);

    // equal digits marked:            12
    REQUIRE(gul::within_abs(3648.2831, 3638.2843, 1000.0) == true);
    REQUIRE(gul::within_abs(3648.2831, 3638.2843, 100.0) == true);
    REQUIRE(gul::within_abs(3648.2831, 3638.2843, 10.0) == true);
    REQUIRE(gul::within_abs(3648.2831, 3638.2823, 10.0) == false);
    REQUIRE(gul::within_abs(3648.2831, 3638.2843, 1.0) == false);

    // equal digits marked:            1
    REQUIRE(gul::within_abs(3648.2831, 3748.2843, 1000.0) == true);
    REQUIRE(gul::within_abs(3648.2831, 3748.2843, 100.0) == false);
    REQUIRE(gul::within_abs(3648.2831, 3748.2843, 10.0) == false);
    REQUIRE(gul::within_abs(3648.2831, 3748.2843, 1.0) == false);

    // diff smaller than epsilon()
    REQUIRE(gul::within_abs(1.23E77, 1.23E77, 1.0) == true);

    // floating point special values must always fail
    REQUIRE(gul::within_abs(std::nan(""), 1.0, 1.0) == false);
    REQUIRE(gul::within_abs(1.0, std::nan(""), 1.0) == false);
    REQUIRE(gul::within_abs(std::numeric_limits<double>::infinity(), 1.0, 1.0) == false);
    REQUIRE(gul::within_abs(1.0, std::numeric_limits<double>::infinity(), 1.0) == false);
    REQUIRE(gul::within_abs(-std::numeric_limits<double>::infinity(), 1.0, 1.0) == false);
    REQUIRE(gul::within_abs(1.0, -std::numeric_limits<double>::infinity(), 1.0) == false);

    // floating point special values must always fail, except when they don't :->
    REQUIRE(gul::within_abs(1.0, 2.0, std::nan("")) == false);
    REQUIRE(gul::within_abs(1.0, 2.0, std::numeric_limits<double>::infinity()) == true);

    // including boundaries
    REQUIRE(gul::within_abs(7.0, 8.0, 1.0) == true);
    REQUIRE(gul::within_abs(8.0, 7.0, 1.0) == true);

    // only very few tests on integers
    REQUIRE(gul::within_abs(7, 8, 1) == true);
    REQUIRE(gul::within_abs(8, 7, 1) == true);
    REQUIRE(gul::within_abs(7, 9, 1) == false);
    REQUIRE(gul::within_abs(9, 7, 1) == false);
    REQUIRE(gul::within_abs(7u, 8u, 1u) == true);
    REQUIRE(gul::within_abs(8u, 7u, 1u) == true);
    REQUIRE(gul::within_abs(7u, 9u, 1u) == false);
    REQUIRE(gul::within_abs(9u, 7u, 1u) == false);

    // integer wraparound test
    auto i1 = std::numeric_limits<int>::min() + 10;
    auto i2 = i1 - 1;
    auto i3 = 60;
    REQUIRE(gul::within_abs(i1, i2, i3) == true);
}

TEST_CASE("test within_ulp()", "[num_util]")
{
    REQUIRE(gul::within_ulp(3.0f/7.0f, 0.42857143282890319824f, 0));

    REQUIRE(gul::within_ulp(1.0, 1.0 + 1 * std::numeric_limits<double>::epsilon(), 0) == false);
    REQUIRE(gul::within_ulp(1.0, 1.0 + 1 * std::numeric_limits<double>::epsilon(), 1) == true);
    REQUIRE(gul::within_ulp(1.0, 1.0 - 1 * std::numeric_limits<double>::epsilon(), 0) == false);
    REQUIRE(gul::within_ulp(1.0, 1.0 - 1 * std::numeric_limits<double>::epsilon(), 1) == true);

    REQUIRE(gul::within_ulp(1.0, 1.0 + 3 * std::numeric_limits<double>::epsilon(), 1) == false);
    REQUIRE(gul::within_ulp(1.0, 1.0 + 3 * std::numeric_limits<double>::epsilon(), 2) == false);
    REQUIRE(gul::within_ulp(1.0, 1.0 + 3 * std::numeric_limits<double>::epsilon(), 3) == true);
    REQUIRE(gul::within_ulp(1.0, 1.0 - 3 * std::numeric_limits<double>::epsilon(), 1) == false);
    REQUIRE(gul::within_ulp(1.0, 1.0 - 3 * std::numeric_limits<double>::epsilon(), 2) == false);
    REQUIRE(gul::within_ulp(1.0, 1.0 - 3 * std::numeric_limits<double>::epsilon(), 3) == true);

    // Typical 4 byte float values allow ~6-7 meaningfull digits, more digits are lost to ULP
    static_assert(sizeof(float) == 4, "Test needs redesign because float resolution changed");
    REQUIRE(gul::within_ulp(543.0f, 543.001f, 3) == false);
    REQUIRE(gul::within_ulp(543.0f, 543.0001f, 3) == true);
}

// Some classes to test clamp()
// We need them public to be able to use operator<() (on B{}).
class A {
protected:
    double m1;
    double m2;
public:
    A(double v1, double v2)
        : m1{ v1 }
        , m2{ v2 }
    {
    }
    double product() const { return m1 * m2; }
};

class B : public A {
public:
    B(double v1, double v2) : A(v1, v2)
    {
    }
    double val() const { return m1; }
    auto friend operator<(B const& a, B const& b) {
        return a.product() < b.product();
    }
};

TEST_CASE("test clamp()", "[num_util]")
{
    // Simplest test
    auto r1 = gul::clamp(5, 3, 7);
    auto r2 = gul::clamp(2, 3, 7);
    auto r3 = gul::clamp(9, 3, 7);
    REQUIRE(r1 == 5);
    REQUIRE(r2 == 3);
    REQUIRE(r3 == 7);

    char c = ' ';
    auto c1 = gul::clamp(c, 'a', 'z');
    REQUIRE(c1 == 'a'); // assumes ASCII or EBCDIC or any other usual char table

    // Test with user class, compare by products with lambda
    auto llimit_a = A{ 3.0, 4.2 };
    auto ulimit_a = A{ 23.1, 32.7 };
    auto v1 = A{ 1.0, 33.3 };
    auto v2 = A{ 1.2, 0.6 };
    auto v3 = A{ 52.1, 22.8 };

    auto x1 = gul::clamp(v1, llimit_a, ulimit_a, [](auto const& a, auto const& b) { return a.product() < b.product(); });
    auto x2 = gul::clamp(v2, llimit_a, ulimit_a, [](auto const& a, auto const& b) { return a.product() < b.product(); });
    auto x3 = gul::clamp(v3, llimit_a, ulimit_a, [](auto const& a, auto const& b) { return a.product() < b.product(); });
    REQUIRE(x1.product() == Approx(v1.product()));
    REQUIRE(x2.product() == Approx(llimit_a.product()));
    REQUIRE(x3.product() == Approx(ulimit_a.product()));

    // Test with user class, compare by products with operator<()
    // (std::min and std::max will be used)
    auto llimit_b = B{ 3.0, 4.2 };
    auto ulimit_b = B{ 23.1, 32.7 };
    auto v4 = B{ 1.0, 33.3 };
    auto v5 = B{ 1.2, 0.6 };
    auto v6 = B{ 52.1, 22.8 };

    auto x4 = gul::clamp(v4, llimit_b, ulimit_b);
    auto x5 = gul::clamp(v5, llimit_b, ulimit_b);
    auto x6 = gul::clamp(v6, llimit_b, ulimit_b);
    REQUIRE(x4.product() == Approx(v4.product()));
    REQUIRE(x5.product() == Approx(llimit_b.product()));
    REQUIRE(x6.product() == Approx(ulimit_b.product()));

    // Test with user class, compare by member value with lambda
    auto x7 = gul::clamp(v4, llimit_b, ulimit_b, [](auto const& a, auto const& b) { return a.val() < b.val(); });
    auto x8 = gul::clamp(v5, llimit_b, ulimit_b, [](auto const& a, auto const& b) { return a.val() < b.val(); });
    auto x9 = gul::clamp(v6, llimit_b, ulimit_b, [](auto const& a, auto const& b) { return a.val() < b.val(); });
    REQUIRE(x7.product() == Approx(llimit_b.product()));
    REQUIRE(x8.product() == Approx(llimit_b.product()));
    REQUIRE(x9.product() == Approx(ulimit_b.product()));
}

// vi:ts=4:sw=4:sts=4:et
