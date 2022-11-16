//
// Copyright (c) 2022-present DeepGrace (complex dot invoke at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/deepgrace/smp
//

#ifndef LISTS_HPP
#define LISTS_HPP

#include <tuple>
#include <cstddef>
#include <utility>

namespace smp
{
    template <template <auto ...> typename T, auto... U, template <auto ...> typename V, auto... W>
    constexpr decltype(auto) operator+(T<U...>, V<W...>)
    {
        return T<U..., W...>();
    }

    template <template <typename ...> typename T, typename... U, template <typename ...> typename V, typename... W>
    constexpr decltype(auto) operator+(T<U...>, V<W...>)
    {
        return T<U..., W...>();
    }

    template <template <typename, auto ...> typename T, typename U, auto... V, template <typename, auto ...> typename W, typename X, auto... Y>
    constexpr decltype(auto) operator+(T<U, V...>, W<X, Y...>)
    {
        return T<U, V..., Y...>();
    }

    template <auto... Args>
    struct auto_pack
    {
        using type = auto_pack<Args...>;

        static constexpr size_t value = sizeof...(Args);

        static constexpr size_t size() noexcept
        {
            return value;
        }
    };

    template <typename... Args>
    struct type_pack
    {
        using type = type_pack<Args...>;

        static constexpr size_t value = sizeof...(Args);

        static constexpr size_t size() noexcept
        {
            return value;
        }
    };

    template <typename T, auto... Args>
    struct args_pack
    {
        using type = args_pack<T, Args...>;

        static constexpr size_t value = sizeof...(Args);

        static constexpr size_t size() noexcept
        {
            return value;
        }
    };

    template <typename T>
    struct pack_size : std::integral_constant<size_t, 0>
    {
    };

    template <template <auto ...> typename T, auto... U>
    struct pack_size<T<U...>> : std::integral_constant<size_t, sizeof...(U)>
    {
    };

    template <template <typename ...> typename T, typename... U>
    struct pack_size<T<U...>> : std::integral_constant<size_t, sizeof...(U)>
    {
    };

    template <template <typename, auto ...> typename T, typename U, auto... V>
    struct pack_size<T<U, V...>> : std::integral_constant<size_t, sizeof...(V)>
    {
    };

    template <typename T>
    inline constexpr auto pack_size_v = pack_size<T>::value;

    template <typename T>
    using rank = std::make_index_sequence<pack_size_v<std::decay_t<T>>>;

    template <auto m, template <typename, auto ...> typename T, typename U, auto... n>
    constexpr decltype(auto) over(T<U, n...>)
    {
        return type_pack<auto_pack<m, n>...>();
    }

    template <typename... Args>
    constexpr decltype(auto) rank_pack(Args&&... args)
    {
        return []<auto... N>(std::index_sequence<N...>)
        {
            return (... + over<N>(rank<Args>()));
        }
        (std::index_sequence_for<Args...>());
    }

    template <typename... Args>
    constexpr decltype(auto) tuple_cat(Args&&... args)
    {
        auto t = std::forward_as_tuple(std::forward<Args>(args)...);

        if constexpr(!sizeof...(Args))
            return t;
        else
        {
            return [&]<template <typename ...> typename T, template <auto ...> typename U, auto... m, auto... n>(T<U<m, n>...>)
            {
                return std::forward_as_tuple(std::get<n>(std::get<m>(t))...);
            }
            (rank_pack(std::forward<Args>(args)...));
        }
    }

    template <typename T>
    struct is_tuple : std::false_type
    {
    };

    template <typename... Args>
    struct is_tuple<std::tuple<Args...>> : std::true_type
    {
    };

    template <typename T>
    inline constexpr auto is_tuple_v = is_tuple<T>::value;

    template <typename... Args>
    using lists = type_pack<Args...>;
}

#endif
