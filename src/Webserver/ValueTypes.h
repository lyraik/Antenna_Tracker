/**
 * @file ValueTypes.h
 * @author Dominik Gschwind
 * @brief
 * @version 0.1
 * @date 2019/06/16
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once
#ifndef _ValueTypes_h_
#define _ValueTypes_h_

#include <cJSON/cJSON.h>
#include <etl/algorithm.h>
#include <stdint.h>

namespace web {
    namespace rest {

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

        static const ValueValidator validators[TYPE_SIZE] = {[](double val) { return std::max(0.0, std::min(360.0, val)); },
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

        inline static void createNumber(double num, bool reaonly, ValueType type, cJSON* parent) {
            cJSON_AddNumberToObject(parent, "value", num);
            cJSON_AddStringToObject(parent, "type", "Number");
            cJSON_AddFalseToObject(parent, "readonly");
            cJSON_AddNumberToObject(parent, "unit", (double)type);
        }
        inline static void createString(const char* str, bool reaonly, cJSON* parent) {
            cJSON_AddStringToObject(parent, "value", str);
            cJSON_AddStringToObject(parent, "type", "String");
            cJSON_AddFalseToObject(parent, "readonly");
        }
    }  // namespace internal
}  // namespace web

#endif  //_ValueTypes_h_