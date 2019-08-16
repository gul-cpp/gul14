/**
 * \file   test_trim.cc
 * \author \ref contributors
 * \date   Created on September 19, 2018
 * \brief  Unit tests for trim(), trim_left(), trim_right(), trim_inplace(),
 *         trim_left_inplace(), and trim_right_inplace().
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
#include "gul/trim.h"

using gul::trim;
using gul::trim_sv;
using gul::trim_left;
using gul::trim_left_sv;
using gul::trim_right;
using gul::trim_right_sv;

using namespace std::literals;

TEST_CASE("trim() works with various examples", "[trim]")
{
    REQUIRE(trim("  Hello\n World\t\r   ") == "Hello\n World");
    REQUIRE(trim("Hello\t\r   ") == "Hello");
    REQUIRE(trim(" \t\r   Hello") == "Hello");
    REQUIRE(trim("Hello") == "Hello");
    REQUIRE(trim(" \n \t ") == "");
    REQUIRE(trim("") == "");
    REQUIRE(trim("\0Hello\0"s) == "\0Hello\0"s);

    REQUIRE(trim("\0 Hello \0"s, "\0 \t"s) == "Hello"s);
    REQUIRE(trim("Hello World", "Held") == "o Wor");
    REQUIRE(trim(" Hello World ", "") == " Hello World ");
}

TEST_CASE("trim_sv() works with various examples", "[trim]")
{
    REQUIRE(trim_sv("  Hello\n World\t\r   ") == "Hello\n World");
    REQUIRE(trim_sv("Hello\t\r   ") == "Hello");
    REQUIRE(trim_sv(" \t\r   Hello") == "Hello");
    REQUIRE(trim_sv("Hello") == "Hello");
    REQUIRE(trim_sv(" \n \t ") == "");
    REQUIRE(trim_sv("") == "");
    REQUIRE(trim_sv("\0Hello\0"s) == "\0Hello\0"s);

    REQUIRE(trim_sv("\0 Hello \0"s, "\0 \t"s) == "Hello"s);
    REQUIRE(trim_sv("Hello World", "Held") == "o Wor");
    REQUIRE(trim_sv(" Hello World ", "") == " Hello World ");
}

TEST_CASE("trim_left() works with various examples", "[trim]")
{
    REQUIRE(trim_left("  Hello\n World\t\r   ") == "Hello\n World\t\r   ");
    REQUIRE(trim_left("Hello\t\r   ") == "Hello\t\r   ");
    REQUIRE(trim_left(" \t\r   Hello") == "Hello");
    REQUIRE(trim_left("Hello") == "Hello");
    REQUIRE(trim_left(" \n \t ") == "");
    REQUIRE(trim_left("") == "");
    REQUIRE(trim_left("\0Hello\0"s) == "\0Hello\0"s);

    REQUIRE(trim_left("\0 Hello \0"s, "\0 \t"s) == "Hello \0"s);
    REQUIRE(trim_left("Hello World", "Hel") == "o World");
    REQUIRE(trim_left(" Hello World ", "") == " Hello World ");
}

TEST_CASE("trim_left_sv() works with various examples", "[trim]")
{
    REQUIRE(trim_left_sv("  Hello\n World\t\r   ") == "Hello\n World\t\r   ");
    REQUIRE(trim_left_sv("Hello\t\r   ") == "Hello\t\r   ");
    REQUIRE(trim_left_sv(" \t\r   Hello") == "Hello");
    REQUIRE(trim_left_sv("Hello") == "Hello");
    REQUIRE(trim_left_sv(" \n \t ") == "");
    REQUIRE(trim_left_sv("") == "");
    REQUIRE(trim_left_sv("\0Hello\0"s) == "\0Hello\0"s);

    REQUIRE(trim_left_sv("\0 Hello \0"s, "\0 \t"s) == "Hello \0"s);
    REQUIRE(trim_left_sv("Hello World", "Hel") == "o World");
    REQUIRE(trim_left_sv(" Hello World ", "") == " Hello World ");
}

TEST_CASE("trim_right() works with various examples", "[trim]")
{
    REQUIRE(trim_right("  Hello\n World\t\r   ") == "  Hello\n World");
    REQUIRE(trim_right("Hello\t\r   ") == "Hello");
    REQUIRE(trim_right(" \t\r   Hello") == " \t\r   Hello");
    REQUIRE(trim_right("Hello") == "Hello");
    REQUIRE(trim_right(" \n \t ") == "");
    REQUIRE(trim_right("") == "");
    REQUIRE(trim_right("\0Hello\0"s) == "\0Hello\0"s);

    REQUIRE(trim_right("\0 Hello \0"s, "\0 \t"s) == "\0 Hello"s);
    REQUIRE(trim_right("Hello World", "ldr") == "Hello Wo");
    REQUIRE(trim_right(" Hello World ", "") == " Hello World ");
}

TEST_CASE("trim_right_sv() works with various examples", "[trim]")
{
    REQUIRE(trim_right_sv("  Hello\n World\t\r   ") == "  Hello\n World");
    REQUIRE(trim_right_sv("Hello\t\r   ") == "Hello");
    REQUIRE(trim_right_sv(" \t\r   Hello") == " \t\r   Hello");
    REQUIRE(trim_right_sv("Hello") == "Hello");
    REQUIRE(trim_right_sv(" \n \t ") == "");
    REQUIRE(trim_right_sv("") == "");
    REQUIRE(trim_right_sv("\0Hello\0"s) == "\0Hello\0"s);

    REQUIRE(trim_right_sv("\0 Hello \0"s, "\0 \t"s) == "\0 Hello"s);
    REQUIRE(trim_right_sv("Hello World", "ldr") == "Hello Wo");
    REQUIRE(trim_right_sv(" Hello World ", "") == " Hello World ");
}
