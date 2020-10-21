//
// Created by wangry on 2019/10/28.
//
#include "myPoint.h"

#define max(x,y)(x)>(y)?(x):(y)
#define min(x,y)(x)<(y)?(x):(y)

myPoint::myPoint(float x, float y)
:m_x(x),
 m_y(y)
{
}

myPoint::~myPoint()
{


}

bool myPoint::InPolygon(std::vector<cv::Point2f> &polygon, bool BoundCheck)
{
	bool inside = false;
	if (BoundCheck){
	   float minX = polygon[0].x;
	   float maxX = polygon[0].x;
	   float minY = polygon[0].y;
	   float maxY = polygon[0].y;
	   for (auto it = polygon.cbegin(); it != polygon.cend(); it++){
		   minX = min((*it).x, minX);
		   maxX = max((*it).x, maxX);
		   minY = min((*it).y, minY);
		   maxY = max((*it).y, maxY);
	   }
	   if (m_x < minX || m_x > maxX || m_y < minY || m_y > maxY)
		   return false;
	}
	int j = polygon.size()- 1;
	for (int i=0; i<polygon.size(); i++){
	   if ((polygon[i].y > m_y) != (polygon[j].y > m_y) &&(
			   m_x < (polygon[j].x - polygon[i].x) * (m_y - polygon[i].y) / (
			   polygon[j].y - polygon[i].y) + polygon[i].x))
		   inside = !inside;
	   j = i;
	}
	return inside;
}



