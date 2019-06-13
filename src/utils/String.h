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

#include <mem.h>
#include <stddef.h>
#include <string.h>

namespace utils {

    struct StringView {
        static constexpr const char* empty = "";

        char* str;
        size_t length;

        template <size_t N>
        StringView(const char (&arr)[N]) : str(const_cast<char*>(arr)), length(N - 1) {}

        StringView(const char* str, size_t length) : str(const_cast<char*>(str)), length(length) {}

        StringView() : StringView(empty, 0) {}

        friend bool operator==(const StringView& lhs, const StringView& rhs) {
            if (lhs.length != rhs.length)
                return false;
            return strncmp(lhs.str, rhs.str, lhs.length) == 0;
        }

        StringView splitAtLastOccurrence(char c) {
            char* newStr = str + length - 1;
            for (size_t i = 0; i < length; ++i, --newStr) {
                if (*newStr == c) {
                    if (!i)
                        break;
                    return StringView{newStr + 1, i};
                }
            }

            return StringView{};
        }
    };

    struct String : public StringView {
        using Destructor = void (*)(char*);

       public:
        static void defaultDestructor(char* data) {
            delete[] data;
        }

       public:
        Destructor dtor;
        size_t capacity;

        String(const String&) = default;
        String(String&&) = default;

        String() : StringView(), dtor(nullptr), capacity(0) {}
        String(const StringView& str) : StringView(str), dtor(nullptr), capacity(0) {}
        String(const char* str, size_t length, size_t cap, Destructor destructor = defaultDestructor) : StringView(str, length), dtor(destructor), capacity(cap) {}
        String(const char* buf, size_t length, bool copy) {
            if (!copy) {
                *this = String{StringView{buf, length}};
            } else {
                if (allocate(length)) {
                    memcpy(str, buf, length);
                }
            }
        }

        ~String() {
            destruct();
        }

        void clear() {
            if (!dtor)
                return;
            memset(str, 0, capacity);
        }

        bool allocate(size_t count) {
            capacity = count;
            str = new char[count + 1];
            if (!str) {
                *this = String{};
                return false;
            }
            str[count] = 0;
            dtor = defaultDestructor;
            return true;
        }

        void destruct() {
            if (dtor) {
                dtor(str);
            }
            *this = StringView{};
        }

        String& operator=(const String&) = default;
        String& operator=(String&&) = default;
    };

}  // namespace utils

#endif  //_String_h_