/**
 * \file   incl_substring_checks.cc
 * \author \ref contributors
 * \date   Created on Aug 21, 2019
 * \brief  Test for inclusion completeness of substring_checks.h.
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

#include "gul/substring_checks.h"

void incl_substring_checks() {
    gul::contains("test", "x");
    gul::contains("test", 'x');
    gul::ends_with("test", "x");
    gul::ends_with("test", 'x');
    gul::starts_with("test", "x");
    gul::starts_with("test", 'x');
}

// vi:ts=4:sw=4:sts=4:et
