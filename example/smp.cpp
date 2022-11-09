//
// Copyright (c) 2022-present DeepGrace (complex dot invoke at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/deepgrace/smp
//

// g++ -I include -m64 -std=c++23 -s -Wall -O3 -o /tmp/smp example/smp.cpp

#include <cassert>
#include <iostream>
#include <smp.hpp>

struct W
{
    int rank;
    std::string key;
    std::string val;
};

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
    // reflect part

    X x { 15.32f, "template" };
    Y y { 2048, 12.14, '+', { 27.85f, "smp" } };

    x.*smp::get<0, X>() = 30.18f;
    smp::get<1>(smp::get<X>(y)) = "Stateful";

    assert(x.f == 30.18f);
    assert(y.x.s == "Stateful");

    assert(smp::index(&X::s) == 1);
    assert(smp::index(&Y::x) == 3);

    static_assert(smp::tuple_size_v<X> == 2);
    static_assert(smp::tuple_size_v<Y> == 4);

    static_assert(std::is_same_v<smp::tuple_element_t<0, X>, float>);
    static_assert(std::is_same_v<smp::tuple_element_t<1, Y>, double>);

    auto f = smp::tie_fuple(x);
    auto t = smp::tie_tuple(y);

    smp::get<std::string&>(f) = "Dark Army";
    std::get<X&>(t) = { 72.68f, "Programs" };

    assert(x.s == "Dark Army");

    assert(y.x.f == 72.68f);
    assert(y.x.s == "Programs");

    x.*smp::get<0, X>() = 48.65f;
    x.*smp::get<1, X>() = "transformer";

    assert(smp::get<1>(f) == "transformer");
    assert(smp::get<float>(f) == 48.65f);

    y.*smp::get<3, Y>() = { 25.65f, "Template" };

    assert(std::get<3>(t).f == 25.65f);
    assert(std::get<X&>(t).s == "Template");

    W w1;
    W w2;

    std::stringstream ss1;
    std::stringstream ss2;

    std::string str1 = "100, \"Modern C++ Template\", \"MetaProgramming Library\"";
    std::string str2 = "101, \"Modern C++ Stateful\", \"MetaProgramming Framework\"";

    ss1 << str1;
    ss1 >> smp::io(w1);

    ss2 << str2;
    ss2 >> smp::io(w2);

    std::cout << smp::io(w1) << std::endl;
    std::cout << smp::io(w2) << std::endl;

    /* outputs
    100, "Modern C++ Template", "MetaProgramming Library"
    101, "Modern C++ Stateful", "MetaProgramming Framework"
    */

    assert(w1.rank == 100);
    assert(w2.rank == 101);

    assert(w1.key == "Modern C++ Template");
    assert(w2.key == "Modern C++ Stateful");

    assert(w1.val == "MetaProgramming Library");
    assert(w2.val == "MetaProgramming Framework");

    std::cout << "smp::lt " << smp::lt(w1, w2) << std::endl;
    std::cout << "smp::le " << smp::le(w1, w2) << std::endl;

    std::cout << "smp::ne " << smp::ne(w1, w2) << std::endl;
    std::cout << "smp::eq " << smp::eq(w1, w2) << std::endl;

    std::cout << "smp::ge " << smp::ge(w1, w2) << std::endl;
    std::cout << "smp::gt " << smp::gt(w1, w2) << std::endl;

    auto f1 = smp::tie_fuple(w1);
    auto f2 = smp::tie_fuple(w2);

    auto t1 = smp::tie_tuple(w1);
    auto t2 = smp::tie_tuple(w2);

    std::cout << "f1 <  f2 " << (f1 <  f2) << std::endl;
    std::cout << "f1 <= f2 " << (f1 <= f2) << std::endl;

    std::cout << "f1 != f2 " << (f1 != f2) << std::endl;
    std::cout << "f1 == f2 " << (f1 == f2) << std::endl;

    std::cout << "f1 >= f2 " << (f1 >= f2) << std::endl;
    std::cout << "f1 >  f2 " << (f1 >  f2) << std::endl;

    std::cout << "t1 <  t2 " << (t1 <  t2) << std::endl;
    std::cout << "t1 <= t2 " << (t1 <= t2) << std::endl;

    std::cout << "t1 != t2 " << (t1 != t2) << std::endl;
    std::cout << "t1 == t2 " << (t1 == t2) << std::endl;

    std::cout << "t1 >= t2 " << (t1 >= t2) << std::endl;
    std::cout << "t1 >  t2 " << (t1 >  t2) << std::endl;

    int fr = 0;
    int tr = 0;

    std::string fk;
    std::string tk;

    std::string fv;
    std::string tv;

    smp::ref_fuple(fr, fk, fv) = w1;
    smp::ref_tuple(tr, tk, tv) = w2;

    assert(fr == 100);
    assert(tr == 101);

    assert(fk == "Modern C++ Template");
    assert(tk == "Modern C++ Stateful");

    assert(fv == "MetaProgramming Library");
    assert(tv == "MetaProgramming Framework");

    auto print_backward = []<typename... Args>(Args&&... args)
    {
        const char* sep = " ";

        ((std::cout << args << sep, sep) = ... = " ");
        std::cout << std::endl;
    };

    smp::for_each(print_backward, w1);
    std::cout << std::endl;

    smp::for_each(print_backward, w2);
    std::cout << std::endl;

    smp::apply(print_backward, w1);
    smp::apply(print_backward, w2);

    std::cout << std::endl;
    smp::zip(print_backward, w1, w2, x, y);

    // indexer part

    static_assert(smp::next<>() == 0);
    static_assert(smp::next<>() == 1);

    static_assert(smp::next<>() == 2);
    static_assert(smp::next<>() == 3);

    static_assert(std::is_same_v<smp::index_sequence_for<>, std::index_sequence<>>);
    static_assert(std::is_same_v<smp::index_sequence_for<int, float, char, int>, std::index_sequence<0, 1, 2, 3>>);

    static_assert(std::is_same_v<smp::take<>, smp::lists<void, void, void, void, int, float, char, int>>);
    smp::clear<>();

    smp::push_front<int>();
    smp::push_back<char>();

    static_assert(std::is_same_v<smp::take<>, smp::lists<int, char>>);

    static_assert(smp::next<>() == 11);
    static_assert(smp::next<>() == 12);

    return 0;
}
