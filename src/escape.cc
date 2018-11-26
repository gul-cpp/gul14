/**
 * \file    escape.cc
 * \brief   Implementation of escape(), unescape().
 * \authors \ref contributors
 * \date    Created on 31 August 2018
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


#include "gul/escape.h"
#include <array>
#include <iomanip>
#include <regex>
#include <stdexcept>
#include <sstream>

using namespace std::literals::string_literals;

namespace gul {

// anonymous namespace to confine helper functions to this translation unit
namespace {

static const std::array<char, 16> hex_table =
    { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };

char get_last_nibble_as_hex(unsigned int i)
{
    return hex_table[i & 0xf];
}

} // anonymous namespace


std::string escape(const std::string& in)
{
    auto escaped = ""s;
    escaped.reserve(in.length());

    static_assert(static_cast<signed char>(128) == -128,
                  "Signed char > 127 does not wrap to negative values as expected");

    for (const signed char c : in)
    {
        switch (c)
        {
            case '"':
                escaped += "\\\"";
                break;
            case '\\':
                escaped += "\\\\";
                break;
            case '\n':
                escaped += "\\n";
                break;
            case '\r':
                escaped += "\\r";
                break;
            case '\t':
                escaped += "\\t";
                break;
            default:
                if (c < 32)
                {
                    // This applies also to all non-ASCII characters (>127) because they
                    // are mapped to negative values
                    escaped += "\\x";
                    escaped += get_last_nibble_as_hex(c >> 4);
                    escaped += get_last_nibble_as_hex(c);
                }
                else
                {
                    escaped += c;
                }
        };
    }

    return escaped;
}

std::string unescape(const std::string& in)
{
    auto re = std::regex{ R"(\\(["\\nrt]|x[[:xdigit:]]{2}))" };
    auto rit  = std::regex_iterator<std::string::const_iterator>{ in.cbegin(), in.cend(), re };
    auto rend = std::regex_iterator<std::string::const_iterator>{ };
    auto last = rend; // last processed

    auto buf = std::stringstream{ };

    for (; rit != rend; ++rit) {
        last = rit;
        buf << rit->prefix();
        if (rit->empty())
            continue;
        auto const c = rit->format("$1");
        switch (c[0]) {
        case '\"':
        case '\\':
            buf << c;
            break;
        case 'n':
            buf << "\n";
            break;
        case 'r':
            buf << "\r";
            break;
        case 't':
            buf << "\t";
            break;
        case 'x':
            buf << static_cast<unsigned char>(std::stoi(rit->format("$1").substr(1), 0, 16));
            break;
        default:
            buf << rit->str();
            break;
        }
    }

    if (last == rend) // no match
        buf << in;
    else
        buf << last->suffix();

    return buf.str();
}

} // namespace gul

/* vim:set noexpandtab softtabstop=4 tabstop=4 shiftwidth=4 textwidth=90 cindent: */
