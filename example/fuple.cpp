//
// Copyright (c) 2022-present DeepGrace (complex dot invoke at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/deepgrace/smp
//

// g++ -I include -m64 -std=c++23 -s -Wall -O3 -o /tmp/fuple example/fuple.cpp

#include <cassert>
#include <iostream>
#include <fuple.hpp>

// fuple (flat tuple) implemented with multiple inheritance is a drop-in replacement for std::tuple

int main(int argc, char* argv[])
{
    smp::fuple f0;
    smp::fuple f1(std::string("C++23"));

    int i = 2022;
    double j = 11.05;

    smp::fuple<short, char> f2;
    smp::fuple<int&, smp::fuple<double&>, char> f3(i, smp::tie(j), 'X');

    int p = 23;
    std::string s = "tmp";

    smp::tie(p, s, smp::ignore) = smp::make_fuple(45, std::string("C++ TMP"), 9.8);

    assert(p == 45);
    assert(s == "C++ TMP");

    auto f4 = smp::make_fuple(j, std::string("template"));
    auto f5 = smp::forward_as_fuple(p, s, std::string("smp"));

    auto f6 = smp::fuple_cat(f0, f1, f2, f3, f4, f5);

    smp::get<int>(f5) = 75;
    smp::get<1>(f5) = std::string("c++ tmp");

    assert(p == 75);
    assert(s == "c++ tmp");

    using type = decltype(f6);

    static_assert(smp::is_fuple_v<type>);
    static_assert(smp::fuple_size_v<type> == 11);

    static_assert(std::is_same_v<smp::fuple_element_t<0, type>, std::string>);
    static_assert(std::is_same_v<smp::fuple_element_t<4, type>, smp::fuple<double&>>);

    // extract the nth member by base class type

    assert((f3.smp::element<0, int&>::value == 2022));
    assert((f3.smp::element<1, smp::fuple<double&>>::value.smp::element<0, double&>::value == 11.05));

    smp::fuple f(1, 3, 2, 'X', std::string("template"), 11.2d);
    smp::fuple g(4, 7, 8, 'Y', std::string("C++23"), 22.4d);

    auto w = smp::make_fuple(f, g);
    using element_t = smp::element<4, std::string>;

    assert(f.element_t::value == "template");

    assert((w.smp::element<0, decltype(f)>::value.element_t::value == "template"));
    assert((w.smp::element<1, decltype(f)>::value.element_t::value == "C++23"));

    using triple = smp::fuple<int, double, std::string>;
    triple f7(20, 11.06, "smp");

    smp::fuple f8(f7);
    smp::fuple f9 = f8;

    triple f10(std::move(f9));
    triple f11(30, 22.12, "smp");

    triple f12(40, 9.23, "tmp");
    triple f13(50, 12.77, "cpp");

    f10 = f11;
    smp::swap(f11, f12);

    assert(smp::get<double>(f8) == 11.06);
    assert(smp::get<std::string>(f9) == "");

    assert(smp::get<int>(f10) == 30);
    assert(smp::get<1>(f11) == 9.23);

    assert(smp::get<2>(f12) == "smp");

    auto print_backward = []<typename... Args>(Args&&... args)
    {
        const char* sep = " ";
        ((std::cout << args << sep, sep) = ... = " ");

        std::cout << std::endl;
    };

    smp::for_each(print_backward, f12);
    std::cout << std::endl;

    smp::for_each(print_backward, f11);
    std::cout << std::endl;

    smp::for_each(print_backward, f13);
    std::cout << std::endl;

    smp::apply(print_backward, f12);
    smp::apply(print_backward, f11);

    smp::apply(print_backward, f13);
    std::cout << std::endl;

    // apply a function to every nth field of std::fuples, takes the longest common fields number

    smp::zip(print_backward, f12, f11, f13);
    std::cout << std::endl;

    std::cout << "f12 <  f13 " << (f12 <  f13) << std::endl;
    std::cout << "f12 <= f13 " << (f12 <= f13) << std::endl;

    std::cout << "f12 != f13 " << (f12 != f13) << std::endl;
    std::cout << "f12 == f13 " << (f12 == f13) << std::endl;

    std::cout << "f12 >= f13 " << (f12 >= f13) << std::endl;
    std::cout << "f12 >  f13 " << (f12 >  f13) << std::endl;

    // convert a fuple to a tuple or vice versa

    auto mf = smp::make_fuple(9, 's');
    auto mt = std::make_tuple(9, 's');

    auto ft = smp::fuple_to_tuple(mf);
    auto tf = smp::tuple_to_fuple(mt);

    assert(mf == tf);
    assert(mt == ft);

    return 0;
}
