//
// Created by wangry on 2019/11/20.
//
#include "count_base.h"

count_base::count_base(int maxDisappeared, int maxdistance,int way_ , 
					int skip_frame_, float confidences_,
					float x0_1_, float y0_1_ , float a0_1_ ,
					float b0_1_, int num_ , float line_up_x_,
					float line_down_y_ )
:m_centroidTracker(maxDisappeared, maxdistance),
m_way(way_),
m_skip_frame(skip_frame_ ),
m_confidences(confidences_),
m_x0_1(x0_1_),
m_y0_1(y0_1_),				  //The ordinate of the center point of region_count
m_a0_1(a0_1_),				  //Long semi-axis of the area
m_b0_1(b0_1_),				  //Short half shaft of the area
m_num(num_),				  //The number of sides of the polygon,if  m_num=4,it is rectangle.If  m_num>30,it is a Round.
m_line_up_x(line_up_x_), 	  //line_up of line_count
m_line_down_y(line_down_y_),
m_totalFrames(0),
m_out (0),
m_in (0),
m_totalDown(0),
m_totalUp(0)
{}

count_base::~count_base(){
	if (pdisplay != nullptr){
		delete pdisplay;
	   pdisplay = nullptr;
	}
	
	if (m_cprototxt != nullptr){
		delete m_cprototxt;
	   m_cprototxt = nullptr;
	}
	
	if (m_cmodel != nullptr){
		delete m_cmodel;
	   m_cmodel = nullptr;
	}
}

void count_base::setWindow(displayWindow* display){
	pdisplay = display;

}
void count_base::initCaffe(const char *cprototxt, const char *cmodel) {
	m_cprototxt = cprototxt;
	m_cmodel = cmodel;
}

int count_base::getOutNum(){
	return m_out;
}
int count_base::getInNum(){
	return m_in;
}
int count_base::getTotalDownNum(){
	return m_totalDown;
}
int count_base::getTotalUpNum(){
	return m_totalUp;
}


