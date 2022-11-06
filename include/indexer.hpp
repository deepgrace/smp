//
// Copyright (c) 2022-present DeepGrace (complex dot invoke at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/deepgrace/smp
//

#ifndef INDEXER_HPP
#define INDEXER_HPP

#include <cstddef>
#include <utility>
#include <lists.hpp>

namespace smp
{
    template <size_t N, typename T>
    struct identity
    {
        using type = T;
        static constexpr size_t value = N;
    };

    #ifdef __GNUC__
    #   pragma GCC diagnostic push
    #   pragma GCC diagnostic ignored "-Wnon-template-friend"
    #endif

    template <size_t N>
    struct dec_i
    {
        friend constexpr decltype(auto) adl_i(dec_i<N>);
    };

    template <typename R, size_t N>
    struct def_i
    {
        friend constexpr decltype(auto) adl_i(dec_i<N>)
        {
            return R();
        }

        static constexpr identity<N, R> tag{};
    };

    #ifdef __GNUC__
    #   pragma GCC diagnostic pop
    #endif

    template <size_t N, auto = []{}>
    inline constexpr bool search = requires(dec_i<N> r){ adl_i(r); };

    template <typename T, size_t N = 0>
    consteval decltype(auto) find()
    {
        if constexpr(search<N>)
        {
            constexpr size_t M = N + 1;

            constexpr size_t D = 2 * N;
            constexpr size_t H = N / 2;

            if constexpr(search<D>)
                return find<T, D + 1>();
            else if constexpr(search<N + H>)
                return find<T, H + M>();
            else
                return find<T, M>();
        }
        else if constexpr(N == 0)
            return identity<N, lists<>>();
        else
            return identity<N - 1, decltype(adl_i(dec_i<N - 1>()))>();
    }

    template <auto v>
    using state_t = typename decltype(v)::type;

    template <typename T = decltype([]{}), size_t N = 0>
    using take = state_t<find<T, N>()>;

    template <bool B, typename T, typename U, size_t N = 0>
    consteval decltype(auto) push()
    {
        if constexpr(search<N>)
            return push<B, T, U, N + 1>();
        else if constexpr(N == 0)
            return def_i<lists<T>, 0>().tag;
        else
        {
            auto l = lists<T>();
            auto r = take<U, N - 1>();

            return def_i<std::conditional_t<B, decltype(l + r), decltype(r + l)>, N>().tag;
        }
    }

    template <typename T, auto s = push<1, T, decltype([]{})>()>
    constexpr decltype(auto) push_front = []{ return s; };

    template <typename T, auto s = push<0, T, decltype([]{})>()>
    constexpr decltype(auto) push_back = []{ return s; };

    template <typename T = decltype([]{})>
    constexpr decltype(auto) curr = []{ return find<T>().value; };

    template <typename T = decltype([]{})>
    constexpr decltype(auto) size = []{ return curr<T>() + search<0>; };

    template <typename T = decltype([]{})>
    constexpr decltype(auto) next = []{ return push_back<std::void_t<>>().value; };

    template <typename T = decltype([]{})>
    constexpr decltype(auto) clear = []{ return def_i<lists<>, curr<>() + 1>().tag; };

    template <typename T, typename... Args>
    struct integer_sequence_for
    {
        static constexpr auto N = size<>();
        using type = std::integer_sequence<T, (push_back<Args>().value - N)...>;
    };

    template <typename T, typename... Args>
    using integer_sequence_for_t = typename integer_sequence_for<T, Args...>::type;

    template <typename... Args>
    using index_sequence_for = integer_sequence_for_t<size_t, Args...>;
}

#endif
