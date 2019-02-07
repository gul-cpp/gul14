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
}

// vi:ts=4:sw=4:et
