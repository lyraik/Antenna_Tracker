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
#include <type_traits>
#include "Utils.h"

namespace utils {

    struct StringView {
        static constexpr const char* EMPTY = "";

       protected:
        const char* m_str;
        size_t m_len;

       public:
        constexpr StringView() : m_str(EMPTY), m_len(0) {}
        template <size_t N>
        constexpr StringView(const char (&arr)[N]) : m_str(arr), m_len(N - 1) {}
        constexpr StringView(const char* str, size_t length) : m_str(str), m_len(length) {}

        explicit StringView(const char* str) : m_str(str), m_len(strlen(str)) {}

        inline const char* str() const {
            return m_str;
        }
        inline size_t len() const {
            return m_len;
        }

        friend bool operator==(const StringView& lhs, const StringView& rhs) {
            if (lhs.len() == rhs.len())
                return strncmp(lhs.str(), rhs.str(), std::min(lhs.len(), rhs.len())) == 0;
            return false;
        }
        friend bool operator!=(const StringView& lhs, const StringView& rhs) {
            return !(lhs == rhs);
        }

        StringView splitAtLastOccurrence(char c) const {
            const char* newStr = str() + len() - 1;
            for (size_t i = 0; i < len(); ++i, --newStr) {
                if (*newStr == c) {
                    if (!i)
                        break;
                    return StringView{newStr + 1, i};
                }
            }
            return StringView{};
        }

        bool startsWith(const StringView& comp, StringView* end = nullptr) const {
            if (comp.len() > m_len || empty()) {
                if (end)
                    *end = StringView{};
                return false;
            }
            if (comp.empty()) {
                if (end)
                    *end = *this;
                return true;
            }

            const char* iter = str();
            const char* iterComp = comp.str();

            size_t i = 0;
            for (; i < comp.len(); ++iter, ++iterComp, ++i) {
                if (*iter != *iterComp)
                    break;
            }

            if (i >= comp.len()) {
                if (end)
                    *end = StringView{iter, m_len - i};
                return true;
            }
            if (end)
                *end = StringView{};
            return false;
        }

        inline bool empty() const {
            return !m_str || !m_len || !(*m_str);
        }

        class Splitter {
           public:
            class Iterator : public virtual std::iterator<std::input_iterator_tag, StringView> {
                char m_delim;
                const StringView* m_str;
                size_t m_curr, m_last;

               public:
                Iterator(const StringView& str, char delim) : m_delim(delim), m_str(&str), m_curr(0), m_last(0) {}
                Iterator(const StringView& str, const char* curr) : m_delim(0), m_str(&str), m_curr(str.len()), m_last(str.len()) {}

                Iterator& operator++(void) {
                    if (m_curr < m_str->len())
                        ++m_curr;
                    m_last = m_curr;
                    for (; m_curr < m_str->len(); ++m_curr) {
                        if (m_str->str()[m_curr] == m_delim) {
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
                    if (m_last >= m_str->len())
                        return StringView{};
                    return StringView{m_str->str() + m_last, (size_t)(m_curr - m_last)};
                }

                friend bool operator==(const Iterator& lhs, const Iterator& rhs) {
                    return lhs.m_curr == rhs.m_curr && lhs.m_last == rhs.m_last && lhs.m_str->str() == rhs.m_str->str();
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
                auto end = Iterator{m_str, m_str.str() + m_str.len()};
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
        using DtorType = void (*)(char*);

       public:
        static void defaultDelete(char* data) {
            delete[] data;
        }
        static void freeDelete(char* data) {
            free(data);
        }

       private:
        size_t m_cap{0};
        DtorType m_dtor{nullptr};

       public:
        constexpr String() : StringView() {}
        template <size_t N>
        constexpr String(const char (&arr)[N]) : StringView(arr, N - 1) {}
        String(const char* str, size_t length, bool shouldCopy) : StringView(str, length) {
            if (shouldCopy)
                copy(str, length);
        }
        String(const StringView& str) : String(str.str(), str.len(), false) {}
        constexpr String(char* str, size_t length, size_t cap, DtorType dtor) : StringView(str, length), m_cap(cap), m_dtor(dtor) {}
        String(size_t cap) : String() {
            allocate(cap);
        }

        String(const String& obj) = delete;
        String(String&& obj) : StringView(static_cast<const StringView&>(obj)), m_cap(obj.m_cap), m_dtor(obj.m_dtor) {
            obj.init();
        }

        ~String() {
            destruct();
        }

       private:
        inline void init() {
            m_str = EMPTY;
            m_len = 0;
            m_cap = 0;
            m_dtor = nullptr;
        }

       public:
        inline char* buf() {
            ASSERT(owner(), LOG_TAG);
            return const_cast<char*>(m_str);
        }
        inline size_t cap() const {
            return m_cap;
        }
        inline bool owner() const {
            return m_dtor;
        }

        char* release() {
            char* result = buf();
            init();
            return result;
        }

        void destruct() {
            if (owner() && m_str) {
                ASSERT(m_cap, LOG_TAG);
                ASSERT(m_dtor, LOG_TAG);

                m_dtor(buf());
                init();
            }
        }

        void clear() {
            if (!owner())
                return;
            memset(buf(), 0, cap());
        }

        bool allocate(size_t count) {
            destruct();

            m_str = new char[count + 1];
            if (!m_str) {
                init();
                return false;
            }
            m_dtor = defaultDelete;
            buf()[count] = '\0';
            m_cap = count;
            return true;
        }

        bool copy(const char* str, size_t len) {
            if (!str)
                return false;
            if (!allocate(len))
                return false;
            memcpy(buf(), str, len);
            m_len = len;
            return true;
        }
        bool copy() {
            if (owner() || empty())
                return false;
            return copy(m_str, m_len);
        }

        static String fromNumber(size_t value) {
            String result{10};
            if (!result.owner())
                return {};

            char* str = result.buf();
            uint8_t i = 0;
            for (++value; value > 0; ++i) {
                size_t digit = (value - 1) % 10;
                value = (value - digit) / 10;

                *str = '0' + digit;
                ++str;
            }

            --str;
            for (char* begin = result.buf(); begin < str; ++begin, --str) {
                char temp = *begin;
                *begin = *str;
                *str = temp;
            }
            result.buf()[i] = 0;
            result.m_len = i;
            return result;
        }

        static String makeHex(const uint8_t* buf, size_t len) {
            String result{2 * len};

            for (size_t i = 0; i < len; ++i, buf += 2) {
                snprintf(&result.buf()[i << 1], 2, "%02hhX", *buf);
            }
            return result;
        }

        static String create(const char* str, size_t length) {
            if (!str)
                return {};
            return String{str, length, true};
        }

        static String fromCStr(const char* str) {
            return create(str, strlen(str));
        }

        static String create(size_t capacity) {
            return String{capacity};
        }

        const StringView& view() const{
            return *this;
        }

        String& operator=(const String& obj) = delete;
        String& operator=(String&& obj) {
            m_str = obj.m_str;
            m_len = obj.m_len;
            m_dtor = obj.m_dtor;
            m_cap = obj.m_cap;

            obj.init();
            return *this;
        }
    };

}  // namespace utils

#endif  //_String_h_