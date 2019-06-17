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

#include <freertos/FreeRTOS.h>
#include <mongoose/mongoose.h>
#include "esp_err.h"
#include "utils/String.h"

namespace web {
    static constexpr const char LOG_TAG[] = "web";

    static constexpr size_t STACK_SIZE = 9000;
    static constexpr size_t MAX_URI_LENGTH = 1024;
    static constexpr size_t FILE_SERVE_BUFFER_SIZE = 512;
    static constexpr size_t FILE_SERVE_MIN_SEND_BUF_SIZE = 50;


    esp_err_t init();
    esp_err_t deinit();

    struct MimeType {
        enum Code : uint8_t { TEXT_PLAIN, TEXT_HTML, TEXT_CSS, TEXT_JS, APP_JSON, COUNT };

        utils::StringView fileExt;
        utils::StringView mimeType;

        static const MimeType* getFromExt(const utils::StringView& fileExt);
        static const MimeType* getFromMime(const utils::StringView& mime);
        static const MimeType& getFromCode(Code code);
    };

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

        void handleRestRequest(mg_connection* con, http_message* msg, const utils::StringView& uri);

       public:
        enum RequestFlag { REQ_FILE = MG_F_USER_1 };
    };

    WebServer* webserver();

}  // namespace web

#endif  //_Webserver_h_
