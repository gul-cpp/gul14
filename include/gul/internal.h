/**
 * \file    internal.h
 * \brief   Definition of macros used internally by GUL.
 * \authors \ref contributors
 * \date    Created on 7 Aug 2019
 *
 * \copyright Copyright 2019 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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

#ifdef _MSC_VER

// For old Visual C++ compilers, including <ciso646> enables the alternative operator
// representations "and", "or", and "not".
#include <ciso646>

#ifdef GUL_COMPILING_SHARED_LIB
#define GUL_EXPORT __declspec(dllexport)
#elif defined(GUL_USING_STATIC_LIB_OR_OBJECTS)
#define GUL_EXPORT 
#else
#define GUL_EXPORT __declspec(dllimport)
#endif

#elif defined(__GNUC__)
#define GUL_EXPORT __attribute__ ((visibility ("default")))

#else
#define GUL_EXPORT

#endif
