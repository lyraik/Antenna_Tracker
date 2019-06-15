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

#pragma once
#ifndef _Utils_h_
#define _Utils_h_

#include <esp_err.h>
#include <esp_log.h>

#define STRINGIFY(x) _STRINGIFY(x)
#define _STRINGIFY(x) #x

#define ASSERT_RET_CHECK(check, tag) \
    do { \
        auto errCode = (check);\
        if (errCode) { \
            utils::logWarn(tag, _STRINGIFY(x) " != ERR_OK", errCode, __FILE__, __LINE__); \
            return errCode; \
        } \
    } while (0)

#define ASSERT_RET(check, result, tag) \
    do { \
        if (!(check)) { \
            utils::logWarn(tag, _STRINGIFY(x), -1, __FILE__, __LINE__); \
            return result; \
        } \
    } while (0)

#define ASSERT(x, tag) \
    do { \
        if (!(x)) { \
            utils::logWarn(tag, _STRINGIFY(x), -1, __FILE__, __LINE__); \
            abort(); \
        } \
    } while (0)

namespace utils {
    static constexpr const char LOG_TAG[] = "utils";

    void logWarn(const char* tag, const char* check, int32_t errCode, const char* file, uint32_t line);
}

#endif //_Utils_h_