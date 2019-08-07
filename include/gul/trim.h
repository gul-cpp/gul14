/**
 * \file   trim.h
 * \author \ref contributors
 * \date   Created on September 19, 2018
 * \brief  Declarations of trim(), trim_left(), trim_right(), trim_sv(), trim_left_sv(),
 *         and trim_right_sv().
 *
 * \copyright Copyright 2018-2019 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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

#include "gul/internal.h"
#include "gul/string_util.h"
#include "gul/string_view.h"

namespace gul {


/**
 * Trim leading and trailing whitespace (or a custom set of characters) from a string,
 * returning a new std::string.
 * Which characters are removed can be customized via the ws_chars parameter.
 *
 * \snippet trim.cc Using trim() with default and custom whitespace
 *
 * \param str   The string that should be trimmed.
 * \param ws_chars  A string containing all the characters that should be treated as
 *                  whitespace (i.e. that are trimmed). If this is empty, no characters
 *                  are trimmed.
 * \returns a trimmed copy of the input string.
 *
 * \see trim_sv() returns a string_view instead of a copied string,<br>
 *      trim_left() and trim_right() %trim only one side of the string,<br>
 *      trim_left_sv() and trim_right_sv() %trim only one side and return a string_view.
 */
GUL_EXPORT
std::string trim(string_view str, string_view ws_chars = default_whitespace_characters);

/**
 * Trim leading and trailing whitespace (or a custom set of characters) from a string,
 * returning a view into the original string.
 * Which characters are removed can be customized via the ws_chars parameter.
 *
 * \snippet trim.cc Using trim_sv() on a mutable string
 * 
 * \param str   The string that should be trimmed.
 * \param ws_chars  A string containing all the characters that should be treated as
 *                  whitespace (i.e. that are trimmed). If this is empty, no characters
 *                  are trimmed.
 * \returns a trimmed string_view that points into the input string.
 *
 * \see trim() returns a copied string instead of a string_view,<br>
 *      trim_left_sv() and trim_right_sv() %trim only one side of the string,<br>
 *      trim_left() and trim_right() %trim only one side and return a copied string.<br>
 */
GUL_EXPORT
string_view trim_sv(string_view str, string_view ws_chars = default_whitespace_characters);

/**
 * Trim leading whitespace (or a custom set of characters) from a string, returning a new
 * std::string.
 * Which characters are removed can be customized via the ws_chars parameter.
 *
 * \snippet trim.cc Using trim_left() and trim_right() with default and custom whitespace
 * 
 * \param str   The string from which leading characters should be trimmed.
 * \param ws_chars  A string containing all the characters that should be treated as
 *                  whitespace (i.e. that are trimmed). If this is empty, no characters
 *                  are trimmed.
 * \returns a trimmed copy of the input string.
 *
 * \see trim_left_sv() returns a string_view instead of a copied string,<br>
 *      trim_right() and trim_right_sv() %trim the other side of the string,<br>
 *      trim() and trim_sv() %trim both sides of the string.
 */
GUL_EXPORT
std::string trim_left(string_view str, string_view ws_chars = default_whitespace_characters);

/**
 * Trim leading whitespace (or a custom set of characters) from a string, returning a view
 * into the original string.
 * Which characters are removed can be customized via the ws_chars parameter.
 *
 * \snippet trim.cc Using trim_left_sv() and trim_right_sv() with default and custom whitespace
 *
 * \param str   The string from which leading characters should be trimmed.
 * \param ws_chars  A string containing all the characters that should be treated as
 *                  whitespace (i.e. that are trimmed). If this is empty, no characters
 *                  are trimmed.
 * \returns a trimmed string_view that points into the input string.
 *
 * \see trim_left() returns a copied string instead of a string_view,<br>
 *      trim_right() and trim_right_sv() %trim the other side of the string,<br>
 *      trim() and trim_sv() %trim both sides of the string.
 */
GUL_EXPORT
string_view trim_left_sv(string_view str, string_view ws_chars = default_whitespace_characters);

/**
 * Trim trailing whitespace (or a custom set of characters) from a string, returning a new
 * std::string.
 * Which characters are removed can be customized via the ws_chars parameter.
 *
 * \snippet trim.cc Using trim_left() and trim_right() with default and custom whitespace
 *
 * \param str   The string from which trailing characters should be trimmed.
 * \param ws_chars  A string containing all the characters that should be treated as
 *                  whitespace (i.e. that are trimmed). If this is empty, no characters
 *                  are trimmed.
 * \returns a trimmed copy of the input string.
 *
 * \see trim_right_sv() returns a string_view instead of a copied string,<br>
 *      trim_left() and trim_left_sv() %trim the other side of the string,<br>
 *      trim() and trim_sv() %trim both sides of the string.
 */
GUL_EXPORT
std::string trim_right(string_view str, string_view ws_chars = default_whitespace_characters);

/**
 * Trim trailing whitespace (or a custom set of characters) from a string, returning a
 * view into the original string.
 * Which characters are removed can be customized via the ws_chars parameter.
 *
 * \snippet trim.cc Using trim_left_sv() and trim_right_sv() with default and custom whitespace
 *
 * \param str   The string from which trailing characters should be trimmed.
 * \param ws_chars  A string containing all the characters that should be treated as
 *                  whitespace (i.e. that are trimmed). If this is empty, no characters
 *                  are trimmed.
 * \returns a trimmed string_view that points into the input string.
 *
 * \see trim_right() returns a copied string instead of a string_view,<br>
 *      trim_left() and trim_left_sv() %trim the other side of the string,<br>
 *      trim() and trim_sv() %trim both sides of the string.
 */
GUL_EXPORT
string_view trim_right_sv(string_view str, string_view ws_chars = default_whitespace_characters);


// TODO: Add trim_*_inplace


} // namespace gul


/**
 * \example trim.cc
 * A simple example on how to use the trim() family of functions for stripping characters
 * from one or both sides of a string.
 */
