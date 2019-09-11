/**
 * \file    gul.h
 * \authors \ref contributors
 * \date    Created on August 24, 2018
 * \brief   Main include file for the General Utility Library.
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

#include "gul/case_ascii.h"
#include "gul/cat.h"
#include "gul/escape.h"
#include "gul/finalizer.h"
#include "gul/hexdump.h"
#include "gul/join_split.h"
#include "gul/num_util.h"
#include "gul/optional.h"
#include "gul/replace.h"
#include "gul/SlidingBuffer.h"
#include "gul/statistics.h"
#include "gul/string_util.h"
#include "gul/string_view.h"
#include "gul/substring_checks.h"
#include "gul/time_util.h"
#include "gul/tokenize.h"
#include "gul/to_number.h"
#include "gul/trim.h"
#include "gul/Trigger.h"
#include "gul/type_name.h"
#include "gul/version.h"

/// Namespace gul contains all functions and classes of the General Utility Library.
namespace gul {

/**
 * \mainpage
 *
 * \image html gul_big.png
 *
 * \section introduction Introduction
 *
 * The General Utility Library (GUL) for C++14 contains often-used utility functions and
 * types that form the foundation for other libraries and programs. The main fields
 * covered by the library are:
 *
 *  - \ref string_utilities
 *  - \ref concurrency_utilities
 *  - \ref time_utilities
 *  - \ref statistics_utilities
 *  - \ref debugging_utilities
 *  - \ref numeric_utilities
 *  - \ref container_utilities
 *  - \ref GSL_utilities
 *  - \ref standard_library_backports
 *  - \ref unit_tests
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
 * To use the library, include the single header file \ref gul.h and link your code
 * against the library (-lgul). If you are on Windows, see \ref using_gul_on_windows.
 *
 * GUL requires at least C++14. All utility functions and classes are enclosed in the
 * namespace \ref gul.
 *
 * \section source_code Obtaining the Source Code
 *
 * You can clone the current master branch of GUL from
 * https://stash.desy.de/scm/gul/libgul.git . You can also head over to
 * https://stash.desy.de/projects/GUL/repos/libgul to browse the source. If you have an
 * account, you can submit pull requests and access development branches there, too. If
 * you need an account, let us know.
 *
 * \section contributing Contributing
 *
 * Bug reports, feature requests and constructive criticism are always welcome. Patches
 * are welcome if they come with a lot of tests. Just contact any of the \ref contributors
 * (we are generally reachable under forename.surname@desy.de - you'll figure it out).
 *
 * \section behind_the_scenes Behind the Scenes
 *
 * \version
 * \ref release_notes
 *
 * \authors
 * \ref contributors
 *
 * \copyright
 * \ref copyright_notice
 */

/**
 * \page release_notes Release Notes
 *
 * \section changelog_1_x 1.x Versions
 *
 * \subsection v1_7 Version 1.7
 *
 * - Add contains_nocase(), starts_with_nocase(), ends_with_nocase()
 * - Bugfix in within_ulp() for subnormal numbers
 * - Fix SlidingBuffer{} constructor for non-trivial elements
 * - Improve precision of to_number()
 * - Improve usability with MSVC
 *
 * \subsection v1_6 Version 1.6
 *
 * - *Released with DOOCS 19.8.0*
 * - Add to_number()
 *
 * \subsection v1_5 Version 1.5
 *
 * - Add gul::optional, a backport of std::optional from C++17
 *
 * \subsection v1_4 Version 1.4
 *
 * - escape() & unescape(): Change argument type from const std::string & to
 *   gul::string_view
 * - within_orders() can now be used with integer arguments
 *
 * \subsection v1_3 Version 1.3
 *
 * - \ref gul::SlidingBuffer "SlidingBuffer": Add
 *   \ref gul::SlidingBuffer::push_back "push_back()",
 *   \ref gul::ShrinkBehavior "ShrinkBehavior"
 * - Minor bugfixes for \ref gul::SlidingBuffer "SlidingBuffer"
 * - GUL can be built with Visual C++/Visual Studio 2019 using Meson/Ninja
 *
 * \subsection v1_2 Version 1.2
 *
 * - Add lowercase_ascii(), lowercase_ascii_inplace(), uppercase_ascii(),
 *   uppercase_ascii_inplace()
 * - Bugfixes for \ref gul::SlidingBuffer "SlidingBuffer":
 *   \ref gul::SlidingBuffer::clear "clear()" did not work with variable-sized buffers,
 *   \ref gul::SlidingBuffer::empty "empty()" was not const
 *
 * \subsection v1_1 Version 1.1
 *
 * - Add \ref FinalAction and \ref finally
 *
 * \subsection v1_0 Version 1.0
 *
 * - *Released with DOOCS 19.5.0*
 * - First release version of GUL
 */

/**
 * \page contributors_and_copyright Contributors & Copyright
 *
 * \section contributors General Utility Library Contributors
 *
 * The following individuals contributed to the General Utility Library:
 *
 * > Lars Fröhlich, Sören Grunewald, Olaf Hensler, Fini Jastrow, Josef Wilgen
 *
 * Additional code distributed with this library was written by authors listed under
 * \ref additional_copyright_notices below.
 *
 * \section copyright_notice Copyright Notice
 *
 * Copyright 2018-2019 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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
 * <dt>catch.h</dt>
 * <dd>Copyright 2019 Two Blue Cubes Ltd.
 *     Distributed under the Boost Software License, Version 1.0 (see
 *     \ref license_boost_1_0 and https://github.com/catchorg/Catch2 for details).</dd>
 *
 * <dt>\ref optional.h</dt>
 * <dd>Copyright 2011-2012 Andrzej Krzemienski.
 *     Distributed under the Boost Software License, Version 1.0 (see
 *     \ref license_boost_1_0 and \ref optional.h for details).</dd>
 *
 * <dt>\ref string_view.h</dt>
 * <dd>Copyright 2012-2015 Marshall Clow, copyright 2015 Beman Dawes.
 *     Distributed under the Boost Software License, Version 1.0 (see
 *     \ref license_boost_1_0 and \ref string_view.h for details).</dd>
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
 * <h4>Joining & Splitting</h4>
 *
 * cat(): Efficiently concatenate an arbitrary number of std::strings, C strings,
 *         string_views, or numbers.
 *
 * join(): Concatenate a vector of strings, adding some glue in between.
 *
 * split(), split_sv(): Split a string at all occurrences of a delimiter string or
 *         regular expression, returning a vector of string or string_view tokens.
 *         `split(" hi ", " ")` returns `{"", "hi", ""}`.
 *
 * tokenize(), tokenize_sv(): Split a string at delimiter characters, returning a vector
 *         of string or string_view tokens. `tokenize(" hi ")` returns `{"hi"}`.
 *
 * <h4>Checking for Substrings</h4>
 *
 * contains(): Check if a string contains a certain string or character.
 *
 * ends_with(): Check if a string ends with a certain string or character.
 *
 * starts_with(): Check if a string starts with a certain string or character.
 *
 * <h4>Replacing & Trimming</h4>
 *
 * replace(): Replace all occurrences of a string in another string.
 *
 * replace_inplace(): Replace all occurrences of a string in another string in-place.
 *
 * trim(), trim_left(), trim_right() etc.: Trim leading and/or trailing whitespace (or a
 *          custom set of characters) from a string, returning a new std::string or a
 *          string_view.
 *
 * <h4>Character Conversions</h4>
 *
 * escape(), unescape(): Replace non-ASCII characters in a string so that it looks like a
 *           C string literal (and vice versa).
 *
 * lowercase_ascii(), uppercase_ascii(): Return the ASCII lowercase/uppercase equivalent
 *           of a given string or character. Non-ASCII characters are not modified.
 *
 * lowercase_ascii_inplace(), uppercase_ascii_inplace(): Replace all ASCII characters in a
 *           string by their lowercase/uppercase equivalents.
 * 
 * <h4>Conversion to Numbers</h4>
 *
 * to_number(): Convert an ASCII string_view into an integer or floating-point number.
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
 * The General Utility Library provides some utilities to make dealing with concurrency
 * (threads, locks, and so on) easier.
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
 * \page statistics_utilities Statistics Utilities
 *
 * The General Utility Library offers some utilities to calculate statistics on data
 * in arbitrary containers. All of the functions are templated and allow specifying an
 * optional accessor function so that they can be used on practically any data type.
 *
 * <h3>Functions</h3>
 *
 * accumulate():
 *     Return the values of all elements somehow combined. A closure has to be specified
 *     to describe how to values are combined.
 *
 * mean():
 *     Calculate the arithmetic mean.
 *
 * median():
 *     Calculate the median.
 *
 * min_max():
 *     Return the minimum and maximum value.
 *
 * remove_outliers():
 *     Remove the data points that are the furthest from the mean of all data points.
 *     If more than one point is to be removed this is done recursively with intermediate
 *     recalculations of the mean.
 *
 * rms():
 *     Calculate the root-mean-square value.
 *
 * standard_deviation():
 *     Calculate the standard deviation.
 *
 * <h3>Classes</h3>
 *
 * \ref gul::MinMax "MinMax":
 *     Holds a pair of two values, typically the minimum and maximum element of something.
 *
 * \ref gul::StandardDeviationMean "StandardDeviationMean":
 *     Holds a pair of two values, typically the standard deviation and the mean value of
 *     something.
 */

/**
 * \page numeric_utilities Numeric Utilities
 *
 * The General Utility Library provides several functions dealing with numbers.
 *
 * <h4>Modifying Values</h4>
 *
 * abs():
 *     Calculate the absolute value, even for unsigned types.
 *
 * clamp():
 *     Coerce a value to be within a given range.
 *
 * <h4>Converting to Numbers</h4>
 *
 * to_number(): Convert an ASCII string_view into an integer or floating-point number.
 *
 * <h4>Approximate Comparisons</h4>
 *
 * within_abs():
 *     Determine if two numbers are almost equal, allowing for an absolute difference.
 *
 * within_orders():
 *     Determine if two numbers are almost equal, comparing only some significant digits.
 *
 * within_ulp():
 *     Determine if two numbers are almost equal, allowing for a difference of a given
 *     number of units-in-the-last-place (ULPs).
 */

/**
 * \page standard_library_backports Standard Library Backports
 *
 * The General Utility Library provides a few classes from the C++ standard library that
 * are not yet available to users of older compilers.
 *
 * \ref gul::optional "optional":
 *     A class template that can either contain a value of a certain type or not.
 *     It should behave like
 *     [std::optional](https://en.cppreference.com/w/cpp/utility/optional)
 *     from C++17 for almost all use cases.
 * 
 * \ref gul::string_view "string_view":
 *     A view to a contiguous sequence of chars. It should behave like
 *     [std::string_view](https://en.cppreference.com/w/cpp/string/basic_string_view)
 *     from C++17 for almost all use cases.
 */

/**
 * \page unit_tests Unit Tests
 *
 * GUL uses the [Catch2 test framework](https://github.com/catchorg/Catch2) for its unit
 * tests. It bundles all of its functionality in a single header file. For convenience,
 * the GUL-internal version of this header can be accessed via:
 * \code
 * #include "gul/catch.h"
 * // Your unit tests here
 * \endcode
 * Please refer to https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md
 * for an introduction to Catch2.
 *
 * \note
 * Please note that unlike all other include files provided with GUL, `catch.h` is *not*
 * automatically included by the default include directive:
 * \code
 * #include "gul.h"
 * // Catch2 is not included!
 * \endcode
 */

/**
 * \page GSL_utilities GSL Excerpts
 *
 * Excerpts of the Guidelines Support Library which contains functions and types
 * that are suggested by the
 * [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines).
 *
 * Some items of the [GSL](https://github.com/Microsoft/GSL) useful enough that
 * GUL provides equivalent solutions. Most frequent use-cases can then be solved
 * without actually including the proper GSL.
 * Note that the implementation can differ from the same-named entities in GSL.
 *
 * finally() & \ref gul::FinalAction "FinalAction":
 *     Execute something when we leave the scope.
 */

/**
 * \page debugging_utilities Debugging Utilities
 *
 * The General Utility Library provides some utilities that are mainly, but not
 * exclusively, helpful for debugging.
 *
 * <h3>Functions</h3>
 *
 * hexdump() & \ref gul::hexdump_stream "hexdump_stream()":
 *     Generate an ASCII hex dump of a container, a range, or a piece of memory.
 *
 * type_name():
 *     Generate a human readable string describing a type.
 *
 * finally() & \ref gul::FinalAction "FinalAction":
 *     Execute something when we leave the scope.
 *
 * <h3>Classes</h3>
 *
 * \ref gul::FailToInstantiate "FailToInstantiate":
 *     Class to help debug types.
 */

/**
 * \page container_utilities Containers
 *
 * The General Utility Library provides some additional containers.
 *
 * SlidingBuffer:
 *     A circular data buffer of (semi-)fixed capacity to which elements can be added at
 *     the front or at the back.
 *
 * SlidingBufferExposed:
 *     The same as SlidingBuffer, but with direct iterator access to the underlying buffer
 *     for maximum performance.
 */

/**
 * \page license_boost_1_0 Boost Software License - Version 1.0
 *
 * August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * \page using_gul_on_windows Using GUL on Windows
 *
 * GUL can be built on Windows with the Meson build system and the Microsoft Visual C++
 * compiler. Please refer to README.md in our
 * [source code repository](https://stash.desy.de/projects/GUL/repos/libgul/browse/) for
 * details. By default, both a static and a dynamic library (a DLL) are built.
 *
 * Because of the way DLLs work in Windows, the GUL header files must differentiate
 * between a statically and a dynamically linked library. GUL does this by means of a
 * preprocessor macro, `GUL_USING_STATIC_LIB_OR_OBJECTS`. We assume that most users will
 * prefer the DLL version, therefore we treat this as the default case:
 * 
 * - If you link your code against the DLL, there is no need to take any further action.
 *   The header files already provide the necessary import declarations.
 * - If you link your code against the static library (or directly against GUL's object
 *   files), you must define the macro GUL_USING_STATIC_LIB_OR_OBJECTS. This will
 *   suppress the DLL import declarations.
 *
 * On other platforms and compilers, the macro is meaningless.
 */

} // namespace gul
