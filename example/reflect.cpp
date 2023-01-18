//
// Copyright (c) 2022-present DeepGrace (complex dot invoke at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/deepgrace/smp
//

// g++ -I include -m64 -std=c++23 -s -Wall -O3 -o /tmp/reflect example/reflect.cpp

#include <map>
#include <set>
#include <list>
#include <array>
#include <deque>
#include <vector>
#include <cassert>
#include <iostream>
#include <forward_list>
#include <unordered_map>
#include <unordered_set>
#include <reflect.hpp>

// A reflection, marshaling and unmarshaling library enable you to manipulate structure (aggregate initializable
// without empty base classes, const fields, references, or C arrays) elements by index or type and provides other
// std::tuple like methods for user defined types in a non-intrusive manner without any macro or boilerplate code

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

// smp can reflect, marshal and unmarshal fundamental types, UDTS and all STL containers
// see line 534

struct Z
{
    int i;
    double d;
    char c;
    X x;
    X* ptr;
    std::string s;
    std::list<int> ages;
    std::deque<std::string> names;
    std::vector<X> xs;
    std::forward_list<std::vector<int>> ints;
    std::shared_ptr<X> sp;
    std::array<X, 3> arrs;
    std::set<int> sets;
    std::map<int, std::string> maps;
    std::multiset<int> multisets;
    std::multimap<int, std::string> multimaps;
    std::unordered_set<int> unordered_sets;
    std::unordered_map<int, std::string> unordered_maps;
    std::unordered_multiset<int> unordered_multisets;
    std::unordered_multimap<int, std::string> unordered_multimaps;
};

int main(int argc, char* argv[])
{
    X x { 21.3f, "metaprogramming" };
    Y y { 2022, 12.05, '*', { 18.47f, "stateful" } };

    // get a field by index or type

    smp::get<0>(x) = 72.9f;
    smp::get<std::string>(smp::get<X>(y)) = "Stateful";

    assert(x.f == 72.9f);
    assert(y.x.s == "Stateful");

    smp::member_v<1>(x) = "MetaProgramming";
    smp::member_v<X>(y) = { 19.14f, "Stateful Template" };

    assert(x.s == "MetaProgramming");
    assert(y.x.s == "Stateful Template");

    // get a field by member pointer

    x.*smp::get<1, X>() = "smp";
    y.*smp::member_pointer_v<1, Y> = 22.75;

    assert(x.s == "smp");
    assert(y.d == 22.75);

    // extract member pointers to a smp::fuple

    auto xms = smp::member_pointers_v<X>;
    auto yms = smp::member_pointers_v<Y>;

    x.*smp::get<0>(xms) = 23.19f;
    y.*smp::get<3>(yms) = { 10.11f, "smp Library" };

    assert(x.f == 23.19f);
    assert(y.x.s == "smp Library");

    // extract the nth member by base class type

    using x_base = smp::element<3, X&>;
    using f_base = smp::element<0, float&>;

    // turn to smp::fuple by reference

    auto fy = smp::tie_fuple(y);
    auto fx = smp::tie_fuple(fy.x_base::value);

    fx.f_base::value = 20.44f;
    assert(y.x.f == 20.44f);

    assert(smp::index(&X::s) == 1);
    assert(smp::index(&Y::x) == 3);

    // get the number of members

    static_assert(smp::arity_v<X> == 2);
    static_assert(smp::arity_v<Y> == 4);

    static_assert(smp::tuple_size_v<X> == 2);
    static_assert(smp::tuple_size_v<Y> == 4);

    // extract the object type from a member pointer type

    static_assert(std::is_same_v<smp::object_t<float X::*>, X>);
    static_assert(std::is_same_v<smp::object_t<double Y::*>, Y>);

    // extract the nth member type

    static_assert(std::is_same_v<smp::member_t<0, X>, float>);
    static_assert(std::is_same_v<smp::member_t<1, Y>, double>);

    static_assert(std::is_same_v<smp::tuple_element_t<1, X>, std::string>);
    static_assert(std::is_same_v<smp::tuple_element_t<2, Y>, char>);

    // extract the member types

    static_assert(std::is_same_v<smp::members_t<X>, smp::fuple<float, std::string>>);
    static_assert(std::is_same_v<smp::members_t<Y>, smp::fuple<int, double, char, X>>);

    // extract the nth member pointer type

    static_assert(std::is_same_v<smp::member_pointer_t<1, X>, std::string X::*>);
    static_assert(std::is_same_v<smp::member_pointer_t<2, Y>, char Y::*>);

    // extract the member pointer types

    static_assert(std::is_same_v<smp::member_pointers_t<X>, smp::fuple<float X::*, std::string X::*>>);
    static_assert(std::is_same_v<smp::member_pointers_t<Y>, smp::fuple<int Y::*, double Y::*, char Y::*, X Y::*>>);

    // turn to smp::fuple or std::tuple by value

    auto f = smp::to_fuple(x);
    auto t = smp::to_tuple(y);

    // fill a structure from a fuple or tuple

    Y y0;

    auto x0 = smp::fill<X>(f);
    smp::fill(y0, t);

    assert(x.f == x0.f);
    assert(x.s == x0.s);

    assert(y.i == y0.i);
    assert(y.d == y0.d);

    // fill a structure from a pack

    Y y1;

    auto x1 = smp::fill<X>(12.98f, std::string("filled from args"));
    smp::fill(y1, 2987, 20.21, '_', X{ 18.21f, std::string("Variadic") });

    assert(x1.f == 12.98f);
    assert(y1.x.s == "Variadic");

    smp::get<0>(f) = 25.67f;
    std::get<1>(t) = 70.71;

    assert(x.f == 23.19f);
    assert(smp::get<0>(f) == 25.67f);

    assert(y.d == 22.75);
    assert(std::get<1>(t) == 70.71);

    // turn to smp::fuple or std::tuple by reference

    auto rf = smp::tie_fuple(x);
    auto rt = smp::tie_tuple(y);

    // fill a structure from a fuple or tuple

    X x2;

    smp::fill(x2, rf);
    auto y2 = smp::fill<Y>(rt);

    assert(x.f == x2.f);
    assert(x.s == x2.s);

    assert(y.i == y2.i);
    assert(y.d == y2.d);

    smp::get<1>(rf) = "TMP";
    std::get<3>(rt) = { 87.45f, "Black Magic" };

    assert(x.s == "TMP");

    assert(y.x.f == 87.45f);
    assert(y.x.s == "Black Magic");

    // marshal and unmarshal a structure

    std::string ys = smp::marshal(y);

    // or
    // auto ys = smp::marshal(smp::tie_fuple(y));
    // auto ys = smp::marshal(smp::tie_tuple(y));

    Y yf = smp::unmarshal<Y>(ys);

    // or
    // auto yf = smp::unmarshal<Y>(ys);

    assert(yf.i == y.i);
    assert(yf.d == y.d);

    assert(yf.c == y.c);
    assert(yf.x.f == y.x.f);

    assert(yf.x.s == y.x.s);

    // marshal and unmarshal a smp::fuple or std::tuple

    auto mf = smp::make_fuple(1, 2.0f, std::string("marshal"), 'X', smp::make_fuple(100.3, std::string("Unmarshal")));
    auto mt = std::make_tuple(2, 3.0f, std::string("Marshal"), 'Y', std::make_tuple(200.3, std::string("unmarshal")));

    std::string fs = smp::marshal(mf);
    std::string ts = smp::marshal(mt);

    auto fu = smp::unmarshal<decltype(mf)>(fs);
    auto tu = smp::unmarshal<decltype(mt)>(ts);

    assert(fu == mf);
    assert(tu == mt);

    assert(smp::get<std::string>(smp::get<4>(fu)) == smp::get<1>(smp::get<4>(mf)));
    assert(std::get<1>(std::get<4>(tu)) == std::get<std::string>(std::get<4>(mt)));

    // marshal and unmarshal a smp::fuple or std::tuple that mixed together

    auto mf0 = smp::make_fuple(1, 2.0f, std::string("marshal"), 'X', std::make_tuple(100.3, std::string("Unmarshal")));
    auto mt0 = std::make_tuple(2, 3.0f, std::string("Marshal"), 'Y', smp::make_fuple(200.3, std::string("unmarshal")));

    auto fs0 = smp::marshal(mf0);
    auto ts0 = smp::marshal(mt0);

    auto fu0 = smp::unmarshal<decltype(mf0)>(fs0);
    auto tu0 = smp::unmarshal<decltype(mt0)>(ts0);

    assert(fu0 == mf0);
    assert(tu0 == mt0);

    assert(std::get<std::string>(smp::get<4>(fu0)) == std::get<1>(smp::get<4>(mf0)));
    assert(smp::get<1>(std::get<4>(tu0)) == smp::get<std::string>(std::get<4>(mt0)));

    // stream operators

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

    // compare methods

    std::cout << "smp::lt " << smp::lt(w1, w2) << std::endl;
    std::cout << "smp::le " << smp::le(w1, w2) << std::endl;

    std::cout << "smp::ne " << smp::ne(w1, w2) << std::endl;
    std::cout << "smp::eq " << smp::eq(w1, w2) << std::endl;

    std::cout << "smp::ge " << smp::ge(w1, w2) << std::endl;
    std::cout << "smp::gt " << smp::gt(w1, w2) << std::endl;

    // turn to smp::fuple or std::tuple by reference

    auto f1 = smp::tie_fuple(w1);
    auto f2 = smp::tie_fuple(w2);

    auto t1 = smp::tie_tuple(w1);
    auto t2 = smp::tie_tuple(w2);

    // compare operators

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

    // assign to tied values from aggregates, can tie to smp::fuple or std::tuple
    // create a smp::fuple or std::tuple of lvalue references to its arguments or instances of smp::ignore.

    /* structured binding with a pack in C++26
    auto&& [...args] = f();

    smp::ref_fuple(args...) = f();
    smp::ref_tuple(args...) = f();
    */

    smp::ref_fuple(fr, fk, fv) = w1;
    smp::ref_tuple(tr, tk, tv) = w2;

    /* or with ignore
    smp::ref_fuple(fr, fk, smp::ignore) = w1;
    smp::ref_tuple(tr, tk, smp::Ignore) = w2;
    */

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

    // apply a function to each field

    smp::for_each(print_backward, w1);
    std::cout << std::endl;

    smp::for_each(print_backward, w2);
    std::cout << std::endl;

    // for each member pointer

    auto mptrs_backward = []<typename T>(T&& t)
    {
        return [&]<typename... Args>(Args&&... args)
        {
            const char* sep = " ";

            ((std::cout << t.*args << sep, sep) = ... = " ");
            std::cout << std::endl;
        };
    };

    smp::for_mptr(mptrs_backward(w1), w1);
    std::cout << std::endl;

    smp::for_mptr(mptrs_backward(w2), w2);
    std::cout << std::endl;

    // apply a function to all fields

    smp::apply(print_backward, w1);
    smp::apply(print_backward, w2);

    // apply a function to every nth field of aggregates, takes the longest common fields number

    std::cout << std::endl;
    smp::zip(print_backward, w1, w2, x, y);

    std::vector<int> v1 { 0, 1, 2, 3 };
    std::vector<int> v2 { 4, 5, 6, 7, 8 };

    std::set<int> sets;
    sets.insert(3);
    sets.insert(2);
    sets.insert(5);

    std::map<int, std::string> maps;
    maps.insert({2, "two"});
    maps.insert({1, "one"});
    maps.insert({5, "five"});

    std::multiset<int> multisets;
    multisets.insert(3);
    multisets.insert(2);
    multisets.insert(2);
    multisets.insert(5);
    multisets.insert(5);

    std::multimap<int, std::string> multimaps;
    multimaps.insert({4, "four"});
    multimaps.insert({4, "four2"});
    multimaps.insert({3, "three"});
    multimaps.insert({3, "three2"});
    multimaps.insert({0, "zero"});
    multimaps.insert({6, "SIX"});
    multimaps.insert({6, "SIX2"});
    multimaps.insert({5, "five"});

    std::unordered_set<int> unordered_sets;
    unordered_sets.insert(3);
    unordered_sets.insert(2);
    unordered_sets.insert(5);

    std::unordered_map<int, std::string> unordered_maps;
    unordered_maps.insert({2, "two"});
    unordered_maps.insert({1, "one"});
    unordered_maps.insert({5, "five"});

    std::unordered_multiset<int> unordered_multisets;
    unordered_multisets.insert(4);
    unordered_multisets.insert(3);
    unordered_multisets.insert(3);

    std::unordered_multimap<int, std::string> unordered_multimaps;
    unordered_multimaps.insert({5, "two"});
    unordered_multimaps.insert({2, "one"});
    unordered_multimaps.insert({2, "five"});

    // a sophisticated object

    Z z1 { 18, 9.87, '*', X{ 53.86f, "^_^" }, &x, "TMP", { 1, 3, 6 }, { "smp", "C++", "template" }, { x, x, x }, { v1, v2 },
          std::make_shared<X>(15.18f, "reflect"), { x, x, x }, sets, maps, multisets, multimaps, unordered_sets,
          unordered_maps, unordered_multisets, unordered_multimaps };

    auto printz = [](const Z& z, const std::string& s)
    {
        std::cout << std::endl;
        std::cout << s << " ptr is " << smp::get<4>(z)->f << " " << smp::get<4>(z)->s << std::endl;
        for (auto& n : smp::get<6>(z))
             std::cout << s << " ages is " << n << std::endl;
        for (auto& n : smp::get<7>(z))
             std::cout << s << " names is " << n << std::endl;
        for (auto& n : smp::get<8>(z))
             std::cout << s << " xs is " << n.f << " " << n.s << std::endl;
        for (auto& v : smp::get<9>(z))
             for (auto& n : v)
                  std::cout << s << " ints is " << n << std::endl;
        std::cout << s << " sp.f is " << smp::get<10>(z)->f << std::endl;
        std::cout << s << " sp.s is " << smp::get<10>(z)->s << std::endl;
        for (auto& n : smp::get<11>(z))
             std::cout << s << " arrs is " << n.f << " " << n.s << std::endl;
        for (auto& n : smp::get<12>(z))
             std::cout << s << " sets is " << n << std::endl;
        for (auto& [k, v] : smp::get<13>(z))
             std::cout << s << " maps is " << k << " " << v << std::endl;
        for (auto& n : smp::get<14>(z))
             std::cout << s << " multisets is " << n << std::endl;
        for (auto& [k, v] : smp::get<15>(z))
             std::cout << s << " multimaps is " << k << " " << v << std::endl;
        for (auto& n : smp::get<16>(z))
             std::cout << s << " unordered_sets is " << n << std::endl;
        for (auto& [k, v] : smp::get<17>(z))
             std::cout << s << " unordered_maps is " << k << " " << v << std::endl;
        for (auto& n : smp::get<18>(z))
             std::cout << s << " unordered_multisets is " << n << std::endl;
        for (auto& [k, v] : smp::get<19>(z))
             std::cout << s << " unordered_multimaps is " << k << " " << v << std::endl;
    };

    static_assert(smp::arity_v<Z> == 20);

    printz(z1, "z1");

    // marshal a sophisticated object
    std::string zstr1 = smp::marshal(z1);

    // unmarshal a sophisticated object
    auto z2 = smp::unmarshal<Z>(zstr1);

    // z1 == z2
    printz(z2, "z2");

    // marshal range of a sophisticated object
    std::string zstr2 = smp::marshal<0, 20>(z2);

    // unmarshal range of a sophisticated object
    auto z3 = smp::unmarshal<0, 20, Z>(zstr2);

    // z2 == z3
    printz(z3, "z3");

    return 0;
}
