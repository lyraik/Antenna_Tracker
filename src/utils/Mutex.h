/**
 * @file Mutex.h
 * @author Dominik Gschwind
 * @brief
 * @version 0.1
 * @date 2019/06/22
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once
#ifndef _Mutex_h_
#define _Mutex_h_

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

namespace utils {

    struct Lock {
        SemaphoreHandle_t handle;

        Lock(SemaphoreHandle_t sHandle) : handle(sHandle) {
            xSemaphoreTake(handle, portMAX_DELAY);
        }
        Lock(Lock&& mtx) {
            handle = mtx.handle;
            mtx.handle = NULL;
        }
        Lock(const Lock&) = delete;

        ~Lock() {
            xSemaphoreGive(handle);
        }

        Lock& operator=(Lock&& lock) {
            handle = lock.handle;
            lock.handle = NULL;
            return *this;
        }
        Lock& operator=(const Lock&) = delete;
    };

    namespace mutex {

        enum Id { };

        template <Id ID>
        static SemaphoreHandle_t get() {
            static StaticSemaphore_t buffer;
            static SemaphoreHandle_t result = xSemaphoreCreateMutexStatic(&buffer);
            return result;
        }
    }  // namespace mutex

}  // namespace utils

#endif  //_Mutex_h_