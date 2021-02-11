/**
 * \file   version.h
 * \author \ref contributors
 * \date   Created on September 14, 2018
 * \brief  Provide information about the library version
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

#ifndef GUL14_VERSION_H_
#define GUL14_VERSION_H_

#include "gul14/internal.h"

namespace gul14 {

/**
 * Holds the git version tag of the sources that the library has
 * been build with.
 * It has the format returned by git describe --tags --always --dirty
 */
GUL_EXPORT
extern char const* version_git;

/**
 * Holds the API version of the library.
 */
GUL_EXPORT
extern char const* version_api;

} // namespace gul14

#endif
