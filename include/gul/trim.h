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
 * The default characters that are treated as whitespace by GUL.
 * This is a string view that contains the space and the most common control characters,
 * namely (with their ASCII codes):
 * - Bell/alert (7)
 * - Backspace (8)
 * - Horizontal tabulator (9)
 * - Newline/line feed (10)
 * - Vertical Tab (11)
 * - Form feed (12)
 * - Carriage return (13)
 * - Space (32)
 *
 * \note
 * The null character is not treated as whitespace by default.
 */
extern const string_view default_whitespace_characters;


/**
 * Trim leading and trailing whitespace (or a custom set of characters) from a string,
 * returning a new std::string.
 * Which characters are removed can be customized via the ws_chars parameter.
 * \param str   The string that should be trimmed.
 * \param ws_chars  A string containing all the characters that should be treated as
 *                  whitespace (i.e. that are trimmed). If this is empty, no characters
 *                  are trimmed.
 * \returns a trimmed copy of the input string.
 */
std::string trim(string_view str, string_view ws_chars = default_whitespace_characters);

/**
 * Trim leading and trailing whitespace (or a custom set of characters) from a string,
 * returning a view into the original string.
 * Which characters are removed can be customized via the ws_chars parameter.
 * \param str   The string that should be trimmed.
 * \param ws_chars  A string containing all the characters that should be treated as
 *                  whitespace (i.e. that are trimmed). If this is empty, no characters
 *                  are trimmed.
 * \returns a string_view that points into the input string.
 */
string_view trim_sv(string_view str, string_view ws_chars = default_whitespace_characters);

/**
 * Trim leading whitespace (or a custom set of characters) from a string, returning a new
 * std::string.
 * Which characters are removed can be customized via the ws_chars parameter.
 * \param str   The string from which leading characters should be trimmed.
 * \param ws_chars  A string containing all the characters that should be treated as
 *                  whitespace (i.e. that are trimmed). If this is empty, no characters
 *                  are trimmed.
 * \returns a trimmed copy of the input string.
 */
std::string trim_left(string_view str, string_view ws_chars = default_whitespace_characters);

/**
 * Trim leading whitespace (or a custom set of characters) from a string, returning a view
 * into the original string.
 * Which characters are removed can be customized via the ws_chars parameter.
 * \param str   The string from which leading characters should be trimmed.
 * \param ws_chars  A string containing all the characters that should be treated as
 *                  whitespace (i.e. that are trimmed). If this is empty, no characters
 *                  are trimmed.
 * \returns a string_view that points into the input string.
 */
string_view trim_left_sv(string_view str, string_view ws_chars = default_whitespace_characters);

/**
 * Trim trailing whitespace (or a custom set of characters) from a string, returning a new
 * std::string.
 * Which characters are removed can be customized via the ws_chars parameter.
 * \param str   The string from which trailing characters should be trimmed.
 * \param ws_chars  A string containing all the characters that should be treated as
 *                  whitespace (i.e. that are trimmed). If this is empty, no characters
 *                  are trimmed.
 * \returns a trimmed copy of the input string.
 */
std::string trim_right(string_view str, string_view ws_chars = default_whitespace_characters);

/**
 * Trim trailing whitespace (or a custom set of characters) from a string, returning a
 * view into the original string.
 * Which characters are removed can be customized via the ws_chars parameter.
 * \param str   The string from which trailing characters should be trimmed.
 * \param ws_chars  A string containing all the characters that should be treated as
 *                  whitespace (i.e. that are trimmed). If this is empty, no characters
 *                  are trimmed.
 * \returns a trimmed copy of the input string.
 */
string_view trim_right_sv(string_view str, string_view ws_chars = default_whitespace_characters);


// TODO: trim_*_inplace to be added in a separate pull request


} // namespace gul
