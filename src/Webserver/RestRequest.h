/**
 * @file RestRequest.h
 * @author Dominik Gschwind
 * @brief
 * @version 0.1
 * @date 2019/06/13
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once
#ifndef _RestRequest_h_
#define _RestRequest_h_

#include <cJSON/cJSON.h>
#include <etl/algorithm.h>
#include <math.h>
#include <tuple>
#include <utility>
#include "Utils/CompileTime.h"
#include "Utils/Json.h"
#include "Utils/Utils.h"

#include "NodeTree.h"

namespace web {
    namespace internal {

        namespace rest {

            static const Node* findNode(utils::StringView uri, size_t& arrayIndex, bool& isArrayNode, char delimitor = '/') {
                const Node* currNode = &rest::api;
                arrayIndex = 0;
                isArrayNode = false;
                // ESP_LOGI(LOG_TAG, "Begin find node '%.*s'", (int)uri.length, uri.str);

                auto range = uri.split(delimitor);
                auto iter = range.begin();
                auto end = range.end();
                utils::StringView str;
                while (iter != end) {
                        ESP_LOGI(LOG_TAG, "A");
                    str = *iter;
                        ESP_LOGI(LOG_TAG, "B");
                    ESP_LOGI(LOG_TAG, "At node '%s'", currNode->name.str);
                    // ESP_LOGI(LOG_TAG, "Searching node with name '%.*s'", (int)str.length, str.str);
                    if (str.empty()) {
                        ++iter;
                        ESP_LOGI(LOG_TAG, "Skipping empty string.");
                        continue;
                    }
                    if (currNode->name == str) {
                        ESP_LOGI(LOG_TAG, "1");
                        ESP_LOGI(LOG_TAG, "Found node '%.*s'", (int)str.length, str.str);
                        if (currNode->children && static_cast<const NodeContainerBase*>(currNode)->isArray) {
                        ESP_LOGI(LOG_TAG, "2");
                            ++iter;
                            if (iter == end) {
                                return currNode;
                            }
                            str = *iter;

                            // get index from uri
                            utils::String indexStr = utils::String::create(str.length);
                            if (indexStr.empty())
                                return nullptr;

                            memcpy(indexStr.str, str.str, str.length);
                            char* strEnd;
                            auto index = strtol(indexStr.str, &strEnd, 10);

                            isArrayNode = true;
                            arrayIndex = (size_t)index;
                            return currNode;
                        } else if (currNode->children) {
                        ESP_LOGI(LOG_TAG, "3");
                            ++iter;
                        ESP_LOGI(LOG_TAG, "2.1");
                            if (iter == end)
                                return currNode;
                        ESP_LOGI(LOG_TAG, "2.2");
                            currNode = currNode->children;
                        ESP_LOGI(LOG_TAG, "2.3");
                        } else {
                        ESP_LOGI(LOG_TAG, "4");
                            ++iter;
                            // ESP_LOGI(LOG_TAG, "No more children '%.*s' iter_at_end=%s", (int)str.length, str.str, iter == end ? "true" : "false");
                            if (iter == end)
                                return currNode;
                            return nullptr;
                        }
                    } else {
                        ESP_LOGI(LOG_TAG, "5");
                        // ESP_LOGI(LOG_TAG, "Node name mismatch dir='%.*s' node='%s'", (int)str.length, str.str, currNode->name.str);
                        if (currNode->next) {
                            ESP_LOGI(LOG_TAG, "Getting next sibling");
                            currNode = currNode->next;
                        } else {
                            ESP_LOGI(LOG_TAG, "No more siblings.");
                            break;
                        }
                    }
                }
                return nullptr;
            }

            static cJSON* createJSON(const Node* node, cJSON* parent, bool isArrayNode = false, size_t arrayIndex = 0) {
                ESP_LOGI(LOG_TAG, "createJSON");
                if (!node)
                    return nullptr;
                if (!parent)
                    parent = cJSON_CreateObject();

                ESP_LOGI(LOG_TAG, "Node '%s'", node->name.str);

                if (node->children) {
                    ESP_LOGI(LOG_TAG, "Node has children %u", static_cast<const NodeContainerBase*>(node)->numChilds);

                    size_t count = 0;
                    node = node->children;

                    ESP_LOGI(LOG_TAG, "Child node %p'", node);
                    for (; node; node = node->next) {
                        ASSERT(node->name.str, rest::LOG_TAG);
                        ESP_LOGI(LOG_TAG, "Child node '%s'", node->name.str);

                        if (node->children && !static_cast<const NodeContainerBase*>(node)->isArray) {
                            ESP_LOGI(LOG_TAG, "Node is not array");
                            createJSON(node, cJSON_AddObjectToObject(parent, node->name.str));

                        } else if (node->children && static_cast<const NodeContainerBase*>(node)->isArray) {
                            ESP_LOGI(LOG_TAG, "Node is array, count elements %u", count);
                            for (uint8_t i = 0; i < count; ++i) {
                                auto numberStr = utils::String::fromNumber(i);
                                cJSON* item = cJSON_AddObjectToObject(parent, numberStr.str);

                                const Node* element = node->children;
                                for (; element; element = element->next) {
                                    createJSON(element, cJSON_AddObjectToObject(item, element->name.str), true, i);
                                }
                            }
                        } else if (!node->children) {
                            ESP_LOGI(LOG_TAG, "Node has no children.");

                            auto item = createJSON(node, cJSON_AddObjectToObject(parent, node->name.str));
                            if (node->name == "count" && cJSON_IsObject(item)) {
                                ESP_LOGI(LOG_TAG, "Found count node.");
                                if (count > 0) {
                                    ESP_LOGW(LOG_TAG, "Encountered multiple nodes named 'count'");
                                }
                                cJSON* value = cJSON_GetObjectItemCaseSensitive(item, "value");
                                if (value && cJSON_IsNumber(value)) {
                                    count = (size_t)value->valuedouble;
                                }
                            }

                        } else {
                            ESP_LOGW(LOG_TAG, "Found invalid node '%s'", node->name.str);
                        }

                        ESP_LOGI(LOG_TAG, "Going to next node %iX", (uintptr_t)node->next);
                        // if (child->next)
                        // ESP_LOGI(LOG_TAG, "--- %s", child->next->name.str);
                    }
                } else {
                    if (isArrayNode) {
                        ArrayLeafNode* leaf = (ArrayLeafNode*)node;

                        ESP_LOGI(LOG_TAG, "ArrayLeafNode, '%s'", leaf->name.str);
                        ASSERT(leaf->read, LOG_TAG);
                        leaf->read(leaf, arrayIndex, parent);
                    } else {
                        LeafNode* leaf = (LeafNode*)node;

                        // ESP_LOGI(LOG_TAG, "LeafNode '%s'", leaf->name.str);
                        ASSERT(leaf->read, LOG_TAG);
                        leaf->read(leaf, parent);
                    }

                    ESP_LOGI(LOG_TAG, "Node finished '%s'", node->name.str);
                }
                return parent;
            }

        }  // namespace rest
    }      // namespace internal
}  // namespace web

#endif  //_RestRequest_h_