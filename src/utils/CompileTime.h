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

}  // namespace utils

#endif  //_CompileTime_h_