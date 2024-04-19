//
// Copyright (c) 2022-present DeepGrace (complex dot invoke at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/deepgrace/smp
//

// g++ -I include -m64 -std=c++23 -s -Wall -O3 -o /tmp/invocable_name example/invocable_name.cpp

#include <tuple>
#include <iostream>
#include <invocable_name.hpp>

enum Week
{
    Su, Mo, Tu, We, Th, Fr, Sa
};

int push(int);

struct X{};

class Y
{
public:
    X pull(int, X);
};

namespace K
{

class Z
{
public:
    X pull(int, X);
    int member;
};

struct U
{
   struct V
   {
   };
};

};

namespace loc
{

template <auto f>
consteval decltype(auto) pretty_function()
{
    return std::string_view{__PRETTY_FUNCTION__};
}

}

namespace nest1
{
   auto f = []<typename... Args>(Args&&... args){};
}

namespace nest2
{
   auto f = []<typename... Args>(Args&&... args){};
}

namespace a::b::c
{
    template <typename T, template <typename ...> typename F>
    T tmp(T a, T b)
    {
       return T{};
    }
}

namespace d::e::f
{
    template <typename T, template <typename ...> typename F>
    T tmp(T a, T b)
    {
       return T{};
    }
}

int main(int argc, char* argv[])
{
    static_assert(smp::arity<Week>() == 7);

    constexpr int k = 3;

    std::cout << "invocable_name" << std::endl;
    std::cout << smp::invocable_name<main>() << std::endl;
    std::cout << smp::invocable_name<push>() << std::endl;
    std::cout << smp::invocable_name<&Y::pull>() << std::endl;
    std::cout << smp::invocable_name<&K::Z::pull>() << std::endl;
    std::cout << smp::invocable_name<&K::Z::member>() << std::endl;
    std::cout << smp::invocable_name<a::b::c::tmp<K::U::V, std::tuple>>() << std::endl;
    std::cout << smp::invocable_name<d::e::f::tmp<K::U::V, std::tuple>>() << std::endl;
    std::cout << smp::invocable_name<a::b::c::tmp<std::tuple<int>, std::tuple>>() << std::endl;
    std::cout << smp::invocable_name<d::e::f::tmp<std::tuple<int>, std::tuple>>() << std::endl;
    std::cout << smp::invocable_name<nest1::f>() << std::endl;
    std::cout << smp::invocable_name<nest2::f>() << std::endl;
    std::cout << smp::invocable_name<[]{}>() << std::endl;
    std::cout << smp::invocable_name<[]<auto i, typename T>(T a, T b){}>() << std::endl;
    std::cout << smp::invocable_name<[]<typename... Args>(Args&&... args){}>() << std::endl;
    std::cout << smp::invocable_name<[]<int i, double d>(float a, char b){}>() << std::endl;
    std::cout << smp::invocable_name_v<[]<float i, typename T>(char a, T t){}> << std::endl;
    std::cout << smp::invocable_name_v<[k]<auto... N, typename T>(int a, T t){}> << std::endl;

    std::cout << std::endl << "invocable_sign" << std::endl;
    std::cout << smp::invocable_sign<main>() << std::endl;
    std::cout << smp::invocable_sign<push>() << std::endl;
    std::cout << smp::invocable_sign<&Y::pull>() << std::endl;
    std::cout << smp::invocable_sign<&K::Z::pull>() << std::endl;
    std::cout << smp::invocable_sign<&K::Z::member>() << std::endl;
    std::cout << smp::invocable_sign<a::b::c::tmp<K::U::V, std::tuple>>() << std::endl;
    std::cout << smp::invocable_sign<d::e::f::tmp<K::U::V, std::tuple>>() << std::endl;
    std::cout << smp::invocable_sign<a::b::c::tmp<std::tuple<int>, std::tuple>>() << std::endl;
    std::cout << smp::invocable_sign<d::e::f::tmp<std::tuple<int>, std::tuple>>() << std::endl;
    std::cout << smp::invocable_sign<nest1::f>() << std::endl;
    std::cout << smp::invocable_sign<nest2::f>() << std::endl;
    std::cout << smp::invocable_sign<[]{}>() << std::endl;
    std::cout << smp::invocable_sign<[]<auto i, typename T>(T a, T b){}>() << std::endl;
    std::cout << smp::invocable_sign<[]<typename... Args>(Args&&... args){}>() << std::endl;
    std::cout << smp::invocable_sign<[]<int i, double d>(float a, char b){}>() << std::endl;
    std::cout << smp::invocable_sign_v<[]<float i, typename T>(char a, T t){}> << std::endl;
    std::cout << smp::invocable_sign_v<[k]<auto... N, typename T>(int a, T t){}> << std::endl;

    std::cout << std::endl << "invocable_stem" << std::endl;
    std::cout << smp::invocable_stem<main>() << std::endl;
    std::cout << smp::invocable_stem<push>() << std::endl;
    std::cout << smp::invocable_stem<&Y::pull>() << std::endl;
    std::cout << smp::invocable_stem<&K::Z::pull>() << std::endl;
    std::cout << smp::invocable_stem<&K::Z::member>() << std::endl;
    std::cout << smp::invocable_stem<a::b::c::tmp<K::U::V, std::tuple>>() << std::endl;
    std::cout << smp::invocable_stem<d::e::f::tmp<K::U::V, std::tuple>>() << std::endl;
    std::cout << smp::invocable_stem<a::b::c::tmp<std::tuple<int>, std::tuple>>() << std::endl;
    std::cout << smp::invocable_stem<d::e::f::tmp<std::tuple<int>, std::tuple>>() << std::endl;
    std::cout << smp::invocable_stem<nest1::f>() << std::endl;
    std::cout << smp::invocable_stem<nest2::f>() << std::endl;
    std::cout << smp::invocable_stem<[]{}>() << std::endl;
    std::cout << smp::invocable_stem<[]<auto i, typename T>(T a, T b){}>() << std::endl;
    std::cout << smp::invocable_stem<[]<typename... Args>(Args&&... args){}>() << std::endl;
    std::cout << smp::invocable_stem<[]<int i, double d>(float a, char b){}>() << std::endl;
    std::cout << smp::invocable_stem_v<[]<float i, typename T>(char a, T t){}> << std::endl;
    std::cout << smp::invocable_stem_v<[k]<auto... N, typename T>(int a, T t){}> << std::endl;

    std::cout << std::endl << "pretty function" << std::endl;
    std::cout << loc::pretty_function<main>() << std::endl;
    std::cout << loc::pretty_function<push>() << std::endl;
    std::cout << loc::pretty_function<&Y::pull>() << std::endl;
    std::cout << loc::pretty_function<&K::Z::pull>() << std::endl;
    std::cout << loc::pretty_function<&K::Z::member>() << std::endl;
    std::cout << loc::pretty_function<a::b::c::tmp<K::U::V, std::tuple>>() << std::endl;
    std::cout << loc::pretty_function<d::e::f::tmp<K::U::V, std::tuple>>() << std::endl;
    std::cout << loc::pretty_function<a::b::c::tmp<std::tuple<int>, std::tuple>>() << std::endl;
    std::cout << loc::pretty_function<d::e::f::tmp<std::tuple<int>, std::tuple>>() << std::endl;
    std::cout << loc::pretty_function<nest1::f>() << std::endl;
    std::cout << loc::pretty_function<nest2::f>() << std::endl;
    std::cout << loc::pretty_function<[]{}>() << std::endl;
    std::cout << loc::pretty_function<[]<auto i, typename T>(T a, T b){}>() << std::endl;
    std::cout << loc::pretty_function<[]<typename... Args>(Args&&... args){}>() << std::endl;
    std::cout << loc::pretty_function<[]<int i, double d>(float a, char b){}>() << std::endl;
    std::cout << loc::pretty_function<[]<float i, typename T>(char a, T t){}>() << std::endl;
    std::cout << loc::pretty_function<[k]<auto... N, typename T>(int a, T t){}>() << std::endl;

    std::cout << std::endl << "__PRETTY_FUNCTION__" << std::endl;
    std::cout << std::string_view{__PRETTY_FUNCTION__} << std::endl;

    return 0;
}
