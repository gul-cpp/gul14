/**
 * \file   test_escape.cc
 * \author \ref contributors
 * \date   Created on August 31, 2018
 * \brief  Part of test suite for string utility functions in the General Utility Library.
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

#include <random>

#include "gul14/cat.h"
#include "gul14/catch.h"
#include "gul14/escape.h"

using namespace std::literals;

TEST_CASE("Compare escaped strings", "[escape]")
{
    REQUIRE(gul14::escape("foo bar baz"s) == "foo bar baz"s);

    // This shows the design idea quite nice:
    REQUIRE(gul14::escape("foo\rbar\nfoobar\tbaz\\qux\""s) ==
              R"(foo\rbar\nfoobar\tbaz\\qux\")"s);

    REQUIRE(gul14::escape("foo\abar\000baz"s) == R"(foo\x07bar\x00baz)"s);

    REQUIRE(gul14::escape("\xff") == "\\xff");
}

TEST_CASE("Compare unescaped strings", "[escape]")
{
    REQUIRE(gul14::unescape("foo bar baz"s) == "foo bar baz"s);

    REQUIRE(gul14::unescape(R"(foo\rbar\nfoobar\tbaz\\qux\")"s) ==
                "foo\rbar\nfoobar\tbaz\\qux\""s);

    REQUIRE(gul14::unescape(R"(foo\x07bar\x00baz)"s) == "foo\abar\000baz"s);

    REQUIRE(gul14::unescape("\\xff") == "\xff");

    auto const s = "foo\abar\000baz"s;
    REQUIRE(gul14::unescape(gul14::escape(s)) == s);
}

TEST_CASE("Check escaping and unescaping with random strings", "[escape]")
{
    std::random_device r;
    std::default_random_engine re(r());
    std::uniform_int_distribution<unsigned short> uniform_dist(0, 255);

    for (int len = 0; len < 100; len++)
    {
        std::string original(len, ' ');

        for (char &c : original)
            c = static_cast<char>(uniform_dist(re));

        auto escaped = gul14::escape(original);

        REQUIRE(original.length() <= escaped.length());

        auto unescaped = gul14::unescape(escaped);

        if (original != unescaped)
        {
            INFO(gul14::cat("Original \"", original, "\" != \"", unescaped,
                          "\" after escape/unescape"));

            std::string str = "Original bytes:              ";

            for (const char &c : original)
                str += gul14::cat("[", static_cast<int>(c), "] ");

            str += "\nBytes after escape/unescape: ";

            for (const char &c : unescaped)
                str += gul14::cat("[", static_cast<int>(c), "] ");

            FAIL(str);
        }
    }
}

// vi:ts=4:sw=4:sts=4:et
