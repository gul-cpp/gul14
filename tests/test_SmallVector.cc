/**
 * \file   test_SmallVector.cc
 * \author \ref contributors
 * \date   Created on August 17, 2020
 * \brief  Test suite for the SmallVector class.
 *
 * \copyright Copyright 2020-2021 Deutsches Elektronen-Synchrotron (DESY), Hamburg
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

#include <algorithm>
#include <iterator>
#include <list>
#include <numeric>
#include <ostream>
#include <random>
#include <string>
#include <type_traits>
#include "gul14/catch.h"
#include "gul14/SmallVector.h"

using namespace std::literals::string_literals;
using gul14::SmallVector;

namespace {

// Helper class for testing the strong exception guarantee
class ThrowingInt
{
public:
    ThrowingInt()
    {
        if (default_construction_throw_countdown_ >= 0)
            --default_construction_throw_countdown_;

        if (default_construction_throw_countdown_ == 0)
            throw std::logic_error("Exception");

        ++global_use_count_;
    }

    explicit ThrowingInt(int v) : v_{ v }
    {
        if (value_construction_throw_countdown_ >= 0)
            --value_construction_throw_countdown_;

        if (value_construction_throw_countdown_ == 0)
            throw std::logic_error("Exception");

        ++global_use_count_;
    }

    ThrowingInt(const ThrowingInt& other)
        : v_{ other.v_ }
    {
        if (copy_construction_throw_countdown_ >= 0)
            --copy_construction_throw_countdown_;

        if (copy_construction_throw_countdown_ == 0)
            throw std::logic_error("Exception");

        ++global_use_count_;
    }

    ThrowingInt(ThrowingInt&& other)
    {
        if (move_throw_countdown_ >= 0)
            --move_throw_countdown_;

        if (move_throw_countdown_ == 0)
            throw std::logic_error("Exception");

        v_ = other.v_;
        other.v_ = -1;

        ++global_use_count_;
    }

    ~ThrowingInt() noexcept
    {
        --global_use_count_;
    }

    ThrowingInt& operator=(const ThrowingInt &other) noexcept
    {
        v_ = other.v_;
        return *this;
    }

    ThrowingInt& operator=(int v) noexcept { v_ = v; return *this; }
    friend bool operator==(const ThrowingInt& a, const ThrowingInt& b) noexcept { return a.v_ == b.v_; }
    friend std::ostream &operator<<(std::ostream &stream, const ThrowingInt &val) { stream << val.v_; return stream; }

    static size_t global_use_count() { return global_use_count_; }
    static void throw_on_copy_construction_in(int n) { copy_construction_throw_countdown_ = n; }
    static void throw_on_default_construction_in(int n) { default_construction_throw_countdown_ = n; }
    static void throw_on_move_in(int n) { move_throw_countdown_ = n; }
    static void throw_on_value_construction_in(int n) { value_construction_throw_countdown_ = n; }

private:
    int v_{ 0 };

    static int copy_construction_throw_countdown_;
    static int default_construction_throw_countdown_;
    static size_t global_use_count_;
    static int move_throw_countdown_;
    static int value_construction_throw_countdown_;
};

int ThrowingInt::copy_construction_throw_countdown_{ -1 };
int ThrowingInt::default_construction_throw_countdown_{ -1 };
size_t ThrowingInt::global_use_count_{ 0 };
int ThrowingInt::move_throw_countdown_{ -1 };
int ThrowingInt::value_construction_throw_countdown_{ -1 };

// A move-only version of ThrowingInt that has no copy constructor defined.
class MoveOnlyThrowingInt : public ThrowingInt
{
public:
    MoveOnlyThrowingInt() : ThrowingInt() {}
    explicit MoveOnlyThrowingInt(int v) : ThrowingInt(v) {}
    MoveOnlyThrowingInt(MoveOnlyThrowingInt&& other) : ThrowingInt(std::move(other)) {};
};

} // anonymous namespace


TEMPLATE_TEST_CASE("SmallVector: Default constructor", "[SmallVector]",
    int, float, std::string)
{
    constexpr int capacity = 4;

    static_assert(std::is_nothrow_default_constructible<SmallVector<TestType, capacity>>::value,
        "SmallVector is_nothrow_default_constructible");
    static_assert(std::is_copy_constructible<SmallVector<TestType, capacity>>::value,
        "SmallVector is_copy_constructible");
    static_assert(std::is_nothrow_copy_constructible<SmallVector<TestType, capacity>>::value ==
        std::is_nothrow_copy_constructible<TestType>::value,
        "SmallVector's is_nothrow_copy_constructible == element's is_nothrow_copy_constructible");
    static_assert(std::is_nothrow_move_constructible<SmallVector<TestType, capacity>>::value,
        "SmallVector is_nothrow_move_constructible");

    SmallVector<TestType, capacity> vec;
    REQUIRE(vec.empty());
    REQUIRE(vec.size() == 0);
    REQUIRE(vec.capacity() == capacity);
}

TEMPLATE_TEST_CASE("SmallVector: Constructor with number of elements", "[SmallVector]",
    int, float, std::string, std::unique_ptr<int>)
{
    constexpr int capacity = 4;

    SmallVector<TestType, capacity> vec(0);
    REQUIRE(vec.empty());
    REQUIRE(vec.size() == 0);
    REQUIRE(vec.capacity() == capacity);

    SmallVector<TestType, capacity> vec2(capacity);
    REQUIRE(vec2.size() == capacity);
    REQUIRE(vec2.capacity() == capacity);
    for (auto i = 0u; i != vec2.size(); ++i)
        REQUIRE(vec2[i] == TestType{});

    SmallVector<TestType, capacity> vec3(capacity + 1);
    REQUIRE(vec3.size() == capacity + 1);
    REQUIRE(vec3.capacity() >= capacity + 1);
    for (const auto& element : vec3)
        REQUIRE(element == TestType{});
}

TEST_CASE("SmallVector: Constructor with number of elements and inititalizer",
    "[SmallVector]")
{
    constexpr int capacity = 4;

    SmallVector<std::string, capacity> vec(0, "one");
    REQUIRE(vec.empty());
    REQUIRE(vec.size() == 0);
    REQUIRE(vec.capacity() == capacity);

    SmallVector<std::string, capacity> vec2(capacity, "two");
    REQUIRE(vec2.size() == capacity);
    REQUIRE(vec2.capacity() == capacity);
    for (decltype(vec2)::SizeType i = 0u; i != vec2.size(); ++i)
        REQUIRE(vec2[i] == "two");

    SmallVector<std::string, capacity> vec3(capacity + 1, "three");
    REQUIRE(vec3.size() == capacity + 1);
    REQUIRE(vec3.capacity() >= capacity + 1);
    for (const auto& str : vec3)
        REQUIRE(str == "three");
}

TEST_CASE("SmallVector: Constructor with iterator range", "[SmallVector]")
{
    constexpr int capacity = 4;

    const std::string* nulptr{ nullptr };
    SmallVector<std::string, capacity> vec(nulptr, nulptr);
    REQUIRE(vec.empty());
    REQUIRE(vec.size() == 0);
    REQUIRE(vec.capacity() == capacity);

    int c_array[2];
    c_array[0] = 0;
    c_array[1] = 1;

    SmallVector<int, capacity> vec2(std::begin(c_array), std::end(c_array));
    REQUIRE(vec2.size() == 2);
    REQUIRE(vec2.capacity() == capacity);
    for (auto i = 0u; i != vec2.size(); ++i)
        REQUIRE(vec2[i] == c_array[i]);

    std::list<std::string> list{ "zero"s, "one"s, "two"s, "three"s, "four"s };
    SmallVector<std::string, capacity> vec3(list.cbegin(), list.cend());
    REQUIRE(vec3.size() == 5);
    REQUIRE(vec3.capacity() >= 5);
    REQUIRE(std::equal(vec3.begin(), vec3.end(), list.begin()));
}

TEST_CASE("SmallVector: Copy constructor", "[SmallVector]")
{
    constexpr int inner_capacity = 6;

    SECTION("Empty vector") {
        SmallVector<float, inner_capacity> ori;
        auto vec{ ori };
        REQUIRE(ori.empty());
        REQUIRE(vec.empty());
        REQUIRE(vec.size() == ori.size());
        REQUIRE(std::equal(vec.begin(), vec.end(), ori.begin()));
    }

    SECTION("Integers, capacity > inner_capacity") {
        SmallVector<int, inner_capacity> ori;
        for (int i = 0; i != 10; ++i)
            ori.push_back(i);
        auto vec{ ori };
        REQUIRE(ori.size() == 10);
        REQUIRE(vec.size() == ori.size());
        REQUIRE(std::equal(vec.begin(), vec.end(), ori.begin()));
    }

    SECTION("Strings, capacity = inner_capacity") {
        std::list<std::string> list{ "zero"s, "one"s, "two"s, "three"s, "four"s };
        SmallVector<std::string, inner_capacity> ori(list.cbegin(), list.cend());
        auto vec{ ori };
        REQUIRE(ori.size() == 5);
        REQUIRE(vec.size() == ori.size());
        REQUIRE(std::equal(vec.begin(), vec.end(), ori.begin()));
    }
}

TEST_CASE("SmallVector: Copy constructor, exception guarantees", "[SmallVector]")
{
    constexpr int inner_capacity = 3;

    REQUIRE(ThrowingInt::global_use_count() == 0);

    SECTION("ValueType throwing on copy") {
        using Vec = SmallVector<ThrowingInt, inner_capacity>;
        Vec ori;

        REQUIRE(std::is_copy_constructible<Vec>::value == true);
        REQUIRE(std::is_nothrow_copy_constructible<Vec>::value == false);

        ori.push_back(ThrowingInt{ 0 });
        ori.push_back(ThrowingInt{ 1 });
        REQUIRE(ThrowingInt::global_use_count() == 2);

        ThrowingInt::throw_on_copy_construction_in(2);
        REQUIRE_THROWS_AS(Vec{ ori }, std::exception);
        REQUIRE(ori.size() == 2u);
        REQUIRE(ori[0] == ThrowingInt{ 0 });
        REQUIRE(ori[1] == ThrowingInt{ 1 });
        REQUIRE(ThrowingInt::global_use_count() == 2);

        Vec copied{ ori };
        REQUIRE(ori.size() == 2u);
        REQUIRE(ori[0] == ThrowingInt{ 0 });
        REQUIRE(ori[1] == ThrowingInt{ 1 });
        REQUIRE(copied.size() == 2u);
        REQUIRE(copied[0] == ThrowingInt{ 0 });
        REQUIRE(copied[1] == ThrowingInt{ 1 });
        REQUIRE(ThrowingInt::global_use_count() == 4);
    }

    REQUIRE(ThrowingInt::global_use_count() == 0);
}

TEST_CASE("SmallVector: Move constructor", "[SmallVector]")
{
    constexpr int inner_capacity = 2;

    SECTION("Empty vector") {
        SmallVector<float, inner_capacity> ori;
        auto vec{ std::move(ori) };
        REQUIRE(ori.empty());
        REQUIRE(vec.empty());
        REQUIRE(vec.size() == ori.size());
        REQUIRE(std::equal(vec.begin(), vec.end(), ori.begin()));
    }

    SECTION("Integers, capacity > inner_capacity") {
        SmallVector<int, inner_capacity> ori;
        for (int i = 0; i != 10; ++i)
            ori.push_back(i);
        auto vec{ std::move(ori) };
        REQUIRE(ori.empty());
        REQUIRE(vec.size() == 10);
        for (int i = 0; i != 10; ++i)
            REQUIRE(vec[i] == i);
    }

    SECTION("Strings, capacity == inner_capacity") {
        std::list<std::string> list{ "zero"s, "one"s, "two"s, "three"s, "four"s };
        SmallVector<std::string, inner_capacity> ori(list.cbegin(), list.cend());
        auto vec{ std::move(ori) };
        REQUIRE(ori.empty());
        REQUIRE(vec.size() == 5);
        REQUIRE(vec[0] == "zero");
        REQUIRE(vec[1] == "one");
        REQUIRE(vec[2] == "two");
        REQUIRE(vec[3] == "three");
        REQUIRE(vec[4] == "four");
    }

    SECTION("unique_ptr, capacity == inner_capacity") {
        SmallVector<std::unique_ptr<int>, inner_capacity> ori;
        for (int i = 0; i != inner_capacity; ++i)
            ori.push_back(std::make_unique<int>(i));
        auto vec{ std::move(ori) };
        REQUIRE(ori.empty());
        REQUIRE(vec.size() == inner_capacity);
        for (int i = 0; i != inner_capacity; ++i)
            REQUIRE(*vec[i] == i);
    }

    SECTION("unique_ptr, capacity > inner_capacity") {
        SmallVector<std::unique_ptr<int>, inner_capacity> ori;
        for (int i = 0; i != 10; ++i)
            ori.push_back(std::make_unique<int>(i));
        auto vec{ std::move(ori) };
        REQUIRE(ori.empty());
        REQUIRE(vec.size() == 10);
        for (int i = 0; i != 10; ++i)
            REQUIRE(*vec[i] == i);
    }
}

TEST_CASE("SmallVector: Move constructor, exception guarantees", "[SmallVector]")
{
    constexpr int inner_capacity = 2;

    REQUIRE(ThrowingInt::global_use_count() == 0);

    SECTION("Copyable & movable ValueType throwing on move or copy") {
        using Vec = SmallVector<ThrowingInt, inner_capacity>;
        Vec ori;

        REQUIRE(std::is_move_constructible<Vec>::value == true);
        REQUIRE(std::is_nothrow_move_constructible<Vec>::value == false);

        ori.push_back(ThrowingInt{ 0 });
        ori.push_back(ThrowingInt{ 1 });
        REQUIRE(ThrowingInt::global_use_count() == 2);

        ThrowingInt::throw_on_copy_construction_in(2);
        REQUIRE_THROWS_AS(Vec{ std::move(ori) }, std::exception); // Falls back to copying elements because move ctor might throw
        REQUIRE(ori.size() == 2u);
        REQUIRE(ori[0] == ThrowingInt{ 0 });
        REQUIRE(ori[1] == ThrowingInt{ 1 });
        REQUIRE(ThrowingInt::global_use_count() == 2);

        ThrowingInt::throw_on_move_in(2);
        Vec copied{ std::move(ori) }; // Falls back to copying elements because move ctor might throw
        REQUIRE(ori.empty());
        ThrowingInt::throw_on_move_in(-1);
        REQUIRE(ThrowingInt::global_use_count() == 2);

        ori.push_back(ThrowingInt{ 0 });
        ori.push_back(ThrowingInt{ 1 });
        ori.push_back(ThrowingInt{ 2 });
        ori.push_back(ThrowingInt{ 3 });
        ThrowingInt::throw_on_copy_construction_in(1);
        ThrowingInt::throw_on_move_in(1);

        Vec moved{ std::move(ori) }; // Can simply steal allocated memory, elements are not moved or copied
        REQUIRE(ori.empty());

        ThrowingInt::throw_on_copy_construction_in(-1); // Disable throwing
        ThrowingInt::throw_on_move_in(-1);
    }

    SECTION("Move-only ValueType throwing on move") {
        using Vec = SmallVector<MoveOnlyThrowingInt, inner_capacity>;
        Vec ori;

        REQUIRE(std::is_copy_constructible<MoveOnlyThrowingInt>::value == false);
        REQUIRE(std::is_move_constructible<MoveOnlyThrowingInt>::value == true);
        REQUIRE(std::is_nothrow_move_constructible<MoveOnlyThrowingInt>::value == false);

        REQUIRE(std::is_move_constructible<Vec>::value == true);
        REQUIRE(std::is_nothrow_move_constructible<Vec>::value == false);

        ori.push_back(MoveOnlyThrowingInt{ 0 });
        ori.push_back(MoveOnlyThrowingInt{ 1 });
        MoveOnlyThrowingInt::throw_on_move_in(2);

        REQUIRE_THROWS_AS(Vec{ std::move(ori) }, std::exception);
        REQUIRE(ori.size() == 2u); // Size does not change, but no guarantee on values

        ori.push_back(MoveOnlyThrowingInt{ 2 });
        ori.push_back(MoveOnlyThrowingInt{ 3 });
        MoveOnlyThrowingInt::throw_on_move_in(2);

        Vec moved{ std::move(ori) }; // Can simply steal allocated memory, elements are not moved
        REQUIRE(ori.empty());

        MoveOnlyThrowingInt::throw_on_move_in(-1); // Disable throwing
    }

    REQUIRE(ThrowingInt::global_use_count() == 0);
}

TEST_CASE("SmallVector: Initializer list constructor", "[SmallVector]")
{
    constexpr int inner_capacity = 4;

    SmallVector<float, inner_capacity> vec{ 0.0f, 1.0f, 2.0f };
    REQUIRE(vec.size() == 3);
    REQUIRE(vec.capacity() >= 3);
    REQUIRE(vec[0] == 0.0f);
    REQUIRE(vec[1] == 1.0f);
    REQUIRE(vec[2] == 2.0f);

    SmallVector<long, inner_capacity> vec1{ 0L, 1L, 2L, 3L, 4L };
    REQUIRE(vec1.size() == 5);
    REQUIRE(vec1.capacity() >= 5);
    REQUIRE(vec1[0] == 0L);
    REQUIRE(vec1[1] == 1L);
    REQUIRE(vec1[2] == 2L);
    REQUIRE(vec1[3] == 3L);
    REQUIRE(vec1[4] == 4L);

    SmallVector<std::string, inner_capacity> vec2{ "zero", "one", "two", "three", "four" };
    REQUIRE(vec2.size() == 5);
    REQUIRE(vec2.capacity() >= 5);
    REQUIRE(vec2[0] == "zero");
    REQUIRE(vec2[1] == "one");
    REQUIRE(vec2[2] == "two");
    REQUIRE(vec2[3] == "three");
    REQUIRE(vec2[4] == "four");
}

TEST_CASE("SmallVector: Destructor", "[SmallVector]")
{
    auto sptr = std::make_shared<int>(0);
    REQUIRE(sptr.use_count() == 1);

    {
        SmallVector<std::shared_ptr<int>, 4> vec;

        for (int i = 1; i <= 4; ++i)
        {
            vec.push_back(sptr);
            REQUIRE(sptr.use_count() == i + 1);
        }
    }

    REQUIRE(sptr.use_count() == 1);

    {
        SmallVector<std::shared_ptr<int>, 4> vec;

        for (int i = 1; i <= 10; ++i)
        {
            vec.push_back(sptr);
            REQUIRE(sptr.use_count() == i + 1);
        }
    }

    REQUIRE(sptr.use_count() == 1);

    sptr = nullptr;
    REQUIRE(sptr.use_count() == 0);
}

TEMPLATE_TEST_CASE("SmallVector: push_back(), operator[]() w/ numbers", "[SmallVector]",
    int, float)
{
    constexpr auto inner_capacity = 4u;

    SmallVector<TestType, inner_capacity> vec;

    // Fill inner buffer
    for (unsigned int i = 0u; i != inner_capacity; ++i)
        vec.push_back(TestType(i));

    for (unsigned int i = 0u; i != inner_capacity; ++i)
        REQUIRE(vec[i] == TestType(i));

    // Fill beyond inner buffer
    for (unsigned int i = inner_capacity; i != 2*inner_capacity; ++i)
        vec.push_back(TestType(i));

    for (unsigned int i = 0u; i < vec.size(); ++i)
    {
        REQUIRE(vec[i] == TestType(i));
        vec[i] = TestType(42 + i);
    }

    for (unsigned int i = 0u; i != vec.size(); ++i)
        REQUIRE(vec[i] == TestType(42 + i));
}

TEST_CASE("SmallVector: Algorithms, range-based for, reverse iteration", "[SmallVector]")
{
    SmallVector<int, 8> vec;

    std::fill_n(std::back_inserter(vec), 42, -1);
    REQUIRE(vec.size() == 42);
    REQUIRE(vec.capacity() >= 42);
    REQUIRE(std::all_of(vec.cbegin(), vec.cend(), [](int a) { return a == -1; }));

    std::iota(vec.begin(), vec.end(), 1);
    REQUIRE(vec.size() == 42);

    int n = 0;
    for (int value : vec)
    {
        ++n;
        REQUIRE(value == n);
    }

    for (auto it = vec.rbegin(); it != vec.rend(); ++it)
    {
        REQUIRE(*it == n);
        --n;
    }
}

TEMPLATE_TEST_CASE("SmallVector: Iterator traits", "[SmallVector]",
                   int, double, std::string)
{
    using SmallVectorIterator = typename SmallVector<TestType, 4>::Iterator;
    using SmallVectorConstIterator = typename SmallVector<TestType, 4>::ConstIterator;

    REQUIRE(
        std::is_base_of<
            std::random_access_iterator_tag,
            typename std::iterator_traits<SmallVectorIterator>::iterator_category
        >::value == true);

    REQUIRE(
        std::is_base_of<
            std::random_access_iterator_tag,
            typename std::iterator_traits<SmallVectorConstIterator>::iterator_category
        >::value == true);
}


//
// Tests for indiviual member functions (in alphabetical order)
//

TEST_CASE("SmallVector: assign(num_elements, element)", "[SmallVector]")
{
    constexpr int capacity = 2;

    SmallVector<std::string, capacity> vec;
    vec.push_back("one");
    vec.assign(0, "element");
    REQUIRE(vec.empty());
    REQUIRE(vec.size() == 0);
    REQUIRE(vec.capacity() == capacity);

    SmallVector<std::string, capacity> vec2;
    vec2.assign(capacity, "element");
    REQUIRE(vec2.size() == capacity);
    REQUIRE(vec2.capacity() == capacity);
    for (decltype(vec2)::SizeType i = 0u; i != vec2.size(); ++i)
        REQUIRE(vec2[i] == "element");

    SmallVector<std::string, capacity> vec3;
    vec3.push_back("one");
    vec3.push_back("two");
    vec3.push_back("three");
    vec3.assign(capacity + 1, "element");
    REQUIRE(vec3.size() == capacity + 1);
    REQUIRE(vec3.capacity() >= capacity + 1);
    for (const auto& str : vec3)
        REQUIRE(str == "element");
}

TEST_CASE("SmallVector: assign(InputIterator begin, InputIterator end)", "[SmallVector]")
{
    constexpr int capacity = 4;

    const std::string* nulptr{ nullptr };
    SmallVector<std::string, capacity> vec;
    vec.push_back("test");
    vec.assign(nulptr, nulptr);
    REQUIRE(vec.empty());
    REQUIRE(vec.size() == 0);
    REQUIRE(vec.capacity() == capacity);

    int c_array[2];
    c_array[0] = 0;
    c_array[1] = 1;

    SmallVector<int, capacity> vec2;
    vec2.assign(std::begin(c_array), std::end(c_array));
    REQUIRE(vec2.size() == 2);
    REQUIRE(vec2.capacity() == capacity);
    for (auto i = 0u; i != vec2.size(); ++i)
        REQUIRE(vec2[i] == c_array[i]);

    std::list<std::string> list{ "zero"s, "one"s, "two"s, "three"s, "four"s };
    SmallVector<std::string, capacity> vec3;
    vec3.push_back("one");
    vec3.push_back("two");
    vec3.assign(list.cbegin(), list.cend());
    REQUIRE(vec3.size() == 5);
    REQUIRE(vec3.capacity() >= 5);
    REQUIRE(std::equal(vec3.begin(), vec3.end(), list.begin()));
}

TEST_CASE("SmallVector: assign(std::initializer_list)", "[SmallVector]")
{
    SmallVector<float, 2> vec;
    vec.assign({ 1.0f, 2.0f, 3.0f });

    REQUIRE(vec.size() == 3);
    REQUIRE(vec[0] == 1.0f);
    REQUIRE(vec[1] == 2.0f);
    REQUIRE(vec[2] == 3.0f);

    SmallVector<std::string, 2> vec2;
    vec2.push_back("old content");

    vec2.assign({ "zero", "one" });
    REQUIRE(vec2.capacity() == 2);
    REQUIRE(vec2.size() == 2);
    REQUIRE(vec2[0] == "zero");
    REQUIRE(vec2[1] == "one");
}

TEST_CASE("SmallVector: at()", "[SmallVector]")
{
    constexpr auto inner_capacity = 2u;

    SmallVector<int, inner_capacity> vec;

    vec.push_back(0);
    vec.push_back(1);
    vec.push_back(2);

    REQUIRE(vec.at(0) == 0);
    REQUIRE(vec.at(1) == 1);
    REQUIRE(vec.at(2) == 2);

    REQUIRE_THROWS_AS(vec.at(3), std::out_of_range);
}

TEST_CASE("SmallVector: back()", "[SmallVector]")
{
    SmallVector<char, 12> vec;

    vec.push_back('a');
    vec.push_back('b');
    REQUIRE(vec.back() == 'b');

    vec.back() = 'B';
    REQUIRE(vec.back() == 'B');
}

TEST_CASE("SmallVector: begin()", "[SmallVector]")
{
    SmallVector<double, 1> vec;

    REQUIRE_NOTHROW(vec.begin());

    vec.push_back('a');
    REQUIRE(*(vec.begin()) == 'a');

    vec.push_back('b');
    REQUIRE(*(vec.begin()) == 'a');

    *(vec.begin()) = 'A';
    REQUIRE(vec[0] == 'A');
}

TEMPLATE_TEST_CASE("SmallVector: capacity()", "[SmallVector]", int, float, std::string)
{
    constexpr auto inner_capacity = 4u;

    SmallVector<TestType, inner_capacity> vec;
    REQUIRE(vec.capacity() == inner_capacity);

    // Fill inner buffer
    for (unsigned int i = 1u; i <= inner_capacity; ++i)
    {
        vec.push_back(TestType{});
        REQUIRE(vec.capacity() == inner_capacity);
    }

    // Require growing to the heap
    for (unsigned int i = inner_capacity + 1; i <= 2 * inner_capacity; ++i)
    {
        vec.push_back(TestType{});
        REQUIRE(vec.capacity() >= vec.size());
    }

    vec.reserve(5 * inner_capacity);
    REQUIRE(vec.capacity() == 5 * inner_capacity);
}

TEST_CASE("SmallVector: cbegin()", "[SmallVector]")
{
    SmallVector<double, 1> vec;

    REQUIRE_NOTHROW(vec.cbegin());

    vec.push_back('a');
    REQUIRE(*(vec.cbegin()) == 'a');

    vec.push_back('b');
    REQUIRE(*(vec.cbegin()) == 'a');
}

TEST_CASE("SmallVector: cend()", "[SmallVector]")
{
    SmallVector<unsigned long, 1> vec;

    REQUIRE(vec.cbegin() == vec.cend());

    vec.push_back(1u);
    REQUIRE(*(std::prev(vec.cend())) == 1u);

    vec.push_back(2u);
    REQUIRE(*(std::prev(vec.cend())) == 2u);
}

TEST_CASE("SmallVector: clear()", "[SmallVector]")
{
    auto sptr = std::make_shared<int>(0);
    REQUIRE(sptr.use_count() == 1);

    SmallVector<std::shared_ptr<int>, 4> vec;

    for (int i = 1; i <= 4; ++i)
    {
        vec.push_back(sptr);
        REQUIRE(sptr.use_count() == i + 1);
    }

    vec.clear();
    REQUIRE(vec.size() == 0);
    REQUIRE(vec.capacity() == 4);

    REQUIRE(sptr.use_count() == 1);

    for (int i = 1; i <= 10; ++i)
    {
        vec.push_back(sptr);
        REQUIRE(sptr.use_count() == i + 1);
    }

    auto old_capacity = vec.capacity();
    REQUIRE(old_capacity >= 10);

    vec.clear();

    REQUIRE(vec.size() == 0);
    REQUIRE(vec.capacity() == old_capacity);

    REQUIRE(sptr.use_count() == 1);

    sptr = nullptr;
    REQUIRE(sptr.use_count() == 0);
}

TEST_CASE("SmallVector: crbegin()", "[SmallVector]")
{
    SmallVector<double, 1> vec;

    REQUIRE_NOTHROW(vec.crbegin());

    vec.push_back('a');
    REQUIRE(*(vec.crbegin()) == 'a');

    vec.push_back('b');
    REQUIRE(*(vec.crbegin()) == 'b');
}

TEST_CASE("SmallVector: crend()", "[SmallVector]")
{
    SmallVector<std::string, 1> vec;

    REQUIRE(vec.crbegin() == vec.crend());

    vec.push_back("one");
    REQUIRE(*(std::prev(vec.crend())) == "one");

    vec.push_back("two");
    REQUIRE(*(std::prev(vec.crend())) == "one");
}

TEST_CASE("SmallVector: emplace()", "[SmallVector]")
{
    constexpr auto inner_capacity = 2u;

    SmallVector<std::pair<int, std::string>, inner_capacity> vec;
    REQUIRE(vec.size() == 0u);

    vec.emplace(vec.begin(), 0, "zero");
    REQUIRE(vec.size() == 1u);

    vec.emplace(vec.end(), 1, "one"s);
    REQUIRE(vec.size() == 2u);

    vec.emplace(vec.begin(), std::make_pair<int, std::string>(2, "two"));
    REQUIRE(vec.size() == 3u);

    vec.emplace(vec.begin() + 1, 3, "three");
    REQUIRE(vec.size() == 4u);

    REQUIRE(vec[0] == std::make_pair<int, std::string>(2, "two"));
    REQUIRE(vec[1] == std::make_pair<int, std::string>(3, "three"));
    REQUIRE(vec[2] == std::make_pair<int, std::string>(0, "zero"));
    REQUIRE(vec[3] == std::make_pair<int, std::string>(1, "one"));
}

TEST_CASE("SmallVector: emplace(), exception guarantee", "[SmallVector]")
{
    constexpr auto inner_capacity = 2u;

    REQUIRE(ThrowingInt::global_use_count() == 0);

    SmallVector<ThrowingInt, inner_capacity> vec;

    vec.emplace(vec.begin(), 0);
    REQUIRE(vec[0] == ThrowingInt{ 0 });
    REQUIRE(ThrowingInt::global_use_count() == 1);

    vec.emplace(vec.begin(), 1);
    REQUIRE(vec.size() == 2u);
    REQUIRE(vec[0] == ThrowingInt{ 1 });
    REQUIRE(vec[1] == ThrowingInt{ 0 });
    REQUIRE(ThrowingInt::global_use_count() == 2);

    ThrowingInt::throw_on_value_construction_in(1);

    REQUIRE_THROWS(vec.emplace(vec.begin() + 1, 2));
    REQUIRE(vec.size() == 2u);
    REQUIRE(vec[0] == ThrowingInt{ 1 });
    REQUIRE(vec[1] == ThrowingInt{ 0 });
    REQUIRE(ThrowingInt::global_use_count() == 2);
}

TEST_CASE("SmallVector: emplace_back()", "[SmallVector]")
{
    constexpr auto inner_capacity = 2u;

    SmallVector<std::pair<int,std::string>, inner_capacity> vec;
    REQUIRE(vec.size() == 0u);

    vec.emplace_back(0, "zero");
    REQUIRE(vec.size() == 1u);

    vec.emplace_back(1, "one"s);
    REQUIRE(vec.size() == 2u);

    vec.emplace_back(std::make_pair<int,std::string>(2, "two"));
    REQUIRE(vec.size() == 3u);

    REQUIRE(vec[0] == std::make_pair<int, std::string>(0, "zero"));
    REQUIRE(vec[1] == std::make_pair<int, std::string>(1, "one"));
    REQUIRE(vec[2] == std::make_pair<int, std::string>(2, "two"));
}

TEST_CASE("SmallVector: emplace_back(), exception guarantee", "[SmallVector]")
{
    constexpr auto inner_capacity = 2u;

    REQUIRE(ThrowingInt::global_use_count() == 0);

    SmallVector<ThrowingInt, inner_capacity> vec;

    vec.emplace_back(0);
    REQUIRE(vec[0] == ThrowingInt{ 0 });
    REQUIRE(ThrowingInt::global_use_count() == 1);

    vec.emplace_back(1);
    REQUIRE(vec[1] == ThrowingInt{ 1 });
    REQUIRE(vec.size() == 2u);
    REQUIRE(ThrowingInt::global_use_count() == 2);

    ThrowingInt::throw_on_value_construction_in(1);

    REQUIRE_THROWS(vec.emplace_back(2));
    REQUIRE(vec[0] == ThrowingInt{ 0 });
    REQUIRE(vec[1] == ThrowingInt{ 1 });
    REQUIRE(vec.size() == 2u);
    REQUIRE(ThrowingInt::global_use_count() == 2);
}

TEST_CASE("SmallVector: erase(ConstIterator)", "[SmallVector]")
{
    auto one = std::make_shared<int>(1);
    auto two = std::make_shared<int>(2);
    auto three = std::make_shared<int>(3);
    auto four = std::make_shared<int>(4);
    auto five = std::make_shared<int>(5);

    SmallVector<std::shared_ptr<int>, 2> vec({ one, two, three, four, five });
    decltype(vec)::Iterator it;
    const auto initial_capacity = vec.capacity();

    REQUIRE(one.use_count() == 2);
    REQUIRE(two.use_count() == 2);
    REQUIRE(three.use_count() == 2);
    REQUIRE(four.use_count() == 2);
    REQUIRE(five.use_count() == 2);

    it = vec.erase(vec.cbegin() + 2);
    REQUIRE(it == vec.begin() + 2);
    REQUIRE(vec.size() == 4);
    REQUIRE(*vec[0] == 1);
    REQUIRE(*vec[1] == 2);
    REQUIRE(*vec[2] == 4);
    REQUIRE(*vec[3] == 5);
    REQUIRE(three.use_count() == 1);

    it = vec.erase(vec.cbegin());
    REQUIRE(it == vec.begin());
    REQUIRE(vec.size() == 3);
    REQUIRE(*vec[0] == 2);
    REQUIRE(*vec[1] == 4);
    REQUIRE(*vec[2] == 5);
    REQUIRE(one.use_count() == 1);

    it = vec.erase(vec.cbegin() + 2);
    REQUIRE(it == vec.begin() + 2);
    REQUIRE(vec.size() == 2);
    REQUIRE(*vec[0] == 2);
    REQUIRE(*vec[1] == 4);
    REQUIRE(five.use_count() == 1);

    REQUIRE(one.use_count() == 1);
    REQUIRE(two.use_count() == 2);
    REQUIRE(three.use_count() == 1);
    REQUIRE(four.use_count() == 2);
    REQUIRE(five.use_count() == 1);

    REQUIRE(vec.capacity() == initial_capacity);
}

TEST_CASE("SmallVector: erase(ConstIterator, ConstIterator)", "[SmallVector]")
{
    auto one = std::make_shared<int>(1);
    auto two = std::make_shared<int>(2);
    auto three = std::make_shared<int>(3);
    auto four = std::make_shared<int>(4);
    auto five = std::make_shared<int>(5);

    SmallVector<std::shared_ptr<int>, 2> vec({ one, two, three, four, five });
    decltype(vec)::Iterator it;
    const auto initial_capacity = vec.capacity();

    REQUIRE(one.use_count() == 2);
    REQUIRE(two.use_count() == 2);
    REQUIRE(three.use_count() == 2);
    REQUIRE(four.use_count() == 2);
    REQUIRE(five.use_count() == 2);

    it = vec.erase(vec.cbegin() + 2, vec.cbegin() + 3);
    REQUIRE(it == vec.begin() + 2);
    REQUIRE(vec.size() == 4);
    REQUIRE(*vec[0] == 1);
    REQUIRE(*vec[1] == 2);
    REQUIRE(*vec[2] == 4);
    REQUIRE(*vec[3] == 5);
    REQUIRE(three.use_count() == 1);

    it = vec.erase(vec.cbegin(), vec.cbegin() + 2);
    REQUIRE(it == vec.begin());
    REQUIRE(vec.size() == 2);
    REQUIRE(*vec[0] == 4);
    REQUIRE(*vec[1] == 5);
    REQUIRE(one.use_count() == 1);
    REQUIRE(two.use_count() == 1);

    it = vec.erase(vec.cbegin() + 2, vec.cbegin() + 2);
    REQUIRE(it == vec.begin() + 2);
    REQUIRE(vec.size() == 2);
    REQUIRE(*vec[0] == 4);
    REQUIRE(*vec[1] == 5);

    it = vec.erase(vec.cbegin() + 1, vec.end());
    REQUIRE(it == vec.begin() + 1);
    REQUIRE(vec.size() == 1);
    REQUIRE(*vec[0] == 4);

    REQUIRE(one.use_count() == 1);
    REQUIRE(two.use_count() == 1);
    REQUIRE(three.use_count() == 1);
    REQUIRE(four.use_count() == 2);
    REQUIRE(five.use_count() == 1);

    REQUIRE(vec.capacity() == initial_capacity);
}

TEST_CASE("SmallVector: end()", "[SmallVector]")
{
    SmallVector<unsigned long, 1> vec;

    REQUIRE(vec.begin() == vec.end());

    vec.push_back(1u);
    REQUIRE(*(std::prev(vec.end())) == 1u);

    vec.push_back(2u);
    REQUIRE(*(std::prev(vec.end())) == 2u);
}

TEST_CASE("SmallVector: front()", "[SmallVector]")
{
    SmallVector<char, 12> vec;

    vec.push_back('a');
    vec.push_back('b');
    REQUIRE(vec.front() == 'a');

    vec.front() = 'A';
    REQUIRE(vec.front() == 'A');
}

TEST_CASE("SmallVector: insert(ConstIterator, const ValueType &)", "[SmallVector]")
{
    SmallVector<std::string, 2> vec;
    decltype(vec)::Iterator it;

    const std::string zero = "zero";
    const std::string pointfive = "0.5";
    const std::string one = "one";
    const std::string two = "two";

    it = vec.insert(vec.begin(), one);
    REQUIRE(it == vec.begin());
    REQUIRE(vec.size() == 1);
    REQUIRE(vec[0] == one);

    it = vec.insert(vec.begin(), zero);
    REQUIRE(it == vec.begin());
    REQUIRE(vec.size() == 2);
    REQUIRE(vec[0] == zero);
    REQUIRE(vec[1] == one);

    it = vec.insert(vec.begin() + 1, pointfive);
    REQUIRE(it == vec.begin() + 1);
    REQUIRE(vec.size() == 3);
    REQUIRE(vec[0] == zero);
    REQUIRE(vec[1] == pointfive);
    REQUIRE(vec[2] == one);

    it = vec.insert(vec.end(), two);
    REQUIRE(it == vec.begin() + 3);
    REQUIRE(vec.size() == 4);
    REQUIRE(vec[0] == zero);
    REQUIRE(vec[1] == pointfive);
    REQUIRE(vec[2] == one);
    REQUIRE(vec[3] == two);


    auto sptr = std::make_shared<int>(42);

    SmallVector<std::shared_ptr<int>, 1> vec2;
    REQUIRE(sptr.use_count() == 1);

    vec2.insert(vec2.begin(), sptr);
    REQUIRE(sptr.use_count() == 2);

    vec2.insert(vec2.begin() + 1, sptr);
    REQUIRE(sptr.use_count() == 3);

    vec2.insert(vec2.end(), sptr);
    REQUIRE(sptr.use_count() == 4);
}

TEST_CASE("SmallVector: insert(ConstIterator, ValueType &&)", "[SmallVector]")
{
    SmallVector<std::string, 2> vec;
    decltype(vec)::Iterator it;

    std::string zero = "zero";
    std::string pointfive = "0.5";
    std::string one = "one";
    std::string two = "two";

    it = vec.insert(vec.begin(), std::move(one));
    REQUIRE(it == vec.begin());
    REQUIRE(vec.size() == 1);
    REQUIRE(vec[0] == "one");

    it = vec.insert(vec.begin(), std::move(zero));
    REQUIRE(it == vec.begin());
    REQUIRE(vec.size() == 2);
    REQUIRE(vec[0] == "zero");
    REQUIRE(vec[1] == "one");

    it = vec.insert(vec.begin() + 1, std::move(pointfive));
    REQUIRE(it == vec.begin() + 1);
    REQUIRE(vec.size() == 3);
    REQUIRE(vec[0] == "zero");
    REQUIRE(vec[1] == "0.5");
    REQUIRE(vec[2] == "one");

    it = vec.insert(vec.end(), std::move(two));
    REQUIRE(it == vec.begin() + 3);
    REQUIRE(vec.size() == 4);
    REQUIRE(vec[0] == "zero");
    REQUIRE(vec[1] == "0.5");
    REQUIRE(vec[2] == "one");
    REQUIRE(vec[3] == "two");


    auto sptr = std::make_shared<int>(42);

    SmallVector<std::shared_ptr<int>, 1> vec2;
    REQUIRE(sptr.use_count() == 1);

    vec2.insert(vec2.begin(), sptr);
    REQUIRE(sptr.use_count() == 2);

    vec2.insert(vec2.begin() + 1, sptr);
    REQUIRE(sptr.use_count() == 3);

    vec2.insert(vec2.end(), sptr);
    REQUIRE(sptr.use_count() == 4);
}

TEST_CASE("SmallVector: insert(ConstIterator, SizeType, const ValueType &)", "[SmallVector]")
{
    SmallVector<int, 10> svec;
    decltype(svec)::Iterator it;

    it = svec.insert(svec.begin(), 2, 42);
    REQUIRE(it == svec.begin());
    REQUIRE(svec.size() == 2);
    REQUIRE(svec[0] == 42);
    REQUIRE(svec[1] == 42);

    it = svec.insert(svec.begin(), 1, 21);
    REQUIRE(it == svec.begin());
    REQUIRE(svec.size() == 3);
    REQUIRE(svec[0] == 21);
    REQUIRE(svec[1] == 42);
    REQUIRE(svec[2] == 42);

    it = svec.insert(svec.begin() + 2, 2, 84);
    REQUIRE(it == svec.begin() + 2);
    REQUIRE(svec.size() == 5);
    REQUIRE(svec[0] == 21);
    REQUIRE(svec[1] == 42);
    REQUIRE(svec[2] == 84);
    REQUIRE(svec[3] == 84);
    REQUIRE(svec[4] == 42);

    svec.clear();
    std::vector<int> vec;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0);

    for (int n = 0; n != 50; ++n)
    {
        int num_elements = dist(gen) % 10;
        int insert_idx =
            (vec.size() == 0) ? 0 : (dist(gen) % static_cast<int>(vec.size()));
        int value = dist(gen) % 100;

        vec.insert(vec.begin() + insert_idx, num_elements, value);

        it = svec.insert(svec.begin() + insert_idx, num_elements, value);
        REQUIRE(it == svec.begin() + insert_idx);
        REQUIRE(svec.size() == vec.size());
    }

    for (int i = 0; i != static_cast<int>(vec.size()); ++i)
        REQUIRE(vec[i] == svec[i]);
}

TEST_CASE("SmallVector: insert(ConstIterator, InputIterator, InputIterator)", "[SmallVector]")
{
    SmallVector<int, 10> svec;
    decltype(svec)::Iterator it;

    const auto val = std::vector<int>({ 1, 2, 3, 4, 5, 6 });
    it = svec.insert(svec.begin(), val.begin(), val.begin() + 2);
    REQUIRE(it == svec.begin());
    REQUIRE(svec.size() == 2);
    REQUIRE(svec[0] == 1);
    REQUIRE(svec[1] == 2);

    it = svec.insert(svec.begin(), val.begin() + 4, val.end());
    REQUIRE(it == svec.begin());
    REQUIRE(svec.size() == 4);
    REQUIRE(svec[0] == 5);
    REQUIRE(svec[1] == 6);
    REQUIRE(svec[2] == 1);
    REQUIRE(svec[3] == 2);

    it = svec.insert(svec.begin() + 2, val.begin() + 2, val.begin() + 4);
    REQUIRE(it == svec.begin() + 2);
    REQUIRE(svec.size() == 6);
    REQUIRE(svec[0] == 5);
    REQUIRE(svec[1] == 6);
    REQUIRE(svec[2] == 3);
    REQUIRE(svec[3] == 4);
    REQUIRE(svec[4] == 1);
    REQUIRE(svec[5] == 2);

    svec.clear();
    std::vector<int> vec;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0);

    for (int n = 0; n != 50; ++n)
    {
        int num_elements = dist(gen) % 10;
        int insert_idx =
            (vec.size() == 0) ? 0 : (dist(gen) % static_cast<int>(vec.size()));

        std::vector<int> new_elements(num_elements);
        std::generate(new_elements.begin(), new_elements.end(), [&]() { return dist(gen) % 100; });

        vec.insert(vec.begin() + insert_idx, new_elements.cbegin(), new_elements.cend());

        it = svec.insert(svec.begin() + insert_idx, new_elements.cbegin(), new_elements.cend());
        REQUIRE(it == svec.begin() + insert_idx);
        REQUIRE(svec.size() == vec.size());
    }

    for (int i = 0; i != static_cast<int>(vec.size()); ++i)
        REQUIRE(vec[i] == svec[i]);
}

TEST_CASE("SmallVector: insert(ConstIterator, std::initializer_list)", "[SmallVector]")
{
    SmallVector<int, 10> vec;
    decltype(vec)::Iterator it;

    std::initializer_list<int> a{ 1, 2, 3 };
    it = vec.insert(vec.begin(), a);
    REQUIRE(it == vec.begin());
    REQUIRE(vec.size() == 3);
    REQUIRE(vec[0] == 1);
    REQUIRE(vec[1] == 2);
    REQUIRE(vec[2] == 3);

    std::initializer_list<int> b{ };
    it = vec.insert(vec.begin() + 1, b);
    REQUIRE(it == vec.begin() + 1);
    REQUIRE(vec.size() == 3);
    REQUIRE(vec[0] == 1);
    REQUIRE(vec[1] == 2);
    REQUIRE(vec[2] == 3);

    std::initializer_list<int> c{ 4, 5, 6 };
    it = vec.insert(vec.begin() + 2, c);
    REQUIRE(it == vec.begin() + 2);
    REQUIRE(vec.size() == 6);
    REQUIRE(vec[0] == 1);
    REQUIRE(vec[1] == 2);
    REQUIRE(vec[2] == 4);
    REQUIRE(vec[3] == 5);
    REQUIRE(vec[4] == 6);
    REQUIRE(vec[5] == 3);
}

TEMPLATE_TEST_CASE("SmallVector: max_size()", "[SmallVector]", int, double, std::string)
{
    SmallVector<TestType, 1> vec;
    REQUIRE(vec.max_size() == std::numeric_limits<size_t>::max());
    REQUIRE(vec.capacity() <= vec.max_size());
    REQUIRE(vec.size() <= vec.max_size());
}

TEST_CASE("SmallVector: operator=(const &)", "[SmallVector]")
{
    constexpr int inner_capacity = 6;

    SECTION("Empty vector") {
        SmallVector<float, inner_capacity> ori;
        SmallVector<float, inner_capacity> vec;
        vec.push_back(1.0f);

        vec = ori;
        REQUIRE(ori.empty());
        REQUIRE(vec.empty());
        REQUIRE(vec.size() == ori.size());
        REQUIRE(std::equal(vec.begin(), vec.end(), ori.begin()));
    }

    SECTION("Integers, capacity > inner_capacity") {
        SmallVector<int, inner_capacity> ori;
        for (int i = 0; i != 10; ++i)
            ori.push_back(i);

        SmallVector<int, inner_capacity> vec;
        vec = ori;
        REQUIRE(ori.size() == 10);
        REQUIRE(vec.size() == ori.size());
        REQUIRE(std::equal(vec.begin(), vec.end(), ori.begin()));
    }

    SECTION("Strings, capacity = inner_capacity") {
        std::list<std::string> list{ "zero"s, "one"s, "two"s, "three"s, "four"s };
        SmallVector<std::string, inner_capacity> ori(list.cbegin(), list.cend());
        SmallVector<std::string, inner_capacity> vec;

        vec = ori;
        REQUIRE(ori.size() == 5);
        REQUIRE(vec.size() == ori.size());
        REQUIRE(std::equal(vec.begin(), vec.end(), ori.begin()));
    }

    SECTION("Self-assignment") {
        SmallVector<long, inner_capacity> vec;
        vec.push_back(42L);

        vec.operator=(vec); // To avoid self-assignment warning from vec = vec
        REQUIRE(vec.size() == 1);
        REQUIRE(vec[0] == 42L);
    }
}

TEST_CASE("SmallVector: operator=(SmallVector &&)", "[SmallVector]")
{
    constexpr int inner_capacity = 2;

    SECTION("Empty vector") {
        SmallVector<float, inner_capacity> ori;
        SmallVector<float, inner_capacity> vec;
        vec = std::move(ori);
        REQUIRE(ori.empty());
        REQUIRE(vec.empty());
        REQUIRE(vec.size() == ori.size());
        REQUIRE(std::equal(vec.begin(), vec.end(), ori.begin()));
    }

    SECTION("Integers, capacity > inner_capacity") {
        SmallVector<int, inner_capacity> ori;
        for (int i = 0; i != 10; ++i)
            ori.push_back(i);
        SmallVector<int, inner_capacity> vec;
        vec = std::move(ori);
        REQUIRE(ori.empty());
        REQUIRE(vec.size() == 10);
        for (int i = 0; i != 10; ++i)
            REQUIRE(vec[i] == i);
    }

    SECTION("Strings, capacity = inner_capacity") {
        std::list<std::string> list{ "zero"s, "one"s, "two"s, "three"s, "four"s };
        SmallVector<std::string, inner_capacity> ori(list.cbegin(), list.cend());

        SmallVector<std::string, inner_capacity> vec;
        vec.push_back("old content");
        vec = std::move(ori);
        REQUIRE(ori.empty());
        REQUIRE(vec.size() == 5);
        REQUIRE(vec[0] == "zero");
        REQUIRE(vec[1] == "one");
        REQUIRE(vec[2] == "two");
        REQUIRE(vec[3] == "three");
        REQUIRE(vec[4] == "four");
    }

    SECTION("unique_ptr, capacity > inner_capacity") {
        SmallVector<std::unique_ptr<int>, inner_capacity> ori;
        for (int i = 0; i != 10; ++i)
            ori.push_back(std::make_unique<int>(i));

        SmallVector<std::unique_ptr<int>, inner_capacity> vec;
        for (int i = 0; i != 20; ++i)
            vec.push_back(std::make_unique<int>(42));
        vec = std::move(ori);
        REQUIRE(ori.empty());
        REQUIRE(vec.size() == 10);
        for (int i = 0; i != 10; ++i)
            REQUIRE(*vec[i] == i);
    }

    SECTION("unique_ptr, capacity < inner_capacity") {
        // Move small vector into big
        SmallVector<std::unique_ptr<int>, inner_capacity> ori;
        for (int i = 0; i != inner_capacity; ++i)
            ori.push_back(std::make_unique<int>(i));

        SmallVector<std::unique_ptr<int>, inner_capacity> vec;
        for (int i = 0; i != inner_capacity * 2; ++i)
            vec.push_back(std::make_unique<int>(42));
        vec = std::move(ori);
        REQUIRE(ori.empty());
        REQUIRE(vec.size() == inner_capacity);
        for (int i = 0; i != inner_capacity; ++i)
            REQUIRE(*vec[i] == i);
    }
}

TEST_CASE("SmallVector: operator=(std::initializer_list)", "[SmallVector]")
{
    SmallVector<float, 2> vec;
    vec = {1.0f, 2.0f, 3.0f};

    REQUIRE(vec.size() == 3);
    REQUIRE(vec[0] == 1.0f);
    REQUIRE(vec[1] == 2.0f);
    REQUIRE(vec[2] == 3.0f);

    SmallVector<std::string, 2> vec2;
    vec2.push_back("old content");

    vec2 = { "zero", "one" };
    REQUIRE(vec2.capacity() == 2);
    REQUIRE(vec2.size() == 2);
    REQUIRE(vec2[0] == "zero");
    REQUIRE(vec2[1] == "one");
}

TEST_CASE("SmallVector: operator==(const SmallVector &)", "[SmallVector]")
{
    SmallVector<int, 3> v1({ 1, 2, 3 });
    SmallVector<int, 3> v2({ 1, 2 });
    SmallVector<int, 3> v3({ 1, 2, 3 });

    REQUIRE((v1 == v1) == true);
    REQUIRE((v1 == v2) == false);
    REQUIRE((v1 == v3) == true);
    REQUIRE((v2 == v1) == false);
    REQUIRE((v2 == v2) == true);
    REQUIRE((v2 == v3) == false);
    REQUIRE((v3 == v1) == true);
    REQUIRE((v3 == v2) == false);
    REQUIRE((v3 == v3) == true);
}

TEMPLATE_TEST_CASE("SmallVector: pop_back()", "[SmallVector]", int, float)
{
    constexpr auto inner_capacity = 2u;

    SmallVector<TestType, inner_capacity> vec;

    for (unsigned int i = 0u; i != inner_capacity; ++i)
        vec.push_back(TestType(i));

    for (unsigned int i = 1u; i <= inner_capacity; ++i)
    {
        vec.pop_back();
        REQUIRE(vec.size() == inner_capacity - i);
        for (unsigned int ii = 0u; ii != vec.size(); ++ii)
            REQUIRE(vec[ii] == TestType(ii));
    }

    for (unsigned int i = 0u; i != 2*inner_capacity; ++i)
        vec.push_back(TestType(i));

    for (unsigned int i = 1u; i <= 2*inner_capacity; ++i)
    {
        vec.pop_back();
        REQUIRE(vec.size() == 2*inner_capacity - i);
        for (unsigned int ii = 0u; ii != vec.size(); ++ii)
            REQUIRE(vec[ii] == TestType(ii));
    }
}

TEST_CASE("SmallVector: push_back() w/ strings", "[SmallVector]")
{
    constexpr auto inner_capacity = 2u;

    SmallVector<std::string, inner_capacity> vec;

    vec.push_back("one");
    vec.push_back("two");
    vec.push_back("three");

    REQUIRE(vec[0] == "one");
    REQUIRE(vec[1] == "two");
    REQUIRE(vec[2] == "three");

    vec[0] = "eins";
    REQUIRE(vec[0] == "eins");
}

TEST_CASE("SmallVector: push_back() w/ unique_ptr (move-only type)", "[SmallVector]")
{
    constexpr auto inner_capacity = 2u;

    SmallVector<std::unique_ptr<int>, inner_capacity> vec;

    vec.push_back(std::make_unique<int>(0));
    vec.push_back(std::make_unique<int>(1));
    vec.push_back(std::make_unique<int>(2));

    REQUIRE(*vec[0] == 0);
    REQUIRE(*vec[1] == 1);
    REQUIRE(*vec[2] == 2);

    vec[0] = nullptr;
    REQUIRE(vec[0] == nullptr);
}

TEST_CASE("SmallVector: push_back(), strong exception guarantee when growing",
    "[SmallVector]")
{
    SmallVector<ThrowingInt, 2> vec;

    vec.push_back(ThrowingInt{ 0 });
    vec.push_back(ThrowingInt{ 1 });

    REQUIRE(vec.size() == 2u);
    REQUIRE(vec[0] == ThrowingInt{ 0 });
    REQUIRE(vec[1] == ThrowingInt{ 1 });

    ThrowingInt::throw_on_move_in(2);
    REQUIRE_NOTHROW(vec.push_back(ThrowingInt{ 2 })); // only 1 move because the resize uses copy
    REQUIRE(vec.size() == 3u);

    REQUIRE_THROWS_AS(vec.push_back(ThrowingInt{ 3 }), std::logic_error); // second move throws
    REQUIRE(vec.size() == 3u);

    REQUIRE(vec.size() == 3u);
    REQUIRE(vec[0] == ThrowingInt{ 0 });
    REQUIRE(vec[1] == ThrowingInt{ 1 });
    REQUIRE(vec[2] == ThrowingInt{ 2 });
}

TEST_CASE("SmallVector: rbegin()", "[SmallVector]")
{
    SmallVector<double, 1> vec;

    REQUIRE_NOTHROW(vec.rbegin());

    vec.push_back('a');
    REQUIRE(*(vec.rbegin()) == 'a');

    vec.push_back('b');
    REQUIRE(*(vec.rbegin()) == 'b');

    *(vec.rbegin()) = 'B';
    REQUIRE(vec[1] == 'B');
}

TEST_CASE("SmallVector: rend()", "[SmallVector]")
{
    SmallVector<std::string, 1> vec;

    REQUIRE(vec.rbegin() == vec.rend());

    vec.push_back("one");
    REQUIRE(*(std::prev(vec.rend())) == "one");

    vec.push_back("two");
    REQUIRE(*(std::prev(vec.rend())) == "one");
}

TEST_CASE("SmallVector: reserve()", "[SmallVector]")
{
    SmallVector<short, 42> vec;

    REQUIRE(vec.capacity() == 42);

    vec.reserve(10);
    REQUIRE(vec.capacity() == 42);

    vec.reserve(42);
    REQUIRE(vec.capacity() == 42);

    vec.reserve(60);
    REQUIRE(vec.capacity() == 60);
}

TEST_CASE("SmallVector: resize(SizeType)", "[SmallVector]")
{
    SmallVector<short, 3> vec;

    vec.resize(2);
    REQUIRE(vec.size() == 2);
    REQUIRE(vec.capacity() == 3);
    REQUIRE(vec[0] == 0);
    REQUIRE(vec[1] == 0);

    vec[0] = 42;
    vec.resize(1);
    REQUIRE(vec.size() == 1);
    REQUIRE(vec.capacity() == 3);
    REQUIRE(vec[0] == 42);

    vec.resize(4);
    REQUIRE(vec.size() == 4);
    REQUIRE(vec.capacity() >= 4);
    REQUIRE(vec[0] == 42);
    REQUIRE(vec[1] == 0);
    REQUIRE(vec[2] == 0);
    REQUIRE(vec[3] == 0);

    vec.resize(0);
    REQUIRE(vec.size() == 0);
    REQUIRE(vec.capacity() >= 4);
}

TEST_CASE("SmallVector: resize(SizeType), strong exception guarantee upon enlarging",
          "[SmallVector]")
{
    REQUIRE(ThrowingInt::global_use_count() == 0);

    SmallVector<ThrowingInt, 3> vec;
    vec.resize(2);
    REQUIRE(vec.size() == 2);
    REQUIRE(vec.capacity() == 3);
    REQUIRE(vec[0] == ThrowingInt{});
    REQUIRE(vec[1] == ThrowingInt{});
    REQUIRE(ThrowingInt::global_use_count() == 2);

    vec[0] = ThrowingInt{ 42 };
    vec[1] = ThrowingInt{ 43 };
    REQUIRE(vec[0] == ThrowingInt{ 42 });
    REQUIRE(vec[1] == ThrowingInt{ 43 });

    ThrowingInt::throw_on_default_construction_in(2);

    REQUIRE_THROWS_AS(vec.resize(5), std::logic_error);
    REQUIRE(vec.size() == 2);
    REQUIRE(vec[0] == ThrowingInt{42});
    REQUIRE(vec[1] == ThrowingInt{43});
    REQUIRE(ThrowingInt::global_use_count() == 2);

    ThrowingInt::throw_on_default_construction_in(-1); // disable throwing

    vec.resize(5);
    REQUIRE(vec.size() == 5);
    REQUIRE(vec.capacity() >= 5);
    REQUIRE(vec[0] == ThrowingInt{42});
    REQUIRE(vec[1] == ThrowingInt{43});
    REQUIRE(vec[2] == ThrowingInt{});
    REQUIRE(vec[3] == ThrowingInt{});
    REQUIRE(vec[4] == ThrowingInt{});
    REQUIRE(ThrowingInt::global_use_count() == 5);
}

TEST_CASE("SmallVector: resize(SizeType, ValueType)", "[SmallVector]")
{
    SECTION("SmallVector<short, 3>")
    {
        SmallVector<short, 3> vec;

        vec.resize(2, 13);
        REQUIRE(vec.size() == 2);
        REQUIRE(vec.capacity() == 3);
        REQUIRE(vec[0] == 13);
        REQUIRE(vec[1] == 13);

        vec[0] = 42;
        vec.resize(1, 13);
        REQUIRE(vec.size() == 1);
        REQUIRE(vec.capacity() == 3);
        REQUIRE(vec[0] == 42);

        vec.resize(4, 13);
        REQUIRE(vec.size() == 4);
        REQUIRE(vec.capacity() >= 4);
        REQUIRE(vec[0] == 42);
        REQUIRE(vec[1] == 13);
        REQUIRE(vec[2] == 13);
        REQUIRE(vec[3] == 13);

        vec.resize(0);
        REQUIRE(vec.size() == 0);
        REQUIRE(vec.capacity() >= 4);
    }

    SECTION("SmallVector<std::shared_ptr<int>, 3>")
    {
        auto sptr = std::make_shared<int>(13);
        REQUIRE(sptr.use_count() == 1);
        SmallVector<std::shared_ptr<int>, 3> vec2;

        vec2.resize(2, sptr);
        REQUIRE(vec2.size() == 2);
        REQUIRE(vec2.capacity() == 3);
        REQUIRE(vec2[0] == sptr);
        REQUIRE(vec2[1] == sptr);
        REQUIRE(sptr.use_count() == 3);

        vec2[0] = std::make_shared<int>(42);
        vec2.resize(1, sptr);
        REQUIRE(vec2.size() == 1);
        REQUIRE(vec2.capacity() == 3);
        REQUIRE(*vec2[0] == 42);
        REQUIRE(sptr.use_count() == 1);

        vec2.resize(4, sptr);
        REQUIRE(vec2.size() == 4);
        REQUIRE(vec2.capacity() >= 4);
        REQUIRE(*vec2[0] == 42);
        REQUIRE(vec2[1] == sptr);
        REQUIRE(vec2[2] == sptr);
        REQUIRE(vec2[3] == sptr);
        REQUIRE(sptr.use_count() == 4);

        vec2.resize(0);
        REQUIRE(vec2.size() == 0);
        REQUIRE(vec2.capacity() >= 4);
        REQUIRE(sptr.use_count() == 1);
    }
}

TEST_CASE("SmallVector: resize(SizeType, ValueType), strong exception guarantee upon "
    "enlarging", "[SmallVector]")
{
    REQUIRE(ThrowingInt::global_use_count() == 0);

    SmallVector<ThrowingInt, 3> vec;
    vec.resize(2, ThrowingInt{ 13 });
    REQUIRE(vec.size() == 2);
    REQUIRE(vec.capacity() == 3);
    REQUIRE(vec[0] == ThrowingInt{ 13 });
    REQUIRE(vec[1] == ThrowingInt{ 13 });
    REQUIRE(ThrowingInt::global_use_count() == 2);

    vec[0] = ThrowingInt{ 42 };
    vec[1] = ThrowingInt{ 43 };
    REQUIRE(vec[0] == ThrowingInt{ 42 });
    REQUIRE(vec[1] == ThrowingInt{ 43 });

    ThrowingInt::throw_on_copy_construction_in(2);

    REQUIRE_THROWS_AS(vec.resize(5, ThrowingInt{ 13 }), std::logic_error);
    REQUIRE(vec.size() == 2);
    REQUIRE(vec[0] == ThrowingInt{ 42 });
    REQUIRE(vec[1] == ThrowingInt{ 43 });
    REQUIRE(ThrowingInt::global_use_count() == 2);

    ThrowingInt::throw_on_copy_construction_in(-1); // disable throwing

    vec.resize(5, ThrowingInt{ 13 });
    REQUIRE(vec.size() == 5);
    REQUIRE(vec.capacity() >= 5);
    REQUIRE(vec[0] == ThrowingInt{ 42 });
    REQUIRE(vec[1] == ThrowingInt{ 43 });
    REQUIRE(vec[2] == ThrowingInt{ 13 });
    REQUIRE(vec[3] == ThrowingInt{ 13 });
    REQUIRE(vec[4] == ThrowingInt{ 13 });
    REQUIRE(ThrowingInt::global_use_count() == 5);
}

TEST_CASE("SmallVector: shrink_to_fit()", "[SmallVector]")
{
    const auto inner_capacity = 2u;

    auto sptr = std::make_shared<int>(42);

    SmallVector<std::shared_ptr<int>, inner_capacity> vec;
    REQUIRE(sptr.use_count() == 1);

    for (auto i_end = 1u; i_end != inner_capacity + 3; ++i_end)
    {
        vec.clear();
        REQUIRE(sptr.use_count() == 1);

        for (auto i = 1u; i <= i_end; ++i)
        {
            vec.push_back(sptr);
            REQUIRE(sptr.use_count() == static_cast<long>(i) + 1);
            REQUIRE(vec.size() == i);
            REQUIRE(vec.capacity() >= i);

            vec.shrink_to_fit();
            REQUIRE(sptr.use_count() == static_cast<long>(i) + 1);
            REQUIRE(vec.size() == i);
            REQUIRE(vec.capacity() == std::max(vec.inner_capacity(), vec.size()));
        }
    }
}

TEMPLATE_TEST_CASE("SmallVector: size()", "[SmallVector]", int, float, std::string)
{
    constexpr auto inner_capacity = 4u;

    SmallVector<TestType, inner_capacity> vec;

    // Fill inner buffer
    for (unsigned int i = 1u; i <= inner_capacity; ++i)
    {
        vec.push_back(TestType{});
        REQUIRE(vec.size() == i);
    }

    // Require growing to the heap
    for (unsigned int i = inner_capacity + 1; i <= 2 * inner_capacity; ++i)
    {
        vec.push_back(TestType{});
        REQUIRE(vec.size() == i);
    }
}

TEST_CASE("SmallVector: swap()", "[SmallVector]")
{
    REQUIRE(ThrowingInt::global_use_count() == 0);

    const std::vector<SmallVector<ThrowingInt, 3>> vectors =
    {
        { ThrowingInt{ 1 }, ThrowingInt{ 2 } }, // internal positive
        { ThrowingInt{ 3 }, ThrowingInt{ 4 }, ThrowingInt{ 5 } }, // internal positive
        { ThrowingInt{ 6 }, ThrowingInt{ 7 }, ThrowingInt{ 8 }, ThrowingInt{ 9 } }, // heap positive
        { ThrowingInt{ -1 }, ThrowingInt{ -2 } }, // internal negative
        { ThrowingInt{ -3 }, ThrowingInt{ -4 }, ThrowingInt{ -5 } }, // internal negative
        { ThrowingInt{ -6 }, ThrowingInt{ -7 }, ThrowingInt{ -8 }, ThrowingInt{ -9 } } // heap negative
    };

    size_t num_ints = 18u;

    REQUIRE(ThrowingInt::global_use_count() == num_ints);

    SECTION("Member function")
    {
        for (const auto& a_ori : vectors)
        {
            for (const auto& b_ori : vectors)
            {
                REQUIRE(ThrowingInt::global_use_count() == num_ints);

                auto a = a_ori;
                auto b = b_ori;

                CAPTURE(a_ori.size());
                CAPTURE(b_ori.size());

                REQUIRE(ThrowingInt::global_use_count() == num_ints + a_ori.size() + b_ori.size());

                a.swap(b);
                REQUIRE(a == b_ori);
                REQUIRE(b == a_ori);
                REQUIRE(a.capacity() == b_ori.capacity());
                REQUIRE(b.capacity() == a_ori.capacity());

                REQUIRE(ThrowingInt::global_use_count() == num_ints + a_ori.size() + b_ori.size());
            }
        }
    }

    SECTION("Free function")
    {
        using std::swap;

        for (const auto& a_ori : vectors)
        {
            for (const auto& b_ori : vectors)
            {
                REQUIRE(ThrowingInt::global_use_count() == num_ints);

                auto a = a_ori;
                auto b = b_ori;

                CAPTURE(a_ori.size());
                CAPTURE(b_ori.size());

                REQUIRE(ThrowingInt::global_use_count() == num_ints + a_ori.size() + b_ori.size());

                swap(a, b);
                REQUIRE(a == b_ori);
                REQUIRE(b == a_ori);
                REQUIRE(a.capacity() == b_ori.capacity());
                REQUIRE(b.capacity() == a_ori.capacity());

                REQUIRE(ThrowingInt::global_use_count() == num_ints + a_ori.size() + b_ori.size());
            }
        }
    }
}

// vi:ts=4:sw=4:sts=4:et
