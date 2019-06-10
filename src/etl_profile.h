/**
 * @file etl_platform.h
 * @author Dominik Gschwind
 * @brief
 * @version 0.1
 * @date 2019/06/10
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once
#ifndef _etl_platform_h_
#define _etl_platform_h_

#if defined(_DEBUG) || defined(DEBUG)
#define ETL_LOG_ERRORS
#else
#define ETL_NO_CHECKS
#endif

#include <etl/profiles/cpp11.h>

#endif //_etl_platform_h_