/**
 * \file   incl_replace.cc
 * \author \ref contributors
 * \date   Created on Aug 21, 2019
 * \brief  Test for inclusion completeness of replace.h.
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

#include "gul/replace.h"

void incl_replace() {
    gul::replace("test", "u", "x"); // 'Ein X fuer ein U vormachen'
    auto s = std::string{ "test" };
    gul::replace_inplace(s, "u", "x");
}

// vi:ts=4:sw=4:sts=4:et
