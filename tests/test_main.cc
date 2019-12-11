/**
 * \file   test_main.cc
 * \author \ref contributors
 * \date   Created on August 28, 2018
 * \brief  Test suite for the General Utility Library.
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

#define CATCH_CONFIG_RUNNER
#include "gul14/catch.h"

#ifdef _MSC_VER
#   include <windows.h>
#endif

int main(int argc, char* argv[])
{
#ifdef _MSC_VER
    // On windows we often start with IDLE_PRIORITY_CLASS:
    //   Process whose threads run only when the system is idle.
    // That priority is used for batch scripts for example.
    //
    // This is of course not acceptable when we want to test some timing.
    SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
#endif
    return Catch::Session().run(argc, argv);
}

// vi:ts=4:sw=4:sts=4:et
