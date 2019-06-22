/**
 * @file Rest.cpp
 * @author Dominik Gschwind
 * @brief
 * @version 0.1
 * @date 2019/06/16
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "Rest.h"
#include <Bluetooth/Bluetooth.h>
#include <esp_log.h>
#include <etl/algorithm.h>
#include <etl/memory.h>
#include <esp_wifi.h>

namespace web {
    namespace rest {

        void handleRead(LeafNode* node, cJSON* parent) {
            if (!node)
                return;
            switch (node->nodeId) {
                case LN_DIR_REF: {
                    createNumber(0.0, false, TYPE_FUL_DEG, parent);
                    return;
                }
                case LN_DIR_VERT: {
                    createNumber(0.0, false, TYPE_FUL_DEG, parent);
                    return;
                }
                case LN_DIR_HORIZ: {
                    createNumber(0.0, false, TYPE_FUL_DEG, parent);
                    return;
                }
                case LN_POS_T_LONG: {
                    createNumber(0.0, false, TYPE_DECIMAL, parent);
                    return;
                }
                case LN_POS_T_LAT: {
                    createNumber(0.0, false, TYPE_DECIMAL, parent);
                    return;
                }
                case LN_POS_T_ALT: {
                    createNumber(0.0, false, TYPE_DECIMAL, parent);
                    return;
                }
                case LN_POS_D_LONG: {
                    createNumber(0.0, false, TYPE_DECIMAL, parent);
                    return;
                }
                case LN_POS_D_LAT: {
                    createNumber(0.0, false, TYPE_DECIMAL, parent);
                    return;
                }
                case LN_POS_D_ALT: {
                    createNumber(0.0, false, TYPE_DECIMAL, parent);
                    return;
                }
                case LN_BT_LIST_COUNT: {
                    createNumber(bt::deviceUids.size(), true, TYPE_SIZE, parent);
                    return;
                }
                case LN_BT_SET: {
                    if (bt::remoteDeviceUid.empty())
                        return createString("", node->readonly, parent);
                    return createString(bt::remoteDeviceUid.str(), node->readonly, parent);
                }
                case LN_WIFI_CONS_COUNT: {
                    break;
                }
                case LN_WIFI_SSID: {
                    break;
                }
                case LN_WIFI_PASSWD: {
                    break;
                }
                case LN_BAT_COUNT: {
                    break;
                }
                case LN_SYS_DBG_TOTMEM: {
                    break;
                }
                case LN_SYS_DBG_USEMEM: {
                    break;
                }
                case LN_SYS_DBG_PROCS: {
                    break;
                }
                case LN_SYS_VER: {
                    break;
                }
                case LN_SYS_RUN: {
                    break;
                }
                case LN_SYS_LOG: {
                    break;
                }
                case LN_SET_STEP_CPS: {
                    break;
                }
                case LN_SET_STEP_VEL: {
                    break;
                }
                case LN_SET_STEP_ACC: {
                    break;
                }
                case LN_SET_SERV_VEL: {
                    break;
                }
                case LN_SET_SERV_ACC: {
                    break;
                }
                default:
                    break;
            }
            createNumber(0.0, true, TYPE_ANGLE_ACCEL, parent);
        }

        void handleWrite(LeafNode* node, cJSON* json) {
            if (!node)
                return;
            switch (node->nodeId) {
                case LN_DIR_REF: {
                    break;
                }
                case LN_DIR_VERT: {
                    break;
                }
                case LN_DIR_HORIZ: {
                    break;
                }
                case LN_POS_T_LONG: {
                    break;
                }
                case LN_POS_T_LAT: {
                    break;
                }
                case LN_POS_T_ALT: {
                    break;
                }
                case LN_POS_D_LONG: {
                    break;
                }
                case LN_POS_D_LAT: {
                    break;
                }
                case LN_POS_D_ALT: {
                    break;
                }
                case LN_BT_LIST_COUNT: {
                    break;
                }
                case LN_BT_SET: {
                    break;
                }
                case LN_WIFI_CONS_COUNT: {
                    wifi_sta_list_t list;
                    if(esp_wifi_ap_get_sta_list(&list) != ESP_OK)
                        return createNumber(0, node->readonly, TYPE_SIZE, json);
                    return createNumber((double)list.num, node->readonly, TYPE_SIZE, json);
                }
                case LN_WIFI_SSID: {
                    break;
                }
                case LN_WIFI_PASSWD: {
                    break;
                }
                case LN_BAT_COUNT: {
                    break;
                }
                case LN_SYS_DBG_TOTMEM: {
                    break;
                }
                case LN_SYS_DBG_USEMEM: {
                    break;
                }
                case LN_SYS_DBG_PROCS: {
                    break;
                }
                case LN_SYS_VER: {
                    break;
                }
                case LN_SYS_RUN: {
                    break;
                }
                case LN_SYS_LOG: {
                    break;
                }
                case LN_SET_STEP_CPS: {
                    break;
                }
                case LN_SET_STEP_VEL: {
                    break;
                }
                case LN_SET_STEP_ACC: {
                    break;
                }
                case LN_SET_SERV_VEL: {
                    break;
                }
                case LN_SET_SERV_ACC: {
                    break;
                }
            }
        }

        void handleReadArray(ArrayLeafNode* node, size_t index, cJSON* parent) {
            switch (node->nodeId) {
                case AN_BT_LIST_NAME: {
                    if (index >= bt::devices.size() || bt::devices[index].empty()) {
                        return createString("not found", node->readonly, parent);
                    }
                    createString(bt::devices[index].str(), node->readonly, parent);
                    return;
                }
                case AN_BT_LIST_UID: {
                    if (index >= bt::deviceUids.size() || bt::deviceUids[index].empty()) {
                        return createString("not found", node->readonly, parent);
                    }
                    createString(bt::deviceUids[index].str(), node->readonly, parent);
                    return;
                }
                case AN_WIFI_CONS_ADDR: {
                    wifi_sta_list_t list;
                    if(esp_wifi_ap_get_sta_list(&list) != ESP_OK || index >= list.num)
                        return;
                    auto& device = list.sta[index];
                    auto macStr = utils::String::makeHex(device.mac, sizeof(device.mac));
                    
                    return createString(macStr.str(), node->readonly, parent);
                }
                case AN_BAT_VOLT: {
                    break;
                }
                case AN_BAT_CURR: {
                    break;
                }
                case AN_BAT_CONS: {
                    break;
                }
            }
            createNumber(0.0, true, TYPE_ANGLE_ACCEL, parent);
        }
        void handleWriteArray(ArrayLeafNode* node, size_t index, cJSON* json) {
            switch (node->nodeId) {
                case AN_BT_LIST_NAME: {
                    break;
                }
                case AN_BT_LIST_UID: {
                    break;
                }
                case AN_WIFI_CONS_ADDR: {
                    break;
                }
                case AN_BAT_VOLT: {
                    break;
                }
                case AN_BAT_CURR: {
                    break;
                }
                case AN_BAT_CONS: {
                    break;
                }
            }
        }

        const Node* findNode(utils::StringView uri, size_t& arrayIndex, bool& isArrayNode, char delimitor) {
            // ESP_LOGI(LOG_TAG, "Begin find node '%.*s'", (int)uri.length, uri.str);

            const Node* currNode = &API;
            const NodeContainerBase* parent = nullptr;
            arrayIndex = 0;
            isArrayNode = false;
            size_t index = 0;

            auto range = uri.split(delimitor);
            auto iter = range.begin();
            auto end = range.end();
            utils::StringView str;

            while (iter != end && currNode) {
                str = *iter;

                // ESP_LOGI(rest::LOG_TAG, "At node '%s'", currNode->name.str);
                // ESP_LOGI(rest::LOG_TAG, "Searching node with name '%.*s'", (int)str.length, str.str);

                if (str.empty()) {
                    ++iter;
                    // ESP_LOGI(rest::LOG_TAG, "Skipping empty string.");
                    continue;
                }

                if (currNode->name == str) {
                    // ESP_LOGI(rest::LOG_TAG, "Found node '%.*s'", (int)str.length, str.str);

                    if (currNode->hasChildren) {
                        if (arrayIndex > 0) {
                            ESP_LOGE(LOG_TAG, "Found array after going into array: not allowed.");
                            return nullptr;
                        }

                        if (static_cast<const NodeContainerBase*>(currNode)->isArray) {
                            ++iter;
                            if (iter == end) {
                                return currNode;
                            }
                            str = *iter;

                            // get index from uri
                            utils::String indexStr{str};
                            indexStr.copy();

                            if (indexStr.empty())
                                return nullptr;

                            char* strEnd;
                            auto index = strtol(indexStr.str(), &strEnd, 10);
                            if (index < 0) {
                                ESP_LOGW(LOG_TAG, "Received negative array index (%ld).", index);
                                return nullptr;
                            }

                            isArrayNode = true;
                            arrayIndex = (size_t)index;

                            ++iter;
                            if (iter == end)
                                return currNode;

                            parent = static_cast<const NodeContainerBase*>(currNode);
                            index = 0;
                            currNode = parent->getChild(parent, index);

                        } else {
                            ++iter;
                            if (iter == end)
                                return currNode;

                            parent = static_cast<const NodeContainerBase*>(currNode);
                            index = 0;
                            currNode = parent->getChild(parent, index);
                        }

                    } else {
                        // ESP_LOGI(rest::LOG_TAG, "No more children '%.*s' iterAtEnd=%s", (int)str.length, str.str, iter == end ? "true" : "false");

                        ++iter;
                        if (iter == end)
                            return currNode;

                        return nullptr;
                    }
                } else {
                    // ESP_LOGI(rest::LOG_TAG, "Node name mismatch dir='%.*s' node='%s'", (int)str.length, str.str, currNode->name.str);

                    if (parent && index < parent->numChilds) {
                        ++index;
                        currNode = parent->getChild(parent, index);

                        // ESP_LOGI(rest::LOG_TAG, "Getting next sibling");

                    } else {
                        // ESP_LOGI(rest::LOG_TAG, "No more siblings.");
                        break;
                    }
                }
            }
            return nullptr;
        }

        cJSON* createJSON(const Node* node, cJSON* json, bool isArrayNode, size_t arrayIndex) {
            if (!node)
                return nullptr;
            if (!json)
                json = cJSON_CreateObject();

            // ESP_LOGI(rest::LOG_TAG, "Node '%s'", node->name.str);

            if (node->hasChildren) {
                // ESP_LOGI(rest::LOG_TAG, "Node has children %u", static_cast<const NodeContainerBase*>(node)->numChilds);

                size_t count = 0;
                const NodeContainerBase* parentNode = static_cast<const NodeContainerBase*>(node);
                size_t numChilds = parentNode->numChilds;

                for (size_t i = 0; i < numChilds; ++i) {
                    const Node* child = parentNode->getChild(parentNode, i);
                    ASSERT(child, LOG_TAG);

                    // ESP_LOGI(rest::LOG_TAG, "Child node '%s'", child->name.str);

                    if (child->hasChildren) {
                        const NodeContainerBase* container = static_cast<const NodeContainerBase*>(child);
                        if (container->isArray) {
                            // ESP_LOGI(rest::LOG_TAG, "Node is array, count elements %u", count);

                            for (uint8_t arrayIndex = 0; arrayIndex < count; ++arrayIndex) {
                                utils::String numberStr = utils::String::fromNumber(arrayIndex);
                                cJSON* item = cJSON_AddObjectToObject(json, numberStr.str());

                                // ESP_LOGI(rest::LOG_TAG, "index %s", numberStr.str);

                                for (size_t elemIndex = 0; elemIndex < container->numChilds; ++elemIndex) {
                                    const Node* elem = container->getChild(container, elemIndex);

                                    // ESP_LOGI(rest::LOG_TAG, "at %p str=%p", elem, elem->name.str);
                                    // ESP_LOGI(rest::LOG_TAG, "element %s", elem->name.str);

                                    createJSON(elem, cJSON_AddObjectToObject(item, elem->name.str()), true, arrayIndex);
                                }
                            }
                        } else {
                            // ESP_LOGI(rest::LOG_TAG, "Node is not array");
                            createJSON(child, cJSON_AddObjectToObject(json, child->name.str()));
                        }
                    } else {
                        // ESP_LOGI(rest::LOG_TAG, "Node has no children.");

                        auto item = createJSON(child, cJSON_AddObjectToObject(json, child->name.str()));

                        if (child->name == "count" && cJSON_IsObject(item)) {
                            // ESP_LOGI(rest::LOG_TAG, "Found count node.");
                            if (count > 0) {
                                ESP_LOGW(rest::LOG_TAG, "Encountered multiple nodes named 'count'");
                            }
                            cJSON* value = cJSON_GetObjectItemCaseSensitive(item, "value");
                            if (value && cJSON_IsNumber(value)) {
                                count = (size_t)value->valuedouble;
                                // ESP_LOGI(rest::LOG_TAG, "count %i", count);
                            }
                        }
                    }
                }
            } else {
                if (isArrayNode) {
                    ArrayLeafNode* leaf = (ArrayLeafNode*)node;

                    // ESP_LOGI(rest::LOG_TAG, "ArrayLeafNode, '%s'", leaf->name.str);
                    ASSERT(leaf->read, rest::LOG_TAG);
                    leaf->read(leaf, arrayIndex, json);
                } else {
                    LeafNode* leaf = (LeafNode*)node;

                    // ESP_LOGI(rest::LOG_TAG, "LeafNode '%s'", leaf->name.str);
                    ASSERT(leaf->read, rest::LOG_TAG);
                    leaf->read(leaf, json);
                }

                // ESP_LOGI(rest::LOG_TAG, "Node finished '%s'", node->name.str);
            }
            return json;
        }

    }  // namespace rest
}  // namespace web