/**
 * \file    starts_with_ends_with.cc
 * \brief   Implementation of starts_with(), ends_with().
 * \authors \ref contributors
 * \date    Created on 26 November 2018
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


#include "gul/starts_with_ends_with.h"

namespace gul {

bool ends_with(string_view str1, string_view str2)
{
    const auto len1 = str1.length();
    const auto len2 = str2.length();

    if (len2 > len1)
        return false;

    if (str1.compare(len1 - len2, len2, str2) == 0)
        return true;
    else
        return false;
}

bool starts_with(string_view str1, string_view str2)
{
    if (str1.compare(0, str2.length(), str2) == 0)
        return true;
    else
        return false;
}

} // namespace gul

/* vim:set noexpandtab softtabstop=4 tabstop=4 shiftwidth=4 textwidth=90 cindent: */
