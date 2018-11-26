/**
 * \file   test_cat.cc
 * \author \ref contributors
 * \date   Created on August 30, 2018
 * \brief  Test suite for cat() from the General Utility Library.
 *
 * \copyright Copyright 2018 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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

using namespace std::literals;
using gul::cat;

TEST_CASE("gul::cat() works for fixed test cases from 0 to 6 arguments", "[cat]")
{
    REQUIRE(cat() == "");
    REQUIRE(cat("Lo") == "Lo");
    REQUIRE(cat("Lu", "la") == "Lula");
    REQUIRE(cat("La", "le", "lu") == "Lalelu");
    REQUIRE(cat("La", "le", "lu", 42) == "Lalelu42");
    REQUIRE(cat("La", "le", "lu", 42, '#') == "Lalelu42#");
    REQUIRE(cat("La", "le", "lu", 42, " # ", 6.0) == "Lalelu42 # 6.000000");
}
