//
// Created by wangry on 2019/9/16.
//
#include "centroid.h"
centroid::centroid(int x,int y){
	m_x = x;
	m_y = y;
}
centroid::centroid()
:m_x(0),
 m_y(0)
{

}

centroid::~centroid(){


}

int centroid::getX(){
	return m_x;
}

int centroid::getY(){

	return m_y;
}
void centroid::setX(int x){
	 m_x =x;
}

void centroid::setY(int y){
	m_y = y;
}

centroid & centroid::operator=( const centroid & other ){
  if ( this != &other )
  {	 
  	 m_x = other.m_x;
	 m_y = other.m_y;
	 
  }
  return *this;
}

