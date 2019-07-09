/**
 * \file    escape.h
 * \brief   Declaration of escape(), unescape().
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
std::string escape(string_view in);

/**
 * Evaluate a string with escaped characters to get the original string back.
 * Does only know the escape sequences used by gul::escape() and can be
 * used as in inverse function.
 *
 * \param in The string with escape sequences
 *
 * \return A new string where the sequences have been evaluated
 */
std::string unescape(string_view in);

} /* namespace gul */

/* vim:set noexpandtab softtabstop=4 tabstop=4 shiftwidth=4 textwidth=90 cindent: */
