/**
 * @file Json.h
 * @author Dominik Gschwind
 * @brief Json utilities
 * @version 0.1
 * @date 2019/06/14
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#pragma once
#ifndef _Json_h_
#define _Json_h_

#include <cJSON/cJSON.h>
#include <etl/memory.h>

namespace utils{

    namespace internal{
        struct jsonDeleter{
            void operator()(cJSON* json){
                cJSON_Delete(json);
            }
        };
    }

    using JsonGuard = etl::unique_ptr<cJSON, internal::jsonDeleter>;

}

#endif //_Json_h_