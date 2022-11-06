//
// Copyright (c) 2022-present DeepGrace (complex dot invoke at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/deepgrace/smp
//

// g++ -I include -m64 -std=c++23 -s -Wall -O3 -o /tmp/visitor example/visitor.cpp

#include <string>
#include <cassert>
#include <visitor.hpp>

// access structure elements by member types

struct X
{
    float f;
    std::string s;
};

struct Y
{
    int i;
    double d;
    char c;
    X x;
};

int main(int argc, char* argv[])
{
    Y y { 2022, 11.03, '|', { 15.9, "template" } };

    // member types can be less then fields

    using type = smp::fuple<int, double, char, X>;
    smp::visitor<type> v;

    static_assert(v.size() == 4);
    static_assert(smp::fuple_size_v<type> == 4);

    static_assert(std::is_same_v<smp::fuple_element_t<0, type>, int>);
    static_assert(std::is_same_v<smp::fuple_element_t<2, type>, char>);

    // extract the nth member by reference 

    v.get<0>(y) = 2023;
    v.get<3>(y).s = "Template";

    assert(y.i == 2023);
    assert(y.x.s == "Template");

    // turn to smp::fuple or std::tuple by value

    auto f = v.get<1, 0>(y);
    auto t = v.get<0, 0>(y);

    y.d = 4.28;

    assert(v.get<1>(y) == 4.28);

    assert(y.d != smp::get<1>(f));
    assert(y.d != std::get<1>(t));

    assert(smp::get<1>(f) == std::get<1>(t));

    // extract the nth member by reference 

    smp::get<2>(f) = 'F';
    std::get<2>(t) = 'T';

    assert(smp::get<2>(f) == 'F');
    assert(std::get<2>(t) == 'T');

    assert(y.c == '|');

    // turn to smp::fuple or std::tuple by reference

    auto rf = v.get<1, 1>(y);
    auto rt = v.get<0, 1>(y);

    smp::get<3>(rf).f = 11.8f;
    std::get<3>(rt).s = "MetaProgramming";

    assert(y.x.f == 11.8f);
    assert(y.x.s == "MetaProgramming");

    return 0;
}
