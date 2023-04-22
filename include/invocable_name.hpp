//
// Copyright (c) 2022-present DeepGrace (complex dot invoke at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/deepgrace/smp
//

#ifndef INVOCABLE_NAME_HPP
#define INVOCABLE_NAME_HPP

#include <utility>
#include <string_view>

namespace smp
{
    constexpr auto npos = std::string_view::npos;

    template <auto f>
    consteval decltype(auto) invocable_name() noexcept
    {
         constexpr std::string_view n{__PRETTY_FUNCTION__};

         constexpr std::string_view k{"[with auto f = "};
         constexpr std::string_view l{"<lambda closure object>"};

         constexpr auto s = l.size();
         constexpr auto p = n.find(k) + k.size();

         constexpr auto r = n.substr(p, s);
         constexpr auto q = r == l ? s : n[p] == '&';

         return n.substr(p + q, n.size() - p - q - 1);
    }

    template <auto f>
    inline constexpr auto invocable_name_v = invocable_name<f>();

    template <auto f>
    consteval decltype(auto) invocable_sign() noexcept
    {
        constexpr auto n{invocable_name<f>()};
        constexpr auto p = n.rfind("::", n.find_first_of('<'));

        return n.substr(p == npos ? 0 : p + 2);
    }

    template <auto f>
    inline constexpr auto invocable_sign_v = invocable_sign<f>();

    template <auto f>
    consteval decltype(auto) invocable_stem() noexcept
    {
        constexpr auto n{invocable_sign<f>()};
        constexpr auto i = n.find_first_of('<');

        constexpr auto s = n.substr(0, i ? i : n.size());

        constexpr auto e = s.find_last_of('>');
        constexpr auto b = s.find_first_of('<');

        if constexpr(b != npos)
            return s.substr(b + 1, e - b);
        else
            return s;
    }

    template <auto f>
    inline constexpr auto invocable_stem_v = invocable_stem<f>();

    template <auto e>
    requires std::is_enum_v<decltype(e)>
    consteval decltype(auto) in_range()
    {
        constexpr auto s = smp::invocable_stem_v<e>;

        return s.find_first_of('(') == npos && s.find_last_of(')') == npos;
    }

    template <typename T>
    requires std::is_enum_v<T>
    consteval decltype(auto) arity()
    {
        return []<auto... N>(std::integer_sequence<int, N...>)
        {
            return (... + in_range<static_cast<T>(N)>());
        }
        (std::make_integer_sequence<int, 100>());
    }
}

#endif
