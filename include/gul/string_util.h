/**
 * \file   string_util.h
 * \brief  Declarations of string utility functions in namespace hlc.
 *
 * \copyright Copyright 2015-2018 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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

#pragma once

#include <string>

namespace gul {


/**
 * Creates a new string where all all non printable characters in the string
 * have been replaced with corresponding escape sequences. Allowing the
 * string to be printed out, e.g in debug messages.
 *
 * \param in The input string
 *
 * \return A new string without non-printables
 */
std::string escape(const std::string& in);

} /* namespace gul */

/* vim:set noexpandtab softtabstop=4 tabstop=4 shiftwidth=4 textwidth=90 cindent: */
