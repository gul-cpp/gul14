/**
 * \file   string_util.cc
 * \brief  Implementation of string utility functions
 *
 * \copyright Copyright 2015-2018 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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
#include <sstream>

namespace gul {

std::string escape(const std::string& in)
{
    auto re = std::regex{ "[^[:print:]]|\\\\" };
    auto rit  = std::regex_iterator<std::string::const_iterator>{ in.cbegin(), in.cend(), re };
    auto rend = std::regex_iterator<std::string::const_iterator>{ };
    auto last = rend; // last processed

    auto buf = std::stringstream{ };

    buf << "\"";

    for (; rit != rend; ++rit) {
        last = rit;
        buf << rit->prefix();
        if (rit->empty())
            continue;
        const auto c = rit->str()[0];
        switch (c) {
        case '\\':
            buf << "\\\\";
            break;
        case '\n':
            buf << "\\n";
            break;
        case '\r':
            buf << "\\r";
            break;
        case '\t':
            buf << "\\t";
            break;
        default:
            buf << "\\x" << std::hex << std::setfill('0') << std::setw(2) << c;
            break;
        }
    }

    if (last == rend) // no match
        buf << in;
    else
        buf << last->suffix();

    buf << "\"";

    return buf.str();
}

} // namespace gul

/* vim:set noexpandtab softtabstop=4 tabstop=4 shiftwidth=4 textwidth=90 cindent: */
