/**
 * \file    replace.h
 * \brief   Declarations of replace(), replace_inplace().
 * \authors \ref contributors
 * \date    Created on 31 August 2018
 *
 * \copyright Copyright 2018-2020 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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

#ifndef GUL14_REPLACE_H_
#define GUL14_REPLACE_H_

#include <string>
#include "gul14/internal.h"
#include "gul14/string_view.h"

namespace gul14 {

/**
 * Replace all occurrences of a string within another string, returning the result
 * as a std::string.
 * Specifically, replace all occurrences of needle within haystack by hammer.
 *
 * \param haystack  The string in which search&replace should take place.
 * \param needle  The string to be searched for. If this is an empty string, a copy
 *                of haystack is returned.
 * \param hammer  The replacement string.
 */
GUL_EXPORT
std::string replace(string_view haystack, string_view needle, string_view hammer);

/**
 * Replace all occurrences of a string within another string in-place.
 * This function replaces all occurrences of needle within haystack by hammer and
 * returns a reference to the modified haystack.
 *
 * \param haystack  The string in which search&replace should take place.
 * \param needle  The string to be searched for. If this is an empty string, haystack is
 *                not changed.
 * \param hammer  The replacement string.
 *
 * \returns the altered haystack.
 */
GUL_EXPORT
std::string& replace_inplace(std::string& haystack, string_view needle, string_view hammer);

} // namespace gul14

#endif

/* vim:set noexpandtab softtabstop=4 tabstop=4 shiftwidth=4 textwidth=90 cindent: */
