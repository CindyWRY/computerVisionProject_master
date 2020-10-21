//
// Created by wangry on 2019/10/28.
//

#ifndef ANDROID_COMPUTERVISIONPROJECT_MASTER_MYPOINT_H
#define ANDROID_COMPUTERVISIONPROJECT_MASTER_MYPOINT_H
#include "opencv2/core.hpp"
#include "opencv2/core/ocl.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video.hpp"
#include <vector>

class myPoint{
private:
	float m_x;
	float m_y;

public:
	myPoint(float x, float y);
	~myPoint();
	float get_x();	
	float get_y();
	bool InPolygon(std::vector<cv::Point2f>& vecPoint, bool b);
};
#endif //ANDROID_COMPUTERVISIONPROJECT_MASTER_MYPOINT_H
