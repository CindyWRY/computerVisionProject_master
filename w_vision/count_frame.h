//
// Created by wangry on 2019/10/24.
//

#ifndef ANDROID_COMPUTERVISIONPROJECT_MASTER_COUNT_FRAME_H
#define ANDROID_COMPUTERVISIONPROJECT_MASTER_COUNT_FRAME_H

#include<list>
#include <vector>
#include <iostream>
#include "count_base.h"


class count_frame : public count_base
{
private:
		
	
	Ptr<BackgroundSubtractorKNN> m_knn;
	cv::Mat rgb;
	void deleteUnexsitedObject();
	void tracking();
	void frame_detect();
	void counting();
	
public:
	
	cv::Mat m_display_RGB_Mat;
	count_frame(int m_maxDisappeared=11, int maxdistance =75,int way_ = 1, 
					int skip_frame_= 10, float confidences_= 0.4,
					float x0_1_= 0.5, float y0_1_= 0.65, float a0_1_= 0.2,
					float b0_1_= 0.1, int num_= 30, float line_up_x_= 0.4,
					float line_down_y_= 0.6
					);
	virtual ~count_frame();
	virtual void processOneFrame(cv::Mat& img);
};
#endif //ANDROID_COMPUTERVISIONPROJECT_MASTER_COUNT_FRAME_H
