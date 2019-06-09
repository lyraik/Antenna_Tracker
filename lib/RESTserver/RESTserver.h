#include <iostream>
#include "esp_http_server.h"
#include "http_parser.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "esp_event_base.h"
#include "esp_event_loop.h"
#include "esp_event_legacy.h"

namespace restserver
{

 extern const uint8_t files_home_html_start[] asm("_binary_src_files_home_html_start");
 extern const uint8_t files_home_html_end[] asm("_binary_src_files_home_html_end");


extern esp_err_t eventhandler(void *ctx, system_event_t *event);
                                
esp_err_t Init();

esp_err_t Deinit();


extern httpd_config_t config;

extern httpd_handle_t server;

esp_err_t getHandler(httpd_req_t *request);

extern httpd_uri_t uriGet;

}