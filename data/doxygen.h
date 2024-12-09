/**
 * \file    doxygen.h
 * \authors \ref contributors
 * \date    Created on August 24, 2018
 * \brief   Doxygen input file for the General Utility Library.
 *
 * \copyright Copyright 2018-2024 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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
 * Namespace gul14 contains all functions and classes of the General Utility Library.
 */
namespace gul14 {

/**
 * \mainpage
 *
 * \image html gul14_big.png
 *
 * \section introduction Introduction
 *
 * The General Utility Library for C++14 (GUL14) contains often-used utility functions and
 * types that form the foundation for other libraries and programs. The main fields
 * covered by the library are:
 *
 *  - \ref string_utilities
 *  - \ref concurrency_utilities
 *  - \ref date_time_utilities
 *  - \ref statistics_utilities
 *  - \ref debugging_utilities
 *  - \ref numeric_utilities
 *  - \ref bit_manipulation
 *  - \ref container_utilities
 *  - \ref GSL_utilities
 *  - \ref metaprogramming
 *  - \ref standard_library_backports
 *  - \ref unit_tests
 *
 * To keep the library useful for as many users as possible, special-purpose code has no
 * place in it. Specifically, the library includes:
 *
 *  - Only code that is potentially useful for a wide range of C++ projects
 *  - No external dependencies except for the C++ and C standard libraries
 *
 * We strive for a very high quality level. To ensure this, we follow a list of
 * \ref quality_standards.
 *
 * \section who_is_using_the_library Who Is Using GUL14?
 *
 * GUL14 was developed at the German national particle accelerator laboratory
 * <a href="https://www.desy.de">DESY</a>. It was originally meant to provide a modern and
 * well-tested foundation for the <a href="https://doocs.desy.de/">DOOCS</a> control
 * system which drives most of the lab's accelerators. It is still used mainly in this
 * role, but has since spread to many unrelated projects as well. At some point we decided
 * to publish it on GitHub to make collaboration outside the lab easier.
 *
 * \section usage Usage
 *
 * To use the library:
 *
 *  - Include the main header file \ref gul14/gul.h (or read more under
 *    \ref how_to_include_gul_headers).
 *  - Link your code against the library (-lgul14).
 *  - If you are on Windows, see \ref using_gul_on_windows.
 *
 * All functions and classes are enclosed in the namespace \ref gul14.
 *
 * GUL14 requires at least C++14. It works fine with newer versions of the standard, but
 * uses its own backport types (e.g. gul14::string_view) in function interfaces.
 *
 * \section installation Installation
 *
 * If you are using the [vcpkg](https://vcpkg.io/) package manager, you can install the
 * library simply by running:
 * \verbatim vcpkg install gul14\endverbatim
 * Otherwise, you may have to build and install it manually. Have a look at the
 * [readme file on GitHub](https://github.com/gul-cpp/gul14/blob/main/README.md).
 *
 * \section source_code Obtaining the Source Code
 *
 * You can browse or clone the source code at https://github.com/gul-cpp/gul14.git.
 *
 * \section contributing Contributing
 *
 * Bug reports and feature requests are always welcome and highly appreciated. So are pull
 * requests, but they should fit into the scope of the library – it may be better to
 * discuss new features in a GitHub issue before diving into the implementation.
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
 * \page how_to_include_gul_headers How to Include GUL14 Headers
 *
 * All header files of the library reside under `gul14/` with no further subdirectories.
 * To get access to almost all of GUL14's functionality, it is sufficient to include the
 * all-in-one header:
 * \code
 * #include <gul14/gul.h>
 * \endcode
 *
 * Two specific header files are not included by this all-in-one header:
 * <ul>
 * <li>`gul14/catch.h`: The header file for the Catch2 unit test framework</li>
 * <li>`gul14/date.h`: Howard Hinnant's header for calendar date functionality</li>
 * </ul>
 * While the unit test framework is simply not useful for inclusion into regular code,
 * `date.h` is excluded chiefly because of its negative impact on compile times. Just
 * include it directly where needed.
 *
 * If you prefer to shave the last seconds off your build times, you are of course free to
 * include also the other GUL14 headers individually without going through `gul.h`. Just
 * follow the documentation of the classes and functions you use to find the associated
 * header files.
 *
 * We do not guarantee that individual header file names will never change (except for
 * `gul14/gul.h`), but we can assure you that this will happen rarely, if ever.
 */

/**
 * \page release_notes Release Notes
 *
 * \section changelog_2_x 2.x Versions
 *
 * \subsection V2_12_1 Version 2.12.1
 *
 * - Allow mutable function objects in ThreadPool::add_task()
 *
 * \subsection V2_12_0 Version 2.12.0
 *
 * - *Released with DOOCS 24.10.0–24.12.1*
 * - Add gul14::to_number<bool>
 *
 * \subsection v2_11_2 Version 2.11.2
 *
 * - *Released with DOOCS 24.3.1*
 * - ThreadPool: Add default constructor for TaskHandle
 *
 * \subsection v2_11_1 Version 2.11.1
 *
 * - Fix missing ThreadPool symbols in shared library
 *
 * \subsection v2_11_0 Version 2.11.0
 *
 * - *Released with DOOCS 24.3.0*
 * - Add ThreadPool
 * - Add backports of std::invoke, std::invoke_result, std::invoke_result_t,
 *   std::is_invocable, and std::is_invocable_r from C++17
 * - Improve accuracy of to_number() for certain platforms
 *
 * \subsection v2_10_0 Version 2.10.0
 *
 * - *Released with DOOCS 24.1.0-24.1.0*
 * - Add gul14::endian, a backport of std::endian from C++20
 * - Add gul14::is_little_endian() and gul14::is_big_endian()
 *
 * \subsection v2_9_2 Version 2.9.2
 *
 * - Add OverloadSet and make_overload_set()
 * - Fix compilation errors in gul14::visit()
 *
 * \subsection v2_9_1 Version 2.9.1
 *
 * - Fix compilation errors in gul14::variant when compiled with C++17 or MSVC
 *
 * \subsection v2_9_0 Version 2.9.0
 *
 * - *Released with DOOCS 23.6.0*
 * - Add gul14::variant, a backport of std::variant from C++17
 * - Add gul14::remove_cvref and gul14::void_t, backports of metaprogramming functions
 *   from C++20 and C++17, respectively.
 *
 * \subsection v2_8_0 Version 2.8.0
 *
 * - Add gul14::expected, a backport of std::expected from C++23
 *
 * \subsection v2_7_1 Version 2.7.1
 *
 * - *Released with DOOCS 22.10.0-23.3.0*
 * - within_orders(0.0, 0.0, 10) results now in true
 *
 * \subsection v2_7 Version 2.7
 *
 * - Add repeat()
 * - Add gcd() and lcm()
 * - Add Howard Hinnant's date.h header
 *
 * \subsection v2_6 Version 2.6
 *
 * - *Released with DOOCS 21.12.0–22.6.1*
 * - Add safe_string()
 * - Add hex_string()
 * - Add IsContainerLike<>
 *
 * \subsection v2_5 Version 2.5
 *
 * - *Released with DOOCS 21.3.0–21.7.1*
 * - Add support for more container types as return type of split() and tokenize()
 * - Add \ref gul14::SmallVector "SmallVector"
 *
 * \subsection v2_4 Version 2.4
 *
 * - Add \ref SlidingBuffer::pop_front() and \ref SlidingBuffer::pop_back()
 *
 * \subsection v2_3 Version 2.3
 *
 * - *Released with DOOCS 20.6.0–20.10.1*
 * - Allow join() to accept arbitrary containers and add a two-iterator interface
 *
 * \subsection v2_2 Version 2.2
 *
 * - *Released with DOOCS 20.5.0*
 * - Add minimum() and maximum()
 *
 * \subsection v2_1 Version 2.1
 *
 * - *Released with DOOCS 20.2.0 and DOOCS 20.3.0*
 * - Add equals_nocase()
 *
 * \subsection v2_0 Version 2.0
 *
 * - *Released with DOOCS 20.1.0*
 * - Rename GUL to GUL14 (applies to namespace, include directory, library name)
 * - Make \ref SlidingBuffer::SlidingBufferIterator "SlidingBufferIterator" random
 *   access instead of bidirectional
 *
 * \section changelog_1_x 1.x Versions
 *
 * \subsection v1_9 Version 1.9
 *
 * - Add gul14::span, a backport of std::span from C++20
 *
 * \subsection v1_8 Version 1.8
 *
 * - *Released with DOOCS 19.11.0*
 * - Add bit_set(), bit_reset(), bit_flip(), bit_test()
 * - Make to_number() unconditionally noexcept
 *
 * \subsection v1_7 Version 1.7
 *
 * - *Released with DOOCS 19.10.0*
 * - Add contains_nocase(), starts_with_nocase(), ends_with_nocase()
 * - Bugfix in within_ulp() for subnormal numbers
 * - Fix SlidingBuffer{} constructor for non-trivial elements
 * - Improve precision of to_number()
 * - Improve usability with MSVC
 *
 * \subsection v1_6 Version 1.6
 *
 * - *Released with DOOCS 19.8.2*
 * - Add to_number()
 *
 * \subsection v1_5 Version 1.5
 *
 * - *Released with DOOCS 19.8.0*
 * - Add gul14::optional, a backport of std::optional from C++17
 *
 * \subsection v1_4 Version 1.4
 *
 * - escape() & unescape(): Change argument type from const std::string & to
 *   gul14::string_view
 * - within_orders() can now be used with integer arguments
 *
 * \subsection v1_3 Version 1.3
 *
 * - \ref gul14::SlidingBuffer "SlidingBuffer": Add
 *   \ref gul14::SlidingBuffer::push_back "push_back()",
 *   \ref gul14::ShrinkBehavior "ShrinkBehavior"
 * - Minor bugfixes for \ref gul14::SlidingBuffer "SlidingBuffer"
 * - GUL can be built with Visual C++/Visual Studio 2019 using Meson/Ninja
 *
 * \subsection v1_2 Version 1.2
 *
 * - Add lowercase_ascii(), lowercase_ascii_inplace(), uppercase_ascii(),
 *   uppercase_ascii_inplace()
 * - Bugfixes for \ref gul14::SlidingBuffer "SlidingBuffer":
 *   \ref gul14::SlidingBuffer::clear "clear()" did not work with variable-sized buffers,
 *   \ref gul14::SlidingBuffer::empty "empty()" was not const
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
 * Copyright 2018-2024 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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
 * <dt>date.h</dt>
 * <dd>Copyright 2015, 2016, 2017 Howard Hinnant, 2016 Adrian Colomitchi, 2017 Florian
 *     Dang, 2017 Paul Thompson, 2018, 2019 Tomasz Kamiński, 2019 Jiangang Zhuang.
 *     Distributed under the MIT License (see \ref license_mit and \ref date.h for
 *     details).</dd>
 *
 * <dt>\ref expected.h</dt>
 * <dd>Written in 2017 by Sy Brand. To the extent possible under law, the author(s) have
 *     dedicated all copyright and related and neighboring rights to this software to the
 *     public domain worldwide. This software is distributed without any warranty.
 *     \ref license_cc0_public_domain and \ref expected.h for details).</dd>
 *
 * <dt>\ref optional.h</dt>
 * <dd>Copyright 2011-2012 Andrzej Krzemienski.
 *     Distributed under the Boost Software License, Version 1.0 (see
 *     \ref license_boost_1_0 and \ref optional.h for details).</dd>
 *
 * <dt>\ref span.h</dt>
 * <dd>Copyright 2018 Tristan Brindle.
 *     Distributed under the Boost Software License, Version 1.0 (see
 *     \ref license_boost_1_0 and \ref span.h for details).</dd>
 *
 * <dt>\ref string_view.h</dt>
 * <dd>Copyright 2012-2015 Marshall Clow, copyright 2015 Beman Dawes.
 *     Distributed under the Boost Software License, Version 1.0 (see
 *     \ref license_boost_1_0 and \ref string_view.h for details).</dd>
 *
 * <dt>\ref traits.h</dt>
 * <dd>Copyright 2015-2017 Michael Park (implementations of invoke, invoke_result,
 *     invoke_result_t, is_invocable, is_invocable_r), copyright 2021-2024 Deutsches
 *     Elektronen-Synchrotron (DESY), Hamburg (other implementations and modifications).
 *     Distributed under the Boost Software License, Version 1.0 (see
 *     \ref license_boost_1_0 and \ref traits.h for details).</dd>
 *
 * <dt>\ref variant.h</dt>
 * <dd>Copyright 2015-2017 Michael Park, copyright 2023-2024 Deutsches
 *     Elektronen-Synchrotron (DESY), Hamburg.
 *     Distributed under the Boost Software License, Version 1.0 (see
 *     \ref license_boost_1_0 and \ref variant.h for details).</dd>
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
 * <h4>Comparisons</h4>
 *
 * equals_nocase(): Determine if two strings are equal, making no distinction between
 *      upper and lower case ASCII characters.
 *
 * contains(), contains_nocase(): Check if a string contains a certain string or
 *      character.
 *
 * ends_with(), ends_with_nocase(): Check if a string ends with a certain string or
 *      character.
 *
 * starts_with(), starts_with_nocase(): Check if a string starts with a certain string or
 *      character.
 *
 * <h4>Joining, Splitting, Repeating</h4>
 *
 * cat(): Efficiently concatenate an arbitrary number of std::strings, C strings,
 *         string_views, or numbers.
 *
 * join(): Concatenate all strings in a range, placing a delimiter between them.
 *
 * repeat(): Repeat a string N times.
 *
 * split(), split_sv(): Split a string at all occurrences of a delimiter string or
 *         regular expression, returning a vector of string or string_view tokens.
 *         `split(" hi ", " ")` returns `{"", "hi", ""}`.
 *
 * tokenize(), tokenize_sv(): Split a string at delimiter characters, returning a vector
 *         of string or string_view tokens. `tokenize(" hi ")` returns `{"hi"}`.
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
 * <h4>Number Conversions</h4>
 *
 * hex_string(): Convert an integer or a range of integers into their hexadecimal ASCII
 *           representation.
 *
 * to_number(): Convert an ASCII string_view into an integer or floating-point number.
 *
 * <h4>C Strings</h4>
 *
 * safe_string(): Safely create a std::string from a char pointer and a length.
 *
 * <h3>Classes</h3>
 *
 * \ref gul14::string_view "string_view":
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
 * \ref gul14::ThreadPool "ThreadPool":
 *     A thread pool and task queue that allows executing tasks on a fixed number of
 *     worker threads.
 *
 * \ref gul14::Trigger "Trigger":
 *     A class that allows sending triggers and waiting for them across different threads,
 *     like an electric trigger line.
 *
 * <h3>Functions</h3>
 *
 * sleep():
 *     Wait for a given amount of time and be woken up from a different thread.
 */

/**
 * \page date_time_utilities Date & Time Utilities
 *
 * The General Utility Library provides a few time-related utility functions.
 *
 * sleep():
 *     Wait for a given amount of time (and be woken up from a different thread, if
 *     desired).
 *
 * tic() & toc():
 *     Measure elapsed time.
 *
 * Furthermore, the library includes Howard Hinnant's date.h header, which extends C++14's
 * `<chrono>` library with types and functionality for handling calendar dates. Most of
 * this functionality has been standardized in C++20. Documentation is available on
 * [GitHub](https://howardhinnant.github.io/date/date.html); just note that the GUL14
 * version lives in the namespace `gul14::date`.
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
 * maximum():
 *     Return the maximum value.
 *
 * mean():
 *     Calculate the arithmetic mean.
 *
 * median():
 *     Calculate the median.
 *
 * minimum():
 *     Return the minimum value.
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
 * \ref gul14::MinMax "MinMax":
 *     Holds a pair of two values, typically the minimum and maximum element of something.
 *
 * \ref gul14::StandardDeviationMean "StandardDeviationMean":
 *     Holds a pair of two values, typically the standard deviation and the mean value of
 *     something.
 */

/**
 * \page numeric_utilities Numeric Utilities
 *
 * <h4>Modifying Values</h4>
 *
 * abs():
 *     Calculate the absolute value, even for unsigned types.
 *
 * clamp():
 *     Coerce a value to be within a given range.
 *
 * <h4>Greatest Common Divisor & Least Common Multiple</h4>
 *
 * gcd():
 *     Calculate the greatest common divisor of two integers.
 *
 * lcm():
 *     Calculate the least common multiple of two integers.
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
 * \page bit_manipulation Bit Manipulation and Testing
 *
 * <h4>Working With Bits in Integral Values</h4>
 *
 * bit_set():
 *     Set a bit in an integral value.
 *
 * bit_reset():
 *     Clear a bit in an integral value.
 *
 * bit_flip():
 *     Flip a bit in an integral value.
 *
 * bit_test():
 *     Test a bit in an integral value.
 *
 * <h4>Endianness</h4>
 *
 * \ref gul14::endian "endian":
 *     An enum to determine the endianness of multi-byte scalars on the current platform,
 *     behaving like [std::endian](https://en.cppreference.com/w/cpp/types/endian) from
 *     C++20.
 *
 * is_big_endian():
 *     Determine if the current platform uses big-endian byte order.
 *
 * is_little_endian():
 *     Determine if the current platform uses little-endian byte order.
 */

/**
 * \page metaprogramming Metaprogramming Utilities and Type Traits
 *
 * The library provides some utilities for template metaprogramming:
 *
 * \ref gul14::invoke "invoke":
 *     A function template that calls a callable object with a given set of arguments.
 *     This is a backport of
 *     [std::invoke](https://en.cppreference.com/w/cpp/utility/functional/invoke) from
 *     C++17.
 *
 * \ref gul14::invoke_result "invoke_result", \ref gul14::invoke_result_t "invoke_result_t":
 *     A metafunction that computes the result of invoking a callable object with the
 *     given arguments. This is a backport of
 *     [std::invoke_result](https://en.cppreference.com/w/cpp/types/result_of) from
 *     C++17.
 *
 * \ref gul14::is_invocable "is_invocable", \ref gul14::is_invocable_r "is_invocable_r":
 *     A type trait that checks whether a callable object can be invoked with a given set
 *     of arguments. This is a backport of
 *     [std::is_invocable](https://en.cppreference.com/w/cpp/types/is_invocable) from
 *     C++17.
 *
 * \ref gul14::IsContainerLike "IsContainerLike":
 *     A type trait to determine if a type behaves like a standard container.
 *
 * \ref gul14::OverloadSet "OverloadSet" and \ref gul14::make_overload_set "make_overload_set()":
 *     A function object type that works like an overload set of functions, and a helper
 *     function to create such an object from a bunch of lambdas.
 *
 * \ref gul14::remove_cvref "remove_cvref":
 *     A metafunction to remove const, volatile, and reference qualifiers from a type.
 *     This is a backport of
 *     [std::remove_cvref](https://en.cppreference.com/w/cpp/types/remove_cvref) from
 *     C++20.
 *
 * \ref gul14::void_t "void_t":
 *     A template typedef that maps an arbitrary list of types to void. This is primarily
 *     useful to detect ill-formed types for SFINAE. This is a backport of
 *     [std::void_t](https://en.cppreference.com/w/cpp/types/void_t) from C++17.
 */

/**
 * \page standard_library_backports Standard Library Backports
 *
 * The General Utility Library provides a few classes from the C++ standard library that
 * are not yet available to users of older compilers.
 *
 * \ref gul14::endian "endian":
 *     An enum to determine the endianness of multi-byte scalars on the current platform,
 *     behaving like [std::endian](https://en.cppreference.com/w/cpp/types/endian) from
 *     C++20.
 *
 * \ref gul14::expected "expected":
 *     A class template that can either contain a value of a certain (expected) type or an
 *     error value. It should behave like
 *     [std::expected](https://en.cppreference.com/w/cpp/utility/expected)
 *     from C++23 for almost all use cases.
 *
 * \ref gul14::in_place_t "in_place_t" etc.:
 *     A small family of types and tags that can be used in the constuctors of
 *     \ref gul14::expected "expected", \ref gul14::optional "optional", and
 *     \ref gul14::variant "variant" to request in-place construction. See
 *     [std::in_place](https://en.cppreference.com/w/cpp/utility/in_place) for
 *     documentation on the corresponding C++17 entities.
 *
 * \ref gul14::optional "optional":
 *     A class template that can either contain a value of a certain type or not.
 *     It should behave like
 *     [std::optional](https://en.cppreference.com/w/cpp/utility/optional)
 *     from C++17 for almost all use cases.
 *
 * \ref gul14::remove_cvref "remove_cvref":
 *     A metafunction to remove const, volatile, and reference qualifiers from a type.
 *     This is a backport of
 *     [std::remove_cvref](https://en.cppreference.com/w/cpp/types/remove_cvref) from
 *     C++20.
 *
 * \ref gul14::span "span":
 *     A view to a contiguous sequence of objects. It should behave like
 *     [std::span](https://en.cppreference.com/w/cpp/container/span) from C++20 for almost
 *     all use cases.
 *
 * \ref gul14::string_view "string_view":
 *     A view to a contiguous sequence of chars. It should behave like
 *     [std::string_view](https://en.cppreference.com/w/cpp/string/basic_string_view)
 *     from C++17 for almost all use cases.
 *
 * \ref gul14::variant "variant":
 *     Sometimes called a "type-safe union", a variant can hold a value of one of a
 *     specified set of types. Unlike a union, it can be queried for the type it is
 *     currently holding and ensures that only the stored type is accessed. The
 *     implementation should behave like
 *     [std::variant](https://en.cppreference.com/w/cpp/utility/variant) from C++17.
 *
 * \ref gul14::void_t "void_t":
 *     A template typedef that maps an arbitrary list of types to void. This is primarily
 *     useful to detect ill-formed types for SFINAE. This is a backport of
 *     [std::void_t](https://en.cppreference.com/w/cpp/types/void_t) from C++17.
 */

/**
 * \page unit_tests Unit Tests
 *
 * GUL uses the [Catch2 test framework](https://github.com/catchorg/Catch2) for its unit
 * tests. It bundles all of its functionality in a single header file. For convenience,
 * the GUL-internal version of this header can be accessed via:
 * \code
 * #include "gul14/catch.h"
 * // Your unit tests here
 * \endcode
 * Please refer to https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md
 * for an introduction to Catch2.
 *
 * \note
 * Please note that unlike all other include files provided with GUL, `catch.h` is *not*
 * automatically included by the default include directive:
 * \code
 * #include "gul14/gul.h"
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
 * finally() & \ref gul14::FinalAction "FinalAction":
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
 * hexdump() & \ref gul14::hexdump_stream "hexdump_stream()":
 *     Generate an ASCII hex dump of a container, a range, or a piece of memory.
 *
 * type_name():
 *     Generate a human readable string describing a type.
 *
 * finally() & \ref gul14::FinalAction "FinalAction":
 *     Execute something when we leave the scope.
 *
 * <h3>Classes</h3>
 *
 * \ref gul14::FailToInstantiate "FailToInstantiate":
 *     Class to help debug types.
 */

/**
 * \page container_utilities Containers
 *
 * The General Utility Library provides several classes that can contain elements of other
 * types.
 *
 * <h3>Multi-Element Containers</h3>
 *
 * SlidingBuffer:
 *     A circular data buffer of (semi-)fixed capacity to which elements can be added at
 *     the front or at the back.
 *
 * SlidingBufferExposed:
 *     The same as SlidingBuffer, but with direct iterator access to the underlying buffer
 *     for maximum performance.
 *
 * SmallVector:
 *     A resizable container with contiguous storage that can hold a specified number of
 *     elements without allocating memory on the heap.
 *
 * <h3>Single-Element and Special-Purpose Containers</h3>
 *
 * \ref gul14::expected "expected":
 *     A class template that can either contain a value of a certain (expected) type or an
 *     error value. It should behave like
 *     [std::expected](https://en.cppreference.com/w/cpp/utility/expected)
 *     from C++23 for almost all use cases.
 *
 * \ref gul14::optional "optional":
 *     A class template that can either contain a value of a certain type or not. It
 *     should behave like
 *     [std::optional](https://en.cppreference.com/w/cpp/utility/optional) from C++17 for
 *     almost all use cases.
 *
 * \ref gul14::variant "variant":
 *     Sometimes called a "type-safe union", a variant can hold a value of one of a
 *     specified set of types. Unlike a union, it can be queried for the type it is
 *     currently holding and ensures that only the stored type is accessed. The
 *     implementation should behave like
 *     [std::variant](https://en.cppreference.com/w/cpp/utility/variant) from C++17.
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
 * \page license_cc0_public_domain Creative Commons Legal Code – CC0 1.0 Universal
 *
 *     CREATIVE COMMONS CORPORATION IS NOT A LAW FIRM AND DOES NOT PROVIDE
 *     LEGAL SERVICES. DISTRIBUTION OF THIS DOCUMENT DOES NOT CREATE AN
 *     ATTORNEY-CLIENT RELATIONSHIP. CREATIVE COMMONS PROVIDES THIS
 *     INFORMATION ON AN "AS-IS" BASIS. CREATIVE COMMONS MAKES NO WARRANTIES
 *     REGARDING THE USE OF THIS DOCUMENT OR THE INFORMATION OR WORKS
 *     PROVIDED HEREUNDER, AND DISCLAIMS LIABILITY FOR DAMAGES RESULTING FROM
 *     THE USE OF THIS DOCUMENT OR THE INFORMATION OR WORKS PROVIDED
 *     HEREUNDER.
 *
 * Statement of Purpose
 *
 * The laws of most jurisdictions throughout the world automatically confer
 * exclusive Copyright and Related Rights (defined below) upon the creator
 * and subsequent owner(s) (each and all, an "owner") of an original work of
 * authorship and/or a database (each, a "Work").
 *
 * Certain owners wish to permanently relinquish those rights to a Work for
 * the purpose of contributing to a commons of creative, cultural and
 * scientific works ("Commons") that the public can reliably and without fear
 * of later claims of infringement build upon, modify, incorporate in other
 * works, reuse and redistribute as freely as possible in any form whatsoever
 * and for any purposes, including without limitation commercial purposes.
 * These owners may contribute to the Commons to promote the ideal of a free
 * culture and the further production of creative, cultural and scientific
 * works, or to gain reputation or greater distribution for their Work in
 * part through the use and efforts of others.
 *
 * For these and/or other purposes and motivations, and without any
 * expectation of additional consideration or compensation, the person
 * associating CC0 with a Work (the "Affirmer"), to the extent that he or she
 * is an owner of Copyright and Related Rights in the Work, voluntarily
 * elects to apply CC0 to the Work and publicly distribute the Work under its
 * terms, with knowledge of his or her Copyright and Related Rights in the
 * Work and the meaning and intended legal effect of CC0 on those rights.
 *
 * 1. Copyright and Related Rights. A Work made available under CC0 may be
 * protected by copyright and related or neighboring rights ("Copyright and
 * Related Rights"). Copyright and Related Rights include, but are not
 * limited to, the following:
 *
 *   i. the right to reproduce, adapt, distribute, perform, display,
 *      communicate, and translate a Work;
 *  ii. moral rights retained by the original author(s) and/or performer(s);
 * iii. publicity and privacy rights pertaining to a person's image or
 *      likeness depicted in a Work;
 *  iv. rights protecting against unfair competition in regards to a Work,
 *      subject to the limitations in paragraph 4(a), below;
 *   v. rights protecting the extraction, dissemination, use and reuse of data
 *      in a Work;
 *  vi. database rights (such as those arising under Directive 96/9/EC of the
 *      European Parliament and of the Council of 11 March 1996 on the legal
 *      protection of databases, and under any national implementation
 *      thereof, including any amended or successor version of such
 *      directive); and
 * vii. other similar, equivalent or corresponding rights throughout the
 *      world based on applicable law or treaty, and any national
 *      implementations thereof.
 *
 * 2. Waiver. To the greatest extent permitted by, but not in contravention
 * of, applicable law, Affirmer hereby overtly, fully, permanently,
 * irrevocably and unconditionally waives, abandons, and surrenders all of
 * Affirmer's Copyright and Related Rights and associated claims and causes
 * of action, whether now known or unknown (including existing as well as
 * future claims and causes of action), in the Work (i) in all territories
 * worldwide, (ii) for the maximum duration provided by applicable law or
 * treaty (including future time extensions), (iii) in any current or future
 * medium and for any number of copies, and (iv) for any purpose whatsoever,
 * including without limitation commercial, advertising or promotional
 * purposes (the "Waiver"). Affirmer makes the Waiver for the benefit of each
 * member of the public at large and to the detriment of Affirmer's heirs and
 * successors, fully intending that such Waiver shall not be subject to
 * revocation, rescission, cancellation, termination, or any other legal or
 * equitable action to disrupt the quiet enjoyment of the Work by the public
 * as contemplated by Affirmer's express Statement of Purpose.
 *
 * 3. Public License Fallback. Should any part of the Waiver for any reason
 * be judged legally invalid or ineffective under applicable law, then the
 * Waiver shall be preserved to the maximum extent permitted taking into
 * account Affirmer's express Statement of Purpose. In addition, to the
 * extent the Waiver is so judged Affirmer hereby grants to each affected
 * person a royalty-free, non transferable, non sublicensable, non exclusive,
 * irrevocable and unconditional license to exercise Affirmer's Copyright and
 * Related Rights in the Work (i) in all territories worldwide, (ii) for the
 * maximum duration provided by applicable law or treaty (including future
 * time extensions), (iii) in any current or future medium and for any number
 * of copies, and (iv) for any purpose whatsoever, including without
 * limitation commercial, advertising or promotional purposes (the
 * "License"). The License shall be deemed effective as of the date CC0 was
 * applied by Affirmer to the Work. Should any part of the License for any
 * reason be judged legally invalid or ineffective under applicable law, such
 * partial invalidity or ineffectiveness shall not invalidate the remainder
 * of the License, and in such case Affirmer hereby affirms that he or she
 * will not (i) exercise any of his or her remaining Copyright and Related
 * Rights in the Work or (ii) assert any associated claims and causes of
 * action with respect to the Work, in either case contrary to Affirmer's
 * express Statement of Purpose.
 *
 * 4. Limitations and Disclaimers.
 *
 *  a. No trademark or patent rights held by Affirmer are waived, abandoned,
 *     surrendered, licensed or otherwise affected by this document.
 *  b. Affirmer offers the Work as-is and makes no representations or
 *     warranties of any kind concerning the Work, express, implied,
 *     statutory or otherwise, including without limitation warranties of
 *     title, merchantability, fitness for a particular purpose, non
 *     infringement, or the absence of latent or other defects, accuracy, or
 *     the present or absence of errors, whether or not discoverable, all to
 *     the greatest extent permissible under applicable law.
 *  c. Affirmer disclaims responsibility for clearing rights of other persons
 *     that may apply to the Work or any use thereof, including without
 *     limitation any person's Copyright and Related Rights in the Work.
 *     Further, Affirmer disclaims responsibility for obtaining any necessary
 *     consents, permissions or other rights required for any use of the
 *     Work.
 *  d. Affirmer understands and acknowledges that Creative Commons is not a
 *     party to this document and has no duty or obligation with respect to
 *     this CC0 or use of the Work.
 */

/**
 * \page license_mit MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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

/**
 * \defgroup catch_h gul14/catch.h
 * \brief The [Catch2 unit testing framework](https://github.com/catchorg/Catch2).
 * \see \ref unit_tests
 */

/**
 * \defgroup date_h gul14/date.h
 * \brief Calendar date library by Howard Hinnant.
 * \see https://github.com/HowardHinnant/date
 */

} // namespace gul14
