/**
 * @file Webserver.cpp
 *
 * @author Dominik Gschwind
 * @author Dominic Möri (mdomin470@gmail.com)
 * @brief
 * @version 0.1
 * @date 2019-05-31
 *
 * @copyright Copyright (c) 2019
 *
 */
#include "Webserver.h"
#include "Filesystem/Filesystem.h"
#include "utils/String.h"
#include "utils/Utils.h"

#include <etl/memory.h>
#include <mongoose/mongoose.h>

#include "FileRequest.h"
#include "RestRequest.h"

namespace web {

    namespace internal {

        RestInterface rest{};

        static constexpr const char HTTP_PORT[] = ":80";
        static constexpr const char WEBPAGE_URI[] = "";
        static constexpr const char REST_URI[] = "/api";

        WebServer* webServer = nullptr;

        const MimeType MIME_TYPES[] = {{utils::StringView{"txt"}, utils::StringView{"text/plain"}},
                                       {utils::StringView{"html\0htm"}, utils::StringView{"text/html"}},
                                       {utils::StringView{"css"}, utils::StringView{"text/css"}},
                                       {utils::StringView{"js\0mjs"}, utils::StringView{"text/javascript"}},
                                       {utils::StringView{"json"}, utils::StringView{"application/json"}}};

    }  // namespace internal

    const MimeType* MimeType::getFromExt(const utils::StringView& fileExt) {
        for (size_t i = 0; i < COUNT; ++i) {
            const MimeType& type = internal::MIME_TYPES[i];

            if (type.fileExt.length < fileExt.length || !fileExt.length)
                continue;
            const char* ext = type.fileExt.str;
            const char* comp = fileExt.str;

            for (size_t extIndex = 0; extIndex < type.fileExt.length; ++extIndex) {
                size_t u = 0;
                bool broke = false;
                for (; u < fileExt.length; ++u) {
                    if (ext[u] != comp[u] || !(ext[extIndex])) {
                        broke = true;
                        break;
                    }
                }

                if (!broke && ext[u] == '\0')
                    return &type;

                ext += u;
                extIndex += u;
                for (; *ext; ++ext, ++extIndex)
                    ;
            }
        }
        return nullptr;
    }
    const MimeType* MimeType::getFromMime(const utils::StringView& mime) {
        for (const auto& e : internal::MIME_TYPES) {
            if (e.mimeType == mime)
                return &e;
        }
        return nullptr;
    }
    const MimeType& MimeType::getFromCode(Code code) {
        return internal::MIME_TYPES[code];
    }

    WebServer* webserver() {
        return internal::webServer;
    }

    esp_err_t init() {
        etl::unique_ptr<WebServer> ptr{new WebServer()};
        if (!ptr.get())
            return ESP_ERR_NO_MEM;

        ASSERT_RET_CHECK(ptr->start(), LOG_TAG);
        ptr.release();
        return ESP_OK;
    }

    esp_err_t deinit() {
        if (internal::webServer)
            internal::webServer->stop();
        return ESP_OK;
    }

    esp_err_t WebServer::start() {
        mg_mgr_init(&m_mgr, this);
        m_socket = mg_bind(&m_mgr, internal::HTTP_PORT, [](mg_connection* c, int ev, void* p) { static_cast<WebServer*>(c->mgr->user_data)->eventHandler(c, ev, p); });

        if (!m_socket) {
            ESP_LOGW(LOG_TAG, "Could not create socket.");
            return ESP_FAIL;
        }

        mg_set_protocol_http_websocket(m_socket);

        // create webserver task
        auto result = xTaskCreate(
            [](void* param) {
                WebServer* server = static_cast<WebServer*>(param);

                // poll for messages until task is notified
                while (!ulTaskNotifyTake(pdFALSE, 0)) {
                    mg_mgr_poll(&server->m_mgr, 500);
                }

                // clean up web server if task is notified
                mg_mgr_free(&server->m_mgr);
                delete server;

                vTaskDelete(server->m_task);
            },
            "WebServer", STACK_SIZE, this, 1, &m_task);

        if (result != pdPASS) {
            ESP_LOGW(LOG_TAG, "Task creation failed (%d).", result);
            return ESP_FAIL;
        }

        internal::webServer = this;

        return ESP_OK;
    }

    esp_err_t WebServer::stop() {
        if (!m_task)
            return ESP_FAIL;

        xTaskNotifyGive(m_task);
        internal::webServer = nullptr;

        return ESP_OK;
    }

    void WebServer::eventHandler(mg_connection* con, int event, void* ptr) {
        switch (event) {
            case MG_EV_HTTP_REQUEST: {
                http_message* msg = static_cast<http_message*>(ptr);

                char addr[32];
                mg_sock_addr_to_str(&con->sa, addr, sizeof(addr), MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);
                ESP_LOGI(LOG_TAG, "Received http request '%.*s' from %s.", (int)msg->uri.len, msg->uri.p, addr);

                utils::StringView uri{msg->uri.p, msg->uri.len};
                utils::StringView result;
                if (uri.startsWith(internal::REST_URI, &result)) {
                    return handleRestRequest(con, msg, uri);
                } else if (uri.startsWith(internal::WEBPAGE_URI, &result)) {
                    return internal::FileRequest::handleRequest(con, msg, result);
                }

                mg_http_send_error(con, 404, nullptr);
                break;
            }
            case MG_EV_SEND: {
                if (internal::FileRequest::handleSend(con, event, ptr))
                    break;
                break;
            }
        }
    }

    void WebServer::handleRestRequest(mg_connection* con, http_message* msg, const utils::StringView& uri) {
        utils::StringView method{msg->method.p, msg->method.len};

        if (method == "GET") {
            size_t index = 0;
            bool arrayNode = false;
            internal::Node* node = internal::rest.findNode(uri, index, arrayNode, '/');

            if (!node) {
                return mg_http_send_error(con, 500, "Node not found");
            }

            cJSON* json = internal::rest.createJSON(node, nullptr, arrayNode, index);
            if (!node) {
                return mg_http_send_error(con, 500, "JSON building failed");
            }

            utils::String jsonStr = cJSON_PrintUnformatted(json);
            if (jsonStr.empty()) {
                return mg_http_send_error(con, 500, "JSON stringify failed");
            }

            mg_send_response_line(con, 200, nullptr);
            mg_printf(con,
                      "API-Version: %s\r\n"
                      "Content-Type: %s\r\n"
                      "Content-Length: %u\r\n\r\n",
                      internal::RestInterface::VERSION.str, MimeType::getFromCode(MimeType::APP_JSON).mimeType.str, jsonStr.length);
            mg_send(con, jsonStr.str, jsonStr.length);
            return;
        } else if (method == "POST") {
        }

        mg_http_send_error(con, 501, nullptr);
    }

}  // namespace web
