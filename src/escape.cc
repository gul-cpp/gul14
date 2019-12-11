/**
 * \file    escape.cc
 * \brief   Implementation of escape(), unescape().
 * \authors \ref contributors
 * \date    Created on 31 August 2018
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


#include "gul14/escape.h"
#include <array>
#include <iomanip>
#include <limits>
#include <regex>
#include <stdexcept>
#include <sstream>

using namespace std::literals::string_literals;

namespace gul14 {

// anonymous namespace to confine helper functions to this translation unit
namespace {

static const std::array<char, 16> hex_table =
    {{ '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' }};

char get_last_nibble_as_hex(unsigned int i)
{
    return hex_table[i & 0xf];
}

} // anonymous namespace


std::string escape(string_view in)
{
    static_assert(std::numeric_limits<signed char>::min() == -128,
                  "Unsupported char type");
    static_assert(std::numeric_limits<signed char>::max() == 127,
                  "Unsupported char type");

    auto escaped = ""s;
    escaped.reserve(in.length());

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
        }
    }

    return escaped;
}

std::string unescape(string_view in)
{
    auto const static re = std::regex{ R"(\\(["\\nrt]|x[[:xdigit:]]{2}))" };
    auto rit  = std::regex_iterator<string_view::const_iterator>{ in.cbegin(), in.cend(), re };
    auto const rend = std::regex_iterator<string_view::const_iterator>{ };
    auto last = decltype(*rit){ };

    if (rit == rend)
        return std::string{ in };

    auto unescaped = ""s;
    unescaped.reserve(in.length());

    for (; rit != rend; ++rit) {
        last = *rit;
        unescaped += rit->prefix();
        if (rit->empty())
            continue;
        auto const matched_escape_sequence = rit->str(1);
        switch (matched_escape_sequence[0]) {
        case '\"':
        case '\\':
            unescaped += matched_escape_sequence;
            break;
        case 'n':
            unescaped += "\n";
            break;
        case 'r':
            unescaped += "\r";
            break;
        case 't':
            unescaped += "\t";
            break;
        case 'x':
            unescaped += static_cast<char>(std::stoi(matched_escape_sequence.substr(1), 0, 16));
            break;
        default:
            unescaped += rit->str();
            break;
        }
    }

    unescaped += last.suffix();

    return unescaped;
}

} // namespace gul14

// vi:ts=4:sw=4:sts=4:et
