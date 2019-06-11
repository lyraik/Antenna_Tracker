/**
 * @file Filesystem.h
 * @author Dominik Gschwind
 * @brief
 * @version 0.1
 * @date 2019/06/11
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once
#ifndef _Filesystem_h_
#define _Filesystem_h_

#include <errno.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_vfs.h>
#include "utils/String.h"

namespace fs {
    static constexpr const char LOG_TAG[] = "fs";

    esp_err_t init();
    esp_err_t deinit();

    size_t getUsedSpace();
    size_t getCapacity();

    using mode_t = const char*;
    struct Mode {
        static constexpr mode_t READ = "r";
        static constexpr mode_t write = "w";
        static constexpr mode_t append = "a";

        static constexpr mode_t readu = "r+";
        static constexpr mode_t writeu = "w+";
        static constexpr mode_t appendu = "a+";
    };

    class File {
       private:
        FILE* m_file;

       public:
        File() : m_file(nullptr) {}
        File(FILE* file) : m_file(file) {}

        ~File() {
            close();
        }

        bool valid(){return m_file;}

        FILE* get() {
            return m_file;
        }
        FILE* release() {
            auto result = m_file;
            m_file = nullptr;
            return result;
        }

        bool open(const char* path, mode_t mode);
        void close();
        bool flush();

        size_t getPos() {
            return ftell(m_file);
        }
        bool setPos(size_t pos, int offset = 0) {
            return fseek(m_file, pos, offset);
        }
        void rewind() {
            return ::rewind(m_file);
        }
        bool isEOF() {
            return feof(m_file) != 0;
        }

        size_t read(void* dst, size_t count);
        size_t write(const void* src, size_t count);

        static bool rename(const char* oldName, const char* newName);
        static bool remove(const char* name);
    };

}  // namespace fs

#endif  //_Filesystem_h_