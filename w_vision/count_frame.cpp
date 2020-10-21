
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
	
	flip(m_display_RGB_Mat, m_display_RGB_Mat, 1); //1����ˮƽ������ת180��

	//resize(m_display_RGB_Mat, m_display_RGB_Mat, Size(resize_width, resize_height), (0, 0), (0, 0), INTER_LINEAR);

	//�ú�����ͼ��Ӧ�����������˲���ͼ��������
	filter2D(m_display_RGB_Mat, m_display_RGB_Mat, -1, kernelCl);//Դ���?

	Mat copy_frame1 = m_display_RGB_Mat.clone();

	Mat copy_frame2;
	//��˹��ͨ�˲���ʹ��ͼ��ģ��,ΪʲôҪ���ģ���?
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
		//����ǰ�����룬���� ǰ���İ�ɫֵ �Լ� ��Ӱ�Ļ�ɫֵ
		m_knn->apply(rgb, fgmask);

		//ǰ�������ֵ�������ǰ�ɫ��0-244���ķ�ǰ�����򣨰��������Լ���Ӱ������Ϊ0�� 
		//ǰ���İ�ɫ��200-255������Ϊ255
		threshold(fgmask, imBin2, 200, 255, THRESH_BINARY);

		// ǰ��������̬ѧ������������������С���壬����ϸ�㴦��������  
		//ƽ���ϴ�����ı߽粻���Ըı������
		morphologyEx(imBin2, mask2, MORPH_OPEN, kernelOp);
         //�ų�С�ͺ�ɫ����
		morphologyEx(mask2, mask3, MORPH_CLOSE, kernelCl);

		//��ȡ����
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
	float x0 =  m_x0_1 * W; //����������ĵ�ĺ�����
    float y0 =  m_y0_1 * H; //����������ĵ��������
    float a0 =  m_a0_1 * W; //����ĳ�����
    float b0 =  m_b0_1 * W; //����Ķ̰���
    float angle = 360/ m_num ; //ÿ���߶�Ӧ�ǵĶ���

	vector<Point> pts;
	vector<Point2f> m;
	vector<Point2f> n;
	
	//ѭ���ǵĸ�������������ĵ�����
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
		line(rgb, Point(0, rgb.rows/2), Point(rgb.cols, rgb.rows/2), Scalar(0, 255, 0), 2, CV_AA);//划线
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

			 	myPoint point1(meanx, meany);//֮ǰ�ĵ�
                myPoint point0(x, y);//���ڵĵ�
                bool isInside0 = point0.InPolygon(m, true);
                bool isInside1 = point1.InPolygon(m, true); 

				//Counting algorithm
                if(!search.getCounted()){
					//���ڵ������棬֮ǰ����
                    if (isInside0 == true && isInside1 == false){ 
						m_in +=1;
						search.setCounted(true);
					}
				}
                else if (search.getCounted()){
					//���ڵ������棬֮ǰ����
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
		if (search == m_centroidTracker.m_objects.end()){//m_objects ����û����������ˣmm_trackableObjects �����
			it = m_trackableObjects.erase(it);
		}else 
			++it;
	} 
}













