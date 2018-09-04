/**
 * \file   string_util.cc
 * \brief  Implementation of string utility functions for the General Utility Library.
 * \author Soeren Grunewald
 * \date   Created on 31 August 2018
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

std::string escape(const std::string& in)
{
    auto re = std::regex{ R"([^[:print:]]|\\|\")" };
    auto rit  = std::regex_iterator<std::string::const_iterator>{ in.cbegin(), in.cend(), re };
    auto rend = std::regex_iterator<std::string::const_iterator>{ };
    auto last = rend; // last processed

    auto buf = std::stringstream{ };

    for (; rit != rend; ++rit) {
        last = rit;
        buf << rit->prefix();
        if (rit->empty())
            continue;
        const auto c = rit->str()[0];
        switch (c) {
        case '\"':
            buf << "\\\"";
            break;
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
            buf << "\\x" << std::hex << std::setfill('0')
				<< std::setw(2) << static_cast<unsigned int>(c);
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
        throw std::invalid_argument("needle is empty");

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
