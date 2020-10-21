//
// Created by wangry on 2019/11/5.
//


#include <iostream>
#include"count_net.h"
#include "myPoint.h"
#include "common.h"


static std::string classNames[] = { "background",
			"aeroplane", "bicycle", "bird", "boat",
			"bottle", "bus", "car", "cat", "chair",
			"cow", "diningtable", "dog", "horse",
			"motorbike", "person", "pottedplant",
			"sheep", "sofa", "train", "tvmonitor" };


count_net::count_net(int maxDisappeared, int maxdistance, int way_, 
						int skip_frame_, float confidences_,
						float x0_1_, float y0_1_, float a0_1_,
						float b0_1_, int num_, float line_up_x_,
						float line_down_y_
						)
:
 count_base(maxDisappeared,maxdistance,way_, skip_frame_ ,confidences_,
			x0_1_, y0_1_, a0_1_,
			b0_1_,num_, line_up_x_, 
			line_down_y_
			){}


count_net::~count_net() {
	
}
	


int count_net::caffe_detect() {

	if (m_display_RGB_Mat.empty())
	{
		return -1;
	}
	m_net = readNetFromCaffe(m_cprototxt, m_cmodel);//Reads a network model stored in Caffe model in memory
	if (m_net.empty()) {
		return -2;
	}
	if (m_display_RGB_Mat.channels() == 4)
		cvtColor(m_display_RGB_Mat, m_display_RGB_Mat, COLOR_BGRA2BGR);

	int cols = m_display_RGB_Mat.cols;
	int rows = m_display_RGB_Mat.rows;

	Mat inputBlob = blobFromImage(m_display_RGB_Mat, 0.007843,
		Size(cols, rows), Scalar(127.5, 127.5, 127.5), false, false); //Convert Mat to batch of images

	m_net.setInput(inputBlob); //set the network input

	//compute output，这是一个4D数，rows and cols can only hold 2 dimensions, so they are not used here, and set to -1
	Mat detection = m_net.forward();   //! [Make forward pass]

	Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());//101*7矩阵

	for (int i = 0; i < detectionMat.rows; i++) {

		float confidence = detectionMat.at<float>(i, 2);      //第二列存放可信度
		int classId = (int)detectionMat.at<float>(i, 1);
		std::string className = classNames[classId];
		if (!(className == "person")) continue;

		//LOGI("confidence : %f",confidence);
		//LOGI("m_minConfidence : %f",m_minConfidence);
		if (confidence > m_confidences) {

			//存放人脸所在的图像中的位置信息
			int xLeftBottom = static_cast<int>(detectionMat.at<float>(i, 3) * m_display_RGB_Mat.cols);
			int yLeftBottom = static_cast<int>(detectionMat.at<float>(i, 4) * m_display_RGB_Mat.rows);
			int xRightTop = static_cast<int>(detectionMat.at<float>(i, 5) * m_display_RGB_Mat.cols);
			int yRightTop = static_cast<int>(detectionMat.at<float>(i, 6) * m_display_RGB_Mat.rows);

			dlib::correlation_tracker tracker;
			tracker.start_track(m_dlib_img, dlib::drectangle(xLeftBottom, yLeftBottom, xRightTop, yRightTop));
			m_trackers.emplace_back(tracker);
		}
	}
	return 0;
}

void count_net::tracking() {
	for (auto it = m_trackers.cbegin(); it != m_trackers.cend(); it++) {
		dlib::correlation_tracker tracker = (*it);
		//keep track
		tracker.update(m_dlib_img);
		dlib::drectangle rect = tracker.get_position();
		if (rect.is_empty())continue;
		double startX = rect.left();
		double startY = rect.top();
		double endX = rect.right();
		double endY = rect.bottom();

		//LOGI("Tracking() m_trackers size =%d", m_trackers.size());
		//LOGI("Tracking, startX =%d,startY=%d, endX=%d, endY=%d",startX,startY, endX, endY);
		m_rects.push_back(rect);
	}
}

void count_net::counting() {
	// region counting
	int W = m_display_RGB_Mat.cols;
	int H = m_display_RGB_Mat.rows;
	float x0 =  m_x0_1 * W; //区域计数中心点的横坐标
	float y0 =  m_y0_1 * H; //区域计数中心点的纵坐标
	float a0 =  m_a0_1 * W; //区域的长半轴
	float b0 =  m_b0_1 * W; //区域的短半轴
	float angle = 360 /  m_num; //每条边对应角的度数

	vector<Point> pts;
	vector<Point2f> m;
	vector<Point2f> n;

	//循环角的个数，存入区域的点坐标
	for (int i = 0; i < (360 / angle); i++) {
		float a = x0 + a0 * cos((angle * i * 3.14) / 180);
		float b = y0 + b0 * sin((angle * i * 3.14) / 180);
		m.push_back(Point2f(a, b));
		n.push_back(Point2f(a, b));
		pts.push_back(Point((int)a, (int)b));
	}
	if ( m_way == 1) {
		polylines(m_display_RGB_Mat, pts, true, Scalar(0, 255, 0), 2);
	}
	else {
		line(m_display_RGB_Mat, Point(0, m_display_RGB_Mat.rows / 2), Point(m_display_RGB_Mat.cols, m_display_RGB_Mat.rows / 2), Scalar(0, 255, 0), 2, CV_AA);//鍒掔嚎
	}
	//objects update rects
	m_centroidTracker.update(m_rects);
	int line = m_display_RGB_Mat.rows / 2;
	for (auto it = m_centroidTracker.m_objects.begin(); it != m_centroidTracker.m_objects.end(); ++it)
	{
		int objID = it->first;
		centroid cent = it->second;

		TrackableObject search;
		auto iterobj = m_trackableObjects.find(objID);
		if (iterobj == m_trackableObjects.end())
		{
			TrackableObject trackableobj(objID, cent);
			search = trackableobj;
		}
		else
		{
			//TrackableObjects
			search = iterobj->second;
			int y = cent.getY();
			int meany = search.getMean_centroids_y();
			if ( m_way == 1) {
				int x = cent.getX();
				int meanx = search.getMean_centroids_x();

				myPoint point1(meanx, meany);//之前的点
				myPoint point0(x, y);//现在的点
				bool isInside0 = point0.InPolygon(m, true);
				bool isInside1 = point1.InPolygon(m, true);

				//Counting algorithm
				if (!search.getCounted()) {
					//现在点在里面，之前不在
					if (isInside0 == true && isInside1 == false) {
						m_in += 1;
						search.setCounted(true);
					}
				}
				else if (search.getCounted()) {
					//现在点在里面，之前不在
					if (isInside0 == false && isInside1 == true) {
						m_out += 1;
						search.setCounted(false);
					}
				}
			}
			else {

				int direction = y - meany;

				//LOGI("direction : %d",direction);
				//LOGI("y : %d", y);
				//LOGI("app.m_display_RGB_Mat.rows/2 : %d", m_display_RGB_Mat.rows/2);
				if (!search.getCounted()) {
					if (direction < 0 && y < line && y > line - 50) {
						m_totalUp += 1;
						search.setCounted(true);
					}
					else if (direction > 0 && y > line && y < line + 50) {
						m_totalDown += 1;
						search.setCounted(true);
					}
				}
			}
		}
		search.addCentroid(cent);
		m_trackableObjects[objID] = search;

		deleteUnexsitedObject();

		std::string text = "P" + std::to_string(objID);
		cv::putText(m_display_RGB_Mat,
			text,
			cv::Point(cent.getX() - 10, cent.getY() - 10),
			cv::FONT_HERSHEY_SIMPLEX,
			0.5,
			cv::Scalar(255, 0, 0),
			2);
		cv::rectangle(m_display_RGB_Mat,
			cv::Point(cent.getX() - 10, cent.getY() - 40),
			cv::Point(cent.getX() + 20, cent.getY() + 40),
			cv::Scalar(255, 0, 0),
			2, 4);
	}
}

void count_net::deleteUnexsitedObject() {

	for (auto it = m_trackableObjects.begin(); it != m_trackableObjects.end();) {
		int id = it->first;
		auto search = m_centroidTracker.m_objects.find(id);
		if (search == m_centroidTracker.m_objects.end()) {//m_objects 里面没有这个对象了m_trackableObjects 中清除
			it = m_trackableObjects.erase(it);
		}
		else
			++it;
	}
}

void count_net::processOneFrame(cv::Mat& img) {

	Mat gray;
	m_display_RGB_Mat= img;
	flip(m_display_RGB_Mat, m_display_RGB_Mat, 1);
	if (m_display_RGB_Mat.channels() == 3) {
		cv::cvtColor(m_display_RGB_Mat, gray, cv::COLOR_RGB2GRAY);
	}
	dlib::assign_image(m_dlib_img, dlib::cv_image<unsigned char>(gray));
	//delete rects	
	m_rects.erase(m_rects.begin(), m_rects.end());
	if (m_totalFrames % m_skip_frame == 0) {
		//delete trackers
		m_trackers.clear();
		caffe_detect();
	}
	else {
		tracking();
	}
	counting();

	std::string totalDownStr;
	std::string totalUpStr;
	if ( m_way == 1) {

		totalDownStr = "OUT: " + std::to_string(m_out);
		totalUpStr = "IN: " + std::to_string(m_in);
	}
	else {
		totalDownStr = "totalDown: " + std::to_string(m_totalDown);
		totalUpStr = "totalUp: " + std::to_string(m_totalUp);
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
		cv::Point(10, m_display_RGB_Mat.rows - ((1 * 20) + 20)),
		cv::FONT_HERSHEY_SIMPLEX,
		0.5,
		cv::Scalar(0, 0, 255),
		2);
	if (pdisplay != nullptr)
		pdisplay->displayMat(m_display_RGB_Mat);
	m_totalFrames += 1;

}





