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
            utils::String name;
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
    };

    utils::StringView log();
    utils::String version();

    Config& inst();
}  // namespace conf

#endif  //_Config_h_