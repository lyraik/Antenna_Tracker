/**
 * @file Filesystem.cpp
 * @author Dominik Gschwind * @brief
 * @version 0.1
 * @date 2019/06/11
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "Filesystem.h"
#include "utils/Utils.h"

#include <esp_spiffs.h>

namespace fs {

    esp_err_t init() {
        esp_vfs_spiffs_conf_t conf{"", "spiffs", 3, false};

        ASSERT_RET_CHECK(esp_vfs_spiffs_register(&conf), LOG_TAG);

        return ESP_OK;
    }

    esp_err_t deinit() {
        return esp_vfs_spiffs_unregister(NULL);
    }

    size_t getUsedSpace() {
        size_t totalBytes, usedBytes;
        if (esp_spiffs_info(NULL, &totalBytes, &usedBytes) != ESP_OK)
            return 0;
        return usedBytes;
    }
    size_t getCapacity() {
        size_t totalBytes, usedBytes;
        if (esp_spiffs_info(NULL, &totalBytes, &usedBytes) != ESP_OK)
            return 0;
        return totalBytes;
    }

    bool File::open(const char* path, mode_t mode) {
        m_file = fopen(path, mode);
        ASSERT_RET(m_file, false, LOG_TAG);
        return true;
    }

    bool File::tryOpen(const char* path, mode_t mode) {
        m_file = fopen(path, mode);
        return m_file;
    }

    void File::close() {
        if (m_file)
            fclose(m_file);
        m_file = nullptr;
    }

    bool File::flush() {
        ASSERT_RET(fflush(m_file) == 0, false, LOG_TAG);
        return true;
    }

    size_t File::read(void* dst, size_t count) {
        auto result = fread(dst, 1, count, m_file);
        ASSERT_RET(ferror(m_file) == 0, result, LOG_TAG);
        return result;
    }

    size_t File::write(const void* src, size_t count) {
        auto result = fwrite(src, 1, count, m_file);
        ASSERT_RET(ferror(m_file) == 0, result, LOG_TAG);
        return result;
    }

    bool File::rename(const char* oldName, const char* newName) {
        ASSERT_RET(::rename(oldName, newName) == 0, false, LOG_TAG);
        return true;
    }

    bool File::remove(const char* name) {
        ASSERT_RET(::remove(name) == 0, false, LOG_TAG);
        return true;
    }

    size_t File::length() {
        ASSERT_RET(fseek(m_file, 0, SEEK_END) == 0, 0, LOG_TAG);

        long int size = ftell(m_file);
        rewind();

        ASSERT_RET(size != -1, 0, LOG_TAG);
        return size;
    }

}  // namespace fs