/**
 * \file    string_util.cc
 * \brief   Implementation of string utilities for the General Utility Library.
 * \authors \ref contributors
 * \date    Created on 31 August 2018
 *
 * \copyright Copyright 2018-2021 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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


#include "gul14/string_util.h"

namespace gul14 {

const string_view default_whitespace_characters{ " \t\r\n\a\b\f\v" };

std::string safe_string(const char* char_ptr, std::size_t length)
{
    if (char_ptr == nullptr)
        return "";

    auto end_ptr = std::find(char_ptr, char_ptr + length, '\0');

    return std::string(char_ptr, end_ptr);
}

} // namespace gul14

/* vim:set expandtab softtabstop=4 tabstop=4 shiftwidth=4 textwidth=90 cindent: */
