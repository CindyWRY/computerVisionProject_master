
#include "count_frame.h"
#include "common.h"
#include "myPoint.h"

#define  LOG_TAG    "count_frame"
count_frame::count_frame(int maxDisappeared, int maxdistance, int way_, 
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
 		)
{
	m_knn = createBackgroundSubtractorKNN();
}

count_frame::~count_frame(){}

void count_frame::tracking(){
    for(auto it=m_trackers.cbegin(); it != m_trackers.cend(); it++){
        dlib::correlation_tracker tracker = (*it);
        //keep track
        tracker.update(m_dlib_img);
        dlib::drectangle rect = tracker.get_position();
		if(rect.is_empty())continue;
        double startX = rect.left();
        double startY = rect.top();
        double endX = rect.right();
        double endY = rect.bottom();

		//LOGI("Tracking() m_trackers size =%d", m_trackers.size());
		//LOGI("Tracking, startX =%d,startY=%d, endX=%d, endY=%d",startX,startY, endX, endY);
        m_rects.push_back(rect);
    }
}

void count_frame::frame_detect(){
	
	Mat kernelCl = (Mat_<float>(5,5) <<
            -0.125, -0.125, -0.125, -0.125, -0.125,
            -0.125,   0.25,   0.25,   0.25, -0.125,
            -0.125,   0.25,    1,     0.25, -0.125,
            -0.125,   0.25,   0.25,   0.25, -0.125,
            -0.125, -0.125, -0.125, -0.125, -0.125);
	
	Mat kernelOp = (Mat_<float>(3,3) <<
			1,1,1,
			1,1,1,
			1,1,1);
	
	flip(m_display_RGB_Mat, m_display_RGB_Mat, 1); //1代表水平方向旋转180度

	//resize(m_display_RGB_Mat, m_display_RGB_Mat, Size(resize_width, resize_height), (0, 0), (0, 0), INTER_LINEAR);

	//该函数对图像应用任意线性滤波器图像变得清晰
	filter2D(m_display_RGB_Mat, m_display_RGB_Mat, -1, kernelCl);//源锟斤拷锟?

	Mat copy_frame1 = m_display_RGB_Mat.clone();

	Mat copy_frame2;
	//高斯低通滤波，使得图像模糊,为什么要变得模糊澹?
	GaussianBlur(copy_frame1, copy_frame2, Size(5, 5), 0);

	//Mat rgb;

	cvtColor(copy_frame2, rgb, CV_BGR2RGB);
	
	int W = rgb.cols;
	int H = rgb.rows;
	int frameArea = W * H;
	int areaTH = frameArea / 500;

	//delete rects	
	m_rects.erase(m_rects.begin(), m_rects.end());
	
	int line_up = (int) m_line_up_x * H;
    int line_down = (int) m_line_down_y * H;

	if (m_totalFrames % m_skip_frame == 0){
		//delete trackers
		m_trackers.clear();

		Mat fgmask,imBin2,mask2,mask3;
		vector<vector<Point>> contours;
		//计算前景掩码，包含 前景的白色值 以及 阴影的灰色值
		m_knn->apply(rgb, fgmask);

		//前景区域二值化，将非白色（0-244）的非前景区域（包含背景以及阴影）均设为0， 
		//前景的白色（200-255）设置为255
		threshold(fgmask, imBin2, 200, 255, THRESH_BINARY);

		// 前景区域形态学处理，开运算用来消除小物体，在纤细点处分离物体  
		//平滑较大物体的边界不明显改变其面积
		morphologyEx(imBin2, mask2, MORPH_OPEN, kernelOp);
         //排除小型黑色区域
		morphologyEx(mask2, mask3, MORPH_CLOSE, kernelCl);

		//获取轮廓
    	findContours(mask3, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		Rect rect;
	    for (size_t i = 0; i < contours.size(); i++){
			double perimeter = arcLength(Mat(contours[i]), true);
			double area = contourArea(contours[i]); 

			if (area > areaTH && perimeter > 20){
				Rect rect = boundingRect(Mat(contours[i])); 
				int x1 = rect.x;
				int y1 = rect.y;
				int x2 = rect.x + rect.width;
				int y2 = rect.y + rect.height;

				//add Tracker
			    dlib::correlation_tracker tracker;
			    //LOGI("addTrackers, left =%d,top=%d, right=%d, botton=%d",x1, y1, x2, y2);
			    tracker.start_track(m_dlib_img, dlib::drectangle(x1, y1, x2, y2));
			    m_trackers.emplace_back(tracker);
		    }
		}
	}
	else{
		//Tracking start
		tracking();
	}
}

void count_frame::counting(){
	// region counting
	int W = rgb.cols;
	int H = rgb.rows;
	float x0 =  m_x0_1 * W; //区域计数中心点的横坐标
    float y0 =  m_y0_1 * H; //区域计数中心点的纵坐标
    float a0 =  m_a0_1 * W; //区域的长半轴
    float b0 =  m_b0_1 * W; //区域的短半轴
    float angle = 360/ m_num ; //每条边对应角的度数

	vector<Point> pts;
	vector<Point2f> m;
	vector<Point2f> n;
	
	//循环角的个数，存入区域的点坐标
	for (int i=0; i < (360/angle); i++){
		 float a = x0 + a0 * cos((angle * i * 3.14) / 180);
         float b = y0 + b0 * sin((angle * i * 3.14) / 180);
         m.push_back(Point2f(a, b));
         n.push_back(Point2f(a, b));
		 pts.push_back(Point((int)a,(int)b));
	}
	if ( m_way == 1){
    	polylines(rgb, pts, true, Scalar(0, 255, 0), 2);
	}else{
		line(rgb, Point(0, rgb.rows/2), Point(rgb.cols, rgb.rows/2), Scalar(0, 255, 0), 2, CV_AA);//鍒掔嚎
	}
	//objects update rects
	m_centroidTracker.update(m_rects);
	int line = rgb.rows/2;
	for ( auto it = m_centroidTracker.m_objects.begin(); it != m_centroidTracker.m_objects.end(); ++it) 
	{
		int objID = it->first;
		centroid cent = it->second;

		TrackableObject search; 
		auto iterobj = m_trackableObjects.find(objID);
		if ( iterobj == m_trackableObjects.end())
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
			if( m_way == 1){
				int x= cent.getX();
				int meanx = search.getMean_centroids_x();

			 	myPoint point1(meanx, meany);//之前的点
                myPoint point0(x, y);//现在的点
                bool isInside0 = point0.InPolygon(m, true);
                bool isInside1 = point1.InPolygon(m, true); 

				//Counting algorithm
                if(!search.getCounted()){
					//现在点在里面，之前不在
                    if (isInside0 == true && isInside1 == false){ 
						m_in +=1;
						search.setCounted(true);
					}
				}
                else if (search.getCounted()){
					//现在点在里面，之前不在
					if (isInside0 == false && isInside1 == true){
						m_out += 1;
                        search.setCounted(false);
					}
				}
			}else{
				
				int direction = y - meany;
				
			//	LOGI("direction : %d",direction);
			//	LOGI("y : %d", y);
			//	LOGI("app.m_display_RGB_Mat.rows/2 : %d", m_display_RGB_Mat.rows/2);
				if(!search.getCounted()){
					if (direction < 0 && y < line && y > line - 50){
						m_totalUp += 1;
						search.setCounted(true);
					}
					else if (direction > 0 && y > line && y < line + 50){
						m_totalDown += 1;
		                search.setCounted(true);
					}
				}
			}
		}
		search.addCentroid(cent);
		m_trackableObjects[objID]= search; 
		deleteUnexsitedObject();
		std::string text = "P"+ std::to_string(objID);
		cv::putText(rgb, 
					text, 
					cv::Point(cent.getX()- 10, cent.getY() - 10),
					cv::FONT_HERSHEY_SIMPLEX, 
					0.5, 
					cv::Scalar(255, 0, 0),
					2);
		cv::rectangle(rgb,
					cv::Point(cent.getX()-10,cent.getY()-40),
					cv::Point(cent.getX()+20,cent.getY()+40),
					cv::Scalar(255, 0, 0),
					2,4);
	}
}

void count_frame::processOneFrame(cv::Mat& img){
	
	Mat gray;
	m_display_RGB_Mat= img;
	if (m_display_RGB_Mat.channels() == 3){
		cv::cvtColor(m_display_RGB_Mat, gray, cv::COLOR_RGB2GRAY);
	}
	dlib::assign_image(m_dlib_img, dlib::cv_image<unsigned char>(gray));
	
	frame_detect();

	counting();
	
	std::string totalDownStr;
	std::string totalUpStr;
	if ( m_way == 1){
		
		totalDownStr = "OUT: " + std::to_string(m_out);
		totalUpStr = "IN: " + std::to_string(m_in);
	}else{
		totalDownStr = "totalDown: " + std::to_string(m_totalDown);
		totalUpStr = "totalUp: " + std::to_string(m_totalUp);
	}

	cv::putText(rgb, 
				totalDownStr, 
				cv::Point(10, rgb.rows - ((0 * 20) + 20)),
				cv::FONT_HERSHEY_SIMPLEX,
				0.5,
				cv::Scalar(255,255, 0),
				2);
	cv::putText(rgb, 
				totalUpStr, 
				cv::Point(10, rgb.rows - ((1 * 20) + 20)),
				cv::FONT_HERSHEY_SIMPLEX,
				0.5,
				cv::Scalar(255,255, 0),
				2);
	if(pdisplay != nullptr)
		pdisplay->displayMat(rgb);
	m_totalFrames += 1;
}
	
	

//void count_frame::setWindow(displayWindow* display){
//	pdisplay = display;

//}


void count_frame::deleteUnexsitedObject(){
	
	for ( auto it = m_trackableObjects.begin(); it != m_trackableObjects.end();){
		int id = it->first;
		auto search = m_centroidTracker.m_objects.find(id);
		if (search == m_centroidTracker.m_objects.end()){//m_objects 里面没有这个对象了m_trackableObjects 中清除
			it = m_trackableObjects.erase(it);
		}else 
			++it;
	} 
}













