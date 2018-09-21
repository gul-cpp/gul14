/**
 * \file    string_util.h
 * \brief   Declarations of string utility functions for the General Utility Library.
 * \authors \ref contributors
 * \date    Created on 31 August 2018
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
 * Create a new string that looks like an ASCII-only C string literal of the input string.
 * This is achieved by replacing all non-printable and non-ASCII characters with a hex
 * code escape in the form `\x01`.
 *
 * A few special cases are implemented to give more readable representations for very
 * common control characters, and of course backslash and double quotes are escaped as
 * well:
 *
 *     CR   ->  \r
 *     NL   ->  \n
 *     TAB  ->  \t
 *     \    ->  \\
 *     "    ->  \"
 *
 * <h4>Example</h4>
 * \code
 * std::cout << escape("Zwei\tFlüsse\nfließen ins Meer.") << "\n";
 * \endcode
 * Output (assuming that the string literal was in Latin-1 encoding):
 *
 *     Zwei\tFl\xfcsse\nflie\xdfen ins Meer.
 *
 * \note
 * The hexadecimal escape always uses two digits. This is different from the C/C++
 * standard, where it can be an arbitrary number of digits. The standard's way makes it
 * impossible to have any hex digit after a hex escape, e.g. `"\x200"` is invalid and not
 * equal to `" 0"` from the standard's point of view.
 *
 * \param in The input string.
 *
 * \returns a new string that contains only ASCII characters.
 */
std::string escape(const std::string& in);

/**
 * Evaluate a string with escaped characters to get the original string back.
 * Does only know the escape sequences used by gul::escape() and can be
 * used as in inverse function.
 *
 * \param in The string with escape sequences
 *
 * \return A new string where the sequences have been evaluated
 */
std::string unescape(const std::string& in);

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
std::string& replace_inplace(std::string& haystack, string_view needle, string_view hammer);

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
std::string replace(string_view haystack, string_view needle, string_view hammer);

} /* namespace gul */

/* vim:set noexpandtab softtabstop=4 tabstop=4 shiftwidth=4 textwidth=90 cindent: */
