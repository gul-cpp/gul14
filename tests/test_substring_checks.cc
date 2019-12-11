/**
 * \file   test_substring_checks.cc
 * \author \ref contributors
 * \date   Created on November 26, 2018
 * \brief  Test suite for contains(), ends_with(), and starts_with().
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

#include "gul14/catch.h"
#include "gul14/substring_checks.h"

using namespace std::literals;
using namespace gul;

TEST_CASE("contains(string_view, string_view)", "[substring_checks]")
{
    auto constexpr test_constexprness = contains("Hello World", "Hello");
    REQUIRE(test_constexprness);
    REQUIRE(contains("Hello World", "Hello"));
    REQUIRE(contains("Hello World", "World"));
    REQUIRE(contains("Hello World", "o Wo"));
    REQUIRE(!contains("Hello World", "lll"));
    REQUIRE(contains("Hello\0World\0"s, "d\0"s));
    REQUIRE(contains("Hello\0World\0"s, "\0W"s));
    REQUIRE(!contains("Hello World", "\0"s));
    REQUIRE(contains("Hello World", ""));
    REQUIRE(contains("", ""));
    REQUIRE(!contains("", "aaa"));
}

TEST_CASE("contains(string_view, char)", "[substring_checks]")
{
    auto constexpr test_constexprness = contains("Hello World", 'H');
    REQUIRE(test_constexprness);
    REQUIRE(contains("Hello World", 'H'));
    REQUIRE(contains("Hello World", 'd'));
    REQUIRE(contains("Hello World", ' '));
    REQUIRE(!contains("Hello World", 'g'));
    REQUIRE(contains("Hello\0World\0"s, '\0'));
    REQUIRE(contains("Hello\0World\0"s, 'd'));
    REQUIRE(!contains("Hello World", '\0'));
}

TEST_CASE("contains_nocase(string_view, string_view)", "[substring_checks]")
{
    auto constexpr test_constexprness = contains_nocase("Hello World", "Hello");
    REQUIRE(test_constexprness);
    REQUIRE(contains_nocase("Hello World", "Hello"));
    REQUIRE(contains_nocase("Hello World", "hello"));
    REQUIRE(contains_nocase("Hello World", "helLo"));
    REQUIRE(contains_nocase("Hello World", "World"));
    REQUIRE(contains_nocase("Hello World", "world"));
    REQUIRE(contains_nocase("Hello World", "woRld"));
    REQUIRE(contains_nocase("Hello World", "o Wo"));
    REQUIRE(contains_nocase("Hello World", "o wo"));
    REQUIRE(contains_nocase("Hello World", "o wO"));
    REQUIRE(!contains_nocase("Hello World", "lll"));
    REQUIRE(contains_nocase("Hello\0World\0"s, "d\0"s));
    REQUIRE(contains_nocase("Hello\0World\0"s, "\0W"s));
    REQUIRE(!contains_nocase("Hello World", "\0"s));
    REQUIRE(contains_nocase("Hello World", ""));
    REQUIRE(contains_nocase("", ""));
    REQUIRE(!contains_nocase("", "aaa"));
}

TEST_CASE("contains_nocase(string_view, char)", "[substring_checks]")
{
    auto constexpr test_constexprness = contains_nocase("Hello World", 'H');
    REQUIRE(test_constexprness);
    REQUIRE(contains_nocase("Hello World", 'H'));
    REQUIRE(contains_nocase("Hello World", 'h'));
    REQUIRE(contains_nocase("Hello World", 'E'));
    REQUIRE(contains_nocase("Hello World", 'd'));
    REQUIRE(contains_nocase("Hello World", 'D'));
    REQUIRE(contains_nocase("Hello World", ' '));
    REQUIRE(!contains_nocase("Hello World", 'g'));
    REQUIRE(contains_nocase("Hello\0World\0"s, '\0'));
    REQUIRE(contains_nocase("Hello\0World\0"s, 'd'));
    REQUIRE(!contains_nocase("Hello World", '\0'));
}

TEST_CASE("ends_with(string_view, string_view)", "[substring_checks]")
{
    auto constexpr test_constexprness = ends_with("Hello World", "Hello World2");
    REQUIRE(!test_constexprness);
    REQUIRE(!ends_with("Hello World", "Hello World2"));
    REQUIRE(ends_with("Hello World", "Hello World"));
    REQUIRE(ends_with("Hello World", "World"));
    REQUIRE(ends_with("Hello World", ""));
    REQUIRE(!ends_with("", "Hello"));
    REQUIRE(ends_with("", ""));
    REQUIRE(ends_with("Hello World\0"s, "\0"s));
    REQUIRE(!ends_with("Hello World", "\0"s));
}

TEST_CASE("ends_with(string_view, char)", "[substring_checks]")
{
    auto constexpr test_constexprness = ends_with("Hello World", 'd');
    REQUIRE(test_constexprness);
    REQUIRE(ends_with("Hello World", 'd'));
    REQUIRE(!ends_with("Hello World", 'D'));
    REQUIRE(!ends_with("", 'd'));
    REQUIRE(ends_with("Hello World\0"s, '\0'));
}

TEST_CASE("ends_with_nocase(string_view, string_view)", "[substring_checks]")
{
    auto constexpr test_constexprness = ends_with_nocase("Hello World", "Hello");
    REQUIRE(!test_constexprness);
    REQUIRE(!ends_with_nocase("Hello World", "Hello World2"));
    REQUIRE(ends_with_nocase("Hello World", "Hello World"));
    REQUIRE(ends_with_nocase("Hello World", "Hello WOrld"));
    REQUIRE(ends_with_nocase("Hello World", "Hello world"));
    REQUIRE(ends_with_nocase("Hello World", "World"));
    REQUIRE(ends_with_nocase("Hello World", "WoRLd"));
    REQUIRE(ends_with_nocase("Hello World", ""));
    REQUIRE(!ends_with_nocase("", "Hello"));
    REQUIRE(ends_with_nocase("", ""));
    REQUIRE(ends_with_nocase("Hello World\0"s, "\0"s));
    REQUIRE(!ends_with_nocase("Hello World", "\0"s));
}

TEST_CASE("ends_with_nocase(string_view, char)", "[substring_checks]")
{
    auto constexpr test_constexprness = ends_with_nocase("Hello World", 'd');
    REQUIRE(test_constexprness);
    REQUIRE(ends_with_nocase("Hello World", 'd'));
    REQUIRE(ends_with_nocase("Hello World", 'D'));
    REQUIRE(ends_with_nocase("Hello WorlD", 'd'));
    REQUIRE(!ends_with_nocase("Hello World", 't'));
    REQUIRE(!ends_with_nocase("", 'd'));
    REQUIRE(ends_with_nocase("Hello World\0"s, '\0'));
}

TEST_CASE("starts_with(string_view, string_view)", "[substring_checks]")
{
    auto constexpr test_constexprness = starts_with("Hello World", "Hello");
    REQUIRE(test_constexprness);
    REQUIRE(!starts_with("Hello World", "Hello World2"));
    REQUIRE(starts_with("Hello World", "Hello World"));
    REQUIRE(starts_with("Hello World", "Hello"));
    REQUIRE(starts_with("Hello World", ""));
    REQUIRE(!starts_with("", "Hello"));
    REQUIRE(starts_with("", ""));
    REQUIRE(starts_with("\0Hello World"s, "\0"s));
    REQUIRE(!starts_with("Hello World", "\0"s));
}

TEST_CASE("starts_with(string_view, char)", "[substring_checks]")
{
    auto constexpr test_constexprness = starts_with("Hello World", 'H');
    REQUIRE(test_constexprness);
    REQUIRE(starts_with("Hello World", 'H'));
    REQUIRE(!starts_with("Hello World", 'h'));
    REQUIRE(!starts_with("", 'H'));
    REQUIRE(starts_with("\0Hello World"s, '\0'));
}

TEST_CASE("starts_with_nocase(string_view, string_view)", "[substring_checks]")
{
    auto constexpr test_constexprness = starts_with_nocase("Hello World", "Hello");
    REQUIRE(test_constexprness);
    REQUIRE(!starts_with_nocase("Hello World", "Hello World2"));
    REQUIRE(starts_with_nocase("Hello World", "Hello World"));
    REQUIRE(starts_with_nocase("Hello World", "Hello WOrld"));
    REQUIRE(starts_with_nocase("Hello World", "Hello world"));
    REQUIRE(starts_with_nocase("Hello World", "Hello"));
    REQUIRE(starts_with_nocase("Hello World", "HeLLo"));
    REQUIRE(starts_with_nocase("Hello World", ""));
    REQUIRE(!starts_with_nocase("", "Hello"));
    REQUIRE(starts_with_nocase("", ""));
    REQUIRE(starts_with_nocase("\0Hello World"s, "\0"s));
    REQUIRE(!starts_with_nocase("Hello World", "\0"s));
}

TEST_CASE("starts_with_nocase(string_view, char)", "[substring_checks]")
{
    auto constexpr test_constexprness = starts_with_nocase("Hello World", 'H');
    REQUIRE(test_constexprness);
    REQUIRE(starts_with_nocase("Hello World", 'H'));
    REQUIRE(starts_with_nocase("Hello World", 'h'));
    REQUIRE(starts_with_nocase("hello World", 'H'));
    REQUIRE(!starts_with_nocase("Hello World", 'e'));
    REQUIRE(!starts_with_nocase("", 'H'));
    REQUIRE(starts_with_nocase("\0Hello World"s, '\0'));
}

// vi:ts=4:sw=4:sts=4:et
