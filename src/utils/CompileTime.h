/**
 * @file CompileTime.h
 * @author Dominik Gschwind
 * @brief
 * @version 0.1
 * @date 2019/06/16
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once
#ifndef _CompileTime_h_
#define _CompileTime_h_

#include <tuple>
#include <utility>

namespace utils {

    template <std::size_t I = 0, typename FuncResT, typename Arg0T, typename Arg1T, typename... Tp>
    constexpr inline typename std::enable_if<I == sizeof...(Tp), void>::type forEach(std::tuple<Tp...>&,
                                                                                     FuncResT (*)(Arg0T, Arg1T, FuncResT),
                                                                                     Arg1T,
                                                                                     FuncResT)  // Unused arguments are given no names.
    {}

    template <std::size_t I = 0, typename FuncResT, typename Arg0T, typename Arg1T, typename... Tp>
    constexpr inline typename std::enable_if<(I < sizeof...(Tp)), void>::type forEach(std::tuple<Tp...>& t, FuncResT (*f)(Arg0T, Arg1T, FuncResT), Arg1T arg1, FuncResT arg2) {
        auto result = f(std::get<I>(t), arg1, arg2);
        forEach<I + 1, FuncResT, Arg0T, Arg1T, Tp...>(t, f, arg1, result);
    }

    template <size_t... Is>
    struct seq {};
    template <size_t N, size_t... Is>
    struct gen_seq : gen_seq<N - 1, N - 1, Is...> {};
    template <size_t... Is>
    struct gen_seq<0, Is...> : seq<Is...> {};

    template <size_t N, class T, class F>
    void apply_one(T& p, F func) {
        func(std::get<N>(p));
    }

    template <class T, class F, size_t... Is>
    void apply(T& p, int index, F func, seq<Is...>) {
        using FT = void(T&, F);
        static constexpr FT* arr[] = {&apply_one<Is, T, F>...};
        arr[index](p, func);
    }

    template <class T, class F>
    void apply(T& p, size_t index, F func) {
        apply(p, index, func, gen_seq<std::tuple_size<T>::value>{});
    }

    template <size_t N, class Tuple, class CommonT>
    constexpr inline CommonT get_one(const Tuple& p) {
        return static_cast<CommonT>(std::get<N>(p));
    }

    template <class CommonT, class Tuple, size_t... Is>
    CommonT get(const Tuple& tuple, size_t index, seq<Is...>) {
        using Func = CommonT (*)(const Tuple&);
        static constexpr Func arr[] = {&get_one<Is, Tuple, CommonT>...};
        return arr[index](tuple);
    }

    template <class CommonT, class... Ts>
    CommonT get(const std::tuple<Ts...>& tuple, size_t index) {
        return get<CommonT>(tuple, index, gen_seq<sizeof...(Ts)>{});
    }

}  // namespace utils

#endif  //_CompileTime_h_