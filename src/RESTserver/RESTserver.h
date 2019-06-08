#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "http_parser.h"
#include "http_server.h"
#include <iostream>

namespace restserver {

    esp_err_t Init();

    esp_err_t Deinit();

    extern httpd_config_t config;

    extern httpd_handle_t server;

    esp_err_t getHandler(httpd_req_t *request);

    extern httpd_uri_t uriGet;

} // namespace restserver