/**
 * @file Webserver.cpp
 * @author Dominic Möri (mdomin470@gmail.com)
 * @brief
 * @version 0.1
 * @date 2019-05-31
 *
 * @copyright Copyright (c) 2019
 *
 */
#include "Webserver.h"
#include "utils/String.h"
#include "utils/Utils.h"

#include <etl/memory.h>
#include <mongoose/mongoose.h>

namespace web {

    static constexpr const char HTTP_PORT[] = ":80";
    WebServer* webServer = nullptr;

    WebServer* webserver() {
        return webServer;
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
        if (webServer)
            webServer->stop();
        return ESP_OK;
    }

    esp_err_t WebServer::start() {
        mg_mgr_init(&m_mgr, this);
        m_socket = mg_bind(&m_mgr, HTTP_PORT, [](mg_connection* c, int ev, void* p) { static_cast<WebServer*>(c->mgr->user_data)->eventHandler(c, ev, p); });

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
            "WebServer", 4096, this, 1, &m_task);

        if (result != pdPASS) {
            ESP_LOGW(LOG_TAG, "Task creation failed (%d).", result);
            return ESP_FAIL;
        }

        webServer = this;

        return ESP_OK;
    }

    esp_err_t WebServer::stop() {
        if (!m_task)
            return ESP_FAIL;

        xTaskNotifyGive(m_task);
        webServer = nullptr;

        return ESP_OK;
    }

    void WebServer::eventHandler(mg_connection* con, int event, void* ptr) {
        if (event == MG_EV_HTTP_REQUEST) {
            http_message* msg = static_cast<http_message*>(ptr);

            char addr[32];
            mg_sock_addr_to_str(&con->sa, addr, sizeof(addr), MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);

            ESP_LOGI(LOG_TAG, "Received http request from %s.", addr);
            mg_send_head(con, 200, -1, "Content-Type: text/plain");
            mg_printf_http_chunk(con, "test");
            mg_send_http_chunk(con, "", 0);
        }
    }

}  // namespace web
