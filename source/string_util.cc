/**
 * \file    string_util.cc
 * \brief   Implementation of string utility functions for the General Utility Library.
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


#include <gul/string_util.h>

#include <iomanip>
#include <regex>
#include <stdexcept>
#include <sstream>

namespace gul {

// anonymous namespace to confine helper functions to this translation unit
namespace {

static std::array<char, 16> hex_table =
    { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };

char get_last_nibble_as_hex(unsigned int i)
{
    return hex_table[i & 0xf];
}

} // anonymous namespace


std::string escape(const std::string& in)
{
    std::string escaped;

    escaped.reserve(in.length());

    for (char c : in)
    {
        unsigned int u = c;

        switch (u)
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
                if (u < 32 || u > 127) // TODO: discuss if we want to escape all non-ASCII characters
                {
                    escaped += "\\x";
                    escaped += get_last_nibble_as_hex(u >> 4);
                    escaped += get_last_nibble_as_hex(u);
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
        const auto c = rit->format("$1");
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

std::string& replace_inplace(std::string& haystack, string_view needle, string_view hammer)
{
    if (needle.empty())
        return haystack;

    auto pos = haystack.find(needle.data(), 0, needle.length());
    while (pos != std::string::npos) {
        haystack.erase(pos, needle.length());
        haystack.insert(pos, hammer.data(), hammer.length());
        pos = haystack.find(needle.data(), pos + hammer.length(), needle.length());
    }
    return haystack;
}

std::string replace(string_view haystack, string_view needle, string_view hammer)
{
    if (needle.empty())
        return std::string(haystack);

    std::string result;

    result.reserve(haystack.length());

    std::size_t pos = 0;
    std::size_t last_pos = 0;
    while ((pos = haystack.find(needle.data(), pos, needle.length())) != std::string::npos)
    {
        result.append(haystack.data() + last_pos, pos - last_pos);
        result.append(hammer.data(), hammer.length());
        pos += needle.length();
        last_pos = pos;
    }

    result.append(haystack.data() + last_pos, haystack.length() - last_pos);
    result.shrink_to_fit();

    return result;
}

} // namespace gul

/* vim:set noexpandtab softtabstop=4 tabstop=4 shiftwidth=4 textwidth=90 cindent: */
