/**
 * \file   trim.cc
 * \author \ref contributors
 * \date   Created on September 19, 2018
 * \brief  Definitions of trim(), trim_left(), trim_right(), trim_inplace(),
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

#include "gul/trim.h"

namespace gul {


std::string trim(string_view str, string_view whitespace_characters)
{
    const auto lpos = str.find_first_not_of(whitespace_characters);

    if (lpos == string_view::npos)
        return "";

    const auto rpos = str.find_last_not_of(whitespace_characters);

    return std::string(str.begin() + lpos, str.begin() + rpos + 1);
}

std::string trim_left(string_view str, string_view whitespace_characters)
{
    const auto pos = str.find_first_not_of(whitespace_characters);

    if (pos == string_view::npos)
        return "";

    return std::string(str.begin() + pos, str.end());
}

std::string trim_right(string_view str, string_view whitespace_characters)
{
    const auto pos = str.find_last_not_of(whitespace_characters);

    if (pos == string_view::npos)
        return "";

    return std::string(str.begin(), str.begin() + pos + 1);
}


} // namespace gul
