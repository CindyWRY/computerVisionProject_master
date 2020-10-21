//
// Created by wangry on 2019/11/14.
//
#include <cmath>
#include <iostream>
#include <string>
#include "myPoint.h"
#include "common.h"
#include "count_optFlow.h"


static std::string classNames[] = { "background",
			"aeroplane", "bicycle", "bird", "boat",
			"bottle", "bus", "car", "cat", "chair",
			"cow", "diningtable", "dog", "horse",
			"motorbike", "person", "pottedplant",
			"sheep", "sofa", "train", "tvmonitor" };


count_optFlow::count_optFlow( int way_, int skip_frame_, float confidences_,
									float x0_1_, float y0_1_, float a0_1_,
									float b0_1_, int num_, float line_up_x_,
									float line_down_y_
)
:m_Inited(false), 
count_base(0,0,way_, skip_frame_ ,confidences_,
 			x0_1_, y0_1_, a0_1_,
 			b0_1_,num_, line_up_x_, 
 			line_down_y_
 		)
{
	RNG rng;
	for (int i = 0; i < 100; i++)
	{
		int r = rng.uniform(0, 256);
		int g = rng.uniform(0, 256);
		int b = rng.uniform(0, 256);
		colors.push_back(Scalar(r, g, b));
	}

}


count_optFlow::~count_optFlow() {
	
}

int count_optFlow::caffe_detect() {

	if (m_display_RGB_Mat.empty()){
		return -1;
	}
	m_net = readNetFromCaffe(m_cprototxt, m_cmodel);
	if (m_net.empty()) {
		return -2;
	}

	if (m_display_RGB_Mat.channels() == 4)
		cvtColor(m_display_RGB_Mat, m_display_RGB_Mat, COLOR_BGRA2BGR);

	int cols = m_display_RGB_Mat.cols;
	int rows = m_display_RGB_Mat.rows;

	Mat inputBlob = blobFromImage(m_display_RGB_Mat, 0.007843,
		Size(cols, rows), Scalar(127.5, 127.5, 127.5), false, false);

	m_net.setInput(inputBlob); 
	Mat detection = m_net.forward();  

	Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

	for (int i = 0; i < detectionMat.rows; i++) {

		float confidence = detectionMat.at<float>(i, 2);    
		int classId = (int)detectionMat.at<float>(i, 1);
		std::string className = classNames[classId];
		if (!(className == "person")) 
			continue;
		if (confidence > m_confidences) {

			int xLeftBottom = static_cast<int>(detectionMat.at<float>(i, 3) * m_display_RGB_Mat.cols);
			int yLeftBottom = static_cast<int>(detectionMat.at<float>(i, 4) * m_display_RGB_Mat.rows);
			int xRightTop = static_cast<int>(detectionMat.at<float>(i, 5) * m_display_RGB_Mat.cols);
			int yRightTop = static_cast<int>(detectionMat.at<float>(i, 6) * m_display_RGB_Mat.rows);

			float cx = (xLeftBottom + xRightTop)/2.0;
			float cy = (yLeftBottom+yRightTop)/2.0;

			m_P.emplace_back(Point2f(cx,cy));
		}
	}
	m_P0 = m_P;
	return 0;
}


//typedef unsigned char uchar;

void count_optFlow::tracking() {
	
	vector<uchar> status;
    vector<float> err;
	Size winSize(15,15);
	
	TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS,10,0.03);

	if(m_P0.empty())
		return;
	
	cvtColor(m_display_RGB_Mat,m_frame_gray, CV_BGR2GRAY);

	if (m_old_gray.empty())
		m_frame_gray.copyTo(m_old_gray);
	
	m_P1.clear();
	cv::calcOpticalFlowPyrLK(m_old_gray, m_frame_gray, 
		                 m_P0,  m_P1,
		                 status, err, winSize, 2, termcrit);
						 					 
	//Draw track
	good_new.clear();
	for (int i = 0; i< m_P1.size(); i++) {
		if( status[i] == 1 ){
			good_new.emplace_back(m_P1[i]);
			line(m_frame_mask,  m_P1[i], m_P0[i], colors[i], 2);
			circle(m_display_RGB_Mat,  m_P1[i], 5, colors[i], 2);
		}         
	}
	
}

void count_optFlow::counting() {
	// region counting
	int W = m_display_RGB_Mat.cols;
	int H = m_display_RGB_Mat.rows;
	float x0 =  m_x0_1 * W; 
	float y0 =  m_y0_1 * H; 
	float a0 =  m_a0_1 * W; 
	float b0 =  m_b0_1 * W; 
	float angle = 360 /  m_num; 

	vector<Point2f> m;
	vector<Point2f> n;

	
	for (int i = 0; i < (360 / angle); i++) {
		float a = x0 + a0 * cos((angle * i * 3.14) / 180);
		float b = y0 + b0 * sin((angle * i * 3.14) / 180);
		m.push_back(Point2f(a, b));
		n.push_back(Point2f(a, b));
		pts.push_back(Point((int)a, (int)b));
	}

	if ( m_way == 1) {

		for (int i=0; i< m_P0.size(); i++) {
			myPoint point1(m_P0[i].x, m_P0[i].y);
			myPoint point0(m_P1[i].x, m_P1[i].y);
			bool isInside0 = point0.InPolygon(m, true);
			bool isInside1 = point1.InPolygon(m, true);

			
			if (isInside0 == true && isInside1 == false) {
				m_in += 1;
				continue;
			}
			
			if (isInside0 == false && isInside1 == true) {
				m_out += 1;
				continue;
			}
		}
	}
	else {

		int line_up = int( m_line_up_x * H);
		int	line_down = int( m_line_down_y * H);
		 for(int i=0; i< m_P0.size(); i++){
			 if( m_P0[i].y > line_up && m_P1[i].y < line_up && fabs(m_P0[i].y - m_P1[i].y) < 50){
				 m_totalUp += 1;
				 cv::line(m_display_RGB_Mat, Point(0, line_up), Point(W, line_up), Scalar(0, 255, 0), 5, CV_AA);
				 continue;
			 }
			 if (m_P0[i].y < line_down &&  m_P1[i].y > line_down && fabs(m_P0[i].y - m_P1[i].y) < 50){
				 m_totalDown += 1;
				 cv::line(m_display_RGB_Mat, Point(0, line_down), Point(W, line_down), Scalar(0, 0, 255), 5, CV_AA);
				 continue;
			  }
		 }

	}
	
}


void count_optFlow::processOneFrame(cv::Mat& img) {
	std::string totalDownStr;
	std::string totalUpStr;
	m_display_RGB_Mat= img;
	m_P.clear();
	flip(m_display_RGB_Mat, m_display_RGB_Mat, 1);

	if(!m_Inited){
		m_frame_mask = Mat::zeros(m_display_RGB_Mat.size(), m_display_RGB_Mat.type());
		m_Inited = true;
	}
	if (m_totalFrames % m_skip_frame == 0) {
		caffe_detect();
	}
	if (!m_P0.empty()) {		
		tracking();
		counting();
	}
	
	if ( m_way == 1) {
		totalDownStr = "OUT: " + std::to_string(m_out);
		totalUpStr = "IN: " + std::to_string(m_in);
		polylines(m_display_RGB_Mat, pts, true, Scalar(0, 255, 0), 2);
	}
	else {
		totalDownStr = "totalDown: " + std::to_string(m_totalDown);
		totalUpStr = "totalUp: " + std::to_string(m_totalUp);
		int W = m_display_RGB_Mat.cols;
		int H = m_display_RGB_Mat.rows;
		int line_up = int( m_line_up_x * H);
		int line_down = int( m_line_down_y * H);
		cv::line(m_display_RGB_Mat, Point(0, line_up), Point(W, line_up), Scalar(255, 0, 0), 2, CV_AA);
		cv::line(m_display_RGB_Mat, Point(0, line_down), Point(W, line_down), Scalar(255, 0, 0), 2, CV_AA);

	}
	cv::putText(m_display_RGB_Mat,
		totalDownStr,
		cv::Point(10, m_display_RGB_Mat.rows - ((0 * 20) + 20)),
		cv::FONT_HERSHEY_SIMPLEX,
		0.5,
		cv::Scalar(0, 0, 255),
		2);
	cv::putText(m_display_RGB_Mat,
		totalUpStr,
		cv::Point(10,m_display_RGB_Mat.rows - ((1 * 20) + 20)),
		cv::FONT_HERSHEY_SIMPLEX,
		0.5,
		cv::Scalar(0, 0, 255),
		2);
	setEnd();
}

void count_optFlow::setEnd() {
	if (pdisplay != nullptr){
		pdisplay->displayMat(m_display_RGB_Mat,m_frame_mask);
	}
	m_totalFrames += 1;
	m_old_gray = m_frame_gray.clone();
	m_P0.clear();
	m_P0 = good_new;
}





