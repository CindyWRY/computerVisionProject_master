//
// Created by wangry on 2019/10/29.
//

#ifndef ANDROID_COMPUTERVISIONPROJECT_MASTER_DISPLAYWINDOW_H
#define ANDROID_COMPUTERVISIONPROJECT_MASTER_DISPLAYWINDOW_H
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
//#include "common.h"

class displayWindow{
public:
	displayWindow();
	virtual~displayWindow();
	virtual void displayMat(cv::Mat& display_RGB_Mat, cv::Mat& img = *(cv::Mat*)nullptr)=0;
};

#endif //ANDROID_COMPUTERVISIONPROJECT_MASTER_DISPLAYWINDOW_H
