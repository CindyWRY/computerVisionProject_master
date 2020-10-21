//
// Created by wangry on 2019/10/8.
//

#ifndef ANDROID_COMPUTERVISIONPROJECT_MASTER_INDEXOFDISTANCE_H
#define ANDROID_COMPUTERVISIONPROJECT_MASTER_INDEXOFDISTANCE_H
//每行中的距离最小值类
class IndexOfDistance{
private:
	int m_row;
	int m_col;
	

public:
	int m_distance;
	IndexOfDistance();
	~IndexOfDistance();
	IndexOfDistance & operator= (const IndexOfDistance & other);
	friend bool operator<(const IndexOfDistance & obj1, const IndexOfDistance & obj2);
	int getRow();
	int getCol();
	int getDistance();

	void setRow(int row);
	void setCol(int col);
	void setDistance(int dis);
	
};

#endif //ANDROID_COMPUTERVISIONPROJECT_MASTER_INDEXOFDISTANCE_H
