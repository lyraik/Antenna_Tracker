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
#include "Filesystem/Filesystem.h"
#include "utils/String.h"
#include "utils/Utils.h"

#include <etl/memory.h>
#include <mongoose/mongoose.h>

namespace web {

    namespace internal {

        static constexpr const char HTTP_PORT[] = ":80";
        static constexpr const char WEBPAGE_URI[] = "";
        static constexpr const char REST_URI[] = "/api";

        static constexpr const char INDEX_FILE_PATH[] = "/index.html";

        WebServer* webServer = nullptr;

        bool matchUri(utils::StringView& uri, utils::StringView match) {
            if (!uri.str || !uri.length || match.length > uri.length)
                return false;

            size_t length = std::min(uri.length, match.length);

            for (size_t i = 0; i < length; ++i, ++uri.str, ++match.str) {
                if (*uri.str != *match.str)
                    return false;
            }

            if (match.length == uri.length) {
                uri.length = 1;
                --uri.str;
                return true;
            } else if (*uri.str == '/') {
                uri.length -= match.length;
                return true;
            }
            return false;
        }

        const MimeType MIME_TYPES[] = {{utils::StringView{"txt"}, utils::StringView{"text/plain"}},
                                       {utils::StringView{"html\0htm"}, utils::StringView{"text/html"}},
                                       {utils::StringView{"css"}, utils::StringView{"text/css"}},
                                       {utils::StringView{"js\0mjs"}, utils::StringView{"text/javascript"}},
                                       {utils::StringView{"json"}, utils::StringView{"application/json"}}};

        struct FileRequest {
            fs::File file;

            static bool sendFile(mg_connection* con, fs::File& file) {
                char buf[FILE_SERVE_BUFFER_SIZE];
                size_t space = sizeof(buf) + FILE_SERVE_MIN_SEND_BUF_SIZE;
                size_t read;

                while (!file.isEOF()) {
                    read = file.read(buf, std::min((size_t)sizeof(buf), space - FILE_SERVE_MIN_SEND_BUF_SIZE));
                    if (!read) {
                        ESP_LOGD(LOG_TAG, "File read returned 0");
                        break;
                    }

                    mg_send_http_chunk(con, buf, read);
                    space = con->send_mbuf.size - con->send_mbuf.len;

                    // if the send buffer is full, we have to wait until the date is sent
                    if (space <= FILE_SERVE_MIN_SEND_BUF_SIZE) {
                        return false;
                    }
                }
                mg_send_http_chunk(con, nullptr, 0);
                con->flags |= MG_F_SEND_AND_CLOSE;

                return true;
            }

            static void handleRequest(mg_connection* con, http_message* msg, utils::StringView uri) {
                if (uri.length > MAX_URI_LENGTH) {
                    return mg_http_send_error(con, 500, "URL too long");
                }

                etl::unique_ptr<char[]> uriStr;
                // if the uri is just the root directory, point it to the index file instead
                if (uri == "/")
                    uri = internal::INDEX_FILE_PATH;
                // allocate new buffer to get a null-terminated string (uri is not null-terminated)
                else {
                    uriStr = etl::unique_ptr<char[]>{new char[uri.length + 1]};
                    memcpy(uriStr.get(), uri.str, uri.length);
                    uriStr[uri.length] = '\0';
                    uri = utils::StringView{uriStr.get(), uri.length};
                }

                // try to open file
                fs::File f;
                if (!f.tryOpen(uri.str, fs::Mode::READ)) {
                    return mg_http_send_error(con, 404, NULL);
                }

                // try to resolve the mime-type
                const MimeType* mimeType = &internal::MIME_TYPES[MimeType::TEXT_PLAIN];
                auto fileExt = uri.splitAtLastOccurrence('.');
                if (fileExt.length) {
                    mimeType = MimeType::getFromExt(fileExt);
                    // if mimetype not found, fall back to text/plain
                    if (!mimeType)
                        mimeType = &MimeType::getFromCode(MimeType::TEXT_PLAIN);
                }

                // send file
                mg_send_response_line(con, 200, NULL);
                mg_printf(con,
                          "Content-Type: %.*s\r\n"
                          "Connection: %s\r\n"
                          "Transfer-Encoding: chunked\r\n"
                          "\r\n",
                          (int)mimeType->mimeType.length, mimeType->mimeType.str, "close");

                if (!sendFile(con, f)) {
                    etl::unique_ptr<FileRequest> fr{new FileRequest()};
                    // if we failed to allocate, abort connection
                    if (!fr.get()) {
                        con->flags |= MG_F_CLOSE_IMMEDIATELY;
                        return;
                    }

                    // move file to FileRequest instance
                    fr->file.reset(f.release());
                    // configure connection
                    con->flags |= WebServer::REQ_FILE;
                    con->user_data = fr.release();
                }
            }

            inline static bool handleSend(mg_connection* con, int event, void* ptr) {
                if (con->flags & WebServer::REQ_FILE) {
                    FileRequest* req = static_cast<FileRequest*>(con->user_data);

                    if (sendFile(con, req->file)) {
                        // if file has been completely sent, clean up
                        delete req;
                        con->flags &= ~WebServer::REQ_FILE;
                    }
                    return true;
                }
                return false;
            }
        };
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
                ESP_LOGI(LOG_TAG, "Received http request from %s.", addr);

                utils::StringView uri{msg->uri.p, msg->uri.len};
                if (internal::matchUri(uri, internal::REST_URI)) {
                    return handleRestRequest(con, msg, uri);
                } else if (internal::matchUri(uri = utils::StringView{msg->uri.p, msg->uri.len}, internal::WEBPAGE_URI)) {
                    return internal::FileRequest::handleRequest(con, msg, uri);
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
        mg_http_send_error(con, 501, NULL);
    }

}  // namespace web
