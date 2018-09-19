/**
 * \file   trim.h
 * \author \ref contributors
 * \date   Created on September 19, 2018
 * \brief  Declarations of trim(), trim_left(), trim_right(), trim_inplace(),
 *         trim_left_inplace(), and trim_right_inplace().
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

#include "gul/string_view.h"

namespace gul {


/**
 * Trim leading and trailing whitespace (or a custom set of characters) from a string.
 * Which characters are removed can be customized via the whitespace_characters parameter.
 * \param str   The string that should be trimmed.
 * \param whitespace_characters  A string containing all the characters that should be
 *                               treated as whitespace (i.e. that are trimmed). If this is
 *                               empty, no characters are trimmed.
 * \returns a trimmed copy of the input string.
 */
std::string trim(string_view str, string_view whitespace_characters = " \t\r\n\a\b\f\v");

/**
 * Trim leading whitespace (or a custom set of characters) from a string.
 * Which characters are removed can be customized via the whitespace_characters parameter.
 * \param str   The string from which leading characters should be trimmed.
 * \param whitespace_characters  A string containing all the characters that should be
 *                               treated as whitespace (i.e. that are trimmed). If this is
 *                               empty, no characters are trimmed.
 * \returns a trimmed copy of the input string.
 */
std::string trim_left(string_view str, string_view whitespace_characters = " \t\r\n\a\b\f\v");

/**
 * Trim trailing whitespace (or a custom set of characters) from a string.
 * Which characters are removed can be customized via the whitespace_characters parameter.
 * \param str   The string from which trailing characters should be trimmed.
 * \param whitespace_characters  A string containing all the characters that should be
 *                               treated as whitespace (i.e. that are trimmed). If this is
 *                               empty, no characters are trimmed.
 * \returns a trimmed copy of the input string.
 */
std::string trim_right(string_view str, string_view whitespace_characters = " \t\r\n\a\b\f\v");


// TODO: trim_*_inplace to be added in a separate pull request


} // namespace gul
