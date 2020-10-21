//
// Created by wangry on 2019/10/29.
//



#ifndef ANDROID_COMPUTERVISIONPROJECT_MASTER_NATIVEWINDOW_H
#define ANDROID_COMPUTERVISIONPROJECT_MASTER_NATIVEWINDOW_H

#include "platform.h"
#include "displayWindow.h"

#ifdef  WIN_PLATFORM
#include <opencv2/highgui/highgui.hpp>
class NativeWindow :public displayWindow
{	
public:
	NativeWindow();
	virtual~NativeWindow();
	virtual void displayMat(cv::Mat& display_RGB_Mat,cv::Mat& img= *(cv::Mat*)nullptr);
};
#else
#include <android/native_window.h>
// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>



class NativeWindow :public displayWindow
{
public:
	NativeWindow(ANativeWindow* native_window);
	~NativeWindow();
	void SetNativeWindow(ANativeWindow* native_window);
	virtual void displayMat(cv::Mat& display_RGB_Mat,cv::Mat& img= *(cv::Mat*)nullptr);

private:
	ANativeWindow* m_native_window;
};

#endif //ANDROID_COMPUTERVISIONPROJECT_MASTER_NATIVEWINDOW_H
#endif
