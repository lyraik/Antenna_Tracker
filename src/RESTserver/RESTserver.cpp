/**
 * @file RESTserver.cpp
 * @author Dominic Möri (mdomin470@gmail.com)
 * @brief
 * @version 0.1
 * @date 2019-05-31
 *
 * @copyright Copyright (c) 2019
 *
 */
#include "RESTserver.h"

namespace restserver {

    esp_err_t getHandler(httpd_req_t *request) {
        const char resp[] = "responding";
        httpd_resp_send(request, resp, sizeof(resp));

        return ESP_OK;
    }
    esp_err_t Init() {
        /**
         * @brief
         *  Wifi Initialization
         */
        tcpip_adapter_init();

        esp_event_loop_create_default();

        wifi_init_config_t wifiInitCfg = WIFI_INIT_CONFIG_DEFAULT();

        if (esp_wifi_init(&wifiInitCfg) != ESP_OK) {
            ESP_LOGI("Error", "coudln't initialize wifi!");
            return ESP_FAIL;
        }

        wifi_sta_config_t stacfg = {"Antennatrack", "example"};
        wifi_config_t wifiCfg = {.sta = stacfg};

        esp_wifi_set_mode(WIFI_MODE_STA);

        esp_wifi_set_config(ESP_IF_WIFI_STA, &wifiCfg);

        esp_wifi_start();

        httpd_uri_t uriGet = {.uri = "/home", .method = HTTP_GET, .handler = getHandler, .user_ctx = NULL};

        httpd_config_t config = HTTPD_DEFAULT_CONFIG();

        httpd_handle_t server = NULL;

        if (!httpd_start(&server, &config) == ESP_OK) {
            httpd_register_uri_handler(server, &uriGet);

        } else {
            // Fehler herausschreiben
            return ESP_FAIL;
        }

        return ESP_OK;
    }

    esp_err_t Deinit() {
        httpd_stop(&server);

        return ESP_OK;
    }

} // namespace restserver
