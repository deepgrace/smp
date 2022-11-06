//
// Copyright (c) 2022-present DeepGrace (complex dot invoke at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/deepgrace/smp
//

#ifndef VISITOR_HPP
#define VISITOR_HPP

#include <bit>
#include <tuple>
#include <fuple.hpp>

namespace smp
{
    template <typename T>
    struct visitor
    {
        constexpr visitor() = default;

        constexpr visitor(visitor&&) = default;
        constexpr visitor(const visitor&) = default;

        visitor& operator=(visitor&& r) = default;
        visitor& operator=(const visitor& r) = default;

        using type = T;
        using byte_t = std::byte;

        using storage_t = aligned_fuple_t<type, byte_t>;

        template <size_t N, bool B = 0, typename U = fuple_element_t<N, type>>
        using pointer_t = std::add_pointer_t<std::conditional_t<B, std::add_const_t<U>, U>>;

        static constexpr size_t value = length_v<type>;

        static constexpr size_t size() noexcept
        {
            return value;
        }

        template <typename U, typename V>
        static constexpr decltype(auto) bit_cast(V&& v)
        {
            return std::bit_cast<U>(std::addressof(v));
        }

        template <size_t N, typename U>
        static constexpr decltype(auto) addressof() noexcept
        {
            return bit_cast<U>(select<N>(s).data[0]);
        }

        template <size_t N, typename U = std::ptrdiff_t>
        static constexpr U offset() noexcept
        {
            return addressof<N, U>() - addressof<0, U>();
        }

        template <size_t N, typename U>
        requires (!is_fuple_v<std::remove_cvref_t<U>>)
        constexpr decltype(auto) get(U&& u) noexcept
        {
            decltype(auto) d = offset<N>();

            decltype(auto) p = std::addressof(u);
            decltype(auto) q = std::is_const_v<std::remove_pointer_t<decltype(p)>>;

            return *reinterpret_cast<pointer_t<N, q>>(reinterpret_cast<pointer_t<N, q, byte_t>>(p) + d);
        }

        template <bool f, bool t, typename U>
        requires (!is_fuple_v<std::remove_cvref_t<U>>)
        constexpr decltype(auto) get(U&& u) noexcept
        {
            return [&]<size_t... N>(std::index_sequence<N...>)
            {
                if constexpr(f)
                {
                    if constexpr(t)
                        return smp::tie(get<N>(std::forward<U>(u))...);
                    else
                        return smp::make_fuple(get<N>(std::forward<U>(u))...);
                }
                else
                {
                    if constexpr(t)
                        return std::tie(get<N>(std::forward<U>(u))...);
                    else
                        return std::make_tuple(get<N>(std::forward<U>(u))...);
                }
            }
            (std::make_index_sequence<value>());
        }

        static constexpr storage_t s{};
    };
}

#endif
