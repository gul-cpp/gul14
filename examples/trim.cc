/**
 * \file   trim.cc
 * \author \ref contributors
 * \date   Created on November 22, 2018
 * \brief  Example for the trim() family of functions from the General Utility Library.
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

#include <iostream>
#include <gul14/gul.h>

using std::cout;
using gul14::trim;
using gul14::trim_left;
using gul14::trim_right;
using gul14::trim_left_sv;
using gul14::trim_right_sv;

int main()
{
//! [Using trim() with default and custom whitespace]
    cout << "[" << trim("\n \b trim(), default whitespace\t \r") << "]\n";
    // prints "[trim(), default whitespace]"

    cout << "[" << trim(".:.:.:trim(), custom whitespace.:.:.:.", ".:") << "]\n";
    // prints "[trim(), custom whitespace]"
//! [Using trim() with default and custom whitespace]

//! [Using trim_left() and trim_right() with default and custom whitespace]
    cout << "[" << trim_left("\n \b trim_left(), default whitespace   ") << "]\n";
    // prints "[trim_left(), default whitespace   ]"

    cout << "[" << trim_right(".:.:.:trim_right(), custom whitespace.:.:.:.", ".:") << "]\n";
    // prints "[.:.:.:trim_right, custom whitespace]"
//! [Using trim_left() and trim_right() with default and custom whitespace]

//! [Using trim_left_sv() and trim_right_sv() with default and custom whitespace]
    cout << "[" << trim_left_sv("\n \b trim_left_sv(), default whitespace   ") << "]\n";
    // prints "[trim_left_sv(), default whitespace   ]"

    cout << "[" << trim_right_sv(".:.:.:trim_right_sv(), custom whitespace.:.:.:.", ".:") << "]\n";
    // prints "[.:.:.:trim_right_sv, custom whitespace]"
//! [Using trim_left_sv() and trim_right_sv() with default and custom whitespace]

//! [Using trim_sv() on a mutable string]
    std::string str = "  string_view  ";
    auto sv = trim_sv(str); // sv is a string_view that points to the original string

    cout << "[" << sv << "]\n";
    // prints "[string_view]"

    str[5] = 'o'; // modify the original string

    cout << "[" << sv << "]\n";
    // prints "[strong_view]"
//! [Using trim_sv() on a mutable string]

    return 0;
}
