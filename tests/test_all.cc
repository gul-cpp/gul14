/**
 * \file   test_all.cc
 * \author Lars Froehlich
 * \date   Created on August 28, 2018
 * \brief  Test suite for the General Utility Library.
 */

// This tells Catch to provide a main() - only do this in one cc file
#define CATCH_CONFIG_MAIN

#include "catch.h"
#include "gul.h"

using namespace std::literals;

TEST_CASE("gul::string_view accepts a string as both char * and std::string, and both "
          "compare equal", "[string_view]")
{
    REQUIRE( gul::string_view{"Test"} == gul::string_view{"Test"s} );
    REQUIRE( gul::string_view{""} == gul::string_view{""s} );
}
