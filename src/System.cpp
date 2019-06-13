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

namespace sys {

    namespace internal{
        Config config;
    }

    utils::StringView log() {
        return {};
    }

    utils::String version() {
        return utils::StringView{};
    }



}  // namespace sys