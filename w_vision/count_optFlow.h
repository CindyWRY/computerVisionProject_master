//
// Created by wangry on 2019/11/14.
//

#ifndef ANDROID_COMPUTERVISIONPROJECT_MASTER_COUNT_OPTFLOW_H
#define ANDROID_COMPUTERVISIONPROJECT_MASTER_COUNT_OPTFLOW_H
#include <cstdlib>
#include<opencv2\dnn\dnn.hpp>
#include<opencv2\core.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include "opencv2/video.hpp"

#include <vector>
#include "displayWindow.h"
#include "count_base.h"

using namespace cv;
using namespace cv::dnn;
using namespace std;

class count_optFlow: public count_base
{
private:
	dnn::Net m_net;

	
	std::vector<Point2f> good_new;
	std::vector<Point2f> m_P0;
	std::vector<Point2f> m_P;
	std::vector<Point> pts;
	std::vector<Point2f> m_P1;
	std::vector<Scalar> colors;
	cv::Mat m_old_gray;
	cv::Mat m_frame_gray;
	cv::Mat m_frame_mask;
	bool m_Inited;
	void tracking();
	void counting();
	int caffe_detect();
	void setEnd();
public:
	
	cv::Mat m_display_RGB_Mat;
	
	count_optFlow(int way_ = 1, int skip_frame_= 10, float confidences_= 0.4,
				float x0_1_= 0.5, float y0_1_= 0.65, float a0_1_= 0.2,
				float b0_1_= 0.1, int num_= 30, float line_up_x_= 0.4,
				float line_down_y_= 0.6
				);
	
	virtual ~count_optFlow();
	virtual void processOneFrame(cv::Mat& img);
};

#endif //ANDROID_COMPUTERVISIONPROJECT_MASTER_COUNT_OPTFLOW_H
