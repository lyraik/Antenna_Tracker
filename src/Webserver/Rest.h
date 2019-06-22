/**
 * @file Rest.h
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
    namespace rest {
        static constexpr const char LOG_TAG[] = "rest";

        static constexpr const char VERSION[] = "1.1";

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

        struct Node {
            const bool hasChildren : 1;
            uint8_t nodeId;

            utils::StringView name;

            constexpr Node(bool _hasChildren, uint8_t _nodeId, const utils::StringView& _name) : hasChildren(_hasChildren), nodeId(_nodeId), name(_name) {}
        };

        struct NodeContainerBase : public Node {
            using ChildGetter = const Node* (*)(const Node*, size_t);

            bool isArray;
            uint8_t numChilds;

            ChildGetter getChild;

            constexpr NodeContainerBase(bool _hasChildren, uint8_t _nodeId, const utils::StringView& _name, bool _isArray, uint8_t _numChilds, ChildGetter _getChild)
                : Node{_hasChildren, _nodeId, _name}, isArray(_isArray), numChilds(_numChilds), getChild(_getChild) {}
        };

        template <class... Nodes>
        struct NodeContainer : public NodeContainerBase {
            using tuple_type = std::tuple<Nodes...>;
            tuple_type nodes;

            inline static const Node* getChild(const Node* _this, size_t index) {
                const auto* ptr = static_cast<const NodeContainer<Nodes...>*>(_this);
                if (index >= ptr->numChilds)
                    return nullptr;
                return &utils::get<const Node&>(ptr->nodes, index);
            }

           public:
            constexpr NodeContainer(const utils::StringView& _name, bool _isArray, tuple_type _nodes, uint8_t _nodeId = 0)
                : NodeContainerBase{true, _nodeId, _name, _isArray, sizeof...(Nodes), &getChild}, nodes(_nodes) {}
        };

        struct LeafNode : public Node {
            using WriteHandler = void (*)(LeafNode*, cJSON*);
            using ReadHandler = void (*)(LeafNode*, cJSON*);

            bool readonly;
            WriteHandler write;
            ReadHandler read;

            constexpr LeafNode(uint8_t _nodeId, const utils::StringView& _name, bool _readonly, WriteHandler wh, ReadHandler rh)
                : Node{false, _nodeId, _name}, readonly(_readonly), write(wh), read(rh) {}
        };

        struct ArrayLeafNode : public Node {
            using WriteHandler = void (*)(ArrayLeafNode*, size_t, cJSON*);
            using ReadHandler = void (*)(ArrayLeafNode*, size_t, cJSON*);

            bool readonly;
            WriteHandler write;
            ReadHandler read;

            constexpr ArrayLeafNode(uint8_t _nodeId, const utils::StringView& _name, bool _readonly, WriteHandler wh, ReadHandler rh)
                : Node{false, _nodeId, _name}, readonly(_readonly), write(wh), read(rh) {}
        };

        void handleRead(LeafNode* node, cJSON* parent);
        void handleWrite(LeafNode* node, cJSON* json);
        void handleReadArray(ArrayLeafNode* node, size_t index, cJSON* parent);
        void handleWriteArray(ArrayLeafNode* node, size_t index, cJSON* json);

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

        static constexpr api_type API{
            /* api begin */
            // -- <header>
            "api", false,
            // -- </header>
            // -- <children>
            api_type::tuple_type{
                //--  [ direction ] 0
                direction_type{"direction", false,
                               direction_type::tuple_type{// reference 0
                                                          LeafNode{LN_DIR_REF, "reference", false, handleWrite, handleRead},
                                                          // vertical 1
                                                          LeafNode{LN_DIR_VERT, "vertical", false, handleWrite, handleRead},
                                                          // horizontal 2
                                                          LeafNode{LN_DIR_HORIZ, "horizontal", false, handleWrite, handleRead}}},
                //-- [ /direction ]

                //-- [ position ] 1
                position_type{"position", false,
                              position_type::tuple_type{
                                  //--  { tracker } 0
                                  position_inner_type{"tracker", false,
                                                      position_inner_type::tuple_type{// longitude (0)
                                                                                      LeafNode{LN_POS_T_LONG, "longitude", true, handleWrite, handleRead},
                                                                                      // lattitude (1)
                                                                                      LeafNode{LN_POS_T_ALT, "lattitude", true, handleWrite, handleRead},
                                                                                      // altitude (2)
                                                                                      LeafNode{LN_POS_T_ALT, "altitude", true, handleWrite, handleRead}}},
                                  //-- { /tracker }
                                  //-- { tracked } 1
                                  position_inner_type{"tracked", false,
                                                      position_inner_type::tuple_type{// longitude (0)
                                                                                      LeafNode{LN_POS_D_LONG, "longitude", true, handleWrite, handleRead},
                                                                                      // lattitude (1)
                                                                                      LeafNode{LN_POS_D_LAT, "lattitude", true, handleWrite, handleRead},
                                                                                      // altitude (2)
                                                                                      LeafNode{LN_POS_D_ALT, "altitude", true, handleWrite, handleRead}}}
                                  //-- { /tracked }
                              }},
                //--   [ /position ]

                //--    [ bluetooth ] 2
                bluetooth_type{"bluetooth", false,
                               bluetooth_type::tuple_type{
                                   //--    { list } 0
                                   bt_list_type{"list", false,
                                                bt_list_type::tuple_type{
                                                    //--   <count> 0
                                                    LeafNode{LN_BT_LIST_COUNT, "count", true, handleWrite, handleRead},
                                                    //--   </count>
                                                    //--   <index> 1
                                                    bt_list_arr_type{"index", true,
                                                                     bt_list_arr_type::tuple_type{// name 0
                                                                                                  ArrayLeafNode{AN_BT_LIST_NAME, "name", true, handleWriteArray, handleReadArray},
                                                                                                  // uid 1
                                                                                                  ArrayLeafNode{AN_BT_LIST_UID, "uid", true, handleWriteArray, handleReadArray}}}
                                                    //--   </index>
                                                }},
                                   //--    { /list }
                                   //--   { set } 1
                                   {LN_BT_SET, "set", false, handleWrite, handleRead}
                                   //--   { /set }
                               }},
                //--    [ /bluetooth ]

                //--    [ wifi ] 3
                wifi_type{
                    "wifi", false,
                    wifi_type::tuple_type{
                        //--   { name } 0
                        LeafNode{LN_WIFI_SSID, "ssid", false, handleWrite, handleRead},
                        //--   { /name }
                        //--   { password } 1
                        LeafNode{LN_WIFI_PASSWD, "password", false, handleWrite, handleRead},
                        //--   { /password }
                        //--    { connections } 2
                        wifi_cons_type{"list", false,
                                       wifi_cons_type::tuple_type{
                                           //--   <count> 0
                                           LeafNode{LN_WIFI_CONS_COUNT, "count", true, handleWrite, handleRead},
                                           //--   </count>
                                           //--   <index> 1
                                           wifi_cons_arr_type{"index", true,
                                                              wifi_cons_arr_type::tuple_type{// address 0
                                                                                             ArrayLeafNode{AN_WIFI_CONS_ADDR, "address", true, handleWriteArray, handleReadArray}}}
                                           //--   </index>
                                       }},
                        //--    { /connections }
                    }},
                //--    [ /wifi ]
                //--    [ battery ] 4
                battery_type{"battery", false,
                             battery_type::tuple_type{
                                 //--   { count } 0
                                 LeafNode{LN_BAT_COUNT, "count", true, handleWrite, handleRead},
                                 //--   { /count }
                                 //--   { index } 1
                                 bat_arr_type{"index", true,
                                              bat_arr_type::tuple_type{// voltage 0
                                                                       ArrayLeafNode{AN_BAT_VOLT, "voltage", true, handleWriteArray, handleReadArray},
                                                                       // current 1
                                                                       ArrayLeafNode{AN_BAT_CURR, "current", true, handleWriteArray, handleReadArray},
                                                                       // consumption 2
                                                                       ArrayLeafNode{AN_BAT_CONS, "consumption", true, handleWriteArray, handleReadArray}}}
                                 //--   { /index }
                             }},
                //--    [ /wifi ]

                //-- [ system ] 5
                system_type{"system", false,
                            system_type::tuple_type{
                                // version (0)
                                LeafNode{LN_SYS_VER, "version", true, handleWrite, handleRead},
                                // runtime (1)
                                LeafNode{LN_SYS_RUN, "runtime", true, handleWrite, handleRead},
                                // log (2)
                                LeafNode{LN_SYS_LOG, "log", true, handleWrite, handleRead},
                                //-- { debug } 3
                                sys_debug_type{"debug", false,
                                               sys_debug_type::tuple_type{// totalMemory (0)
                                                                          LeafNode{LN_SYS_DBG_TOTMEM, "totalMemory", true, handleWrite, handleRead},
                                                                          // usedMemory (1)
                                                                          LeafNode{LN_SYS_DBG_USEMEM, "usedMemory", true, handleWrite, handleRead},
                                                                          // processCount (2)
                                                                          LeafNode{LN_SYS_DBG_PROCS, "processCount", true, handleWrite, handleRead}}}
                                //-- { /debug }
                            }},
                //--   [ /system ]

                //-- [ settings ] 6
                settings_type{"settings", false,
                              settings_type::tuple_type{
                                  //--  { stepper } 0
                                  settings_step_type{"stepper", false,
                                                     settings_step_type::tuple_type{// changePreStep (0)
                                                                                    LeafNode{LN_SET_STEP_CPS, "changePerStep", false, handleWrite, handleRead},
                                                                                    // velocity (1)
                                                                                    LeafNode{LN_SET_STEP_VEL, "velocity", true, handleWrite, handleRead},
                                                                                    // acceleration (2)
                                                                                    LeafNode{LN_SET_STEP_ACC, "acceleration", true, handleWrite, handleRead}}},
                                  //-- { /stepper }
                                  //-- { servo } 1
                                  settings_serv_type{"servo", false,
                                                     settings_serv_type::tuple_type{// velocity (0)
                                                                                    LeafNode{LN_SET_SERV_VEL, "velocity", true, handleWrite, handleRead},
                                                                                    // acceleration (1)
                                                                                    LeafNode{LN_SET_SERV_ACC, "acceleration", true, handleWrite, handleRead}}}
                                  //-- { /servo }
                              }}
                //--   [ /settings ]
            }
            // -- </children>
            /* api end */
        };

        const Node* findNode(utils::StringView uri, size_t& arrayIndex, bool& isArrayNode, char delimitor = '/');
        cJSON* createJSON(const Node* node, cJSON* parent, bool isArrayNode = false, size_t arrayIndex = 0);

    }  // namespace rest
}  // namespace web

#endif  //_NodeTree_h_