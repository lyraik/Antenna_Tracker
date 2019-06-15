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
#include "Utils/Json.h"
#include "Utils/Utils.h"

namespace web {
    namespace internal {

        using ValueValidator = double (*)(double);

        enum ValueType : uint8_t {
            TYPE_FUL_DEG = 0,
            TYPE_HALF_DEG,
            TYPE_DECIMAL,
            TYPE_INTEGER,
            TYPE_POS_INTEGER,
            TYPE_VOLTAGE,
            TYPE_CURRENT,
            TYPE_CONSUMPTION,
            TYPE_TIME,
            TYPE_NUM_BYTES,
            TYPE_DEG_PER_STEP,
            TYPE_ANGLE_SPEED,
            TYPE_ANGLE_ACCEL,
            TYPE_SIZE
        };

        ValueValidator validators[TYPE_SIZE] = {[](double val) { return std::max(0.0, std::min(360.0, val)); },
                                                [](double val) { return std::max(0.0, std::min(180.0, val)); },
                                                [](double val) { return val; },
                                                [](double val) { return round(val); },
                                                [](double val) { return std::max(0.0, round(val)); },
                                                [](double val) { return val; },
                                                [](double val) { return val; },
                                                [](double val) { return std::max(0.0, val); },
                                                [](double val) { return std::max(0.0, val); },
                                                [](double val) { return std::max(0.0, std::min(5.0, val)); },
                                                [](double val) { return std::max(0.0, val); },
                                                [](double val) { return std::max(0.0, val); }};

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
            Node* parent;
            Node* children;
            Node* next;
            uint8_t numChilds;

            utils::StringView name;
        };

        template <class... Nodes>
        struct NodeContainer : public Node {
            bool isArray;
            using tuple_type = std::tuple<Nodes...>;
            tuple_type nodes;

            template <size_t Index>
            typename std::tuple_element<Index, tuple_type>::type& get() {
                return std::get<Index>(nodes);
            }

            NodeContainer(Node* parent, Node* children, Node* nhxt, uint8_t nc, utils::StringView name, bool isArray, tuple_type nodes)
                : Node{parent, children, next, nc, name}, isArray(isArray), nodes(nodes) {}

            constexpr static uint8_t size = sizeof...(Nodes);
        };

        struct LeafNode : public Node {
            using WriteHandler = void (*)(LeafNode*, cJSON*);
            using ReadHandler = cJSON* (*)(LeafNode*);

            bool readonly;
            WriteHandler write;
            ReadHandler read;

            LeafNode(Node* parent, Node* children, Node* next, uint8_t nc, utils::StringView name, bool readonly, WriteHandler wh, ReadHandler rh)
                : Node{parent, children, next, nc, name}, readonly(readonly), write(wh), read(rh) {}
        };

        struct ArrayLeafNode : public Node {
            using WriteHandler = void (*)(ArrayLeafNode*, size_t, cJSON*);
            using ReadHandler = cJSON* (*)(ArrayLeafNode*, size_t);

            bool readonly;
            WriteHandler write;
            ReadHandler read;

            ArrayLeafNode(Node* parent, Node* children, Node* next, uint8_t nc, utils::StringView name, bool readonly, WriteHandler wh, ReadHandler rh)
                : Node{parent, children, next, nc, name}, readonly(readonly), write(wh), read(rh) {}
        };

        cJSON* createNumber(double num, bool reaonly, ValueType type) {
            auto* json = cJSON_CreateObject();
            cJSON_AddNumberToObject(json, "value", num);
            cJSON_AddStringToObject(json, "type", "Number");
            cJSON_AddFalseToObject(json, "readonly");
            cJSON_AddNumberToObject(json, "unit", (double)type);
            return json;
        }
        cJSON* createString(const char* str, bool reaonly, ValueType type) {
            auto* json = cJSON_CreateObject();
            cJSON_AddStringToObject(json, "value", str);
            cJSON_AddStringToObject(json, "type", "Number");
            cJSON_AddFalseToObject(json, "readonly");
            cJSON_AddNumberToObject(json, "unit", (double)type);
            return json;
        }

        cJSON* handleRead(LeafNode* node) {
            return createNumber(0.0, true, TYPE_ANGLE_ACCEL);
        }
        void handleWrite(LeafNode* node, cJSON* json) {}

        cJSON* handleReadArray(ArrayLeafNode* node, size_t index) {
            return createNumber(0.0, true, TYPE_ANGLE_ACCEL);
        }
        void handleWriteArray(ArrayLeafNode* node, size_t index, cJSON* json) {}

        struct Interface {
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

            api_type api{
                /* api begin */
                // -- <header>
                nullptr, &api.get<0>(), nullptr, api.size, "api", false,
                // -- </header>
                // -- <children>
                api_type::tuple_type{
                    //--  [ direction ] 0
                    direction_type{&api, &api.get<0>().get<0>(), &api.get<1>(), 3, "direction", false,
                                   direction_type::tuple_type{// reference 0
                                                              LeafNode{&api.get<0>(), nullptr, &api.get<0>().get<1>(), LN_DIR_REF, "reference", false, handleWrite, handleRead},
                                                              // vertical 1
                                                              LeafNode{&api.get<0>(), nullptr, &api.get<0>().get<2>(), LN_DIR_VERT, "vertical", false, handleWrite, handleRead},
                                                              // horizontal 2
                                                              LeafNode{&api.get<0>(), nullptr, nullptr, LN_DIR_HORIZ, "horizontal", false, handleWrite, handleRead}}},
                    //-- [ /direction ]

                    //-- [ position ] 1
                    position_type{
                        &api, &api.get<1>().get<0>(), &api.get<2>(), 2, "position", false,
                        position_type::tuple_type{
                            //--  { tracker } 0
                            position_inner_type{&api.get<1>(), &api.get<1>().get<0>().get<0>(), &api.get<1>().get<1>(), 3, "tracker", false,
                                                position_inner_type::tuple_type{
                                                    // longitude (0)
                                                    LeafNode{&api.get<1>().get<0>(), nullptr, &api.get<1>().get<0>().get<1>(), LN_POS_T_LONG, "longitude", true, handleWrite, handleRead},
                                                    // lattitude (1)
                                                    LeafNode{&api.get<1>().get<0>(), nullptr, &api.get<1>().get<0>().get<2>(), LN_POS_T_ALT, "lattitude", true, handleWrite, handleRead},
                                                    // altitude (2)
                                                    LeafNode{&api.get<1>().get<0>(), nullptr, nullptr, LN_POS_T_ALT, "altitude", true, handleWrite, handleRead}}},
                            //-- { /tracker }
                            //-- { tracked } 1
                            position_inner_type{&api.get<1>(), &api.get<1>().get<1>().get<0>(), nullptr, 3, "tracked", false,
                                                position_inner_type::tuple_type{
                                                    // longitude (0)
                                                    LeafNode{&api.get<1>().get<1>(), nullptr, &api.get<1>().get<1>().get<1>(), LN_POS_D_LONG, "longitude", true, handleWrite, handleRead},
                                                    // lattitude (1)
                                                    LeafNode{&api.get<1>().get<1>(), nullptr, &api.get<1>().get<1>().get<2>(), LN_POS_D_LAT, "lattitude", true, handleWrite, handleRead},
                                                    // altitude (2)
                                                    LeafNode{&api.get<1>().get<1>(), nullptr, nullptr, LN_POS_D_ALT, "altitude", true, handleWrite, handleRead}}}
                            //-- { /tracked }
                        }},
                    //--   [ /position ]

                    //--    [ bluetooth ] 2
                    bluetooth_type{
                        &api, &api.get<2>().get<0>(), &api.get<3>(), 2, "bluetooth", false,
                        bluetooth_type::tuple_type{
                            //--    { list } 0
                            bt_list_type{&api.get<2>(), &api.get<2>().get<0>().get<0>(), &api.get<2>().get<1>(), 2, "list", false,
                                         bt_list_type::tuple_type{
                                             //--   <count> 0
                                             LeafNode{&api.get<2>().get<0>(), nullptr, &api.get<2>().get<0>().get<1>(), LN_BT_LIST_COUNT, "count", true, handleWrite, handleRead},
                                             //--   </count>
                                             //--   <index> 1
                                             bt_list_arr_type{&api.get<2>().get<0>(), &api.get<2>().get<0>().get<1>().get<0>(), nullptr, 2, "", true,
                                                              bt_list_arr_type::tuple_type{
                                                                  // name 0
                                                                  ArrayLeafNode{&api.get<2>().get<0>().get<1>(), nullptr, &api.get<2>().get<0>().get<1>().get<1>(), AN_BT_LIST_NAME, "name", true,
                                                                                handleWriteArray, handleReadArray},
                                                                  // uid 1
                                                                  ArrayLeafNode{&api.get<1>().get<1>(), nullptr, nullptr, AN_BT_LIST_UID, "uid", true, handleWriteArray, handleReadArray}}}
                                             //--   </index>
                                         }},
                            //--    { /list }
                            //--   { set } 1
                            {&api.get<2>(), nullptr, nullptr, LN_BT_SET, "set", false, handleWrite, handleRead}
                            //--   { /set }
                        }},
                    //--    [ /bluetooth ]

                    //--    [ wifi ] 3
                    wifi_type{&api, &api.get<3>().get<0>(), &api.get<4>(), 3, "wifi", false,
                              wifi_type::tuple_type{
                                  //--   { name } 0
                                  LeafNode{&api.get<3>(), nullptr, &api.get<3>().get<1>(), LN_WIFI_SSID, "ssid", false, handleWrite, handleRead},
                                  //--   { /name }
                                  //--   { password } 1
                                  LeafNode{&api.get<3>(), nullptr, &api.get<3>().get<2>(), LN_WIFI_PASSWD, "password", false, handleWrite, handleRead},
                                  //--   { /password }
                                  //--    { connections } 2
                                  wifi_cons_type{&api.get<3>(), &api.get<3>().get<2>().get<0>(), nullptr, 2, "list", false,
                                                 wifi_cons_type::tuple_type{
                                                     //--   <count> 0
                                                     LeafNode{&api.get<3>().get<2>(), nullptr, &api.get<3>().get<2>().get<1>(), LN_WIFI_CONS_COUNT, "count", true, handleWrite, handleRead},
                                                     //--   </count>
                                                     //--   <index> 1
                                                     wifi_cons_arr_type{&api.get<3>().get<2>(), &api.get<3>().get<2>().get<1>().get<0>(), nullptr, 1, "", true,
                                                                        wifi_cons_arr_type::tuple_type{// address 0
                                                                                                       ArrayLeafNode{&api.get<3>().get<2>().get<1>(), nullptr, nullptr, AN_WIFI_CONS_ADDR,
                                                                                                                     "address", true, handleWriteArray, handleReadArray}}}
                                                     //--   </index>
                                                 }},
                                  //--    { /connections }
                              }},
                    //--    [ /wifi ]
                    //--    [ battery ] 4
                    battery_type{
                        &api, &api.get<4>().get<0>(), &api.get<5>(), 2, "battery", false,
                        battery_type::tuple_type{
                            //--   { count } 0
                            LeafNode{&api.get<4>(), nullptr, &api.get<4>().get<1>(), LN_BAT_COUNT, "count", true, handleWrite, handleRead},
                            //--   { /count }
                            //--   { index } 1
                            bat_arr_type{&api.get<4>(), &api.get<4>().get<1>().get<0>(), nullptr, 3, "", true,
                                         bat_arr_type::tuple_type{
                                             // voltage 0
                                             ArrayLeafNode{&api.get<4>().get<1>(), nullptr, &api.get<4>().get<1>().get<1>(), AN_BAT_VOLT, "voltage", true, handleWriteArray, handleReadArray},
                                             // current 1
                                             ArrayLeafNode{&api.get<4>().get<1>(), nullptr, &api.get<4>().get<1>().get<2>(), AN_BAT_CURR, "current", true, handleWriteArray, handleReadArray},
                                             // consumption 2
                                             ArrayLeafNode{&api.get<4>().get<1>(), nullptr, nullptr, AN_BAT_CONS, "consumption", true, handleWriteArray, handleReadArray}}}
                            //--   { /index }
                        }},
                    //--    [ /wifi ]

                    //-- [ system ] 5
                    system_type{&api, &api.get<5>().get<0>(), &api.get<6>(), 4, "system", false,
                                system_type::tuple_type{
                                    // version (0)
                                    LeafNode{&api.get<5>(), nullptr, &api.get<5>().get<1>(), LN_SYS_VER, "version", true, handleWrite, handleRead},
                                    // runtime (1)
                                    LeafNode{&api.get<5>(), nullptr, &api.get<5>().get<2>(), LN_SYS_RUN, "runtime", true, handleWrite, handleRead},
                                    // log (2)
                                    LeafNode{&api.get<5>(), nullptr, &api.get<5>().get<3>(), LN_SYS_LOG, "log", true, handleWrite, handleRead},
                                    //-- { debug } 3
                                    sys_debug_type{&api.get<5>(), &api.get<5>().get<3>().get<0>(), nullptr, 3, "debug", false,
                                                   sys_debug_type::tuple_type{
                                                       // totalMemory (0)
                                                       LeafNode{&api.get<5>().get<3>(), nullptr, &api.get<5>().get<3>().get<1>(), LN_SYS_DBG_TOTMEM, "totalMemory", true, handleWrite, handleRead},
                                                       // usedMemory (1)
                                                       LeafNode{&api.get<5>().get<3>(), nullptr, &api.get<5>().get<3>().get<2>(), LN_SYS_DBG_USEMEM, "usedMemory", true, handleWrite, handleRead},
                                                       // processCount (2)
                                                       LeafNode{&api.get<5>().get<3>(), nullptr, nullptr, LN_SYS_DBG_PROCS, "processCount", true, handleWrite, handleRead}}}
                                    //-- { /debug }
                                }},
                    //--   [ /system ]

                    //-- [ settings ] 6
                    settings_type{
                        &api, &api.get<6>().get<0>(), nullptr, 2, "settings", false,
                        settings_type::tuple_type{
                            //--  { stepper } 0
                            settings_step_type{&api.get<6>(), &api.get<6>().get<0>().get<0>(), &api.get<6>().get<1>(), 3, "stepper", false,
                                               settings_step_type::tuple_type{
                                                   // changePreStep (0)
                                                   LeafNode{&api.get<6>().get<0>(), nullptr, &api.get<6>().get<0>().get<1>(), LN_SET_STEP_CPS, "changePerStep", false, handleWrite, handleRead},
                                                   // velocity (1)
                                                   LeafNode{&api.get<6>().get<0>(), nullptr, &api.get<6>().get<0>().get<2>(), LN_SET_STEP_VEL, "velocity", true, handleWrite, handleRead},
                                                   // acceleration (2)
                                                   LeafNode{&api.get<6>().get<0>(), nullptr, nullptr, LN_SET_STEP_ACC, "acceleration", true, handleWrite, handleRead}}},
                            //-- { /stepper }
                            //-- { servo } 1
                            settings_serv_type{&api.get<6>(), &api.get<6>().get<1>().get<0>(), nullptr, 2, "servo", false,
                                               settings_serv_type::tuple_type{
                                                   // velocity (0)
                                                   LeafNode{&api.get<6>().get<1>(), nullptr, &api.get<6>().get<1>().get<1>(), LN_SET_SERV_VEL, "velocity", true, handleWrite, handleRead},
                                                   // acceleration (1)
                                                   LeafNode{&api.get<6>().get<1>(), nullptr, nullptr, LN_SET_SERV_ACC, "acceleration", true, handleWrite, handleRead}}}
                            //-- { /servo }
                        }}
                    //--   [ /settings ]
                }
                // -- </children>
                /* api end */
            };

            Interface() {}

            Node* findNode(utils::StringView uri, char delimitor = '/'){
                Node* currNode = &api;
                for(auto str : uri.split(delimitor)){
                    if(str.empty()) continue;


                }
                return nullptr;
            }
        };

    }  // namespace internal
}  // namespace web

#endif  //_RestRequest_h_