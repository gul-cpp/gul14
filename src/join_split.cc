/**
 * \file    join_split.cc
 * \brief   Implementation of split() and split_sv().
 * \authors \ref contributors
 * \date    Created on 31 August 2018
 *
 * \copyright Copyright 2018-2020 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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


#include <gul14/join_split.h>

using namespace std::literals::string_literals;

namespace gul14 {


namespace {

template<typename StringType>
std::vector<StringType> split(gul14::string_view text, gul14::string_view delimiter) {
    using SizeType = typename StringType::size_type;

    auto result = std::vector<StringType>{ };
    auto search_start = SizeType{ 0 };
    auto push_start = search_start;

    for (;;) {
        const auto hit = text.find(delimiter.data(), search_start, delimiter.size());
        if (hit == StringType::npos)
            break;
        const auto hit_len = hit - push_start;
        result.emplace_back(text.substr(push_start, hit_len));
        search_start += std::max(delimiter.size() + hit_len, SizeType{1});
        push_start += delimiter.size() + hit_len;
    }
    result.emplace_back(text.substr(push_start));
    return result;
}

} // anonymous namespace


std::vector<std::string> split(const std::string& text, const std::regex& delimiter) {
    auto const end = std::sregex_iterator{ };
    auto result = std::vector<std::string>{ };

    auto parts = std::sregex_iterator(text.begin(), text.end(), delimiter);
    if (parts == end)
        return { text };

    auto previous = std::sregex_iterator{ };
    for (;parts != end; ++parts) {
        if (parts == previous and not parts->length())
            break;
        result.push_back(parts->prefix());
        previous = parts;
    }
    result.push_back(previous->suffix());
    return result;
}

std::vector<std::string> split(string_view text, string_view delimiter) {
    return split<std::string>(text, delimiter);
}

std::vector<string_view> split_sv(string_view text, string_view delimiter) {
    return split<string_view>(text, delimiter);
}

} // namespace gul14

// vi:ts=4:sw=4:et
