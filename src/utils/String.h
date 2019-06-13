/**
 * @file String.h
 * @author Dominik Gschwind
 * @brief
 * @version 0.1
 * @date 2019/06/10
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once
#ifndef _String_h_
#define _String_h_

#include <stddef.h>

namespace utils {

    struct StringView {
        char* str;
        size_t length;

        template <size_t N>
        StringView(const char (&arr)[N]) : str(const_cast<char*>(arr)), length(N - 1) {}

        StringView(const char* str, size_t length) : str(const_cast<char*>(str)), length(length) {}

        StringView() : StringView(nullptr, 0) {}

        friend bool operator==(const StringView& lhs, const StringView& rhs) {
            if (lhs.length != rhs.length)
                return false;
            return strncmp(lhs.str, rhs.str, lhs.length) == 0;
        }

        StringView splitAtLastOccurrence(char c) {
            char* newStr = str + length - 1;
            for(size_t i = 0; i < length; ++i, --newStr){
                if(*newStr == c){
                    if(!i) break;
                    return StringView{newStr + 1, i};
                }
            }

            return StringView{};
        }
    };

}  // namespace utils

#endif  //_String_h_