/**
 * @file Utils.h
 * @author Dominik Gschwind
 * @brief
 * @version 0.1
 * @date 2019/06/10
 *
 * @copyright Copyright (c) 2019
 *
 */

#include <esp_err.h>
#include <esp_log.h>

#define STRINGIFY(x) _STRINGIFY(x)
#define _STRINGIFY(x) #x

#define RETURN_ON_ERROR(x, tag) \
    do { \
        auto errCode = (x); \
        if (errCode != ESP_OK) { \
            ESP_LOGD(tag, "<%s:%d> [%s] %s failed.", __FILE__, __LINE__, __ASSERT_FUNC, STRINGIFY(x)); \
            return errCode; \
        } \
    } while (0)

#define ASSERT(x, tag) \
    do { \
        if (!(x)) { \
            ESP_LOGD(tag, "<%s:%d> [%s] Assert %s.", __FILE__, __LINE__, __ASSERT_FUNC, STRINGIFY(x)); \
            assert(false);\
        } \
    } while (0)
