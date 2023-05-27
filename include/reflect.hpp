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

#include <memory>
#include <cstring>
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

    template <size_t N, typename T, auto = 0>
    using type_t = decltype(adl_r(dec_r<N, std::remove_cvref_t<T>>()));

    template <size_t N, typename T>
    struct member
    {
        using type = type_t<N, T, arity_v<std::remove_cvref_t<T>>>;
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
            return fuple<type_t<N, U>...>();
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
            return fuple<type_t<N, U> U::*...>();
        }
        (std::make_index_sequence<arity_v<U>>()));
    };

    template <typename T>
    using member_pointers_t = typename member_pointers<T>::type;

    template <bool f, bool t, typename U>
    requires (!is_fuple_v<std::remove_cvref_t<U>> && !is_tuple_v<std::remove_cvref_t<U>>)
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
    requires (!is_fuple_v<std::remove_cvref_t<T>> && !is_tuple_v<std::remove_cvref_t<T>>)
    constexpr decltype(auto) for_each(F&& f, T&& t)
    {
        return smp::for_each(std::forward<F>(f), tie_fuple(std::forward<T>(t)));
    }

    template <typename F, typename T>
    requires (!is_fuple_v<std::remove_cvref_t<T>>)
    constexpr decltype(auto) apply(F&& f, T&& t)
    {
        return smp::apply(std::forward<F>(f), tie_fuple(std::forward<T>(t)));
    }

    template <auto... N, typename T>
    constexpr decltype(auto) choose(T&& t)
    {
        if constexpr(is_tuple_v<std::remove_cvref_t<T>>)
            return std::tie(std::get<N>(std::forward<T>(t))...);
        else
            return smp::tie(smp::get<N>(std::forward<T>(t))...);
    }

    template <typename I, typename T>
    constexpr decltype(auto) choose(T&& t)
    {
        return [&]<auto... N>(const std::index_sequence<N...>&)
        {
            return choose<N...>(std::forward<T>(t));
        }
        (I());
    }

    template <size_t lower, size_t upper, typename T>
    constexpr decltype(auto) range(T&& t)
    {
        return [&]<size_t... N>(std::index_sequence<N...>)
        {
            if constexpr(is_tuple_v<std::remove_cvref_t<T>>)
                return std::tie(std::get<N>(std::forward<T>(t))...);
            else
                return smp::tie(smp::get<N>(std::forward<T>(t))...);
        }
        (std::make_index_sequence<upper - lower>());
    }

    template <bool B, typename U, typename L, typename S, typename T>
    constexpr decltype(auto) copy(L&& l, S&& s, T&& t, size_t size = sizeof(U))
    {
        if constexpr(B)
            s.resize(l + size);

        auto dst = s.data() + l;
        auto src = std::addressof(std::forward<T>(t));

        if constexpr(B)
            std::memcpy(dst, src, size);
        else
            std::memcpy(src, dst, size);

        return size;
    }

    struct assigner
    {
        template <bool B, typename L, typename S, typename T>
        constexpr decltype(auto) seq(L&& l, S&& s, T&& t, size_t size)
        {
            if constexpr(requires { t.resize(0); })
                t.resize(size);

            for (auto& v : t)
                 replicate<B>(std::forward<L>(l), std::forward<S>(s), v);
        }

        template <bool B, typename L, typename S, typename T, typename U = std::remove_cvref_t<T>>
        constexpr decltype(auto) ass(L&& l, S&& s, T&& t, size_t size)
        {
            if constexpr(B)
                assign<B, U>(std::forward<L>(l), std::forward<S>(s), std::forward<T>(t));
            else
                assign<B, U>(std::forward<L>(l), std::forward<S>(s), std::forward<T>(t), size);
        }

        template <bool B, typename L, typename S, typename T>
        constexpr decltype(auto) browse(L&& l, S&& s, T&& t, size_t size)
        {
            using U = std::remove_cvref_t<T>;

            if constexpr(! requires { typename U::key_type; typename U::value_type; })
                seq<B>(std::forward<L>(l), std::forward<S>(s), std::forward<T>(t), size);
            else
                ass<B>(std::forward<L>(l), std::forward<S>(s), std::forward<T>(t), size);
        }

        template <bool B, typename U, typename L, typename S, typename T>
        constexpr decltype(auto) assign(L&& l, S&& s, T&& t)
        {
            for (auto& p : t)
            {
                 if constexpr(! requires { typename U::mapped_type; })
                     replicate<B>(std::forward<L>(l), std::forward<S>(s), p);
                 else
                 {
                     replicate<B>(std::forward<L>(l), std::forward<S>(s), p.first);
                     replicate<B>(std::forward<L>(l), std::forward<S>(s), p.second);
                 }
            }
        }

        template <bool B, typename U, typename L, typename S, typename T>
        constexpr decltype(auto) assign(L&& l, S&& s, T&& t, size_t size)
        {
            for (size_t i = 0; i != size; ++i)
            {
                 typename U::key_type key;
                 replicate<B>(std::forward<L>(l), std::forward<S>(s), key);

                 if constexpr(! requires { typename U::mapped_type; })
                     t.emplace(key);
                 else
                 {
                     typename U::mapped_type val;
                     replicate<B>(std::forward<L>(l), std::forward<S>(s), val);

                     t.emplace(key, val);
                 }
            }
        }

        template <bool B, typename L, typename S, typename T>
        constexpr decltype(auto) assign(L&& l, S&& s, T&& t, size_t size)
        {
            if constexpr(B)
            {
                l += size;
                s += std::forward<T>(t);
            }
            else
            {
                t.resize(size);
                l += copy<B, size_t>(std::forward<L>(l), std::forward<S>(s), t[0], size);
            }
        }

        template <bool B, typename L, typename S, typename T>
        constexpr decltype(auto) assign(L&& l, S&& s, T&& t)
        {
            smp::for_each([&]<typename U>(U&& u)
            {
                if constexpr(B)
                    l = s.length();

                replicate<B>(std::forward<L>(l), std::forward<S>(s), std::forward<U>(u));
            }, std::forward<T>(t));

            if constexpr(B)
                return std::forward<S>(s);
            else
                return std::forward<T>(t);
        }

        template <bool B, typename L, typename S, typename T>
        constexpr decltype(auto) replicate(L&& l, S&& s, T&& t)
        {
            using U = std::remove_cvref_t<T>;

            if constexpr(std::is_enum_v<U> || std::is_fundamental_v<U>)
                l += copy<B, U>(std::forward<L>(l), std::forward<S>(s), std::forward<T>(t));
            else if constexpr(std::is_pointer_v<U> || requires { typename U::weak_type; })
            {
                if constexpr(!B)
                {
                    if constexpr(std::is_pointer_v<U>)
                        t = new std::remove_pointer_t<U>();
                    else
                        t = std::make_shared<typename U::element_type>();
                }

                replicate<B>(std::forward<L>(l), std::forward<S>(s), *t);
            }
            else if constexpr(requires { t.has_value(); })
            {
                bool size = t.has_value();
                l += copy<B, bool>(std::forward<L>(l), std::forward<S>(s), size);

                if (size)
                {
                    if constexpr(!B)
                        t = typename std::remove_cvref_t<T>::value_type();

                    replicate<B>(std::forward<L>(l), std::forward<S>(s), *t);
                }
            }
            else if constexpr(requires { t.begin(); t.end(); })
            {
                size_t size = 0;

                if constexpr(requires { t.size(); })
                    size = B * t.size();
                else
                    size = B * std::distance(t.begin(), t.end());

                l += copy<B, size_t>(std::forward<L>(l), std::forward<S>(s), size);

                if constexpr(std::is_same_v<U, std::string>)
                    assign<B>(std::forward<L>(l), std::forward<S>(s), std::forward<T>(t), size);
                else
                    browse<B>(std::forward<L>(l), std::forward<S>(s), std::forward<T>(t), size);
            }
            else if constexpr(std::is_class_v<U>)
                assign<B>(std::forward<L>(l), std::forward<S>(s), std::forward<T>(t));

            if constexpr(B)
                return std::forward<S>(s);
            else
                return std::forward<T>(t);
        }
    };

    template <typename S, typename T>
    constexpr decltype(auto) marshal(S&& s, T&& t)
    {
        return assigner().replicate<1>(0, std::forward<S>(s), std::forward<T>(t));
    }

    template <typename T>
    constexpr decltype(auto) marshal(T&& t)
    {
        std::string s;
        marshal(s, std::forward<T>(t));

        return s;
    }

    template <typename T>
    constexpr decltype(auto) marshal()
    {
        T t;

        return marshal(t);
    }

    template <size_t lower, size_t upper, typename S, typename T>
    constexpr decltype(auto) marshal(S&& s, T&& t)
    {
        return marshal(std::forward<S>(s), range<lower, upper>(std::forward<T>(t)));
    }

    template <size_t lower, size_t upper, typename T>
    constexpr decltype(auto) marshal(T&& t)
    {
        std::string s;
        marshal<lower, upper>(s, std::forward<T>(t));

        return s;
    }

    template <size_t lower, size_t upper, typename T>
    constexpr decltype(auto) marshal()
    {
        T t;

        return marshal<lower, upper>(t);
    }

    template <typename S, typename T>
    constexpr decltype(auto) unmarshal(size_t& l, S&& s, T&& t)
    {
        return assigner().replicate<0>(l, std::forward<S>(s), std::forward<T>(t));
    }

    template <typename S, typename T>
    constexpr decltype(auto) unmarshal(S&& s, T&& t)
    {
        size_t l = 0;
        unmarshal(l, std::forward<S>(s), std::forward<T>(t));

        return std::forward<T>(t);
    }

    template <typename T, typename S>
    constexpr decltype(auto) unmarshal(S&& s)
    {
        T t;
        unmarshal(std::forward<S>(s), t);

        return t;
    }

    template <size_t lower, size_t upper, typename S, typename T>
    constexpr decltype(auto) unmarshal(size_t& l, S&& s, T&& t)
    {
        return unmarshal(l, std::forward<S>(s), range<lower, upper>(std::forward<T>(t)));
    }

    template <size_t lower, size_t upper, typename S, typename T>
    constexpr decltype(auto) unmarshal(S&& s, T&& t)
    {
        size_t l = 0;
        unmarshal<lower, upper>(l, std::forward<S>(s), std::forward<T>(t));

        return std::forward<T>(t);
    }

    template <size_t lower, size_t upper, typename T, typename S>
    constexpr decltype(auto) unmarshal(S&& s)
    {
        T t;
        unmarshal<lower, upper>(std::forward<S>(s), t);

        return t;
    }

    template <auto... N, typename S, typename T>
    constexpr decltype(auto) serialize(S&& s, T&& t)
    {
        return marshal(std::forward<S>(s), choose<N...>(std::forward<T>(t)));
    }

    template <auto... N, typename T>
    constexpr decltype(auto) serialize(T&& t)
    {
        std::string s;
        serialize<N...>(s, std::forward<T>(t));

        return s;
    }

    template <typename T, auto... N>
    constexpr decltype(auto) serialize()
    {
        T t;

        return serialize<N...>(t);
    }

    template <auto... N, typename S, typename T>
    constexpr decltype(auto) deserialize(size_t& l, S&& s, T&& t)
    {
        return unmarshal(l, std::forward<S>(s), choose<N...>(std::forward<T>(t)));
    }

    template <auto... N, typename S, typename T>
    constexpr decltype(auto) deserialize(S&& s, T&& t)
    {
        size_t l = 0;
        deserialize<N...>(l, std::forward<S>(s), std::forward<T>(t));

        return std::forward<T>(t);
    }

    template <typename T, auto... N, typename S>
    constexpr decltype(auto) deserialize(S&& s)
    {
        T t;
        deserialize<N...>(std::forward<S>(s), t);

        return t;
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
        return smp::for_each(std::forward<F>(f), member_pointers_v<std::remove_cvref_t<T>>);
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
        if constexpr(requires { typename std::remove_cvref_t<T>::traits_type; })
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
        if constexpr(requires { std::declval<S>() << std::declval<T>(); })
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
        if constexpr(requires { std::declval<S>() >> std::declval<T>(); })
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
        if constexpr(requires { t < u; })
            return std::forward<T>(t) < std::forward<U>(u);
        else
            return tie_fuple(std::forward<T>(t)) < tie_fuple(std::forward<U>(u));
    }

    template <typename T, typename U>
    requires (!is_fuple_v<std::remove_cvref_t<T>> && !is_fuple_v<std::remove_cvref_t<U>>)
    constexpr bool ne(T&& t, U&& u)
    {
        if constexpr(requires { t != u; })
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
