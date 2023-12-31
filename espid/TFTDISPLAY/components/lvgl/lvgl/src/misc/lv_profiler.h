/**
 * @file lv_profiler.h
 *
 */

#ifndef LV_PROFILER_H
#define LV_PROFILER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "C:\Users\YUVA\Desktop\espid\TFTDISPLAY\components\lvgl\lvgl\lvgl.h\lv_conf_internal.h"

#if LV_USE_PROFILER

#include LV_PROFILER_INCLUDE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#else

#define LV_PROFILER_BEGIN
#define LV_PROFILER_END

#endif /*LV_USE_PROFILER*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PROFILER_H*/
