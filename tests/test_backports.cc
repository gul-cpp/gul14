/**
 * \file   test_backports.cc
 * \author Lars Froehlich
 * \date   Created on August 30, 2018
 * \brief  Test suite for standard library backports in the General Utility Library.
 */

#include "catch.h"
#include "gul.h"

using namespace std::literals;

TEST_CASE("gul::string_view accepts a string as both char * and std::string, and both "
          "compare equal", "[string_view]")
{
    REQUIRE( gul::string_view{"Test"} == gul::string_view{"Test"s} );
    REQUIRE( gul::string_view{""} == gul::string_view{""s} );
}
