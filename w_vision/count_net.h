//
// Created by wangry on 2019/11/5.
//

#ifndef ANDROID_COMPUTERVISIONPROJECT_MASTER_COUNT_NET_H
#define ANDROID_COMPUTERVISIONPROJECT_MASTER_COUNT_NET_H


#include <cstdlib>
#include<opencv2\dnn\dnn.hpp>

#include <list>
#include <vector>
#include <iostream>
#include <iomanip>

#include "CentroidTracker.h"
#include "TrackableObject.h"
#include "count_base.h"

using namespace cv::dnn;

#define LOG_TAG "count_net"

class count_net: public count_base
{
private:
	dnn::Net m_net;
	void tracking();
	void deleteUnexsitedObject();
	void counting();
	int caffe_detect();
	
public:
	
	cv::Mat m_display_RGB_Mat;
	dlib::array2d<unsigned char> m_dlib_img;
	count_net(int maxDisappeared=11, int maxdistance =75,int way_ = 1, 
				int skip_frame_= 10, float confidences_= 0.4,
				float x0_1_= 0.5, float y0_1_= 0.65, float a0_1_= 0.2,
				float b0_1_= 0.1, int num_= 30, float line_up_x_= 0.4,
				float line_down_y_= 0.6
				);
	
	virtual ~count_net();
	virtual void processOneFrame(cv::Mat& img);
};
#endif //ANDROID_COMPUTERVISIONPROJECT_MASTER_COUNT_NET_H
