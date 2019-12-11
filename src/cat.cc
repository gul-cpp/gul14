/**
 * \file    cat.cc
 * \authors \ref contributors
 * \brief   Implementation of cat() for two or more parameters.
 * \date    Created on August 31, 2018
 *
 * \copyright Copyright 2018-2019 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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

#include "gul14/cat.h"

namespace gul {


std::string cat(const ConvertingStringView& s1, const ConvertingStringView& s2)
{
    std::string str;
    str.reserve(s1.length() + s2.length());
    str.append(s1.data(), s1.length());
    str.append(s2.data(), s2.length());
    return str;
}

std::string cat(const ConvertingStringView& s1, const ConvertingStringView& s2, const ConvertingStringView& s3)
{
    std::string str;
    str.reserve(s1.length() + s2.length() + s3.length());
    str.append(s1.data(), s1.length());
    str.append(s2.data(), s2.length());
    str.append(s3.data(), s3.length());
    return str;
}

std::string cat(std::initializer_list<ConvertingStringView> pieces)
{
    std::string str;

    std::size_t len = 0;
    for (const ConvertingStringView &piece : pieces)
        len += piece.size();

    str.reserve(len);

    for (const ConvertingStringView &piece : pieces)
        str.append(piece.data(), piece.size());

    return str;
}

} // namespace gul
