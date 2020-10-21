//
// Created by wangry on 2019/9/9.
//

#include "TrackableObject.h"
#include "common.h"
#define  LOG_TAG    "TrackableObject"

TrackableObject::TrackableObject(int objectID, centroid centroidobj){

    m_iobjectID = objectID;
	m_listcentroids.emplace_back(centroidobj);
	m_bcounted = false;
}
TrackableObject::~TrackableObject()
{

}

TrackableObject::TrackableObject()
	:m_listcentroids(),
	m_iobjectID(0),
	m_bcounted(false)
{
	
}


int TrackableObject::getMean_centroids_x(){
	int sum =0;
	for(auto it = m_listcentroids.begin(); it != m_listcentroids.end(); it++)
	{
		sum += (*it).getX();
	}
	int size = m_listcentroids.size();
	//LOGI("getMean_centroids_x sum :%d ",sum);
	//LOGI("getMean_centroids_x size :%d ",size);
	//LOGI("getMean_centroids_x :%d ",sum/size);
	return (sum/size);
}


int TrackableObject::getMean_centroids_y(){
	int sum =0;
	for(auto it = m_listcentroids.begin(); it != m_listcentroids.end(); it++)
	{
		sum += (*it).getY();
	}
	int size = m_listcentroids.size();
	//LOGI("getMean_centroids_y sum :%d ",sum);
	//LOGI("getMean_centroids_y size :%d ",size);
	//LOGI("getMean_centroids_y :%d ",sum/size);
	return (sum/size);
}

void TrackableObject::addCentroid(centroid centroidobj){
	m_listcentroids.emplace_back(centroidobj);
}

TrackableObject & TrackableObject::operator=( const TrackableObject & other ){
	if ( this != &other )
  {
     m_iobjectID = other.m_iobjectID;
	 m_listcentroids = other.m_listcentroids;
	 m_bcounted = other.m_bcounted;
  }

  return *this;

}

bool TrackableObject::getCounted(){
	return m_bcounted;
}
void TrackableObject::setCounted(bool b){
	m_bcounted = b;
}
