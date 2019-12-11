/**
 * \file    join_split.cc
 * \brief   Implementation of string utility functions for the General Utility Library.
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


#include <gul14/join_split.h>

using namespace std::literals::string_literals;

namespace gul14 {


namespace {

template<typename StringType>
std::string join(const std::vector<StringType>& parts, string_view glue) {
    using SizeType = typename StringType::size_type;

    if (parts.empty())
        return "";

    const SizeType last_idx = parts.size() - 1; // last valid index

    SizeType len = 0;
    for (const auto &part : parts)
        len += part.size();
    len += last_idx * glue.size();

    std::string result;
    result.reserve(len);

    for (SizeType i = 0; i < last_idx; ++i) {
        result += parts[i];
        result.append(glue.data(), glue.size());
    }
    result += parts[last_idx];
    return result;
}

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

std::string join(const std::vector<std::string>& parts, string_view glue) {
    return join<std::string>(parts, glue);
}

std::string join(const std::vector<string_view>& parts, string_view glue) {
    return join<string_view>(parts, glue);
}

} // namespace gul14

// vi:ts=4:sw=4:et
