/**
 * @file Wifi.cpp
 * @author Dominik Gschwind
 * @brief
 * @version 0.1
 * @date 2019/06/10
 *
 * @copyright Copyright (c) 2019
 *
 */

#include <string.h>
#include <esp_event_loop.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_log.h>
#include <lwip/init.h>

#include "Wifi.h"
#include "utils/Utils.h"

namespace wifi {

    //esp_err_t handleEvent(void *ctx, system_event_t *event) {}

    esp_err_t init(utils::StringView ssid, utils::StringView password) {
        tcpip_adapter_init();
        RETURN_ON_ERROR(esp_event_loop_init(nullptr, nullptr), LOG_TAG);

        wifi_init_config_t initCfg = WIFI_INIT_CONFIG_DEFAULT();
        RETURN_ON_ERROR(esp_wifi_init(&initCfg), LOG_TAG);

        RETURN_ON_ERROR(esp_wifi_set_storage(WIFI_STORAGE_RAM), LOG_TAG);

        // configure accesspoint
        wifi_ap_config_t apcfg;

        ASSERT(ssid.length < sizeof(apcfg.ssid), LOG_TAG);
        memcpy(apcfg.ssid, ssid.str, ssid.length + 1);
        apcfg.ssid_len = ssid.length;
        ASSERT(password.length < sizeof(apcfg.password), LOG_TAG);
        memcpy(apcfg.password, password.str, password.length + 1);
        apcfg.channel = 6;
        apcfg.authmode = WIFI_AUTH_WPA2_PSK;
        apcfg.ssid_hidden = false;
        apcfg.max_connection = 3;
        apcfg.beacon_interval = 100;

        wifi_config_t wifiCfg = {.ap = apcfg};
        esp_wifi_set_mode(WIFI_MODE_AP);
        esp_wifi_set_config(ESP_IF_WIFI_AP, &wifiCfg);

        // start wifi ap
        RETURN_ON_ERROR(esp_wifi_start(), LOG_TAG);

        return ESP_OK;
    }
    esp_err_t deinit() {
        esp_wifi_stop();
        esp_wifi_deinit();
        return ESP_OK;
    }

} // namespace wifi