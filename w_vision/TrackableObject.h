//
// Created by wangry on 2019/9/9.
//

#ifndef ANDROID_COMPUTERVISIONPROJECT_MASTER_TRACKABLEOBJECT_H
#define ANDROID_COMPUTERVISIONPROJECT_MASTER_TRACKABLEOBJECT_H
#include<list>
#include "centroid.h"

using namespace std;

class TrackableObject{
public:
	TrackableObject();
    TrackableObject(int objectID, centroid centroid);
	~TrackableObject();
	TrackableObject & operator=( const TrackableObject & other );
	int getMean_centroids_y();
	int getMean_centroids_x();
	void addCentroid(centroid obj);
	bool getCounted();
	void setCounted(bool b);
private:
		int m_iobjectID;
		list<centroid> m_listcentroids;//一个可跟踪对象可以有很多个中心点
		bool m_bcounted;
};
#endif //ANDROID_COMPUTERVISIONPROJECT_MASTER_TRACKABLEOBJECT_H
