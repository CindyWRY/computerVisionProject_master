//
// Created by wangry on 2019/9/16.
//

#ifndef ANDROID_COMPUTERVISIONPROJECT_MASTER_CENTROID_H
#define ANDROID_COMPUTERVISIONPROJECT_MASTER_CENTROID_H

class centroid{
private:
    int m_x;
    int m_y;
public:
    centroid(int x,int y);
	centroid();
    ~centroid();
    int getX();
    int getY();
	void setX(int x);
	void setY(int y);
	centroid & operator=( const centroid & other );
};
#endif //ANDROID_COMPUTERVISIONPROJECT_MASTER_CENTROID_H
