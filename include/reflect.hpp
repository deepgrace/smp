//
// Copyright (c) 2022-present DeepGrace (complex dot invoke at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/deepgrace/smp
//

#ifndef REFLECT_HPP
#define REFLECT_HPP

#include <iomanip>
#include <visitor.hpp>

namespace smp
{
    #ifdef __GNUC__
    #   pragma GCC diagnostic push
    #   pragma GCC diagnostic ignored "-Wnon-template-friend"
    #endif

    template <size_t N, typename T>
    struct dec_r
    {
        friend constexpr decltype(auto) adl_r(dec_r<N, T>);
    };

    template <typename R, size_t N, typename T>
    struct def_r
    {
        friend constexpr decltype(auto) adl_r(dec_r<N, T>)
        {
            return R();
        }
    };

    #ifdef __GNUC__
    #   pragma GCC diagnostic pop
    #endif

    template <size_t N, typename T>
    struct universal
    {
        template <typename R, size_t = sizeof(def_r<R, N, T>)>
        operator R()
        {
        }
    };

    template <typename T>
    requires std::is_aggregate_v<T>
    consteval decltype(auto) arity(auto... Args)
    {
        if constexpr(! requires { T{ Args... }; })
            return sizeof...(Args) - 1;
        else
            return arity<T>(Args..., universal<sizeof...(Args), T>());
    }

    template <typename T>
    inline constexpr auto arity_v = arity<T>();

    template <size_t N, typename T>
    struct member
    {
        static constexpr size_t value = arity_v<std::remove_cvref_t<T>>;
        using type = decltype(adl_r(dec_r<N, std::remove_cvref_t<T>>()));
    };

    template <size_t N, typename T>
    using member_t = typename member<N, T>::type;

    template <size_t N, typename T>
    struct member_pointer
    {
        using type = member_t<N, T> T::*;
    };

    template <size_t N, typename T>
    using member_pointer_t = typename member_pointer<N, T>::type;

    template <typename T>
    struct object;

    template <typename R, typename T>
    struct object<R T::*> : std::type_identity<T>
    {
    };

    template <typename T>
    using object_t = typename object<T>::type;

    template <typename T>
    struct members
    {
        using U = std::remove_cvref_t<T>;

        using type = decltype([]<size_t... N>(std::index_sequence<N...>)
        {
            return fuple<decltype(adl_r(dec_r<N, U>()))...>();
        }
        (std::make_index_sequence<arity_v<U>>()));
    };

    template <typename T>
    using members_t = typename members<T>::type;

    template <typename T>
    struct member_pointers
    {
        using U = std::remove_cvref_t<T>;

        using type = decltype([]<size_t... N>(std::index_sequence<N...>)
        {
            return fuple<decltype(adl_r(dec_r<N, U>())) U::*...>();
        }
        (std::make_index_sequence<arity_v<U>>()));
    };

    template <typename T>
    using member_pointers_t = typename member_pointers<T>::type;

    template <bool f, bool t, typename U>
    requires (!is_fuple_v<std::remove_cvref_t<U>>)
    static constexpr decltype(auto) expand(U&& u)
    {
        return visitor<members_t<std::remove_cvref_t<U>>>().template get<f, t>(std::forward<U>(u));
    }

    template <typename T>
    requires (!is_fuple_v<std::remove_cvref_t<T>>)
    static constexpr decltype(auto) to_tuple(T&& t)
    {
        return expand<0, 0>(std::forward<T>(t));
    }

    template <typename T>
    requires (!is_fuple_v<std::remove_cvref_t<T>>)
    static constexpr decltype(auto) tie_tuple(T&& t)
    {
        return expand<0, 1>(std::forward<T>(t));
    }

    template <typename T>
    requires (!is_fuple_v<std::remove_cvref_t<T>>)
    static constexpr decltype(auto) to_fuple(T&& t)
    {
        return expand<1, 0>(std::forward<T>(t));
    }

    template <typename T>
    requires (!is_fuple_v<std::remove_cvref_t<T>>)
    static constexpr decltype(auto) tie_fuple(T&& t)
    {
        return expand<1, 1>(std::forward<T>(t));
    }

    template <size_t N, typename U>
    requires (!is_fuple_v<std::remove_cvref_t<U>>)
    constexpr decltype(auto) get(U&& u)
    {
        return get<N>(tie_fuple(std::forward<U>(u)));
    }

    template <typename T, typename U>
    requires (!is_fuple_v<std::remove_cvref_t<U>>)
    constexpr decltype(auto) get(U&& u)
    {
        return get<T>(tie_fuple(std::forward<U>(u)));
    }

    template <size_t N, typename U>
    requires (!is_fuple_v<std::remove_cvref_t<U>>)
    constexpr decltype(auto) member_v(U&& u)
    {
        return get<N>(std::forward<U>(u));
    }

    template <typename T, typename U>
    requires (!is_fuple_v<std::remove_cvref_t<U>>)
    constexpr decltype(auto) member_v(U&& u)
    {
        return get<T>(std::forward<U>(u));
    }

    template <typename F, typename... Args>
    requires (!is_fuple_v<std::remove_cvref_t<Args>> && ...)
    void zip(F&& f, Args&&... args)
    {
        zip(std::forward<F>(f), tie_fuple(std::forward<Args>(args))...);
    }

    template <typename F, typename T>
    requires (!is_fuple_v<std::remove_cvref_t<T>>)
    constexpr decltype(auto) for_each(F&& f, T&& t)
    {
        return  smp::for_each(std::forward<F>(f), tie_fuple(std::forward<T>(t)));
    }

    template <typename F, typename T>
    requires (!is_fuple_v<std::remove_cvref_t<T>>)
    constexpr decltype(auto) apply(F&& f, T&& t)
    {
        return  smp::apply(std::forward<F>(f), tie_fuple(std::forward<T>(t)));
    }

    template <bool B, template <typename ...> typename T, typename... Args>
    struct ruple : T<Args&...>
    {
        using T<Args&...>::T;

        template <typename U>
        constexpr ruple& operator=(U&& u)
        {
            if constexpr(B)
                T<Args&...>::operator=(tie_fuple(std::forward<U>(u)));
            else
                T<Args&...>::operator=(tie_tuple(std::forward<U>(u)));

            return *this;
        }
    };

    template <template <typename ...> typename T, typename... Args>
    constexpr T<Args...> tie(Args&... args)
    {
        return T<Args...>(args...);
    }

    template <typename... Args>
    using fuple_ref_t = ruple<1, smp::fuple, Args...>;

    template <typename... Args>
    using tuple_ref_t = ruple<0, std::tuple, Args...>;

    template <typename... Args>
    constexpr decltype(auto) ref_fuple(Args&... args)
    {
        return tie<fuple_ref_t, Args...>(args...);
    }

    template <typename... Args>
    constexpr decltype(auto) ref_tuple(Args&... args)
    {
        return tie<tuple_ref_t, Args...>(args...);
    }

    template <size_t N, typename T, typename U = members_t<T>, typename V = visitor<U>>
    constexpr decltype(auto) get(V v = V())
    {
        return std::bit_cast<fuple_element_t<N, U> T::*>(v.template offset<N>());
    }

    template <size_t N, typename T>
    decltype(auto) member_pointer_v = []<typename U>()
    {
        return get<N, T, U>();
    }.template operator()<members_t<T>>();

    template <typename T>
    decltype(auto) member_pointers_v = []<typename U>()
    {
        return [&]<size_t... N>(std::index_sequence<N...>)
        {
            return make_fuple(get<N, T, U>()...);
        }
        (std::make_index_sequence<fuple_size_v<U>>());
    }.template operator()<members_t<T>>();

    template <typename F, typename T>
    requires (!is_fuple_v<std::remove_cvref_t<T>>)
    constexpr decltype(auto) for_mptr(F&& f, T&& t)
    {
        return  smp::for_each(std::forward<F>(f), member_pointers_v<std::remove_cvref_t<T>>);
    }

    template <typename R, typename T>
    constexpr size_t index(R T::* m)
    {
        static constexpr T t{};
        decltype(auto) p = &(t.*m);

        return apply([&](auto&...e)
        {
            size_t i = 0;

            for (auto m : { (void*)&e ... })
                 if (i += m != p; m == p)
                     break;

            return i;
        }, tie_fuple(t));
    }

    template <typename T>
    using io_t = element<0, T>;

    template <typename T>
    constexpr decltype(auto) io(T&& value)
    {
        return io_t<T>(std::forward<T>(value));
    }

    template <typename T>
    constexpr decltype(auto) quoted(T&& t)
    {
        if constexpr(requires{ typename std::remove_cvref_t<T>::traits_type; })
            return std::quoted(std::forward<T>(t));
        else
            return std::forward<T>(t);
    }

    template <bool B, size_t lower, size_t upper, typename S, typename T>
    requires (is_fuple_v<std::remove_cvref_t<T>>)
    constexpr static S& apply(S& s, T&& t)
    {
        if constexpr(!!lower)
        {
            if constexpr(B)
                s << ", ";
            else
            {
                char c;
                s >> c >> c;
            }
        }

        if constexpr(B)
            s << smp::quoted(smp::get<lower>(std::forward<T>(t)));
        else
            s >> smp::quoted(smp::get<lower>(std::forward<T>(t)));

        if constexpr(lower + 1 != upper)
            return apply<B, lower + 1, upper>(s, std::forward<T>(t));
        else
            return s;
    }

    template <typename S, typename T>
    requires (!is_fuple_v<std::remove_cvref_t<T>>)
    constexpr S& operator<<(S& s, io_t<T>&& t)
    {
        if constexpr(requires{ std::declval<S>() << std::declval<T>(); })
            return s << std::forward<T>(t.value);
        else 
        {
            decltype(auto) f = tie_fuple(std::forward<T>(t.value));

            return apply<1, 0, fuple_size_v<decltype(f)>>(s, f);
        }
    }

    template <typename S, typename T>
    requires (!is_fuple_v<std::remove_cvref_t<T>>)
    constexpr S& operator>>(S& s, io_t<T>&& t)
    {
        if constexpr(requires{ std::declval<S>() >> std::declval<T>(); })
            return s >> std::forward<T>(t.value);
        else 
        {
            s.flags(typename S::fmtflags(0));
            decltype(auto) f = tie_fuple(std::forward<T>(t.value));

            return apply<0, 0, fuple_size_v<decltype(f)>>(s, f);
        }
    }

    template <typename T, typename U>
    requires (!is_fuple_v<std::remove_cvref_t<T>> && is_fuple_v<std::remove_cvref_t<U>>)
    constexpr decltype(auto) fill(T&& t, U&& u)
    {
        tie_fuple(std::forward<T>(t)) = std::forward<U>(u);

        return std::forward<T>(t);
    }

    template <typename T, typename U>
    requires (!is_fuple_v<std::remove_cvref_t<T>> && is_tuple_v<std::remove_cvref_t<U>>)
    constexpr decltype(auto) fill(T&& t, U&& u)
    {
        tie_tuple(std::forward<T>(t)) = std::forward<U>(u);

        return std::forward<T>(t);
    }

    template <typename T, typename... Args>
    requires (!is_fuple_v<std::remove_cvref_t<T>>)
    constexpr decltype(auto) fill(T&& t, Args&&... args)
    {
        tie_fuple(std::forward<T>(t)) = forward_as_fuple(std::forward<Args>(args)...);

        return std::forward<T>(t);
    }

    template <typename T, typename... Args>
    requires (!is_fuple_v<std::remove_cvref_t<T>>)
    constexpr decltype(auto) fill(Args&&... args)
    {
        T t;
        fill(t, std::forward<Args>(args)...);

        return t;
    }

    template <typename T, typename U>
    requires (!is_fuple_v<std::remove_cvref_t<T>> && !is_fuple_v<std::remove_cvref_t<U>>)
    constexpr bool lt(T&& t, U&& u)
    {
        if constexpr(requires{ t < u; })
            return std::forward<T>(t) < std::forward<U>(u);
        else
            return tie_fuple(std::forward<T>(t)) < tie_fuple(std::forward<U>(u));
    }

    template <typename T, typename U>
    requires (!is_fuple_v<std::remove_cvref_t<T>> && !is_fuple_v<std::remove_cvref_t<U>>)
    constexpr bool ne(T&& t, U&& u)
    {
        if constexpr(requires{ t != u; })
            return std::forward<T>(t) != std::forward<U>(u);
        else
            return tie_fuple(std::forward<T>(t)) != tie_fuple(std::forward<U>(u));
    }

    template <typename T, typename U>
    requires (!is_fuple_v<std::remove_cvref_t<T>> && !is_fuple_v<std::remove_cvref_t<U>>)
    constexpr bool le(T&& t, U&& u)
    {
        return !lt(std::forward<U>(u), std::forward<T>(t));
    }

    template <typename T, typename U>
    requires (!is_fuple_v<std::remove_cvref_t<T>> && !is_fuple_v<std::remove_cvref_t<U>>)
    constexpr bool eq(T&& t, U&& u)
    {
        return !ne(std::forward<T>(t), std::forward<U>(u));
    }

    template <typename T, typename U>
    requires (!is_fuple_v<std::remove_cvref_t<T>> && !is_fuple_v<std::remove_cvref_t<U>>)
    constexpr bool ge(T&& t, U&& u)
    {
        return le(std::forward<U>(u), std::forward<T>(t));
    }

    template <typename T, typename U>
    requires (!is_fuple_v<std::remove_cvref_t<T>> && !is_fuple_v<std::remove_cvref_t<U>>)
    constexpr bool gt(T&& t, U&& u)
    {
        return lt(std::forward<U>(u), std::forward<T>(t));
    }

    template <typename T = std::void_t<>>
    struct less
    {
        constexpr bool operator()(const T& x, const T& y) const
        {
            return smp::lt(x, y);
        }
    };

    template <>
    struct less<void>
    {
        template <typename T, typename U>
        constexpr bool operator()(T&& t, U&& u) const
        {
            return smp::lt(std::forward<T>(t), std::forward<U>(u));
        }

        using is_transparent = std::void_t<>;
    };

    template <typename T = std::void_t<>>
    struct not_equal_to
    {
        constexpr bool operator()(const T& x, const T& y) const
        {
            return smp::ne(x, y);
        }
    };

    template <>
    struct not_equal_to<void>
    {
        template <typename T, typename U>
        constexpr bool operator()(T&& t, U&& u) const
        {
            return smp::ne(std::forward<T>(t), std::forward<U>(u));
        }

        using is_transparent = std::void_t<>;
    };

    template <typename T = std::void_t<>>
    struct less_equal
    {
        constexpr bool operator()(const T& x, const T& y) const
        {
            return smp::le(x, y);
        }
    };

    template <>
    struct less_equal<void>
    {
        template <typename T, typename U>
        constexpr bool operator()(T&& t, U&& u) const
        {
            return smp::le(std::forward<T>(t), std::forward<U>(u));
        }

        using is_transparent = std::void_t<>;
    };

    template <typename T = std::void_t<>>
    struct equal_to
    {
        constexpr bool operator()(const T& x, const T& y) const
        {
            return smp::eq(x, y);
        }
    };

    template <>
    struct equal_to<void>
    {
        template <typename T, typename U>
        constexpr bool operator()(T&& t, U&& u) const
        {
            return smp::eq(std::forward<T>(t), std::forward<U>(u));
        }

        using is_transparent = std::void_t<>;
    };

    template <typename T = std::void_t<>>
    struct greater_equal
    {
        constexpr bool operator()(const T& x, const T& y) const
        {
            return smp::ge(x, y);
        }
    };

    template <>
    struct greater_equal<void>
    {
        template <typename T, typename U>
        constexpr bool operator()(T&& t, U&& u) const
        {
            return smp::ge(std::forward<T>(t), std::forward<U>(u));
        }

        using is_transparent = std::void_t<>;
    };

    template <typename T = std::void_t<>>
    struct greater
    {
        constexpr bool operator()(const T& x, const T& y) const
        {
            return smp::gt(x, y);
        }
    };

    template <>
    struct greater<void>
    {
        template <typename T, typename U>
        constexpr bool operator()(T&& t, U&& u) const
        {
            return smp::gt(std::forward<T>(t), std::forward<U>(u));
        }

        using is_transparent = std::void_t<>;
    };

    template <typename T>
    struct tuple_size : std::integral_constant<size_t, arity_v<std::remove_cvref_t<T>>>
    {
    };

    template <typename T>
    inline constexpr size_t tuple_size_v = tuple_size<T>::value;

    template <size_t N, typename T>
    struct tuple_element : member<N, T>
    {
    };

    template <size_t N, typename T>
    using tuple_element_t = typename tuple_element<N, T>::type;
}
#endif
