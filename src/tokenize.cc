/**
 * \file    tokenize.cc
 * \authors \ref contributors
 * \brief   Implementation of tokenize(), tokenize_sv().
 * \date    Created on September 3, 2018
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

#include "gul14/tokenize.h"

namespace gul14 {

// Anonymous namespace to make the template visible only to this translation unit
namespace {

template <class StringType>
std::vector<StringType> tokenize(string_view str, string_view delimiters)
{
    std::vector<StringType> tokens;

    string_view::size_type token_start = 0;
    string_view::size_type token_end = 0;

    while (true)
    {
        token_start = str.find_first_not_of(delimiters, token_end);
        if (token_start == string_view::npos)
            break;

        token_end = str.find_first_of(delimiters, token_start);
        if (token_end == string_view::npos)
        {
            tokens.emplace_back(str.data() + token_start, str.length() - token_start);
            break;
        }
        else
            tokens.emplace_back(str.data() + token_start, token_end - token_start);
    }

    return tokens;
}

} // anonymous namespace

std::vector<std::string> tokenize(string_view str, string_view delimiters)
{
    return tokenize<std::string>(str, delimiters);
}

std::vector<string_view> tokenize_sv(string_view str, string_view delimiters)
{
    return tokenize<string_view>(str, delimiters);
}

} // namespace gul14
