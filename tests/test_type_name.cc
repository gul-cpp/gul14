/**
 * \file   test_type_name.cc
 * \author \ref contributors
 * \date   Created on April 11, 2019
 * \brief  Test suite for type_name().
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

#include <sstream>

#include "gul/catch.h"
#include "gul/type_name.h"

using namespace std::literals::string_literals;
using namespace Catch::Matchers;

template <typename T>
class clever {
public:
    explicit clever(T) {}
};

TEST_CASE("Type-name Test", "[type_name]")
{
    SECTION("test some stdlib types") {
        auto oss = std::ostringstream{ };
        oss << gul::type_name<decltype(oss)>();
        REQUIRE_THAT(oss.str(), Contains("ostringstream"));

        oss.str("");
        oss << gul::type_name<std::string>();
        REQUIRE_THAT(oss.str(), Contains("basic_string"));

        oss.str("");
        oss << gul::type_name<std::size_t>();
        REQUIRE_THAT(oss.str(), Contains("unsigned"));
        REQUIRE_THAT(oss.str(), Contains("int") or Contains("long"));

        // Check that the indices on __PRETTY_FUNCTION__ are correct
        REQUIRE_THAT(oss.str(), !Contains("["));
        REQUIRE_THAT(oss.str(), !Contains("]"));
    }
    SECTION("test some basic types") {
        auto oss = std::ostringstream{ };
        oss << gul::type_name<int>();
        REQUIRE_THAT(oss.str(), Contains("int"));

        auto& x = "test";
        oss.str("");
        oss << gul::type_name<decltype(x)>();
        REQUIRE_THAT(oss.str(), Contains("const"));
        REQUIRE_THAT(oss.str(), Contains("char"));
        REQUIRE_THAT(oss.str(), Contains("[5]"));

        oss.str("");
        oss << gul::type_name<std::decay_t<decltype(x)>>();
        REQUIRE_THAT(oss.str(), Contains("const"));
        REQUIRE_THAT(oss.str(), Contains("char"));
        REQUIRE_THAT(oss.str(), Contains("*"));

        auto f = 2.3f;
        oss.str("");
        oss << gul::type_name<decltype(f)>();
        REQUIRE_THAT(oss.str(), Contains("float"));
    }
    SECTION("test some user types") {
        auto c = clever<int>{ 1 };
        auto oss = std::ostringstream{ };
        oss << gul::type_name<decltype(c)>();
        REQUIRE_THAT(oss.str(), Contains("clever"));
        REQUIRE_THAT(oss.str(), Contains("int"));
    }
    SECTION("test compile type output") {
        // There is not much checking here, because the type
        // of 'c' will be output in a compiler error message,
        // when the following is not commented out:

        // auto c = clever<int>{ 1 };
        // gul::FailToInstantiate<decltype(c)>{};
    }
}

// vi:ts=4:sw=4:et:sts=4
