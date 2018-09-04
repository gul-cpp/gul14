/**
 * \file   split_join.cc
 * \brief  Implementation of string utility functions for the General Utility Library.
 * \author Fini Jastrow
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


#include <gul/join_split.h>

using namespace std::literals::string_literals;

namespace gul {

std::vector<std::string> split(const std::string& text, const std::regex delimiter) {
    auto const end = std::sregex_iterator{ };
    auto result = std::vector<std::string>{ };

    auto parts = std::sregex_iterator(text.begin(), text.end(), delimiter);
    if (parts == end)
        return { text };

    auto previous = std::sregex_iterator{ };
    for (;parts != end; ++parts) {
        result.push_back(parts->prefix());
        previous = parts;
    }
    result.push_back(previous->suffix());
    return result;
}

std::vector<std::string> split(string_view text, string_view delimiter) {
    auto result = std::vector<std::string>{ };
    auto search_start = gul::string_view::size_type{ 0 };
    auto push_start = search_start;

    for (;;) {
        auto const hit = text.find(delimiter.data(), search_start, delimiter.size());
        if (hit == gul::string_view::npos)
            break;
        auto const hit_len = hit - push_start;
        result.emplace_back(text.substr(push_start, hit_len));
        search_start += std::max(delimiter.size() + hit_len, decltype(delimiter.size()){1});
        push_start += delimiter.size() + hit_len;
    }
    result.emplace_back(text.substr(push_start));
    return result;
}

std::string join(std::vector<std::string> parts, string_view glue) {
    auto result = ""s;
    auto const size = parts.size();
    if (size < 1)
        return result;
    auto const last = size - 1; // last valid index

    for (auto i = decltype(last){ 0 }; i < last; ++i) {
        result += parts[i];
        result.append(glue.data(), glue.size());
    }
    result += parts[last];
    return result;
}

} // namespace gul

/* vim:set noexpandtab softtabstop=4 tabstop=4 shiftwidth=4 textwidth=90 cindent: */
