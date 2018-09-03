/**
 * \file   gul.h
 * \author Lars Froehlich, Soeren Grunewald, Olaf Hensler, Fini Jastrow, Tobias Tempel,
 *         Josef Wilgen
 * \date   Created on August 24, 2018
 * \brief  Main include file for the General Utility Library.
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

/**
 * \mainpage
 *
 * \image html desy.png
 *
 * \section introduction Introduction
 * 
 * The General Utility Library (short: GUL) contains often-used utility functions and
 * types that form the foundation for other libraries and programs. The main fields
 * covered by the library are:
 * 
 *  - \ref string_utilities
 *  - Exceptions
 *  - Time
 *  - Numerical helpers
 *  - \ref backports of standard library features that are not yet available on common compilers
 *
 * To keep the library useful for as many users as possible, special-purpose code has no
 * place in it. Specifically, the library includes:
 *
 *  - No code that is useful for only one project
 *  - No control system related code (DOOCS, TINE, TANGO, ...)
 *  - No external dependencies except for the C++ and C standard libraries
 *
 * The General Utility Library strives for a very high quality level. To ensure this, we
 * follow a list of \ref quality_standards.
 *
 * \section usage Usage
 *
 * GUL requires at least C++14. All utility functions and classes are enclosed in the
 * namespace \ref gul.
 *
 * To use the library, include the single header file \link gul.h \endlink and link
 * your code against the library (-lgul).
 *
 * \authors Lars Froehlich, Soeren Grunewald, Olaf Hensler, Fini Jastrow, Tobias Tempel,
 *          Josef Wilgen
 */

/**
 * \page quality_standards Quality Standards
 *
 * The General Utility Library strives for a very high quality level. This is ensured by
 * following a set of best practices:
 * - __Style:__ Code must adhere to the
 *   [MCS style guide for C++](https://confluence.desy.de/pages/viewpage.action?pageId=41648870)
 *   and it should follow the
 *   [C++ Core Guidelines](http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines).
 * - __Documentation:__ Every function, type, or other entity in the API must be fully
 *   documented via Doxygen. This includes a clear description of purpose and
 *   functionality. For functions, all parameters, return values, thrown exceptions, and
 *   pre-/postconditions are described. For classes, all class invariants are clearly
 *   stated.
 * - __Unit tests:__ Every entity in the library must have a set of associated unit tests.
 * - __Code review:__ Every commit to the library must be signed off by at least two
 *   developers. Every developer must ensure that all of the other quality criteria are
 *   fulfilled.
 */

/**
 * \page string_utilities String Utilities
 *
 * The General Utility Library provides a number of utility functions and classes to help
 * with strings.
 *
 * <h3>Functions</h3>
 * - gul::split(): Separate a string at all occurences of a delimiter
 * - gul::join(): Concatenate string elements of a vector interspaced with glue
 * - gul::cat() Efficiently concatenate an arbitrary number of std::strings, C strings,
 *              string_views, or numbers.
 *   \code
 *   std::string str = gul::cat("This might replace ", 1e6, " uses of sprintf() "s, "in our code.");
 *   \endcode
 *
 * <h3>Classes</h3>
 * - gul::string_view  A view to a contiguous sequence of chars. The GUL version is
 *                     a backport of
 *                     [std::string_view](https://en.cppreference.com/w/cpp/string/basic_string_view)
 *                     from libc++ for C++17.
 */

/**
 * \page backports Backports
 *
 * The General Utility Library provides a few classes from C++ standard libraries that
 * are not yet available to users of older compilers. Here's a list:
 * - gul::string_view  A view to a contiguous sequence of chars. The GUL version is
 *                     a backport of
 *                     [std::string_view](https://en.cppreference.com/w/cpp/string/basic_string_view)
 *                     from libc++ for C++17.
 */

#pragma once

#include "gul/cat.h"
#include "gul/join_split.h"
#include "gul/string_util.h"
#include "gul/string_view.h"

/// Namespace gul contains all functions and classes of the General Utility Library.
namespace gul {}
