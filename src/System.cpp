/**
 * @file System.cpp
 * @author Dominik Gschwind
 * @brief
 * @version 0.1
 * @date 2019/06/13
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "System.h"
#include "Filesystem/Filesystem.h"
#include "Utils/Json.h"

namespace sys {

    namespace internal {
        Config config;

        constexpr const char CJSON_ERROR[] = "JSON creation failed.";
        constexpr const char STRINGIFY_ERROR[] = "Stringify of JSON failed.";
        constexpr const char PARSE_ERROR[] = "Parse of JSON failed.";

        constexpr const char DIRECTION[] = "direction";
        constexpr const char ANGLE_FROM_NORTH[] = "angleFromNorth";
        constexpr const char BLUETOOTH[] = "bluetooth";
        constexpr const char NAME[] = "name";
        constexpr const char UID[] = "uid";
        constexpr const char WIFI[] = "wifi";
        constexpr const char SSID[] = "ssid";
        constexpr const char PASSWORD[] = "password";
        constexpr const char STEPPER[] = "stepper";
        constexpr const char CHANGE_PER_STEP[] = "changePerStep";
        constexpr const char VELOCITY[] = "velocity";
        constexpr const char ACCELERATION[] = "acceleration";
        constexpr const char SERVO[] = "servo";
    }  // namespace internal

#define CHECK_AND_RET_MSG(x, msg) \
    do { \
        if (!(x)) { \
            ESP_LOGE(LOG_TAG, "%s", (msg)); \
            return; \
        } \
    } while (0)
#define CHECK_AND_RET(x) CHECK_AND_RET_MSG(x, internal::CJSON_ERROR)
#define CHECK_AND_RET2(x) CHECK_AND_RET_MSG(x, internal::PARSE_ERROR)

    utils::StringView log() {
        return {};
    }

    utils::String version() {
        return utils::StringView{};
    }

    void Config::save() {
        fs::File conf;
        if (!conf.open(CONFIG_FILE_PATH, fs::Mode::WRITE)) {
            return;
        }

        // construct json object
        auto json = cJSON_CreateObject();
        CHECK_AND_RET(json);
        utils::JsonGuard guard{json};

        // direction
        cJSON* ptr;
        CHECK_AND_RET(ptr = cJSON_AddObjectToObject(json, internal::DIRECTION));
        CHECK_AND_RET(cJSON_AddNumberToObject(ptr, internal::ANGLE_FROM_NORTH, direction.angleFromNorth));
        // bluetooth
        CHECK_AND_RET(ptr = cJSON_AddObjectToObject(json, internal::BLUETOOTH));
        CHECK_AND_RET(cJSON_AddStringToObject(ptr, internal::NAME, bluetooth.name.str()));
        CHECK_AND_RET(cJSON_AddStringToObject(ptr, internal::UID, bluetooth.uid.str()));
        // wifi
        CHECK_AND_RET(ptr = cJSON_AddObjectToObject(json, internal::WIFI));
        CHECK_AND_RET(cJSON_AddStringToObject(ptr, internal::SSID, wifi.ssid.str()));
        CHECK_AND_RET(cJSON_AddStringToObject(ptr, internal::PASSWORD, wifi.password.str()));
        // stepper
        CHECK_AND_RET(ptr = cJSON_AddObjectToObject(json, internal::STEPPER));
        CHECK_AND_RET(cJSON_AddNumberToObject(ptr, internal::CHANGE_PER_STEP, stepper.changePerStep));
        CHECK_AND_RET(cJSON_AddNumberToObject(ptr, internal::VELOCITY, stepper.velocity));
        CHECK_AND_RET(cJSON_AddNumberToObject(ptr, internal::ACCELERATION, stepper.acceleration));
        // servo
        CHECK_AND_RET(ptr = cJSON_AddObjectToObject(json, internal::SERVO));
        CHECK_AND_RET(cJSON_AddNumberToObject(ptr, internal::VELOCITY, servo.velocity));
        CHECK_AND_RET(cJSON_AddNumberToObject(ptr, internal::ACCELERATION, servo.acceleration));

        auto str = cJSON_PrintUnformatted(json);
        CHECK_AND_RET_MSG(!str.empty(), internal::STRINGIFY_ERROR);

        conf.write((const void*)str.str(), str.len());
    }

    void Config::load() {
        ESP_LOGI(LOG_TAG, "before file");
        fs::File conf;
        if (!conf.open(CONFIG_FILE_PATH, fs::Mode::READ)) {
            return;
        }

        ESP_LOGI(LOG_TAG, "before length");
        size_t length = conf.length();
        ESP_LOGI(LOG_TAG, "len %d", length);
        utils::String buf{length};
        ESP_LOGI(LOG_TAG, "len %d ptr %d", length, (uintptr_t)buf.str());

        size_t read = conf.read(buf.buf(), length);
        ASSERT_RET(read == length, , LOG_TAG);

        // parse json object
        const char* parseEnd = nullptr;
        auto json = cJSON_ParseWithOpts(buf.str(), &parseEnd, false);
        if (!json) {
            ESP_LOGE(LOG_TAG, "%s Last position:\n%s", internal::PARSE_ERROR, parseEnd);
            return;
        }
        utils::JsonGuard guard{json};

        cJSON *ptr, *val;
        CHECK_AND_RET2(cJSON_IsObject(json));

        CHECK_AND_RET2(ptr = cJSON_GetObjectItemCaseSensitive(json, internal::DIRECTION));
        CHECK_AND_RET2(cJSON_IsObject(ptr));
        CHECK_AND_RET2(val = cJSON_GetObjectItemCaseSensitive(ptr, internal::ANGLE_FROM_NORTH));
        CHECK_AND_RET2(cJSON_IsNumber(val));
        direction.angleFromNorth = val->valuedouble;

        CHECK_AND_RET2(ptr = cJSON_GetObjectItemCaseSensitive(json, internal::BLUETOOTH));
        CHECK_AND_RET2(cJSON_IsObject(ptr));
        CHECK_AND_RET2(val = cJSON_GetObjectItemCaseSensitive(ptr, internal::NAME));
        CHECK_AND_RET2(cJSON_IsString(val));
        bluetooth.name = utils::String::fromCStr(val->valuestring);
        CHECK_AND_RET2(val = cJSON_GetObjectItemCaseSensitive(ptr, internal::UID));
        CHECK_AND_RET2(cJSON_IsString(val));
        bluetooth.uid = utils::String::fromCStr(val->valuestring);

        CHECK_AND_RET2(ptr = cJSON_GetObjectItemCaseSensitive(json, internal::WIFI));
        CHECK_AND_RET2(cJSON_IsObject(ptr));
        CHECK_AND_RET2(val = cJSON_GetObjectItemCaseSensitive(ptr, internal::SSID));
        CHECK_AND_RET2(cJSON_IsString(val));
        wifi.ssid = utils::String::fromCStr(val->valuestring);
        CHECK_AND_RET2(val = cJSON_GetObjectItemCaseSensitive(ptr, internal::PASSWORD));
        CHECK_AND_RET2(cJSON_IsString(val));
        wifi.password = utils::String::fromCStr(val->valuestring);

        CHECK_AND_RET2(ptr = cJSON_GetObjectItemCaseSensitive(json, internal::STEPPER));
        CHECK_AND_RET2(cJSON_IsObject(ptr));
        CHECK_AND_RET2(val = cJSON_GetObjectItemCaseSensitive(ptr, internal::CHANGE_PER_STEP));
        CHECK_AND_RET2(cJSON_IsNumber(val));
        stepper.changePerStep = val->valuedouble;
        CHECK_AND_RET2(val = cJSON_GetObjectItemCaseSensitive(ptr, internal::VELOCITY));
        CHECK_AND_RET2(cJSON_IsNumber(val));
        stepper.velocity = val->valuedouble;
        CHECK_AND_RET2(val = cJSON_GetObjectItemCaseSensitive(ptr, internal::ACCELERATION));
        CHECK_AND_RET2(cJSON_IsNumber(val));
        stepper.acceleration = val->valuedouble;

        CHECK_AND_RET2(ptr = cJSON_GetObjectItemCaseSensitive(json, internal::SERVO));
        CHECK_AND_RET2(cJSON_IsObject(ptr));
        CHECK_AND_RET2(val = cJSON_GetObjectItemCaseSensitive(ptr, internal::VELOCITY));
        CHECK_AND_RET2(cJSON_IsNumber(val));
        servo.velocity = val->valuedouble;
        CHECK_AND_RET2(val = cJSON_GetObjectItemCaseSensitive(ptr, internal::ACCELERATION));
        CHECK_AND_RET2(cJSON_IsNumber(val));
        servo.acceleration = val->valuedouble;
    }

    Config& inst() {
        return internal::config;
    }

}  // namespace sys