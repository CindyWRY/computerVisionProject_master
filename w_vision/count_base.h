//
// Created by wangry on 2019/11/20.
//

#ifndef ANDROID_COMPUTERVISIONPROJECT_MASTER_COUNT_BASE_H
#define ANDROID_COMPUTERVISIONPROJECT_MASTER_COUNT_BASE_H
#include "opencv2/core.hpp"
#include "opencv2/core/ocl.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/video.hpp"
//dlib
#include <dlib/image_io.h>
#include <dlib/dir_nav.h>
#include <dlib/opencv.h>
#include <dlib/image_processing.h>
#include <list>
#include <vector>
#include <iostream>
#include <iomanip>

#include "CentroidTracker.h"
#include "TrackableObject.h"


#include "displayWindow.h"

using namespace std;
using namespace cv;

class count_base{
public:
	
	int  m_way;
	int m_skip_frame;
	float m_confidences;
	float  m_x0_1;
	float  m_y0_1;
	float  m_a0_1;
	float  m_b0_1;
	int  m_num;
	float  m_line_up_x;
	float  m_line_down_y;
	int m_totalFrames;
	int m_out;
	int m_in;
	int m_totalDown;
    int m_totalUp;
	
	displayWindow* pdisplay;
	const char *m_cprototxt;
	const char *m_cmodel;

	list<dlib::correlation_tracker>  m_trackers;
  	std::vector<dlib::drectangle> m_rects;
  	CentroidTracker m_centroidTracker;
    std::map<int, TrackableObject> m_trackableObjects;
	dlib::array2d<unsigned char> m_dlib_img;
public:
	count_base(int m_maxDisappeared, int maxdistance,int way_, 
					int skip_frame_, float confidences_,
					float x0_1_, float y0_1_, float a0_1_,
					float b0_1_, int num_, float line_up_x_,
					float line_down_y_);
	virtual ~count_base();
	virtual void processOneFrame(cv::Mat& img)=0;
	void initCaffe(const char *cprototxt, const char *cmodel);
	void setWindow(displayWindow* pdisplay);
	int getOutNum();
	int getInNum();
	int getTotalDownNum();
	int getTotalUpNum();
												
};


#endif //ANDROID_COMPUTERVISIONPROJECT_MASTER_COUNT_BASE_H
