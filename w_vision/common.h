//
// Created by wangry on 2019/8/27.
//

#ifndef ANDROID_COMPUTERVISIONPROJECT_MASTER_COMMON_H
#define ANDROID_COMPUTERVISIONPROJECT_MASTER_COMMON_H
#include "platform.h"

//#define WIN_PLATFORM

#ifdef WIN_PLATFORM
//windows log api
#else
	#include <android/log.h>
	#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG, __VA_ARGS__)
	#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG, __VA_ARGS__)
	#define LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG, __VA_ARGS__)
	#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG, __VA_ARGS__)
	#define LOGF(...)  __android_log_print(ANDROID_LOG_FATAL,LOG_TAG, __VA_ARGS__)
#endif

#endif //ANDROID_COMPUTERVISIONPROJECT_MASTER_COMMON_H


