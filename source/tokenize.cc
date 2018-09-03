/**
 * \file   tokenize.cc
 * \author Lars Froehlich
 * \brief  Implementation of tokenize(), tokenize_string_view().
 * \date   Created on September 3, 2018
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

#include "gul/tokenize.h"

namespace gul {

std::vector<std::string> tokenize(string_view str, string_view delimiters)
{
    std::vector<std::string> tokens;

    std::string::size_type token_start = 0;
    std::string::size_type token_end = 0;

    while (true)
    {
        token_start = str.find_first_not_of(delimiters, token_end);
        if (token_start == std::string::npos)
            break;

        token_end = str.find_first_of(delimiters, token_start);
        if (token_end == std::string::npos)
        {
            tokens.emplace_back(str.substr(token_start, std::string::npos));
            break;
        }
        else
            tokens.emplace_back(str.substr(token_start, token_end - token_start));
    }

    return tokens;
}

std::vector<string_view> tokenize_string_view(string_view str, string_view delimiters)
{
    std::vector<string_view> tokens;

    std::string::size_type token_start = 0;
    std::string::size_type token_end = 0;

    while (true)
    {
        token_start = str.find_first_not_of(delimiters, token_end);
        if (token_start == std::string::npos)
            break;

        token_end = str.find_first_of(delimiters, token_start);
        if (token_end == std::string::npos)
        {
            tokens.push_back(str.substr(token_start, std::string::npos));
            break;
        }
        else
            tokens.push_back(str.substr(token_start, token_end - token_start));
    }

    return tokens;
}

} // namespace gul
