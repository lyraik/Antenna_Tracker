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
#include <RESTserver.h>
namespace restserver
{


esp_err_t eventhandler(void *ctx, system_event_t *event)
{
     if (event->event_id == SYSTEM_EVENT_AP_STACONNECTED) {
        system_event_ap_staconnected_t* event = (system_event_ap_staconnected_t*) ctx;
       // ESP_LOGI("wifi handler", "station "MACSTR" join, AID=%d",
         //        MAC2STR(event->mac), event->aid);
        ESP_LOGI("wifi handler", "System connect!");
    }
     else if (event->event_id == SYSTEM_EVENT_AP_STADISCONNECTED) {
        system_event_ap_stadisconnected_t* event = (system_event_ap_stadisconnected_t*) ctx;
        ESP_LOGI("wifi handler", "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
    return ESP_OK;
}

esp_err_t getHandler(httpd_req_t *request)
{

    size_t size = (const char)(files_home_html_end - files_home_html_start);


    httpd_resp_send(request,(const char * )(files_home_html_start),size);

    return ESP_OK;
}
esp_err_t Init()
{
    /**
     * @brief 
     *  Wifi Initialization 
     */
    tcpip_adapter_init();
    
    esp_event_loop_init(eventhandler,NULL);

    wifi_init_config_t wifiInitCfg = WIFI_INIT_CONFIG_DEFAULT();

    if(esp_wifi_init(&wifiInitCfg) != ESP_OK)
    {
        ESP_LOGI("WIFI Error", "coudln't initialize wifi!");
        return ESP_FAIL;
    }

    
    esp_wifi_set_storage(WIFI_STORAGE_RAM);

    wifi_ap_config_t apcfg = {
        "Antennatracker",
        "mypassword",
        strlen("Antennatracker"),
        6,
        WIFI_AUTH_WPA_PSK,
        0,
        10
    };

    wifi_config_t wifiCfg = { .ap = apcfg };
    
    esp_wifi_set_mode(WIFI_MODE_AP);

    esp_wifi_set_config(ESP_IF_WIFI_AP,&wifiCfg);


    if(esp_wifi_start() != ESP_OK)
    {
        ESP_LOGI("WIFI Error", "wifi coudln't start!");
        return ESP_FAIL;

    }

    httpd_uri_t uriGet = {
        .uri = "/home",
        .method = HTTP_GET,
        .handler = getHandler,
        .user_ctx = NULL
    };

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    httpd_handle_t server = NULL;

    if(httpd_start(&server,&config)== ESP_OK)
    {
        httpd_register_uri_handler(server, &uriGet);

    }
    else
    {
        ESP_LOGI("httpd uri handler", "couldn't register uri handler!");
        //Fehler herausschreiben
        return ESP_FAIL;
    }

    return ESP_OK;
}

 esp_err_t Deinit()
{
    httpd_stop(&server);

    return ESP_OK;
}

}

