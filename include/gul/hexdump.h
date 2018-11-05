/**
 * \file   hexdump.h
 * \author \ref contributors
 * \date   Created on September 25, 2018
 * \brief  Declarations of hexdump()
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

#pragma once

#include <iomanip>
#include <sstream>
#include <type_traits>
#include <utility>
#include "gul/string_view.h"

////// Overview of the prototypes contained in here, but without template specifications:
//
// std::string hexdump(IteratorT begin, IteratorT end, string_view prompt = "")
// std::string hexdump(const ElemT* buf, size_t len, string_view prompt = "")
// std::string hexdump(const ContainerT& cont, string_view prompt = "")
// std::string hexdump(StringT str, string_view prompt = "")
//
// struct HexdumpParameterForward
//
// HexdumpParameterForward<...> hexdump_stream(IteratorT begin, IteratorT end, std::string prompt = "")
// HexdumpParameterForward<...> hexdump_stream(const ElemT* buf, size_t buflen, std::string prompt = "")
// HexdumpParameterForward<...> hexdump_stream(const ContainerT& cont, std::string prompt = "")
// HexdumpParameterForward<...> hexdump_stream(ContainerT&& cont, std::string prompt = "")
//
// std::ostream& operator<< (std::ostream& os, const HexdumpParameterForward<...>& hdp)
//
//////

namespace gul {

namespace detail {

//////
//
// Until we have concepts ;-)
//

// Helper to identify types that have cbegin() and cend() and have integral data
template <typename T, typename = int>
struct IsHexDumpContainer : std::false_type { };

template <typename T>
struct IsHexDumpContainer <T,
    typename std::enable_if_t<
        std::is_integral<typename std::iterator_traits<decltype(std::declval<T>().cbegin())>::value_type>::value,
        decltype(std::declval<T>().cbegin(),
                 std::declval<T>().cend(),
                 0)
    >>
    : std::true_type { };

// Helper to identify types that are ForwardIterators or Pointers
//
// We enforce that the data pointed to is integer.
// We assume that every iterator that is not an input_iterator is at least
// a forward_iterator.
// We also check the existence of dereference and increment operators (this might be superfluous).
// (Type int is used as a dummy.)
template <typename T, typename = int>
struct IsHexDumpIterator : std::false_type { };

template <typename T>
struct IsHexDumpIterator <T,
    typename std::enable_if_t<
        std::is_integral<typename std::iterator_traits<T>::value_type>::value
        and not std::is_same<typename std::iterator_traits<T>::iterator_category,
                             std::input_iterator_tag>::value,
        decltype(std::declval<T>().operator*(),
                 std::declval<T>().operator++(),
                 0)
    >>
    : std::true_type { };

template <typename T>
struct IsHexDumpIterator <T,
    typename std::enable_if_t<
        std::is_pointer<T>::value
        and std::is_integral<typename std::remove_pointer<T>::type>::value,
        int
    >>
    : std::true_type { };

// Helper to identify stream types that we can use for output
template <typename StreamT,
    typename = std::enable_if_t<std::is_convertible<
        StreamT*,
        std::basic_ostream<typename StreamT::char_type, typename StreamT::traits_type>*>::value>>
struct IsHexDumpStream : std::true_type { };

// Here is the template actually doing the hexdump
// It is called by the different hexdump*() versions
template<typename StreamT, typename IteratorT,
    typename = std::enable_if_t<detail::IsHexDumpStream<StreamT>::value>,
    typename = std::enable_if_t<detail::IsHexDumpIterator<IteratorT>::value>>
StreamT& hexdump_stream(StreamT& dest, const IteratorT& begin, const IteratorT& end, string_view prompt = "")
{
    const auto maxelem = 1000ul * 16; // 1000 lines with 16 elements each
    // Get the number of hex digits to represent any value of a given integral type ElemT
    // Use an approximation of std::iterator_traits<IteratorT>::value_type that works with raw pointers also
    auto it = IteratorT{ begin };
    const auto nod = sizeof(*it) * 2;

    std::string indent(prompt.length(), ' ');
    std::string empty(nod + 1, ' ');
    dest << std::hex << std::setfill('0');

    // Inspired by epatel @ stack overflow
    // https://stackoverflow.com/a/29865
    size_t i, j;
    for (i = 0; (it != end and i < maxelem) or (i == 0); i += 16) {
        dest << (i ? indent : prompt) << std::setw(6) << i << ": ";
        auto line = it;
        for (j = 0; j < 16; ++j) {
            if (it != end) {
                const unsigned long long ch = static_cast<
                        typename std::make_unsigned<
                            typename std::decay<decltype(*it)>::type
                        >::type
                    >(*it++);
                dest << std::setw(nod) << ch << ' ';
            } else {
                if (nod != 2) {
                    break;
                }
                dest << empty;
            }
        }
        if (nod == 2) { // only char
            // Here we re-visit the iterator from the beginning of the line, thus
            // requiring ForwardIterators over InputOperators
            dest << ' ';
            for (j = 0; j < 16 and line != end; ++j, ++line) {
                    dest << (isprint(*line) ? *line : '.');
            }
        }
        dest << "\n";
    }
    if (it != end) {
        dest << indent << "[output truncated...]\n";
    }
    return dest;
}

} // namespace detail

//////
// Functions returning a string
//

/**
 * Generate a hexdump of a data range and return it as a string.
 *
 * The elements of the data range must be of integral type. They are dumped as unsigned
 * integer values with their native width: Chars as "00" to "ff", 16-bit integers as
 * "0000" to "ffff", and so on. If the elements are of type char, also a textual
 * representation of the printable characters is dumped. An optional prompt can be added
 * in front of the hexdump.
 *
 * \code
 * std::string x = "test\nthe Ä west!\t\r\n";
 * std::string str = gul::hexdump(x.begin(), x.end(), "debug -> ");
 * std::cerr << str;
 * \endcode
\verbatim
deBug -> 000000: 74 65 73 74 0a 74 68 65 20 c3 84 20 77 65 73 74  test.the .. west
         000010: 21 09 0d 0a                                      !...
\endverbatim
 *
 * \code
 * std::array<int, 8> ar = {{ 0, 1, 5, 2, -0x300fffff, 2, 5, 1999 }};
 * std::string str = gul::hexdump(begin(ar), end(ar));
 * std::cout << str;
 * \endcode
\verbatim
000000: 00000000 00000001 00000005 00000002 cff00001 00000002 00000005 000007cf
\endverbatim
 *
 * \param begin  ForwardIterator to the first data element to be dumped
 * \param end  ForwardIterator past the last data element to be dumped
 * \param prompt (optional) String that prefixes the dump text
 *
 * \returns a string containing the dump
 */
template<typename IteratorT,
    typename = std::enable_if_t<detail::IsHexDumpIterator<IteratorT>::value>>
std::string hexdump(IteratorT begin, IteratorT end, string_view prompt = "")
{
    std::stringstream o{ };
    return detail::hexdump_stream(o, begin, end, prompt).str();
}

/**
 * \overload
 *
 * \param buf  Pointer to the buffer to dump
 * \param len  Number of elements to dump
 * \param prompt (optional) String that prefixes the dump text
 *
 * \returns a string containing the dump
 */
template<typename ElemT,
    typename = std::enable_if_t<std::is_integral<ElemT>::value>>
std::string hexdump(const ElemT* buf, size_t len, string_view prompt = "")
{
    std::stringstream o{ };
    return detail::hexdump_stream(o, buf, buf + len, prompt).str();
}

/**
 * \overload
 *
 * \param cont  Reference to the container whose contents should be dumped; the container
 *              must provide ForwardIterators for .cbegin() and .cend()
 * \param prompt (optional) String that prefixes the dump text
 *
 * \returns a string containing the dump
 */
template<typename ContainerT,
    typename = std::enable_if_t<detail::IsHexDumpContainer<ContainerT>::value>>
std::string hexdump(const ContainerT& cont, string_view prompt = "")
{
    std::stringstream o{ };
    return detail::hexdump_stream(o, cont.cbegin(), cont.cend(), prompt).str();
}

//////
// Functions returning a forwarder object
// (Support for 'stream << hexdump' without intermediate data image)
//

/**
 * Helper object used to enable a convenient syntax to dump things to a stream.
 * \code
 * std::cerr << hexdump_stream(mydata.data(), mydata.size()) << "\n";
 * \endcode
 * It just forwards the parameters to hexdump_stream() to the appropriate operator<<.
 * Member cont holds the container if a temporary has been used for dumping.
 */
template<typename IteratorT, typename ContainerT = void*>
struct HexdumpParameterForward {
    IteratorT begin;
    IteratorT end;
    const std::string prompt;
    ContainerT cont;
};

/**
 * Generate a hexdump of a data range and write it to a stream.
 *
 * The elements of the data range must be of integral type. They are dumped as unsigned
 * integer values with their native width: Chars as "00" to "ff", 16-bit integers as
 * "0000" to "ffff", and so on. If the elements are of type char, also a textual
 * representation of the printable characters is dumped. An optional prompt can be added
 * in front of the hexdump.
 *
 * \code
 * std::string x = "test\nthe Ä west!\t\r\n";
 * std::cerr << gul::hexdump_stream(x.begin(), x.end(), "debug -> ");
 * \endcode
\verbatim
debug -> 000000: 74 65 73 74 0a 74 68 65 20 c3 84 20 77 65 73 74  test.the .. west
         000010: 21 09 0d 0a                                      !...
\endverbatim
 *
 * \code
 * std::array<int, 8> ar = {{ 0, 1, 5, 2, -0x300fffff, 2, 5, 1999 }};
 * std::cout << gul::hexdump_stream(begin(ar), end(ar));
 * \endcode
\verbatim
000000: 00000000 00000001 00000005 00000002 cff00001 00000002 00000005 000007cf
\endverbatim
 *
 * \param begin ForwardIterator to the first data to be dumped
 * \param end ForwardIterator past the last data element to be dumped
 * \param prompt (optional) String that prefixes the dump text
 *
 * \returns a helper object to be used with operator<< on streams
 */
template<typename IteratorT,
    typename = std::enable_if_t<detail::IsHexDumpIterator<IteratorT>::value>>
HexdumpParameterForward<const IteratorT>
hexdump_stream(const IteratorT& begin, const IteratorT& end, std::string prompt = "")
{
    return { begin, end, std::move(prompt), nullptr };
}

/**
 * \overload
 *
 * \param buf Pointer to the buffer to dump
 * \param len Number of elements in the buffer (i.e. number of elements to dump)
 * \param prompt (optional) String that prefixes the dump text
 *
 * \returns a helper object to be used with operator<< on streams
 */
template<typename ElemT,
    typename = std::enable_if_t<std::is_integral<ElemT>::value>>
HexdumpParameterForward<const ElemT* const>
hexdump_stream(const ElemT* buf, size_t len, std::string prompt = "")
{
    return { buf, buf + len, std::move(prompt), nullptr };
}

/**
 * \overload
 *
 * \param cont  Reference to the container to dump
 * \param prompt (optional) String that prefixes the dump text
 *
 * \returns a helper object to be used with operator<< on streams
 */
template<typename ContainerT,
    typename = std::enable_if_t<detail::IsHexDumpContainer<ContainerT>::value>>
HexdumpParameterForward<const decltype(std::declval<ContainerT>().cbegin())>
hexdump_stream(const ContainerT& cont, std::string prompt = "")
{
    return { cont.cbegin(), cont.cend(), std::move(prompt), nullptr };
}

/**
 * \overload
 *
 * \param cont  Reference to the container to dump if is a temporary
 * \param prompt (optional) String that prefixes the dump text
 *
 * \returns a helper object to be used with operator<< on streams
 */
template<typename ContainerT,
    typename = std::enable_if_t<detail::IsHexDumpContainer<ContainerT>::value,
    decltype(HexdumpParameterForward<decltype(std::declval<ContainerT>().cbegin()), ContainerT>{ }, 0)>>
HexdumpParameterForward<decltype(std::declval<ContainerT>().cbegin()), ContainerT>
hexdump_stream(ContainerT&& cont, std::string prompt = "")
{
    auto cheap_dummy = cont.cbegin();
    // The temporary container must be moved to retain the values until we need them after operator<<.
    auto ret = HexdumpParameterForward<decltype(std::declval<ContainerT>().cbegin()), ContainerT>{
        cheap_dummy,
        cheap_dummy,
        std::move(prompt),
        std::forward<ContainerT>(cont)};
    ret.begin = ret.cont.cbegin(); // (re)construct iterator after move
    ret.end = ret.cont.cend();
    return ret;
}

/**
 * Overload of std::ostream's operator<< to enable a convenient syntax to dump
 * things to a stream. Can also be used for ofstreams or stringstreams.
 * \code
 * std::cerr << hexdump_stream(mydata.data(), mydata.size()) << "\n";
 * \endcode
 * The stream is filled successively with the dumped data, no internal representation
 * of the dump is generated.
 */
template<typename IteratorT, typename ContainerT>
std::ostream& operator<< (std::ostream& os, const HexdumpParameterForward<IteratorT, ContainerT>& hdp) {
    return detail::hexdump_stream(os, hdp.begin, hdp.end, hdp.prompt);
}


} // namespace gul

// vi:ts=4:sw=4:et
