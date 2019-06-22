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

#include <atomic>
#include "Filesystem/Filesystem.h"
#include "Webserver.h"

#pragma once
#ifndef _FileRequest_h_
#define _FileRequest_h_

namespace web {
    namespace internal {

        static std::atomic<size_t> numRequests{0};

        struct Releaser {
            bool should;
            Releaser(bool value = false) : should(value) {}
            Releaser(const Releaser&) = delete;
            Releaser(Releaser&& r) {
                should = r.should;
                r.should = false;
            }
            ~Releaser() {
                if (should)
                    --numRequests;
            }
            Releaser transfer() {
                bool value = should;
                should = false;
                return Releaser{value};
            }
            Releaser& operator=(const Releaser&) = delete;
            Releaser& operator=(Releaser&& r){
                should = r.should;
                r.should = false;
                return *this;
            }
        };

        struct FileRequest {
            fs::File file;
            utils::String filePath;
            bool postponed = false;
            Releaser releaser{true};

            static bool sendFile(mg_connection* con, etl::unique_ptr<FileRequest>& req, utils::String path = {}) {
                fs::File file;
                Releaser releaser{true};

                if (!req || req->postponed) {
                    if (req) {
                        path = req->filePath.view();
                        releaser = req->releaser.transfer();
                        // reset timer
                        mg_set_timer(con, 0);
                    } else {
                        // if the uri is just the root directory, point it to the index file instead
                        if (path == "/")
                            path = INDEX_FILE_PATH;
                        // allocate new buffer to get a null-terminated string (uri is not null-terminated)
                        else {
                            path.copy();
                        }
                    }

                    // try to open file
                    if (!file.open(path.str(), fs::Mode::READ)) {
                        mg_http_send_error(con, 404, fs::File::getError().str());
                        return true;
                    }

                    // try to resolve the mime-type
                    const MimeType* mimeType = &MimeType::getFromCode(MimeType::TEXT_PLAIN);
                    auto fileExt = path.splitAtLastOccurrence('.');
                    if (fileExt.len()) {
                        mimeType = MimeType::getFromExt(fileExt);
                        // if mimetype not found, fall back to text/plain
                        if (!mimeType)
                            mimeType = &MimeType::getFromCode(MimeType::TEXT_PLAIN);
                    }

                    // send header
                    mg_send_response_line(con, 200, NULL);
                    mg_printf(con,
                              "Content-Type: %.*s\r\n"
                              "Connection: %s\r\n"
                              "Transfer-Encoding: chunked\r\n"
                              "\r\n",
                              (int)mimeType->mimeType.len(), mimeType->mimeType.str(), "close");
                } else {
                    file.reset(req->file.release());
                    releaser = req->releaser.transfer();
                }

                ASSERT(file.valid(), LOG_TAG);

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
                    if (space <= FILE_SERVE_MIN_SEND_BUF_SIZE && con->send_mbuf.size >= FILE_SERVE_BUFFER_SIZE) {
                        // create FileRequest object if it doesnt exist and populate it
                        if (!req) {
                            req = etl::unique_ptr<FileRequest>{new FileRequest()};
                            // if we failed to allocate, abort connection
                            if (!req.get()) {
                                con->flags |= MG_F_CLOSE_IMMEDIATELY;
                                con->flags &= ~WebServer::REQ_FILE;
                                return true;
                            }
                        } else {
                            req->filePath = utils::StringView{};
                            req->postponed = false;
                        }
                        req->file.reset(file.release());

                        // setup connection
                        con->flags |= WebServer::REQ_FILE;
                        req->releaser = releaser.transfer();
                        con->user_data = req.release();

                        return false;
                    }
                }
                mg_send_http_chunk(con, nullptr, 0);
                con->flags |= MG_F_SEND_AND_CLOSE;
                con->flags &= ~WebServer::REQ_FILE;
                con->user_data = nullptr;

                return true;
            }

            static void handleRequest(mg_connection* con, http_message* msg, utils::StringView uri) {
                if (uri.len() > MAX_URI_LENGTH) {
                    return mg_http_send_error(con, 500, "URL too long");
                }

                ++numRequests;
                if (numRequests > MAX_FILE_REQUESTS) {
                    etl::unique_ptr<FileRequest> fr{new FileRequest()};
                    // if we failed to allocate, abort connection
                    if (!fr.get()) {
                        con->flags |= MG_F_CLOSE_IMMEDIATELY;
                        --numRequests;
                        return;
                    }
                    fr->postponed = true;
                    fr->filePath = uri;
                    fr->filePath.copy();

                    // configure connection
                    con->flags |= WebServer::REQ_FILE;
                    con->user_data = fr.release();

                    // set timer
                    mg_set_timer(con, mg_time() + FILE_REQUEST_POSTPONE_TIME);
                    return;
                }

                etl::unique_ptr<FileRequest> req;
                sendFile(con, req, uri);
            }

            inline static bool handleTimer(mg_connection* con, int event, void* ptr) {
                if ((con->flags & WebServer::REQ_FILE) && con->user_data && static_cast<FileRequest*>(con->user_data)->postponed) {
                    FileRequest* ptr = static_cast<FileRequest*>(con->user_data);

                    if (numRequests > MAX_FILE_REQUESTS) {
                        mg_set_timer(con, mg_time() + FILE_REQUEST_POSTPONE_TIME);
                        return true;
                    }

                    etl::unique_ptr<FileRequest> req{ptr};
                    sendFile(con, req);

                    return true;
                }
                return false;
            }

            inline static bool handleSend(mg_connection* con, int event, void* ptr) {
                if (con->flags & WebServer::REQ_FILE) {
                    FileRequest* ptr = static_cast<FileRequest*>(con->user_data);

                    etl::unique_ptr<FileRequest> req{ptr};
                    sendFile(con, req);

                    return true;
                }
                return false;
            }
        };
    }  // namespace internal
}  // namespace web

#endif  //_FileRequest_h_