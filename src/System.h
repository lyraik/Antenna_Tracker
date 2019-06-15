/**
 * @file Config.h
 * @author Dominik Gschwind
 * @brief
 * @version 0.1
 * @date 2019/06/13
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once
#ifndef _Config_h_
#define _Config_h_

#include "utils/String.h"
#include <stddef.h>

namespace sys {

    static constexpr const char LOG_TAG[] = "sys";

    static constexpr const char CONFIG_FILE_PATH[] = "/config.json";

    struct Config {
        struct {
            float angleFromNorth;
        } direction;

        // position

        struct {
            utils::String name;
            utils::String uid;
        } bluetooth;

        struct {
            utils::String ssid;
            utils::String password;
        } wifi;

        // battery

        utils::StringView version;

        struct {
            float changePerStep;
            float velocity;
            float acceleration;
        } stepper;

        struct {
            float velocity;
            float acceleration;
        } servo;

        void save();
        void load();
    };

    utils::StringView log();
    utils::String version();

    Config& inst();
}  // namespace conf

#endif  //_Config_h_