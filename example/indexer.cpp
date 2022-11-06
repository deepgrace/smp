//
// Copyright (c) 2022-present DeepGrace (complex dot invoke at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/deepgrace/smp
//

// g++ -I include -m64 -std=c++23 -s -Wall -O3 -o /tmp/indexer example/indexer.cpp

#include <indexer.hpp>

// A compile time type list and index sequence generator

int main(int argc, char* argv[])
{
    // get current index
    static_assert(smp::curr<>() == 0);

    // get size of indices
    static_assert(smp::size<>() == 0);

    // get current type list
    static_assert(std::is_same_v<smp::take<>, smp::lists<>>);

    // get next index

    static_assert(smp::next<>() == 0);
    static_assert(smp::next<>() == 1);

    static_assert(smp::next<>() == 2);
    static_assert(smp::next<>() == 3);

    // get current index
    static_assert(smp::curr<>() == 3);

    // get size of indices
    static_assert(smp::size<>() == 4);

    // get current type list
    static_assert(std::is_same_v<smp::take<>, smp::lists<void, void, void, void>>);

    // push types

    smp::push_front<int>();
    smp::push_back<char>();

    // get current index
    static_assert(smp::curr<>() == 5);

    // get size of indices
    static_assert(smp::size<>() == 6);

    // get current type list
    static_assert(std::is_same_v<smp::take<>, smp::lists<int, void, void, void, void, char>>);

    // clear current type list
    smp::clear<>();

    // get current index
    static_assert(smp::curr<>() == 6);

    // get size of indices
    static_assert(smp::size<>() == 7);

    // get current type list
    static_assert(std::is_same_v<smp::take<>, smp::lists<>>);

    // push types

    smp::push_front<short>();
    smp::push_back<double>();

    // get current type list
    static_assert(std::is_same_v<smp::take<>, smp::lists<short, double>>);

    // get next index
    static_assert(smp::next<>() == 9);

    // get current index
    static_assert(smp::curr<>() == 9);

    // get size of indices
    static_assert(smp::size<>() == 10);

    // get current type list
    static_assert(std::is_same_v<smp::take<>, smp::lists<short, double, void>>);

    // index sequence for types
    static_assert(std::is_same_v<smp::index_sequence_for<int, float, char, int>, std::index_sequence<0, 1, 2, 3>>);

    // get current index
    static_assert(smp::curr<>() == 13);

    // get size of indices
    static_assert(smp::size<>() == 14);

    // get current type list
    static_assert(std::is_same_v<smp::take<>, smp::lists<short, double, void, int, float, char, int>>);

    // index sequence for types
    static_assert(std::is_same_v<smp::index_sequence_for<>, std::index_sequence<>>);

    // get current index
    static_assert(smp::curr<>() == 13);

    // get size of indices
    static_assert(smp::size<>() == 14);

    // get current type list
    static_assert(std::is_same_v<smp::take<>, smp::lists<short, double, void, int, float, char, int>>);

    // push types

    smp::push_front<char>();
    smp::push_back<double>();

    // get current index
    static_assert(smp::curr<>() == 15);

    // get size of indices
    static_assert(smp::size<>() == 16);

    // get current type list
    static_assert(std::is_same_v<smp::take<>, smp::lists<char, short, double, void, int, float, char, int, double>>);

    // index sequence for types

    static_assert(std::is_same_v<smp::index_sequence_for<>, std::index_sequence<>>);
    static_assert(std::is_same_v<smp::index_sequence_for<int, char>, std::index_sequence<0, 1>>);

    // clear current type list
    smp::clear<>();

    // get current index
    static_assert(smp::curr<>() == 18);

    // get size of indices
    static_assert(smp::size<>() == 19);

    // get current type list
    static_assert(std::is_same_v<smp::take<>, smp::lists<>>);

    // get next index
    static_assert(smp::next<>() == 19);

    // get current index
    static_assert(smp::curr<>() == 19);

    // get size of indices
    static_assert(smp::size<>() == 20);

    // get current type list
    static_assert(std::is_same_v<smp::take<>, smp::lists<void>>);

    // push types

    smp::push_back<int>();
    smp::push_back<std::nullptr_t>();

    smp::push_front<short>();
    smp::push_front<size_t>();

    // get current type list
    static_assert(std::is_same_v<smp::take<>, smp::lists<size_t, short, void, int, std::nullptr_t>>);

    // get current index
    static_assert(smp::curr<>() == 23);

    // get size of indices
    static_assert(smp::size<>() == 24);

    // get next index
    static_assert(smp::next<>() == 24);

    // get current type list
    static_assert(std::is_same_v<smp::take<>, smp::lists<size_t, short, void, int, std::nullptr_t, void>>);

    return 0;
}
