/**
 * \file    case_ascii.cc
 * \brief   Implementation of lowercase_ascii(), lowercase_ascii_inplace(),
 *          uppercase_ascii(), and uppercase_ascii_inplace().
 * \authors \ref contributors
 * \date    Created on 28 May 2019
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

#include "gul/case_ascii.h"


namespace gul {

std::string lowercase_ascii(gul::string_view str)
{
    std::string result;
    result.reserve(str.size());

    for (char c : str)
        result.push_back(lowercase_ascii(c));

    return result;
}

std::string &lowercase_ascii_inplace(std::string &str) noexcept
{
    for (char &c : str)
        c = lowercase_ascii(c);

    return str;
}

std::string uppercase_ascii(gul::string_view str)
{
    std::string result;
    result.reserve(str.size());

    for (char c : str)
        result.push_back(uppercase_ascii(c));

    return result;
}

std::string &uppercase_ascii_inplace(std::string &str) noexcept
{
    for (char &c : str)
        c = uppercase_ascii(c);

    return str;
}

} // namespace gul

/* vim:set noexpandtab softtabstop=4 tabstop=4 shiftwidth=4 textwidth=90 cindent: */
