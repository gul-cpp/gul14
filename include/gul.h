/**
 * \file    gul.h
 * \authors \ref contributors
 * \date    Created on August 24, 2018
 * \brief   Main include file for the General Utility Library.
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
 * \image html gul_big.png
 *
 * \section introduction Introduction
 * 
 * The General Utility Library (GUL) contains often-used utility functions and types that
 * form the foundation for other libraries and programs. The main fields covered by the
 * library are:
 * 
 *  - \ref string_utilities
 *  - \ref concurrency_utilities
 *  - \ref time_utilities
 *  - Numerical helpers
 *  - \ref backports of standard library features that are not yet available on common
 *    compilers
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
 * To use the library, include the single header file \ref gul.h and link your code
 * against the library (-lgul).
 *
 * \authors \ref contributors
 * \copyright \ref copyright_notice
 */

/**
 * \page contributors_and_copyright Contributors & Copyright
 *
 * \section contributors General Utility Library Contributors
 *
 * The following individuals contributed to the General Utility Library:
 *
 * > Lars Fröhlich, Sören Grunewald, Olaf Hensler, Fini Jastrow, Tobias Tempel,
 * > Josef Wilgen
 *
 * Additional code distributed with this library was written by authors listed under
 * \ref additional_copyright_notices below.
 *
 * \section copyright_notice Copyright Notice
 *
 * Copyright 2018 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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
 *
 * \section additional_copyright_notices Additional Copyright Notices
 *
 * This library contains third-party code that is distributed under its own copyright
 * notice:
 *
 * <dl>
 * <dt>\ref string_view.h</dt>
 * <dd>Copyright Marshall Clow 2012-2015, copyright Beman Dawes 2015.
 *     Distributed under the Boost Software License, Version 1.0 (see  \ref string_view.h
 *     for details).</dd>
 * </dl>
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
 *
 * <h4>Splitting & Joining</h4>
 *
 * cat(): Efficiently concatenate an arbitrary number of std::strings, C strings,
 *        string_views, or numbers.
 *
 * join(): Concatenate a vector of strings, adding some glue in between.
 *
 * split(): Separate a string at all occurences of a delimiter string or regular
 *          expression.
 *
 * tokenize(): Split a string into a vector of string tokens at delimiter characters.
 *
 * tokenize_sv(): Split a string into a vector of string_views that point into
 *                         the original string.
 *
 * <h4>Find & Replace</h4>
 *
 * replace(): Replace all occurrences of a string in another string.
 *
 * replace_inplace(): Replace all occurrences of a string in another string in-place.
 *
 * <h4>Dealing with Special Characters</h4>
 *
 * escape(): Replace non-printable characters in a string so that it looks like a C string
 *           literal.
 *
 * unescape(): Convert a string with escaped characters back to its original form.
 *
 * 
 * <h3>Classes</h3>
 *
 * \ref gul::string_view "string_view":
 *     A view to a contiguous sequence of chars. The GUL version is a backport of
 *     [std::string_view](https://en.cppreference.com/w/cpp/string/basic_string_view)
 *     from libc++ for C++17.
 */

/**
 * \page concurrency_utilities Concurrency Utilities
 *
 * GUL provides some utilities to make dealing with concurrency (threads, locks, and so
 * on) easier.
 *
 * <h3>Classes</h3>
 *
 * \ref gul::Trigger "Trigger":
 *     A class that allows sending triggers and waiting for them across different threads,
 *     like an electric trigger line.
 *
 * <h3>Functions</h3>
 *
 * sleep():
 *     Wait for a given amount of time and be woken up from a different thread.
 */

/**
 * \page time_utilities Time Utilities
 *
 * The General Utility Library provides a few time-related utility functions.
 *
 * sleep():
 *     Wait for a given amount of time (and be woken up from a different thread, if
 *     desired).
 *
 * tic() & toc():
 *     Measure elapsed time.
 */

/**
 * \page backports Backports
 *
 * The General Utility Library provides a few classes from C++ standard libraries that
 * are not yet available to users of older compilers. Here's a list:
 *
 * \ref gul::string_view "string_view":
 *     A view to a contiguous sequence of chars. The GUL version is a backport of
 *     [std::string_view](https://en.cppreference.com/w/cpp/string/basic_string_view)
 *     from libc++ for C++17.
 */

#pragma once

#include "gul/cat.h"
#include "gul/join_split.h"
#include "gul/string_util.h"
#include "gul/string_view.h"
#include "gul/time_util.h"
#include "gul/tokenize.h"
#include "gul/trim.h"
#include "gul/Trigger.h"
#include "gul/version.h"

/// Namespace gul contains all functions and classes of the General Utility Library.
namespace gul {}
