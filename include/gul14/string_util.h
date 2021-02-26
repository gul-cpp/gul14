/**
 * \file    string_util.h
 * \brief   Declarations of string utility functions for the General Utility Library.
 * \authors \ref contributors
 * \date    Created on 31 August 2018
 *
 * \copyright Copyright 2018-2021 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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

#ifndef GUL14_STRING_UTIL_H_
#define GUL14_STRING_UTIL_H_

#include <string>
#include "gul14/internal.h"
#include "gul14/string_view.h"

namespace gul14 {

/// \cond HIDE_SYMBOLS
namespace detail {

// Add a gul14::string_view to the back of a string container using emplace_back().
template <typename StringContainer>
void emplace_back(StringContainer c, string_view sv)
{
    c.emplace_back(sv);
}

} // namespace detail
/// \endcond


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
GUL_EXPORT
extern const string_view default_whitespace_characters;

} // namespace gul14

#endif

/* vim:set noexpandtab softtabstop=4 tabstop=4 shiftwidth=4 textwidth=90 cindent: */
