/**
 * @file Wifi.h
 * @author Dominik Gschwind
 * @brief 
 * @version 0.1
 * @date 2019/06/10
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#pragma once
#ifndef _Wifi_h_
#define _Wifi_h_

#include "utils/String.h"

#include <esp_err.h>


namespace wifi{
    static constexpr const char LOG_TAG[] = "wifi";

    esp_err_t init(utils::StringView ssid, utils::StringView password);
    esp_err_t deinit();
}


#endif //_Wifi_h_