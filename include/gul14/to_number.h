/**
 * \file    to_number.h
 * \brief   Declaration of to_number().
 * \authors \ref contributors
 * \date    Created on 19 July 2019
 *
 * \copyright Copyright 2019-2020 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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

#ifndef GUL14_TO_NUMBER_H_
#define GUL14_TO_NUMBER_H_

#include <array>
#include <cstdlib>
#include <cmath>
#include <exception>
#include <limits>
#include <type_traits>

#include "gul14/internal.h"
#include "gul14/optional.h"
#include "gul14/string_view.h"
#include "gul14/substring_checks.h"

namespace gul14 {

/// \cond HIDE_SYMBOLS
namespace detail {

constexpr inline bool is_digit(char c) noexcept
{
    return c >= '0' && c <= '9';
}

constexpr inline bool is_nan_specifier(char c) noexcept
{
    if (c >= '0' && c <= '9')
        return true;
    if (c >= 'a' && c <= 'z')
        return true;
    if (c >= 'A' && c <= 'Z')
        return true;
    if (c == '_')
        return true;
    return false;
}

template <typename NumberType, bool count_magnitude = false>
constexpr inline optional<NumberType> to_unsigned_integer(gul14::string_view str,
        NumberType* magnitude = nullptr) noexcept
{
#ifndef __GNUC__
    constexpr NumberType max_tenth = std::numeric_limits<NumberType>::max() / 10;
#endif

    if (str.empty())
        return nullopt;

    NumberType result = 0;

    for (char c : str)
    {
        if (!is_digit(c))
            return nullopt;

#ifdef __GNUC__
        if (__builtin_mul_overflow(result, NumberType{ 10 }, &result)) // NOLINT(cppcoreguidelines-pro-type-vararg)
            return nullopt;

        if (__builtin_add_overflow(result, static_cast<NumberType>(c - '0'), &result)) // NOLINT(cppcoreguidelines-pro-type-vararg)
            return nullopt;
#else
        if (result > max_tenth)
            return nullopt;

        result *= 10;

        auto last = result;

        result += c - '0';
        if (result < last)
            return nullopt;
#endif
        if /*constexpr*/ (count_magnitude)
            *magnitude *= NumberType{ 10 };
    }

    return result;
}

/* Parse a signed exponent specifier.
 * May start with a leading sign ('+' or '-'). The exponent value is limited to
 * the range of int. The used range with a long double conversion is usually in
 * the range -5000 to 5000, so this is not really a limitation.
 */
constexpr optional<int> parse_exponent(string_view str) noexcept
{
    bool negative = false;

    switch (str.front())
    {
    case '+':
        str.remove_prefix(1);
        break;
    case '-':
        str.remove_prefix(1);
        negative = true;
        break;
    default:
        break;
    }

    auto opt_exp = to_unsigned_integer<int>(str);

    if (!opt_exp)
        return nullopt;

    if (negative)
        return -*opt_exp;
    return *opt_exp;
}

// For some 'long double' types with a big mantissa uint64 is not large enough.
// We resort to __uint128, which is a non standard extension in GCC and clang.
// But only if we need to.
// Note that on some compilers there are no std::numeric_limits<> for the extension
// type, and then asserts later on will fail. But usually that compilers have small
// long double types.
template <typename NumberType>
using FloatConversionIntType =
    typename std::conditional<
        (std::numeric_limits<uint64_t>::digits10 >= std::numeric_limits<NumberType>::digits10),
        uint64_t,
        #ifdef __SIZEOF_INT128__ // GCC, clang, intel
            __uint128_t
        #else
            uint64_t
        #endif
    >::type;

/**
 * Convert two string_views into one floating point number.
 * The two strings are used as if they are seamlessly appended to each other.
 * The resulting character sequence is then parsed to a floating point value
 * of type NumberType under the assumption that a decimal dot exists after the
 * first digit (thus normalised form).
 * The first digit may not be a sign.
 * The first digit may not be the digit zero (unless the total value is zero).
 *
 * Example: "123" "456" -> 1.23456
 *
 * \tparam NumberType Destination numeric type.
 * \param str  The signless string to be converted.
 *
 * \returns a ParseInfNanResult that contains a gul14::optional with the number
 */
template <typename NumberType>
constexpr inline gul14::optional<NumberType> to_normalized_float(gul14::string_view i1, gul14::string_view i2) noexcept
{
    static_assert(std::numeric_limits<FloatConversionIntType<NumberType>>::digits10
            >= std::numeric_limits<NumberType>::digits10,
            "FloatConversionIntType is too small for NumberType");

    i1 = i1.substr(0, std::min(i1.length(),
                size_t(std::numeric_limits<FloatConversionIntType<NumberType>>::digits10)));
    i2 = i2.substr(0, std::min(i2.length(),
                size_t(std::numeric_limits<FloatConversionIntType<NumberType>>::digits10) - i1.length()));

    FloatConversionIntType<NumberType> accu{ 0 };

    auto magnitude = FloatConversionIntType<NumberType>{ 1 };

    if (not i2.empty()) {
        auto f2 = to_unsigned_integer<FloatConversionIntType<NumberType>, true>(i2, &magnitude);
        if (not f2.has_value())
            return nullopt;
        accu = *f2;
    }
    if (not i1.empty()) {
        auto i2_magnitude = magnitude;
        auto f1 = to_unsigned_integer<FloatConversionIntType<NumberType>, true>(i1, &magnitude);
        if (not f1.has_value())
            return nullopt;
        accu += (*f1 * i2_magnitude);
    }

    return static_cast<NumberType>(accu) / static_cast<NumberType>(magnitude / 10); // NOLINT(bugprone-integer-division): Precision loss is not possible with normalized accu
}

template <typename NumberType>
struct ParseInfNanResult {
    bool result_valid;
    optional<NumberType> result;
};

/**
 * Convert a string_view into a floating point number.
 * This function just parses NAN and INF specifying strings.
 *
 * \tparam NumberType Destination numeric type.
 * \param str  The signless string to be converted.
 *
 * \returns a ParseInfNanResult that contains a gul14::optional with the number
 *          if the conversion was successful. If there was a conversion error,
 *          the returned optional is empty. If nothing special has been found
 *          the result_valid member is false.
 *
 * \note
 * To be specific, the return value is one of
 * - NAN
 * - INF
 * - nullopt (on parse error)
 * - !result_valid (on nothing-to-do, i.e possibly number found)
 */
template <typename NumberType>
constexpr inline ParseInfNanResult<NumberType> parse_inf_nan(gul14::string_view str) noexcept
{
    auto const strlength = str.length();
    if (strlength == 0)
        return { true, {} };

    if (gul14::starts_with_nocase(str, "inf")) {
        if (strlength == 3 /* strlen("inf") */ )
            return { true, make_optional(std::numeric_limits<NumberType>::infinity()) };
        if (strlength == 8 /* strlen("infinity") */
                and gul14::starts_with_nocase(str.substr(3), "inity"))
            return { true, make_optional(std::numeric_limits<NumberType>::infinity()) };
        return { true, {} };
    }

    if (gul14::starts_with_nocase(str, "nan")) {
        if (strlength == 3 /* strlen("nan") */ )
            return { true, make_optional(std::numeric_limits<NumberType>::quiet_NaN()) };
        if (strlength < 5 /* strlen("nan()") */ or str[3] != '(' or str.back() != ')')
            return { true, {} };
        str.remove_prefix(4);
        str.remove_suffix(1);
        while (str.length()) {
            if (not is_nan_specifier(str.front()))
                return { true, {} };
            str.remove_prefix(1);
        }
        // We do not use the NaN specifier
        return { true, make_optional(std::numeric_limits<NumberType>::quiet_NaN()) };
    }
    return { false, {} };
}

/**
 * Convert a string_view into a floating point number.
 * No sign is allowed at the beginning of the number (thus unsigned float).
 *
 * The mantissa and a possibly existing exponent are independently converted to
 * an integer numeric value and combined to a floating point value. The
 * mantissa is normalized in string form prior to conversion and the exponent
 * is shifted accordingly.
 *
 * Only the C locale is used, i.e. the decimal dot is '.'.
 *
 * \tparam NumberType Destination numeric type.
 * \param str  The signless string to be converted.
 *
 * \returns a gul14::optional that contains the number if the conversion was successful. If
 *          there was a conversion error, the return value is empty.
 */
template <typename NumberType>
constexpr inline optional<NumberType> to_unsigned_float(gul14::string_view str) noexcept
{
    auto inf_nan = parse_inf_nan<NumberType>(str);
    if (inf_nan.result_valid)
        return inf_nan.result;

    int exponent = 0;
    auto e_pos = str.find_first_of("eE");
    if (e_pos != gul14::string_view::npos)
    {
        if (e_pos + 1 == str.size())
            return nullopt;

        auto str_exponent = str.substr(e_pos + 1);

        str = str.substr(0, e_pos);

        auto opt_exp = detail::parse_exponent(str_exponent);

        if (!opt_exp)
            return nullopt;

        exponent = *opt_exp;
    }

    gul14::string_view str_before_point{ str };
    gul14::string_view str_after_point;

    auto point_pos = str.find('.');
    if (point_pos != gul14::string_view::npos)
    {
        str_before_point = str.substr(0, point_pos);
        str_after_point = str.substr(point_pos + 1);
    }

    if (str_before_point.empty() && str_after_point.empty())
        return nullopt;

    // Get rid of leading zeros
    while (!str_before_point.empty() and str_before_point[0] == '0')
        str_before_point.remove_prefix(1);

    // Normalize the number
    if (str_before_point.empty()) {
        auto const old_digits = str_after_point.length();
        while (!str_after_point.empty() and str_after_point[0] == '0')
            str_after_point.remove_prefix(1);

        if (str_after_point.empty())
            return { 0 };

        str_before_point = str_after_point.substr(0, 1);
        str_after_point.remove_prefix(1);
        exponent -= static_cast<int>(old_digits - str_after_point.length());
    } else {
        exponent += static_cast<int>(str_before_point.length() - 1);
    }

    // Now the incoming number string is like this:
    // "s.tr_before_point" "str_after_point" E exponent
    //   ^                                           ^
    //   | here is the decimal dot, virtually        | corrected exponent

    using long_double = long double;
    using CalcType = std::conditional_t<
        std::greater<>()(sizeof(NumberType), sizeof(double)),
        long_double, double>;

    auto norm_val = to_normalized_float<CalcType>(str_before_point, str_after_point);
    if (not norm_val.has_value())
        return nullopt;

    return NumberType(std::pow(CalcType(10), CalcType(exponent)) * *norm_val);
}

/**
 * Convert a string_view into a floating point number using std::strtold.
 * This function parses the ASCII representation of a number (e.g. "123" or "1.3e10") into
 * a (floating point) number using the long double algorithm of the standard library. For
 * this, an intermediate string is created.
 *
 * \tparam NumberType Destination numeric type, usually long double.
 * \param str  The string to be converted into a number.
 *
 * \returns an optional that contains std::strtold()'s return value (include HUGE_VAL).
 * Parsing failure (including number end before string end) is returned by std::nullopt.
 *
 * \note This function is used as fallback solution if we can not use gul14::to_unsigned_float()
 * for the desired conversions. This can have two reasons:
 * - The internally used integer type is too small compared to the floating point type.
 * - On Apple clang 8.0.0 std::pow<long double>() is inaccurate.
 */
template <typename NumberType>
inline optional<NumberType> strtold_wrapper(gul14::string_view str) noexcept
{
    if (str.empty())
        return nullopt;

    try
    {
        auto input = std::string{ str };
        char* process_end;
        auto value = static_cast<NumberType>(std::strtold(input.c_str(), &process_end));

        if (input.data() + input.size() != process_end) // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic): Pointer arithmetic needed because strtold gives pointer back
            return nullopt;
        return value;
    }
    catch (const std::exception &)
    {
        return nullopt;
    }
}

} // namespace detail
/// \endcond


/**
 * Convert an ASCII string_view into a number.
 * This function parses the ASCII representation of a number (e.g. "123" or "1.3e10") into
 * an optional integer or floating-point number.
 *
 * \code
 * gul14::optional<int> result = gul14::to_number<int>("42");
 *
 * if (result.has_value())
 *     std::cout << "The answer is " << result.value() << ".\n";
 *
 * if (result)
 *     std::cout << "The answer is " << *result << ".\n";
 * \endcode
 *
 * <h4>Design Goals</h4>
 *
 * to_number() shares many characteristics with std::atoi(), std::stod() and the like, but
 * follows its own set of design goals:
 * - Its input type is string_view: No null-termination and no temporary std::string are
 *   required for the conversion.
 * - Conversion errors are reported by returning a gul14::optional without a value.
 * - The function does not allocate.
 * - Performance should in most cases be better than and in few cases not much worse than
 *   std::sto[ildf].
 * - to_number<>() can be used in a constexpr context *if* the compiler and standard
 *   library support this. Support for floating-point numbers may lack even if integer
 *   conversion works.
 *
 * <h4>Input Format</h4>
 *
 * The allowed number format depends on the chosen numeric output type.
 * <h5>Unsigned integral types</h5>
 * * Accept only digits ("123", "042"=42).
 *
 * <h5>Signed integral types</h5>
 * * Allow additionally a leading minus sign as well ("-42"). No leading plus sign is allowed, though.
 *
 * <h5>Floating-point types</h5>
 * Recognize additionally
 * * A decimal point ("1.2", ".5", "12.") and exponential
 *   notation using a small or capital "e" ("12e5", "4.2e1", ".2e-4", "2.E5").
 * * Infinity expressions: (optional minus sign) INF or INFINITY ignoring case.
 * * Not-a-number expressions: (optional minus sign) NAN or NAN(char_sequence) ignoring case.
     The char_sequence can only contain digits, Latin letters, and underscores.
     The result is a quiet NaN floating-point value.
 *
 * The behavior with surrounding whitespace is *undefined*, so
 * it should be removed before passing input to this function.
 * This means to_number() accepts a subset of C++17's from_chars() input format; where it
 * supports the input format it is modeled close to from_chars().
 *
 * \tparam NumberType Destination numeric type
 * \param str  The string to be converted into a number.
 * \returns a gul14::optional that contains the number if the conversion was successful. If
 *          there was a conversion error, the return value is empty.
 *          If the input describes a number whose parsed value is not in the range representable
 *          by \b NumberType, the return value is empty.
 *
 * \note
 * This function has different overloads for unsigned integers, signed integers, and
 * floating-point types.
 * \note
 * The floating-point overload allocates an intermediate string if
 * - the intermediate integer type is too small in comparison to NumberType or
 * - this function is used with long double on Apple Clang.
 *
 * \since GUL version 1.6
 * \since GUL version 1.7 the NAN and INF floating point conversion
 */
// Overload for unsigned integer types.
template <typename NumberType,
          std::enable_if_t<std::is_integral<NumberType>::value &&
                           std::is_unsigned<NumberType>::value, int> = 0>
constexpr inline optional<NumberType> to_number(gul14::string_view str) noexcept
{
    return detail::to_unsigned_integer<NumberType>(str);
}

// Overload for signed integer types.
template <typename NumberType,
          std::enable_if_t<std::is_integral<NumberType>::value &&
                           std::is_signed<NumberType>::value, int> = 0>
constexpr inline optional<NumberType> to_number(gul14::string_view str) noexcept
{
    if (str.empty())
        return nullopt;

    if (str.front() == '-')
    {
        using UnsignedT = std::make_unsigned_t<NumberType>;
        constexpr auto max_abs_negative_value =
            static_cast<UnsignedT>(std::numeric_limits<NumberType>::max()) + 1;

        str.remove_prefix(1);

        auto result = detail::to_unsigned_integer<UnsignedT>(str);
        if (!result)
            return nullopt;

        if (*result == max_abs_negative_value)
            return std::numeric_limits<NumberType>::lowest();
        else if (*result > max_abs_negative_value)
            return nullopt;

        return static_cast<NumberType>(-static_cast<NumberType>(*result));
    }

    return detail::to_unsigned_integer<NumberType>(str);
}

// Overload for floating-point types.
template <typename NumberType,
    std::enable_if_t<std::is_floating_point<NumberType>::value, int> = 0>
constexpr inline optional<NumberType> to_number(gul14::string_view str) noexcept
{
    if (str.empty())
        return nullopt;

    if (
#ifdef __APPLE__
        // Apple clang 8.0.0 has a bug with std::pow and long double types,
        // where the result is off by a huge amount. Use std::strtold() here.
        (sizeof(NumberType) > sizeof(double)) ||
#endif
#ifdef _MSC_VER
#    pragma warning( push )
#    pragma warning( disable: 4127 ) // conditional expression is constant
#endif
            (std::numeric_limits<detail::FloatConversionIntType<NumberType>>::digits10
            <= std::numeric_limits<NumberType>::digits10)) {
#ifdef _MSC_VER
#    pragma warning( pop )
#endif
        // Too big for our approach. Resort to non-constexpr functionality.
        // This actually never happenes with the currently supported platforms / compilers.
        // (Except long double on Darwin)
        return detail::strtold_wrapper<NumberType>(str);
    }

    if (str.front() == '-')
    {
        str.remove_prefix(1);
        auto result = detail::to_unsigned_float<NumberType>(str);
        if (!result)
            return nullopt;
        return -*result;
    }

    return detail::to_unsigned_float<NumberType>(str);
}

} // namespace gul14

#endif

// vi:ts=4:sw=4:et:sts=4
