/**
 * \file   trim.cc
 * \author \ref contributors
 * \date   Created on September 19, 2018
 * \brief  Implementation of trim(), trim_left(), trim_right(), trim_sv(), trim_left_sv(),
 *         and trim_right_sv().
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


std::string trim(string_view str, string_view ws_chars)
{
    return std::string(trim_sv(str, ws_chars));
}

string_view trim_sv(string_view str, string_view ws_chars)
{
    const auto lpos = str.find_first_not_of(ws_chars);

    if (lpos == string_view::npos)
        return "";

    const auto rpos = str.find_last_not_of(ws_chars);

    return str.substr(lpos, rpos - lpos + 1);
}

std::string trim_left(string_view str, string_view ws_chars)
{
    return std::string(trim_left_sv(str, ws_chars));
}

string_view trim_left_sv(string_view str, string_view ws_chars)
{
    const auto pos = str.find_first_not_of(ws_chars);

    if (pos == string_view::npos)
        return "";

    return str.substr(pos);
}

std::string trim_right(string_view str, string_view ws_chars)
{
    return std::string(trim_right_sv(str, ws_chars));
}

string_view trim_right_sv(string_view str, string_view ws_chars)
{
    const auto pos = str.find_last_not_of(ws_chars);

    if (pos == string_view::npos)
        return "";

    return str.substr(0, pos + 1);
}


} // namespace gul
