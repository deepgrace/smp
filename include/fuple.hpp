//
// Copyright (c) 2022-present DeepGrace (complex dot invoke at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/deepgrace/smp
//

#ifndef FUPLE_HPP
#define FUPLE_HPP

#include <utility>
#include <algorithm>
#include <functional>
#include <lists.hpp>

namespace smp
{
    template <typename T>
    using urd_t = std::unwrap_ref_decay_t<T>;

    template <typename T, typename... Args>
    using outer_t = typename std::conditional_t<!sizeof...(Args), T, std::type_identity<T>>::type;

    template <typename T>
    using inner_t = outer_t<urd_t<outer_t<T>>>;

    struct ignore_t
    {
        template <typename T>
        constexpr decltype(auto) operator=(T&&) const noexcept
        {
        }
    };

    inline constexpr ignore_t ignore;

    template <size_t N, typename T>
    struct element
    {
        using type = T;

        T value;
    };

    template <size_t N, typename T>
    constexpr T& select(element<N, T>& m) noexcept
    {
        return m.value;
    }

    template <size_t N, typename T>
    constexpr T&& select(element<N, T>&& m) noexcept
    {
        return std::forward<T>(m.value);
    }

    template <size_t N, typename T>
    constexpr const T& select(const element<N, T>& m) noexcept
    {
        return m.value;
    }

    template <typename T, typename... Args>
    struct elements;

    template <>
    struct elements<std::index_sequence<>>
    {
        using type = lists<>;

        constexpr elements() = default;

        constexpr elements(elements&&) = default;
        constexpr elements(const elements&) = default;

        static constexpr size_t value = 0;

        static constexpr size_t size() noexcept
        {
            return value;
        }
    };

    template <size_t... N, typename... Args>
    struct elements<std::index_sequence<N...>, Args...> : element<N, Args>...
    {
        using type = lists<element<N, Args>...>;

        constexpr elements() = default;

        constexpr elements(elements&&) = default;
        constexpr elements(const elements&) = default;

        constexpr elements(Args&&... args) : element<N, Args>(std::forward<Args>(args))...
        {
        }

        static constexpr size_t value = sizeof...(Args);

        static constexpr size_t size() noexcept
        {
            return value;
        }
    };

    template <typename... Args>
    struct fuple : elements<std::index_sequence_for<Args...>, Args...>
    {
        using elements<std::index_sequence_for<Args...>, Args...>::elements;

        constexpr fuple() = default;

        constexpr fuple(fuple&&) = default;
        constexpr fuple(const fuple&) = default;

        template <typename T>
        constexpr fuple& assign(T&& t)
        {
            [&]<size_t... N>(std::index_sequence<N...>)
            {
                (..., (select<N>(*this) = select<N>(std::forward<T>(t))));
            }
            (std::index_sequence_for<Args...>());

            return *this;
        }

        constexpr fuple& operator=(fuple<Args...>&& r)
        {
            return assign(std::move(r));
        }

        constexpr fuple& operator=(const fuple<Args...>& r)
        {
            return assign(r);
        }

        template <typename... Brgs>
        constexpr fuple& operator=(fuple<Brgs...>&& r)
        {
            return assign(std::move(r));
        }

        template <typename... Brgs>
        constexpr fuple& operator=(const fuple<Brgs...>& r)
        {
            return assign(r);
        }
    };

    template <typename... Args>
    fuple(Args...) -> fuple<Args...>;

    template <typename T>
    struct is_fuple : std::false_type
    {
    };

    template <typename... Args>
    struct is_fuple<fuple<Args...>> : std::true_type
    {
    };

    template <typename T>
    inline constexpr auto is_fuple_v = is_fuple<T>::value;

    template <template <typename ...> typename F, typename T, typename U = T, typename... Args>
    constexpr size_t index()
    {
        return F<T, U>() ? 0 : index<F, T, Args...>() + 1;
    }

    template <bool strip, typename... Args>
    constexpr size_t index()
    {
        return index<std::is_same, std::conditional_t<strip, std::remove_cvref_t<Args>, Args>...>();
    }

    template <typename T, bool strip = 0, typename... Args>
    constexpr decltype(auto) fuple_index(fuple<Args...>& t) noexcept
    {
        return index<strip, T, Args...>();
    }

    template <typename T, bool strip = 0, typename... Args>
    constexpr decltype(auto) fuple_index(fuple<Args...>&& t) noexcept
    {
        return index<strip, T, Args...>();
    }

    template <typename T, bool strip = 0, typename... Args>
    constexpr decltype(auto) fuple_index(const fuple<Args...>& t) noexcept
    {
        return index<strip, T, Args...>();
    }

    template <size_t N, typename... Args>
    requires (N < sizeof...(Args))
    constexpr decltype(auto) get(fuple<Args...>& t) noexcept
    {
        return select<N>(t);
    }

    template <size_t N, typename... Args>
    requires (N < sizeof...(Args))
    constexpr decltype(auto) get(fuple<Args...>&& t) noexcept
    {
        return select<N>(std::move(t));
    }

    template <size_t N, typename... Args>
    requires (N < sizeof...(Args))
    constexpr decltype(auto) get(const fuple<Args...>& t) noexcept
    {
        return select<N>(t);
    }

    template <typename T, typename... Args>
    constexpr decltype(auto) get(fuple<Args...>& t) noexcept
    {
        return select<index<1, T, Args...>()>(t);
    }

    template <typename T, typename... Args>
    constexpr decltype(auto) get(fuple<Args...>&& t) noexcept
    {
        return select<index<1, T, Args...>()>(std::move(t));
    }

    template <typename T, typename... Args>
    constexpr decltype(auto) get(const fuple<Args...>& t) noexcept
    {
        return select<index<1, T, Args...>()>(t);
    }

    template <typename T>
    struct fuple_size;

    template <typename... Args>
    struct fuple_size<fuple<Args...>> : std::integral_constant<size_t, sizeof...(Args)>
    {
    };

    template <typename T>
    inline constexpr size_t fuple_size_v = fuple_size<T>::value;

    template <typename T>
    inline constexpr size_t length_v = fuple_size_v<std::remove_cvref_t<T>>;

    template <size_t N, typename T>
    struct fuple_element;

    template <size_t N, typename... Args>
    struct fuple_element<N, fuple<Args...>>
    {
        using type = std::remove_cvref_t<decltype(get<N>(std::declval<fuple<Args...>>()))>;
    };

    template <size_t N, typename T>
    using fuple_element_t = outer_t<fuple_element<N, T>>;

    template <typename... Args, typename... Brgs>
    constexpr decltype(auto) swap(fuple<Args...>& p, fuple<Brgs...>& q)
    {
        constexpr size_t l = fuple<Args...>::size();
        constexpr size_t r = fuple<Brgs...>::size();

        [&]<size_t... N>(std::index_sequence<N...>)
        {
            (..., std::swap(select<N>(p), select<N>(q)));
        }
        (std::make_index_sequence<l < r ? l : r>());
    }

    template <typename... Args>
    constexpr decltype(auto) tie(Args&... args)
    {
        return fuple<Args&...>(args...);
    }

    template <typename... Args>
    constexpr decltype(auto) make_fuple(Args&&... args)
    {
        return fuple(static_cast<urd_t<Args>>(args)...);
    }

    template <typename... Args>
    constexpr decltype(auto) forward_as_fuple(Args&&... args)
    {
        return fuple<Args&&...>(std::forward<Args>(args)...);
    }

    template <typename T, template <typename ...> typename U, typename... Args>
    constexpr decltype(auto) outer(U<Args...>)
    {
        return U<outer_t<T, Args>...>();
    }

    template <template <typename ...> typename T, typename... Args>
    constexpr decltype(auto) outer(T<Args...>)
    {
        return (... + outer<Args>(inner_t<Args>()));
    }

    template <template <typename ...> typename T, typename... Args>
    constexpr decltype(auto) inner(T<Args...>)
    {
        return (... + inner_t<Args>());
    }

    template <typename T, typename U>
    constexpr decltype(auto) over(U&& u)
    {
        return std::forward<outer_t<T>>(std::forward<U>(u).T::value);
    }

    template <typename... Args>
    requires (is_fuple_v<std::remove_cvref_t<Args>> && ...)
    constexpr decltype(auto) fuple_cat(Args&&... args)
    {
        if constexpr(!sizeof...(Args))
            return fuple();
        else
        {
            decltype(auto) f = outer_t<fuple<Args&&...>>();
            decltype(auto) t = forward_as_fuple(std::forward<Args>(args)...);

            return [&]<template <typename ...> typename T, typename... O, typename... I>(T<O...>, T<I...>)
            {
                return forward_as_fuple(over<I>(over<O>(t))...);
            }
            (outer(f), inner(f));
        }
    }

    template <size_t lower, size_t upper, typename F, typename... Args>
    requires (is_fuple_v<std::remove_cvref_t<Args>> && ...)
    void zip(F&& f, Args&&... args)
    {
        std::invoke(std::forward<F>(f), get<lower>(std::forward<Args>(args))...);

        if constexpr(lower + 1 != upper)
            zip<lower + 1, upper>(std::forward<F>(f), std::forward<Args>(args)...);
    }

    template <typename F, typename... Args>
    requires (is_fuple_v<std::remove_cvref_t<Args>> && ...)
    void zip(F&& f, Args&&... args)
    {
        constexpr size_t size = std::min({length_v<Args>...});

        zip<0, size>(std::forward<F>(f), std::forward<Args>(args)...);
    }

    template <typename F, typename T>
    requires (is_fuple_v<std::remove_cvref_t<T>> || is_tuple_v<std::remove_cvref_t<T>>)
    constexpr decltype(auto) for_each(F&& f, T&& t)
    {
        return [&]<size_t... N>(std::index_sequence<N...>)
        {
            if constexpr(is_fuple_v<std::remove_cvref_t<T>>)
                (..., std::invoke(std::forward<F>(f), smp::get<N>(std::forward<T>(t))));
            else
                (..., std::invoke(std::forward<F>(f), std::get<N>(std::forward<T>(t))));

            return std::forward<F>(f);
        }
        (rank<T>());
    }

    template <typename F, typename T>
    requires is_fuple_v<std::remove_cvref_t<T>>
    constexpr decltype(auto) apply(F&& f, T&& t)
    {
        return [&]<size_t... N>(std::index_sequence<N...>)
        {
            return std::invoke(std::forward<F>(f), get<N>(std::forward<T>(t))...);
        }
        (rank<T>());
    }

    template <size_t i, size_t j, typename F, typename T, typename U>
    requires (is_fuple_v<std::remove_cvref_t<T>> && is_fuple_v<std::remove_cvref_t<U>>)
    constexpr decltype(auto) apply(F&& f, T&& t, U&& u)
    {
        if constexpr(i == j)
            return std::invoke(std::forward<F>(f), std::remove_cvref_t<T>::size(), std::remove_cvref_t<U>::size());
        else
        {
            decltype(auto) l = get<i>(std::forward<T>(t));
            decltype(auto) r = get<i>(std::forward<U>(u));

            return std::invoke(std::forward<F>(f), l, r) || (l == r && apply<i + 1, j>(std::forward<F>(f), std::forward<T>(t), std::forward<U>(u)));
        }
    }

    template <typename F, typename T, typename U>
    requires (is_fuple_v<std::remove_cvref_t<T>> && is_fuple_v<std::remove_cvref_t<U>>)
    constexpr decltype(auto) apply(F&& f, T&& t, U&& u)
    {
        constexpr size_t l = std::remove_cvref_t<T>::size();
        constexpr size_t r = std::remove_cvref_t<U>::size();

        return apply<0, l < r ? l : r>(std::forward<F>(f), std::forward<T>(t), std::forward<U>(u));
    }

    template <typename T, typename U>
    requires (is_fuple_v<std::remove_cvref_t<T>> && is_fuple_v<std::remove_cvref_t<U>>)
    constexpr bool operator<(T&& t, U&& u)
    {
        return apply(std::less<>(), std::forward<T>(t), std::forward<U>(u));
    }

    template <typename T, typename U>
    requires (is_fuple_v<std::remove_cvref_t<T>> && is_fuple_v<std::remove_cvref_t<U>>)
    constexpr bool operator!=(T&& t, U&& u)
    {
        return apply(std::not_equal_to<>(), std::forward<T>(t), std::forward<U>(u));
    }

    template <typename T, typename U>
    requires (is_fuple_v<std::remove_cvref_t<T>> && is_fuple_v<std::remove_cvref_t<U>>)
    constexpr bool operator<=(T&& t, U&& u)
    {
        return !(std::forward<U>(u) < std::forward<T>(t));
    }

    template <typename T, typename U>
    requires (is_fuple_v<std::remove_cvref_t<T>> && is_fuple_v<std::remove_cvref_t<U>>)
    constexpr bool operator==(T&& t, U&& u)
    {
        return !(std::forward<T>(t) != std::forward<U>(u));
    }

    template <typename T, typename U>
    requires (is_fuple_v<std::remove_cvref_t<T>> && is_fuple_v<std::remove_cvref_t<U>>)
    constexpr bool operator>=(T&& t, U&& u)
    {
        return std::forward<U>(u) <= std::forward<T>(t);
    }

    template <typename T, typename U>
    requires (is_fuple_v<std::remove_cvref_t<T>> && is_fuple_v<std::remove_cvref_t<U>>)
    constexpr bool operator>(T&& t, U&& u)
    {
        return std::forward<U>(u) < std::forward<T>(t);
    }

    template <bool f, bool t, typename U>
    requires (is_fuple_v<std::remove_cvref_t<U>> || is_tuple_v<std::remove_cvref_t<U>>)
    static constexpr decltype(auto) expand(U&& u)
    {
        if constexpr(f)
        {
            return smp::apply([]<typename... Args>(Args&&... args)
            {
                if constexpr(t)
                    return smp::tie(std::forward<Args>(args)...);
                else
                    return std::make_tuple(std::forward<Args>(args)...);
            }, std::forward<U>(u));
        }
        else
        {
            return std::apply([]<typename... Args>(Args&&... args)
            {
                if constexpr(t)
                    return std::tie(std::forward<Args>(args)...);
                else
                    return smp::make_fuple(std::forward<Args>(args)...);
            }, std::forward<U>(u));
        }
    }

    template <typename T>
    requires is_fuple_v<std::remove_cvref_t<T>>
    constexpr decltype(auto) fuple_to_tuple(T&& t)
    {
        return expand<1, 0>(std::forward<T>(t));
    }

    template <typename T>
    requires is_tuple_v<std::remove_cvref_t<T>>
    constexpr decltype(auto) tuple_to_fuple(T&& t)
    {
        return expand<0, 0>(std::forward<T>(t));
    }

    template <typename T>
    requires is_fuple_v<std::remove_cvref_t<T>>
    constexpr decltype(auto) tied_fuple(T&& t)
    {
        return expand<1, 1>(std::forward<T>(t));
    }

    template <typename T>
    requires is_tuple_v<std::remove_cvref_t<T>>
    constexpr decltype(auto) tied_tuple(T&& t)
    {
        return expand<0, 1>(std::forward<T>(t));
    }

    template <typename T, typename U = std::byte>
    struct aligned_storage
    {
        alignas(T) U data[sizeof(T)];
    };

    template <typename T = std::byte, typename... Args>
    struct aligned_union
    {
        alignas(Args...) T data[std::max({sizeof(Args)...})];
    };

    template <typename T, typename U = std::byte>
    struct aligned_fuple;

    template <template <typename ...> typename T, typename... Args, typename U>
    struct aligned_fuple<T<Args...>, U>
    {
        using type = fuple<aligned_storage<Args, U>...>;

        static constexpr size_t value = sizeof...(Args);

        static constexpr size_t size() noexcept
        {
            return value;
        }
    };

    template <typename T, typename U = std::byte>
    using aligned_fuple_t = outer_t<aligned_fuple<T, U>>;
}

#endif
