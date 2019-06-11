/**
 * @file Webserver.h
 * @author Dominik Gschwind
 * @brief
 * @version 0.1
 * @date 2019/06/10
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once
#ifndef _Webserver_h_
#define _Webserver_h_

#include "utils/String.h"
#include <freertos/FreeRTOS.h>
#include <mongoose/mongoose.h>
#include "esp_err.h"

namespace web {
    static constexpr const char LOG_TAG[] = "web";

    esp_err_t init();
    esp_err_t deinit();

    class WebServer {
       private:
        mg_mgr m_mgr;
        mg_connection* m_socket;
        TaskHandle_t m_task;

       public:
        WebServer() : m_mgr(), m_socket(nullptr), m_task(nullptr) {}

        esp_err_t start();
        esp_err_t stop();

       private:
        void eventHandler(mg_connection* con, int event, void* ptr);

        void handleFileRequest(mg_connection* con, http_message* msg, const utils::StringView& uri);
        void handleRestRequest(mg_connection* con, http_message* msg, const utils::StringView& uri);
    };

    WebServer* webserver();

}  // namespace web

#endif  //_Webserver_h_
