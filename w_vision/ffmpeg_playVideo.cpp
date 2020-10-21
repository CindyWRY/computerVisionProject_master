//
// Created by wangry on 2019/8/26.
//

#include "platform.h"

#ifdef WIN_PLATFORM

#else
#include "ffmpeg_playVideo.h"
#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/log.h>
#include "common.h"
#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


#include <opencv2/dnn.hpp>
#include "TrackableObject.h"
#include "count_frame.h"
#include "displayWindow.h"
#include "NativeWindow.h"
#include "count_net.h"



#define net

//static CV_Main app;
static count_frame f(11, 75, 0, 10, 0.7, 0.5, 0.65, 0.2, 0.1, 30, 0.4, 0.6);
//static count_frame f;
static count_net cn(11, 75, 0, 10, 0.78, 0.5, 0.65, 0.2, 0.1, 30, 0.4, 0.6);

#define  LOG_TAG    "JNI"

JNIEXPORT void JNICALL Java_ffmpeg_playVideo_setSuface
  (JNIEnv *env, jobject, jobject surface)
{
#ifdef net
	cn.setWindow(new NativeWindow(ANativeWindow_fromSurface(env, surface)));
#else
	f.setWindow(new NativeWindow(ANativeWindow_fromSurface(env, surface)));
#endif
 }

JNIEXPORT void JNICALL Java_ffmpeg_playVideo_count_1frame_1start
  (JNIEnv *, jobject, jlong addrRGB, jlong addrGray){
  
    LOGI("jni count frame start!!!");
    Mat& display_RGB_Mat = *(Mat*)addrRGB;
#ifdef net
	//cn.m_display_RGB_Mat = display_RGB_Mat;
	cn.processOneFrame(display_RGB_Mat);
#else
	//f.m_display_RGB_Mat = display_RGB_Mat;
	f.processOneFrame(display_RGB_Mat);
#endif
}


JNIEXPORT void JNICALL Java_ffmpeg_playVideo_setAllparamer
  (JNIEnv *env, jobject instance, jstring prototxt, jstring model, jstring path_, jobject surface){

   // 记录结果
   #ifdef net
      int result;
      const char *cprototxt = env->GetStringUTFChars(prototxt, 0);
      const char *cmodel = env->GetStringUTFChars(model,0);
	  cn.initCaffe((char *)cprototxt, (char *)cmodel);
  #endif
}

#endif

