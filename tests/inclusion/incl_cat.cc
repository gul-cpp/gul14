/**
 * \file   incl_cat.cc
 * \author \ref contributors
 * \date   Created on Aug 21, 2019
 * \brief  Test for inclusion completeness of cat.h.
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

#include "gul/cat.h"

void incl_cat() {
    gul::cat(1);
    gul::cat(1, 2);
    gul::cat(1, 2, 3);
    gul::cat(1, 2, 3, 4);

    gul::cat(std::string{ "test" });
    gul::cat("test");
    gul::cat('x');
    gul::cat(1);
    gul::cat(1l);
    gul::cat(1ll);
    gul::cat(1u);
    gul::cat(1ul);
    gul::cat(1ull);
    gul::cat(1.1f);
    gul::cat(1.1);
}

// vi:ts=4:sw=4:sts=4:et
