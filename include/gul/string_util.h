/**
 * \file   string_util.h
 * \brief  Declarations of string utility functions for the General Utility Library.
 * \author Soeren Grunewald, Lars Froehlich
 * \date   Created on 31 August 2018
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

#pragma once

#include <string>
#include "gul/string_view.h"

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

/**
 * Replace all occurrences of a string within another string in-place.
 * This function replaces all occurrences of needle within haystack by hammer and
 * returns a reference to the modified haystack.
 *
 * \param haystack The string where we should search and replace
 * \param needle What shall be replaced
 * \param hammer The replacement string
 *
 * \return The altered haystack
 *
 * \exception An invalid argument exception is thrown if needle is empty
 */
std::string& replace_inplace(std::string& haystack, string_view needle, string_view hammer);

/**
 * Replace all occurrences of a string within another string, returning the result
 * as a std::string.
 * Specifically, replace all occurrences of needle within haystack by hammer.
 *
 * \param haystack The string in which search&replace should take place.
 * \param needle   The string to be searched for. If this is an empty string, a copy
 *                 of haystack is returned.
 * \param hammer   The replacement string.
 */
std::string replace(string_view haystack, string_view needle, string_view hammer);

} /* namespace gul */

/* vim:set noexpandtab softtabstop=4 tabstop=4 shiftwidth=4 textwidth=90 cindent: */
