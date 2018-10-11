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

////// Overview if the prototypes contained in here, but without template specifications:
//
// std::string hexdump(const IteratorT begin, const IteratorT end, const std::string& prompt = "")
// std::string hexdump(const ElemT* const buf, const size_t buflen, const std::string& prompt = "")
// std::string hexdump(const ContainerT& cont, const std::string& prompt = "")
// std::string hexdump(StringT str, const std::string& prompt = "")
//
// StreamT& hexdump_stream(StreamT& dest, IteratorT it, const IteratorT end, const std::string& prompt = "")
// StreamT& hexdump_stream(StreamT& dest, const ElemT* const buf, const size_t buflen, const std::string& prompt = "")
//
// struct HexdumpParameterForward
//
// HexdumpParameterForward<...> hexdump_stream(IteratorT begin, IteratorT end, const std::string& prompt = "")
// HexdumpParameterForward<...> hexdump_stream(const ElemT* const buf, const size_t buflen, const std::string& prompt = "")
// HexdumpParameterForward<...> hexdump_stream(const ContainerT& cont, const std::string& prompt = "")
//
// std::ostream& operator<< (std::ostream& os, const HexdumpParameterForward<ElemT>& hdp)
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

} // namespace detail

//////
// Functions returning a stream
//

/**
 * Generate a hexdump of a structure/buffer on a stream.
 * The elements of the buffer are dumped with their native width. I.e. when the elements are
 * 16 bit wide the numbers are dumped as 16bit integers.
 * Elements have to be an integral type.
 * If the elements are of type char, also a textual representation of the printable
 * characters is dumped.
 * The destination steam must be convertible to std::basic_ostream.
 *
 * \param dest The stream to use for dumping
 * \param it ForwardIterator to the first data to be dumped
 * \param end ForwardIterator past the last data element to be dumped
 * \param prompt (optional) String that prefixes the dump text
 *
 * \returns Ref to the stream used to dump
 */
template<typename StreamT, typename IteratorT,
    typename = std::enable_if_t<detail::IsHexDumpStream<StreamT>::value>,
    typename = std::enable_if_t<detail::IsHexDumpIterator<IteratorT>::value>>
StreamT& hexdump_stream(StreamT& dest, IteratorT it, const IteratorT end, const std::string& prompt = "")
{
    const auto maxelem = 1000ul * 16; // 1000 lines with 16 elements each
    // Get the number of hex digits to represent any value of a given integral type ElemT
    // Use an approximation of std::iterator_traits<IteratorT>::value_type that works with raw pointers also
    const auto nod = sizeof(decltype(*std::declval<IteratorT>())) * 2;

    std::string indent(prompt.length(), ' ');
    std::string empty(nod + 1, ' ');
    dest << std::hex << std::setfill('0');

    // Inspired by epatel @ stack overflow
    // https://stackoverflow.com/a/29865
    size_t i, j;
    for (i = 0; it != end and i < maxelem; i += 16) {
        dest << (i ? indent : prompt) << std::setw(6) << i << ": ";
        auto line = it;
        for (j = 0; j < 16; ++j) {
            if (it != end) {
                const unsigned long long ch = static_cast<
                        typename std::make_unsigned<
                            typename std::decay<decltype(*std::declval<IteratorT>())>::type
                        >::type
                    >(*it++);
                dest << std::setw(nod) << ch << ' ';
            } else {
                if (nod != 2)
                    break;
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
    if (it != end)
        dest << indent << "[output truncated...]\n";
    return dest;
}

/**
 * Generate a hexdump of a structure/buffer on a stream.
 * The elements of the buffer are dumped with their native width. I.e. when the elements are
 * 16 bit wide the numbers are dumped as 16bit integers.
 * Elements have to be an integral type.
 * If the elements are of type char, also a textual representation of the printable
 * characters is dumped.
 * The destination steam must be convertible to std::basic_ostream.
 *
 * \param dest The stream to use for dumping
 * \param buf Pointer to the buffer to dump
 * \param buflen Number of elements in the buffer (i.e. number of elements to dump)
 * \param prompt (optional) String that prefixes the dump text
 *
 * \returns Ref to the stream used to dump
 */
template<typename StreamT, typename ElemT,
    typename = std::enable_if_t<detail::IsHexDumpStream<StreamT>::value>,
    typename = std::enable_if_t<std::is_integral<ElemT>::value>>
StreamT& hexdump_stream(StreamT& dest, const ElemT* const buf, const size_t buflen, const std::string& prompt = "")
{
    return hexdump_stream(dest, buf, buf + buflen, prompt);
}

//////
// Functions returning a string
//

/**
 * Generate a hexdump of a structure/buffer.
 * The elements of the buffer are dumped with their native width. I.e. when the elements are
 * 16 bit wide the numbers are dumped as 16bit integers.
 * Elements have to be an integral type.
 * If the elements are of type char, also a textual representation of the printable
 * characters is dumped.
 *
 * The result is a std::string:
 * \code
 * std::string dump1 = hexdump(data.begin(), data.end());
 * const auto dump2 = hexdump(data.begin(), data.end());
 * std::cerr << hexdump(data.begin(), data.end(), "ERROR 42: ");
 * \endcode
 *
 * \param begin ForwardIterator to the first data to be dumped
 * \param end ForwardIterator past the last data element to be dumped
 * \param prompt (optional) String that prefixes the dump text
 *
 * \returns a string containing the dump
 */
template<typename IteratorT,
    typename = std::enable_if_t<detail::IsHexDumpIterator<IteratorT>::value>>
std::string hexdump(const IteratorT begin, const IteratorT end, const std::string& prompt = "")
{
    std::stringstream o{ };
    return hexdump_stream(o, begin, end, prompt).str();
}

/**
 * Generate a hexdump of a structure/buffer.
 * The elements of the buffer are dumped with their native width. I.e. when the elements are
 * 16 bit wide the numbers are dumped as 16bit integers.
 * Elements have to be an integral type.
 * If the elements are of type char, also a textual representation of the printable
 * characters is dumped.
 *
 * The result is a std::string:
 * \code
 * std::string dump1 = hexdump(data, len);
 * const auto dump2 = hexdump(data, len);
 * std::cerr << hexdump(data, len, "ERROR 42: ");
 * \endcode
 *
 * \param buf Pointer to the buffer to dump
 * \param buflen Number of elements in the buffer (i.e. number of elements to dump)
 * \param prompt (optional) String that prefixes the dump text
 *
 * \returns a string containing the dump
 */
template<typename ElemT,
    typename = std::enable_if_t<std::is_integral<ElemT>::value>>
std::string hexdump(const ElemT* const buf, const size_t buflen, const std::string& prompt = "")
{
    std::stringstream o{ };
    return hexdump_stream(o, buf, buf + buflen, prompt).str();
}

/**
 * Generate a hexdump of a (probably STL) container.
 * The elements of the container are dumped with their native width. I.e. when the elements are
 * 16 bit wide the numbers are dumped as 16bit integers.
 * Elements have to be an integral type.
 * If the elements are of type char, also a textual representation of the printable
 * characters is dumped.
 *
 * The result is a std::string:
 * \code
 * std::string dump1 = hexdump(mycontainer);
 * const auto dump2 = hexdump(mycontainer);
 * std::cerr << hexdump(mycontainer, "ERROR 42: ");
 * \endcode
 * mycontainer can be std::array, std::vector, ...
 *
 * \param cont Reference to the container to dump
 * \param prompt (optional) String that prefixes the dump text
 *
 * \returns a string containing the dump
 */
template<typename ContainerT,
    typename = std::enable_if_t<detail::IsHexDumpContainer<ContainerT>::value>,
    typename = std::enable_if_t<not std::is_convertible<ContainerT, string_view>::value> >
std::string hexdump(const ContainerT& cont, const std::string& prompt = "")
{
    std::stringstream o{ };
    return hexdump_stream(o, cont.cbegin(), cont.cend(), prompt).str();
}

/**
 * Generate a hexdump of a string.
 * The elements in the string are dumped with their native width. I.e. when the elements are
 * 16 bit wide the numbers are dumped as 16bit integers.
 * If the elements are of type char, also a textual representation of the printable
 * characters is dumped.
 *
 * The result is a std::string:
 * \code
 * std::string dump1 = hexdump(mystring);
 * const auto dump2 = hexdump(mystring);
 * std::cerr << hexdump(mystring, "ERROR 42: ");
 * \endcode
 *
 * \param str The string to dump
 * \param prompt (optional) String that prefixes the dump text
 *
 * \returns a string containing the dump
 */
template<typename StringT,
    typename = std::enable_if_t<std::is_convertible<StringT, string_view>::value> >
std::string hexdump(StringT str, const std::string& prompt = "")
{
    // We need to create a string_view object explicitely because there is no
    // automatic type conversion in template arguments, and we need it to pass
    // the iterators along...
    // (Special case, needed for C string literals)
    const auto sv = string_view{ str };
    std::stringstream o{ };
    return hexdump_stream(o, sv.cbegin(), sv.cend(), prompt).str();
}

//////
// Functions returning a forwarder object
// (Support for 'stream << hexdump' without intermediate data image)
//

/**
 * Helper object used to enable a conveniant syntax to dump things to a stream.
 * \code
 * std::cerr << hexdump_stream(mydata.data(), mydata.size()) << "\n";
 * \endcode
 * It just forwards the parameters to hexdump_stream() to the appropriate operator<<.
 */
template<typename IteratorT>
struct HexdumpParameterForward {
    const IteratorT begin;
    const IteratorT end;
    const std::string& prompt;
};

/**
 * Generate a hexdump of a structure/buffer on a stream.
 * The elements of the buffer are dumped with their native width. I.e. when the elements are
 * 16 bit wide the numbers are dumped as 16bit integers.
 * Elements have to be an integral type.
 * If the elements are of type char, also a textual representation of the printable
 * characters is dumped.
 * The destination steam must be convertible to std::basic_ostream.
 *
 * \code
 * std::cout << hexdump_stream(mydata.begin(), mydata.end()) << "\n";
 * my_filestream << hexdump_stream(data, data + len);
 * std::cerr << hexdump_stream(d.begin(), d.end(), "ERROR 42: ");
 * \endcode
 *
 * \param begin ForwardIterator to the first data to be dumped
 * \param end ForwardIterator past the last data element to be dumped
 * \param prompt (optional) String that prefixes the dump text
 *
 * \returns a helper object to be used with operator<< on streams
 */
template<typename IteratorT,
    typename = std::enable_if_t<detail::IsHexDumpIterator<IteratorT>::value>>
HexdumpParameterForward<IteratorT>
hexdump_stream(IteratorT begin, IteratorT end, const std::string& prompt = "")
{
    return { begin, end, prompt };
}

/**
 * Generate a hexdump of a structure/buffer on a stream.
 * The elements of the buffer are dumped with their native width. I.e. when the elements are
 * 16 bit wide the numbers are dumped as 16bit integers.
 * Elements have to be an integral type.
 * If the elements are of type char, also a textual representation of the printable
 * characters is dumped.
 * The destination steam must be convertible to std::basic_ostream.
 *
 * \code
 * std::cout << hexdump_stream(mydata.begin(), mydata.end()) << "\n";
 * my_filestream << hexdump_stream(data, data + len);
 * std::cerr << hexdump_stream(d.begin(), d.end(), "ERROR 42: ");
 * \endcode
 *
 * \param buf Pointer to the buffer to dump
 * \param buflen Number of elements in the buffer (i.e. number of elements to dump)
 * \param prompt (optional) String that prefixes the dump text
 *
 * \returns a helper object to be used with operator<< on streams
 */
template<typename ElemT,
    typename = std::enable_if_t<std::is_integral<ElemT>::value>>
HexdumpParameterForward<const ElemT* const>
hexdump_stream(const ElemT* const buf, const size_t buflen, const std::string& prompt = "")
 {
    return { buf, buf + buflen, prompt };
 }

/**
 * Generate a hexdump of a structure/buffer on a stream.
 * The elements of the buffer are dumped with their native width. I.e. when the elements are
 * 16 bit wide the numbers are dumped as 16bit integers.
 * Elements have to be an integral type.
 * If the elements are of type char, also a textual representation of the printable
 * characters is dumped.
 * The destination steam must be convertible to std::basic_ostream.
 *
 * \code
 * std::cout << hexdump_stream(mydata.begin(), mydata.end()) << "\n";
 * my_filestream << hexdump_stream(data, data + len);
 * std::cerr << hexdump_stream(d.begin(), d.end(), "ERROR 42: ");
 * \endcode
 *
 * \param cont Reference to the container to dump
 * \param prompt (optional) String that prefixes the dump text
 *
 * \returns a helper object to be used with operator<< on streams
 */
template<typename ContainerT,
    typename = std::enable_if_t<detail::IsHexDumpContainer<ContainerT>::value>>
HexdumpParameterForward<decltype(std::declval<ContainerT>().cbegin())>
hexdump_stream(const ContainerT& cont, const std::string& prompt = "")
{
    return { cont.cbegin(), cont.cend(), prompt };
}

/**
 * Overload of std::ostream's operator<< to enable a conventiant syntax to dump
 * things to a stream. Can also be used for ofstreams or stringstreams.
 * \code
 * std::cerr << hexdump_stream(mydata.data(), mydata.size()) << "\n";
 * \endcode
 * The stream is filled successively with the dumped data, no internal representation
 * of the dump is generated.
 */
template<typename IteratorT,
    typename = std::enable_if_t<detail::IsHexDumpIterator<IteratorT>::value>>
std::ostream& operator<< (std::ostream& os, const HexdumpParameterForward<IteratorT>& hdp) {
    return hexdump_stream(os, hdp.begin, hdp.end, hdp.prompt);
}


} // namespace gul
