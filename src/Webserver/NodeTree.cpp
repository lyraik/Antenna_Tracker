/**
 * @file NodeTree.cpp
 * @author Dominik Gschwind
 * @brief
 * @version 0.1
 * @date 2019/06/16
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "NodeTree.h"

namespace web {
    namespace internal {
        namespace rest {

            const api_type api {
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
                    bluetooth_type{
                        "bluetooth", false,
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
                    wifi_type{"wifi", false,
                              wifi_type::tuple_type{
                                  //--   { name } 0
                                  LeafNode{LN_WIFI_SSID, "ssid", false, handleWrite, handleRead},
                                  //--   { /name }
                                  //--   { password } 1
                                  LeafNode{LN_WIFI_PASSWD, "password", false, handleWrite, handleRead},
                                  //--   { /password }
                                  //--    { connections } 2
                                  wifi_cons_type{
                                      "list", false,
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

            void handleRead(LeafNode* node, cJSON* parent) {
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
                        createNumber(0.0, true, TYPE_ANGLE_ACCEL, parent);
                        break;
                }
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
                }
            }

            void handleReadArray(ArrayLeafNode* node, size_t index, cJSON* parent) {
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

        }  // namespace rest
    }      // namespace internal
}  // namespace web