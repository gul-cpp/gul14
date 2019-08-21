/**
 * \file   incl_case_ascii.cc
 * \author \ref contributors
 * \date   Created on Aug 21, 2019
 * \brief  Test for inclusion completeness of case_ascii.h.
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

void incl_case_ascii() {
    auto x = std::string{ "test" };
    gul::lowercase_ascii('x');
    gul::lowercase_ascii("test");
    gul::lowercase_ascii_inplace(x);
    gul::uppercase_ascii('x');
    gul::uppercase_ascii("test");
    gul::uppercase_ascii_inplace(x);
}

// vi:ts=4:sw=4:sts=4:et
