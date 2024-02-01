/**
 * \file    gul.h
 * \authors \ref contributors
 * \date    Created on August 24, 2018
 * \brief   Main include file for the General Utility Library.
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

#ifndef GUL14_GUL_H_
#define GUL14_GUL_H_

/**
 * \defgroup gul_h gul14/gul.h
 * \brief All-in-one include file for GUL14.
 * \details
 * This file includes almost all other headers provided by the library. Two specific files
 * are excluded, however:
 * <ul>
 * <li>`gul14/catch.h`: The header file for the Catch2 unit test framework</li>
 * <li>`gul14/date.h`: Howard Hinnant's header for calendar date functionality</li>
 * </ul>
 * While the unit test framework is simply not useful for inclusion into regular code,
 * `date.h` is excluded chiefly because of its negative impact on compile times. Just
 * include it directly where needed.
 */

#include "gul14/bit_manip.h"
#include "gul14/case_ascii.h"
#include "gul14/cat.h"
// #include "gul14/catch.h" not included because it is only useful for unit tests
// #include "gul14/date.h" not included by default to reduce compile times
#include "gul14/escape.h"
#include "gul14/expected.h"
#include "gul14/finalizer.h"
#include "gul14/gcd_lcm.h"
#include "gul14/hexdump.h"
#include "gul14/join_split.h"
#include "gul14/num_util.h"
#include "gul14/optional.h"
#include "gul14/replace.h"
#include "gul14/SlidingBuffer.h"
#include "gul14/SmallVector.h"
#include "gul14/span.h"
#include "gul14/statistics.h"
#include "gul14/string_util.h"
#include "gul14/string_view.h"
#include "gul14/substring_checks.h"
#include "gul14/ThreadPool.h"
#include "gul14/time_util.h"
#include "gul14/to_number.h"
#include "gul14/tokenize.h"
#include "gul14/traits.h"
#include "gul14/Trigger.h"
#include "gul14/trim.h"
#include "gul14/type_name.h"
#include "gul14/variant.h"
#include "gul14/version.h"

#endif
