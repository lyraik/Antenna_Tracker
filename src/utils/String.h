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
#include <iterator>
#include "Utils.h"

namespace utils {

    struct StringView {
        static constexpr const char* EMPTY = "";

        char* str;
        size_t length;

        template <size_t N>
        constexpr StringView(const char (&arr)[N]) : str(const_cast<char*>(arr)), length(N - 1) {}

        StringView(const char* str, size_t length) : str(const_cast<char*>(str)), length(length) {}

        StringView() : StringView(EMPTY, 0) {}

        friend bool operator==(const StringView& lhs, const StringView& rhs) {
            if (lhs.length != rhs.length)
                return false;
            return strncmp(lhs.str, rhs.str, lhs.length) == 0;
        }
        friend bool operator!=(const StringView& lhs, const StringView& rhs) {
            return !(lhs == rhs);
        }

        StringView splitAtLastOccurrence(char c) const {
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

        bool startsWith(const StringView& comp, StringView* end = nullptr) const {
            if (comp.length > length || empty()) {
                if (end)
                    *end = StringView{};
                return false;
            }
            if (comp.empty()) {
                if (end)
                    *end = *this;
                return true;
            }

            const char* iter = str;
            const char* iterComp = comp.str;

            size_t i = 0;
            for (; i < comp.length; ++iter, ++iterComp, ++i) {
                if (*iter != *iterComp)
                    break;
            }

            if (i >= comp.length) {
                if (end)
                    *end = StringView{iter, length - i};
                return true;
            }
            if (end)
                *end = StringView{};
            return false;
        }

        bool empty() const {
            return !str || !(*str) || !length;
        }

        class Splitter {
           public:
            class Iterator : public virtual std::iterator<std::input_iterator_tag, StringView> {
                char m_delim;
                const StringView* m_str;
                size_t m_curr, m_last;

               public:
                Iterator(const StringView& str, char delim) : m_delim(delim), m_str(&str), m_curr(0), m_last(0) {}
                Iterator(const StringView& str, const char* curr) : m_delim(0), m_str(&str), m_curr(str.length), m_last(str.length) {}

                Iterator& operator++(void) {
                    if (m_curr < m_str->length)
                        ++m_curr;
                    m_last = m_curr;
                    for (; m_curr < m_str->length; ++m_curr) {
                        if (m_str->str[m_curr] == m_delim) {
                            break;
                        }
                    }
                    return *this;
                }

                Iterator operator++(int) {
                    Iterator result{*this};
                    ++(*this);
                    return result;
                }

                value_type operator*() const {
                    return StringView{m_str->str + m_last, (size_t)(m_curr - m_last)};
                }

                friend bool operator==(const Iterator& lhs, const Iterator& rhs) {
                    return lhs.m_curr == rhs.m_curr && lhs.m_last == rhs.m_last;
                }
                friend bool operator!=(const Iterator& lhs, const Iterator& rhs) {
                    return !(lhs == rhs);
                }
            };

           private:
            const StringView& m_str;
            char m_delim;

           public:
            Splitter(const StringView& str, char delim) : m_str(str), m_delim(delim) {}

            Iterator begin() {
                return Iterator{m_str, m_delim};
            }
            Iterator end() {
                auto end = Iterator{m_str, m_str.str + m_str.length};
                auto iter = begin();
                auto lastNonEmpty = iter;
                for (; iter != end; ++iter)
                    if (!(*iter).empty())
                        lastNonEmpty = iter;

                return iter;
            }
        };
        Splitter split(char delim) {
            return Splitter{*this, delim};
        }
    };

    struct String : public StringView {
        using Destructor = void (*)(char*);

       public:
        static void defaultDestructor(char* data) {
            delete[] data;
        }
        static void freeDestructor(char* data) {
            free(data);
        }

       public:
        Destructor dtor;
        size_t capacity;

        String(const String& obj) : StringView((const char*)obj.str, obj.length), dtor(nullptr), capacity(0) {}
        String(String&& obj) : StringView((const char*)obj.str, obj.length), dtor(obj.dtor), capacity(obj.capacity) {
            obj.str = nullptr;
            obj.dtor = nullptr;
        }

        String() : StringView(), dtor(nullptr), capacity(0) {}
        String(const StringView& str) : StringView(str), dtor(nullptr), capacity(0) {}
        String(char* str, size_t length, size_t cap, Destructor destructor = defaultDestructor) : StringView(str, length), dtor(destructor), capacity(cap) {}
        String(const char* buf, size_t len, bool copy) {
            if (!copy) {
                *this = String{StringView{buf, length}};
            } else {
                if (allocate(length)) {
                    memcpy(str, buf, length);
                    length = len;
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
            str = new char[count + 1];
            if (!str) {
                return false;
            }
            capacity = count;
            str[count] = 0;
            dtor = defaultDestructor;
            return true;
        }

        void destruct() {
            if (dtor && str) {
                dtor(str);
            }
            str = nullptr;
            length = 0;
            capacity = 0;
            dtor = nullptr;
        }

        static String fromNumber(size_t num) {
            auto result = create(10);
            char* str = result.str - 1;
            uint8_t i = 0;
            for (; num > 0; ++i) {
                size_t digit = num % 10;
                num = num / 10;

                ++str;
                *str = '0' + digit;
            }

            for (char* begin = result.str; begin < str; ++begin, --str) {
                char temp = *begin;
                *begin = *str;
                *str = temp;
            }
            result.str[i] = 0;
            return result;
        }

        static String create(const char* str, bool copy = true) {
            if (!str)
                return {};
            size_t length = strlen(str);

            String result{};
            if (!result.allocate(length))
                return {};
            memcpy(result.str, str, length);
            result.length = length;

            return result;
        }

        static String create(size_t capacity) {
            char* buf = new char[capacity + 1];
            ASSERT_RET(buf, String{}, LOG_TAG);
            buf[capacity] = 0;
            return String{buf, capacity, capacity};
        }

        String& operator=(const String& obj) {
            str = obj.str;
            length = obj.length;
            capacity = 0;
            dtor = nullptr;
            return *this;
        }
        String& operator=(String&& obj) {
            str = obj.str;
            length = obj.length;
            dtor = obj.dtor;
            capacity = obj.capacity;

            obj.str = nullptr;
            obj.dtor = nullptr;
            return *this;
        }
    };

}  // namespace utils

#endif  //_String_h_