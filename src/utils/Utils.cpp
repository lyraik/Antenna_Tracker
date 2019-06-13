/**
 * @file Utils.cpp
 * @author Dominik Gschwind
 * @brief
 * @version 0.1
 * @date 2019/06/11
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "Utils.h"

#include <esp_err.h>

namespace utils {
    void logWarn(const char* tag, const char* check, int32_t errCode, const char* file, uint32_t line) {
        ESP_LOGW(tag, "[%s:%d] Check '%s' failed (%d).", file, line, check, errCode);
    }
}  // namespace utils