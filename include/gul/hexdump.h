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
// std::string hexdump(const ContainerT& cont, string_view prompt = "")
//
// struct HexdumpParameterForward
//
// HexdumpParameterForward<...> hexdump_stream(IteratorT begin, IteratorT end, std::string prompt = "")
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
        std::is_integral<typename std::iterator_traits<decltype(
                                    std::declval<T>().cbegin())>::value_type>::value,
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
        std::basic_ostream<typename StreamT::char_type,
                            typename StreamT::traits_type>*>::value>>
struct IsHexDumpStream : std::true_type { };

/// \cond PRIVATE

// Here is the template actually doing the hexdump
// It is called by the different hexdump*() versions
template<typename StreamT, typename IteratorT,
    typename = std::enable_if_t<detail::IsHexDumpStream<StreamT>::value>,
    typename = std::enable_if_t<detail::IsHexDumpIterator<IteratorT>::value>>
StreamT& hexdump_stream(StreamT& dest, const IteratorT& begin, const IteratorT& end,
    string_view prompt = "")
{
    constexpr auto maxelem = 1000ul * 16; // 1000 lines with 16 elements each

    // Get the number of hex digits to represent any value of the given integral type
    constexpr auto nod = sizeof(*begin) * 2;

    const std::string indent(prompt.length(), ' ');
    const std::string empty(nod + 1, ' ');

    dest << std::hex << std::setfill('0');

    auto it = IteratorT{ begin };

    // Inspired by epatel @ stack overflow, https://stackoverflow.com/a/29865
    for (size_t i = 0; (it != end and i < maxelem) or (i == 0); i += 16) {
        dest << (i ? indent : prompt) << std::setw(6) << i << ": ";
        auto line = it;
        for (size_t j = 0; j < 16; ++j) {
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
            for (size_t j = 0; j < 16 and line != end; ++j, ++line) {
                const auto c = static_cast<unsigned char>(*line);
                dest << static_cast<char>(isprint(c) ? c : '.'); // isprint() is only defined for unsigned char, but only char creates character output
            }
        }
        dest << "\n";
    }
    if (it != end) {
        dest << indent << "[output truncated...]\n";
    }
    return dest;
}
/// \endcond

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
 * std::cerr << hexdump_stream(mydata.begin(), mydata.end()) << "\n";
 * \endcode
 * It just forwards the parameters to hexdump_stream() to the appropriate operator<<.
 * Member cont_ holds the container if a temporary has been used for dumping.
 */
template<typename IteratorT, typename ContainerT = void*>
struct HexdumpParameterForward {
    /// Iterator to begin of elements to be dumped (in iterator mode)
    IteratorT begin_;
    /// Iterator past end of elements to be dumped (in iterator mode)
    IteratorT end_;
    /// Possible prompt to prepend to the dump
    std::string prompt_;
    /// A container with the elements to be dumped (in container/temporary mode)
    ContainerT cont_;

    HexdumpParameterForward() = default;

    /**
     * Construct a hexdump parameter forwarder object.
     * \param begin_it  Start iterator (meaningless if a container is passed).
     * \param end_it    End iterator (meaningless if a container is passed).
     * \param prompt    A string to be printed alongside the hexdump output.
     * \param cont      The container to be hexdumped (can be nullptr if iterators are
     *                  used).
     */
    HexdumpParameterForward(
        IteratorT begin_it, IteratorT end_it, std::string prompt, ContainerT&& cont)
        : begin_ { begin_it }
        , end_ { end_it }
        , prompt_ { std::move(prompt) }
        , cont_ { std::forward<ContainerT>(cont) }
    {
        regenerate_iterators<ContainerT>();
    }

    /**
     * Copy constructor (automatically updates the begin_ and end_ interator members if
     * the copied object holds a container).
     */
    HexdumpParameterForward(const HexdumpParameterForward& other) { *this = other; }

    /**
     * Move constructor (automatically updates the begin_ and end_ interator members if
     * the moved-from object holds a container).
     */
    HexdumpParameterForward(HexdumpParameterForward&& other) noexcept
    {
        *this = std::move(other);
    }

    /**
     * Copy assignment (automatically updates the begin_ and end_ interator members if
     * the copied object holds a container).
     */
    HexdumpParameterForward& operator=(const HexdumpParameterForward& other)
    {
        if (this == &other)
            return *this;

        begin_ = other.begin_;
        end_ = other.end_;
        prompt_ = other.prompt_;
        cont_ = other.cont_;

        regenerate_iterators<ContainerT>();

        return *this;
    }

    /**
     * Move assignment (automatically updates the begin_ and end_ interator members if
     * the moved-from object holds a container).
     */
    HexdumpParameterForward& operator=(HexdumpParameterForward&& other) noexcept
    {
        if (this == &other)
            return *this;

        begin_ = std::move(other.begin_);
        end_ = std::move(other.end_);
        prompt_ = std::move(other.prompt_);
        cont_ = std::move(other.cont_);

        regenerate_iterators<ContainerT>();

        return *this;
    }

private:
    template <typename ContType,
        std::enable_if_t<!detail::IsHexDumpContainer<ContType>::value, int> = 0
        >
    void regenerate_iterators() noexcept {}

    template <typename ContType,
        std::enable_if_t<detail::IsHexDumpContainer<ContType>::value, int> = 0
        >
    void regenerate_iterators() noexcept
    {
        begin_ = cont_.begin();
        end_ = cont_.end();
    }
};

/**
 * Generate a hexdump of a data range that can be efficiently written to a stream using
 * operator<<.
 *
 * Where hexdump() writes all of its output into one monolithic string, hexdump_stream()
 * returns a tiny helper object that can efficiently send its output to an output stream
 * via operator<<. This means that the following two lines produce the exact same output,
 * but the stream version uses less resources:
 * \code
 * std::cout << gul::hexdump_stream(x.begin(), x.end()) << "\n"; // good
 * std::cout << gul::hexdump(x.begin(), x.end()) << "\n"; // also good, but allocates a temporary string
 * \endcode
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
        decltype(HexdumpParameterForward<decltype(std::declval<ContainerT>().cbegin()),
                     ContainerT> {}, 0)>>
HexdumpParameterForward<decltype(std::declval<ContainerT>().cbegin()), ContainerT>
hexdump_stream(ContainerT&& cont, std::string prompt = "")
{
    // The temporary container must be moved to retain the values until we need them
    // after operator<<.
    return { cont.cbegin(), cont.cbegin(), std::move(prompt),
             std::forward<ContainerT>(cont) };
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
std::ostream& operator<<(
    std::ostream& os, const HexdumpParameterForward<IteratorT, ContainerT>& hdp)
{
    return detail::hexdump_stream(os, hdp.begin_, hdp.end_, hdp.prompt_);
}


} // namespace gul

// vi:ts=4:sw=4:et
