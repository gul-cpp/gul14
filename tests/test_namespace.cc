/**
 * \file   test_namespace.cc
 * \author \ref contributors
 * \date   Created on December 12, 2019
 * \brief  Unit tests for the alias namespace gul::.
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

#include "gul14/catch.h"
#include "gul.h"

TEST_CASE("Symbols can be reached via alias namespace gul::", "[namespace]")
{
    REQUIRE(gul::cat('a', 'b', 'c') == "abc");
    REQUIRE(gul::to_number<int>("42") == 42);
    REQUIRE(gul::version_api.empty() == false);
}

// vi:ts=4:sw=4:sts=4:et
