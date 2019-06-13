/**
 * @file FileRequest.h
 * @author Dominik Gschwind
 * @brief
 * @version 0.1
 * @date 2019/06/13
 *
 * @copyright Copyright (c) 2019
 *
 */

#include <etl/algorithm.h>
#include <etl/memory.h>
#include <mongoose/mongoose.h>

#include "Filesystem/Filesystem.h"
#include "Webserver.h"

#pragma once
#ifndef _FileRequest_h_
#define _FileRequest_h_

namespace web {
    namespace internal {

        static constexpr const char INDEX_FILE_PATH[] = "/index.html";

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
                const MimeType* mimeType = &MimeType::getFromCode(MimeType::TEXT_PLAIN);
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
}  // namespace web

#endif  //_FileRequest_h_