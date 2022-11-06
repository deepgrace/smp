//
// Copyright (c) 2022-present DeepGrace (complex dot invoke at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/deepgrace/smp
//

// g++ -I include -m64 -std=c++23 -s -Wall -O3 -o /tmp/lists example/lists.cpp

#include <cassert>
#include <lists.hpp>

// An expression template (fused operations) to generate type list and index sequence for smp::fuple_cat and smp::tuple_cat

int main(int argc, char* argv[])
{
    std::tuple<int, char> t1;
    std::tuple<short> t2;

    std::tuple<float, double, size_t> t3;

    auto c1 = smp::tuple_cat(t1, t2, t3);
    auto c2 = std::tuple_cat(t1, t2, t3);

    assert(c1 == c2);

    return 0;
}
