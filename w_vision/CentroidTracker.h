//
// Created by wangry on 2019/9/9.
//

#ifndef ANDROID_COMPUTERVISIONPROJECT_MASTER_CENTROIDTRACKER_H
#define ANDROID_COMPUTERVISIONPROJECT_MASTER_CENTROIDTRACKER_H

#include <iostream>
#include <map>
#include <string>
#include "centroid.h"
#include "IndexOfDistance.h"
#include <dlib/image_processing.h>
#include <cmath>
#include <set>
#include <vector>

using namespace std;

class CentroidTracker{

private:
	
    int m_nextObjectID;
	int m_maxDisappeared;
    int m_maxDistance;
    std::map<int,int> m_disappeared;//对象ID 和消失的次数
	std::set<int> m_unUsedRows;
	std::set<int> m_unUsedCols;
	void deregister(int objectID);
    void Register(centroid objcentroid);
	void setIdex(int i, int j);
	void setIdex(std::map<int, centroid> &inputCentroids);
	bool findIdex(IndexOfDistance &objDis,std::set<int> &usedRows, std::set<int> &usedCols);
	void Dimendistance(std::map<int, centroid> &inputCentroids, std::vector<IndexOfDistance> & IndexOfDistance);
	int distance(centroid &objectCentroid, centroid &inputCentroid);
	void deleteUsed(int row , int col);
	
 public:
    std::map<int, centroid> m_objects;
    CentroidTracker(int maxDisappeared=11, int maxDistance=75);
    ~CentroidTracker();
    void update(std::vector<dlib::drectangle> rects);
	void deleteObjects();
 };
#endif //ANDROID_COMPUTERVISIONPROJECT_MASTER_CENTROIDTRACKER_H
