/**
 * \file   incl_num_util.cc
 * \author \ref contributors
 * \date   Created on Aug 21, 2019
 * \brief  Test for inclusion completeness of num_util.h.
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

#include "gul/num_util.h"

void incl_num_util() {
    gul::abs(1);
    gul::abs(1.1);
    gul::abs(1u);
    gul::within_orders(1, 1, 1.0);
    gul::within_abs(1, 1, 1);
    gul::within_abs(1.0, 1.0, 1.0);
    gul::within_ulp(1.0, 1.0, 1u);
    gul::clamp(1, 1, 1);
    gul::clamp(1, 1, 1, [](auto, auto) -> bool { return true; });
}

// vi:ts=4:sw=4:sts=4:et
