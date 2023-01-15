# smp [![LICENSE](https://img.shields.io/github/license/deepgrace/smp.svg)](https://github.com/deepgrace/smp/blob/master/LICENSE_1_0.txt) [![Language](https://img.shields.io/badge/language-C%2B%2B20-blue.svg)](https://en.cppreference.com/w/cpp/compiler_support) [![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20MacOS%20%7C%20Windows-lightgrey.svg)](https://github.com/deepgrace/smp)

> **Advanced C++ Stateful Template MetaProgramming Library**

## Overview
```cpp
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

    smp::apply(print_backward, w1);
    smp::apply(print_backward, w2);

    std::cout << std::endl;
    smp::zip(print_backward, w1, w2, x, y);

    // marshal and unmarshal a structure

    std::string xs = smp::marshal(x);
    std::string ys = smp::marshal(y);

    X x0 = smp::unmarshal<X>(xs);
    Y y0 = smp::unmarshal<Y>(ys);

    assert(smp::eq(x, x0));

    assert(y.i == y0.i);
    assert(y.d == y0.d);

    assert(y.c == y0.c);
    assert(smp::eq(y.x, y0.x));

    // marshal and unmarshal with allocated structure

    X x1;
    std::string s0;

    smp::marshal(s0, x);
    smp::unmarshal(s0, x1);

    assert(smp::eq(x1, x));

    Y y1;
    std::string s1 = "prefix";

    size_t prelen = s1.length();
    size_t length = s1.length();

    smp::marshal(s1, y);
    smp::unmarshal(length, s1, y1);

    assert(y.i == y1.i);
    assert(y.d == y1.d);

    assert(y.c == y1.c);
    assert(smp::eq(y.x, y1.x));

    assert(length == s1.length());
    assert(smp::marshal(y).length() == length - prelen);

    auto y2 = smp::unmarshal<Y>(s1.substr(prelen));

    assert(y.i == y2.i);
    assert(y.d == y2.d);

    assert(y.c == y2.c);
    assert(smp::eq(y.x, y2.x));

    // marshal and unmarshal a smp::fuple or std::tuple

    std::string ws1 = smp::marshal(f1);
    std::string ws2 = smp::marshal(t2);

    auto w3 = smp::unmarshal<W>(ws1);
    auto w4 = smp::unmarshal<W>(ws2);

    assert(smp::eq(w1, w3));
    assert(smp::eq(w2, w4));

    auto f0 = smp::make_fuple(1, 2.0f, std::string("marshal"), 'X', std::make_tuple(100.3, std::string("Unmarshal")));
    auto t0 = std::make_tuple(2, 3.0f, std::string("Marshal"), 'Y', smp::make_fuple(200.3, std::string("unmarshal")));

    std::string fs0 = smp::marshal(f0);
    std::string ts0 = smp::marshal(t0);

    auto f3 = smp::unmarshal<decltype(f0)>(fs0);
    auto t3 = smp::unmarshal<decltype(t0)>(ts0);

    assert(f3 == f0);
    assert(t3 == t0);

    assert(std::get<std::string>(smp::get<4>(f3)) == std::get<1>(smp::get<4>(f0)));
    assert(smp::get<1>(std::get<4>(t3)) == smp::get<std::string>(std::get<4>(t0)));

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
```

## Introduction
smp is a stateful metaprogramming library, which is header-only, extensible and modern C++ oriented.  
It exhibits a form of stateful metaprogramming of compile time type list, index sequence generator and provides many powerful algorithms for manipulating structure elements, marshaling and unmarshaling a structure, smp::fuple or std::tuple.

smp is mainly consist of three parts:
- **fuple**   A flat tuple implemented with multiple inheritance is a drop-in replacement for std::tuple
- **indexer** A compile time type list and index sequence generator with queryable type states embeded in it 
- **reflect** A reflection, marshaling and unmarshaling library enable you to manipulate structure elements by index or type and provides many std::tuple like methods

## Compiler requirements
The library relies on a C++20 compiler and standard library, but nothing else is required.

More specifically, smp requires a compiler/standard library supporting the following C++20 features (non-exhaustively):
- concepts
- lambda templates
- All the C++20 type traits from the <type_traits> header

## Building
smp is header-only. To use it just add the necessary `#include` line to your source files, like this:
```cpp
#include <smp.hpp>
```

To build the example with cmake, `cd` to the root of the project and setup the build directory:
```bash
mkdir build
cd build
cmake ..
```

Make and install the executables:
```
make -j4
make install
```
The executables are now located at the `bin` directory of the root of the project.  
The example can also be built with the script `build.sh`, just run it, the executables will be put at the `/tmp` directory.

## Full example
Please see [example](example).

## License
smp is licensed as [Boost Software License 1.0](LICENSE_1_0.txt).
