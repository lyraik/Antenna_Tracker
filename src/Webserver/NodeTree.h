/**
 * @file NodeTree.h
 * @author Dominik Gschwind
 * @brief
 * @version 0.1
 * @date 2019/06/16
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once
#ifndef _NodeTree_h_
#define _NodeTree_h_

#include <cJSON/cJSON.h>
#include "Utils/CompileTime.h"
#include "Utils/Json.h"
#include "Utils/String.h"
#include "Utils/Utils.h"

#include "ValueTypes.h"

namespace web {
    namespace internal {
        namespace rest {
            static constexpr const char LOG_TAG[] = "rest";
        }

        struct Node {
            const Node* parent;
            const Node* children;
            const Node* next;
            uint8_t nodeId;

            utils::StringView name;

            constexpr Node(const Node* _parent, const Node* _children, const Node* _next, uint8_t _nodeId, const utils::StringView& _name)
                : parent(_parent), children(_children), next(_next), nodeId(_nodeId), name(_name) {}
        };

        struct NodeContainerBase : public Node {
            bool isArray;
            uint8_t numChilds;

            NodeContainerBase(const Node* _parent, const Node* _children, const Node* _next, uint8_t _nodeId, const utils::StringView& _name, bool _isArray, uint8_t _numChilds)
                : Node{_parent, _children, _next, _nodeId, _name}, isArray(_isArray), numChilds(_numChilds) {}
        };

        template <class... Nodes>
        struct NodeContainer : public NodeContainerBase {
            using tuple_type = std::tuple<Nodes...>;
            tuple_type nodes;

            template <size_t Index>
            typename std::tuple_element<Index, tuple_type>::type& get() {
                return std::get<Index>(nodes);
            }

           private:
            static Node* updateNextAndParent(Node& node, Node* p, Node* l) {
                // ESP_LOGI(rest::LOG_TAG, "Update '%s', last='%s', parent='%s'", node.name.str, l ? l->name.str : "", p ? p->name.str : "");
                if (!node.children)
                    node.parent = p;
                if (!l && p)
                    p->children = &node;
                if (l)
                    l->next = &node;
                return &node;
            }

           public:
            NodeContainer(const utils::StringView& _name, bool _isArray, tuple_type _nodes, uint8_t _nodeId = 0, const Node* _parent = nullptr, const Node* _next = nullptr)
                : NodeContainerBase{_parent, nullptr, _next, _nodeId, _name, _isArray, sizeof...(Nodes)}, nodes(_nodes) {
                // set parent and next for each child node
                ESP_LOGI(rest::LOG_TAG, "Initialize %s", _name.str);
                utils::forEach<0, Node*, Node&, Node*>(nodes, updateNextAndParent, this, nullptr);
            }
        };

        struct LeafNode : public Node {
            using WriteHandler = void (*)(LeafNode*, cJSON*);
            using ReadHandler = void (*)(LeafNode*, cJSON*);

            bool readonly;
            WriteHandler write;
            ReadHandler read;

            LeafNode(uint8_t _nodeId, const utils::StringView& _name, bool _readonly, WriteHandler wh, ReadHandler rh, const Node* _parent = nullptr, const Node* _next = nullptr)
                : Node{_parent, nullptr, _next, _nodeId, _name}, readonly(_readonly), write(wh), read(rh) {
                ESP_LOGI(rest::LOG_TAG, "node '%s' %p, %p", name.str, read, write);
            }
        };

        struct ArrayLeafNode : public Node {
            using WriteHandler = void (*)(ArrayLeafNode*, size_t, cJSON*);
            using ReadHandler = void (*)(ArrayLeafNode*, size_t, cJSON*);

            bool readonly;
            WriteHandler write;
            ReadHandler read;

            ArrayLeafNode(uint8_t _nodeId, const utils::StringView& _name, bool _readonly, WriteHandler wh, ReadHandler rh, const Node* _parent = nullptr, const Node* _next = nullptr)
                : Node{_parent, nullptr, _next, _nodeId, _name}, readonly(_readonly), write(wh), read(rh) {}
        };

        namespace rest {

            void handleRead(LeafNode* node, cJSON* parent);
            void handleWrite(LeafNode* node, cJSON* json);
            void handleReadArray(ArrayLeafNode* node, size_t index, cJSON* parent);
            void handleWriteArray(ArrayLeafNode* node, size_t index, cJSON* json);

            enum LeafNodeId {
                LN_DIR_REF,
                LN_DIR_VERT,
                LN_DIR_HORIZ,
                LN_POS_T_LONG,
                LN_POS_T_LAT,
                LN_POS_T_ALT,
                LN_POS_D_LONG,
                LN_POS_D_LAT,
                LN_POS_D_ALT,
                LN_BT_LIST_COUNT,
                LN_BT_SET,
                LN_WIFI_CONS_COUNT,
                LN_WIFI_SSID,
                LN_WIFI_PASSWD,
                LN_BAT_COUNT,
                LN_SYS_DBG_TOTMEM,
                LN_SYS_DBG_USEMEM,
                LN_SYS_DBG_PROCS,
                LN_SYS_VER,
                LN_SYS_RUN,
                LN_SYS_LOG,
                LN_SET_STEP_CPS,
                LN_SET_STEP_VEL,
                LN_SET_STEP_ACC,
                LN_SET_SERV_VEL,
                LN_SET_SERV_ACC
            };
            enum ArrayLeafNodeId { AN_BT_LIST_NAME, AN_BT_LIST_UID, AN_WIFI_CONS_ADDR, AN_BAT_VOLT, AN_BAT_CURR, AN_BAT_CONS };

            static constexpr utils::StringView VERSION = "1.1";

            using direction_type = NodeContainer<LeafNode, LeafNode, LeafNode>;

            using position_inner_type = NodeContainer<LeafNode, LeafNode, LeafNode>;
            using position_type = NodeContainer<position_inner_type, position_inner_type>;

            using bt_list_arr_type = NodeContainer<ArrayLeafNode, ArrayLeafNode>;
            using bt_list_type = NodeContainer<LeafNode, bt_list_arr_type>;
            using bluetooth_type = NodeContainer<bt_list_type, LeafNode>;

            using wifi_cons_arr_type = NodeContainer<ArrayLeafNode>;
            using wifi_cons_type = NodeContainer<LeafNode, wifi_cons_arr_type>;
            using wifi_type = NodeContainer<LeafNode, LeafNode, wifi_cons_type>;

            using bat_arr_type = NodeContainer<ArrayLeafNode, ArrayLeafNode, ArrayLeafNode>;
            using battery_type = NodeContainer<LeafNode, bat_arr_type>;

            using sys_debug_type = NodeContainer<LeafNode, LeafNode, LeafNode>;
            using system_type = NodeContainer<LeafNode, LeafNode, LeafNode, sys_debug_type>;

            using settings_step_type = NodeContainer<LeafNode, LeafNode, LeafNode>;
            using settings_serv_type = NodeContainer<LeafNode, LeafNode>;
            using settings_type = NodeContainer<settings_step_type, settings_serv_type>;

            using api_type = NodeContainer<direction_type, position_type, bluetooth_type, wifi_type, battery_type, system_type, settings_type>;

            extern const api_type api;

        }  // namespace rest

    }  // namespace internal
}  // namespace web

#endif  //_NodeTree_h_