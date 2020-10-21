//
// Created by wangry on 2019/10/8.
//
#include "IndexOfDistance.h"

IndexOfDistance::IndexOfDistance()
:m_row(0),
 m_col(0),
 m_distance(0)
{
}
IndexOfDistance & IndexOfDistance::operator= (const IndexOfDistance & other){
	if(this != &other){
		m_row = other.m_row;
		m_col =other.m_col;
		m_distance = other.m_distance;
	}
	return *this;
}

IndexOfDistance::~IndexOfDistance(){

}

int IndexOfDistance::getRow(){
	return m_row;
}

int IndexOfDistance::getCol(){
	return m_col;
}

int IndexOfDistance::getDistance(){
	return m_distance;
}

void IndexOfDistance::setRow(int row){
	m_row = row;
}
void IndexOfDistance::setCol(int col){
	m_col = col;
}

void IndexOfDistance::setDistance(int dis){
	m_distance = dis;
}

bool operator<(const IndexOfDistance& obj1, const IndexOfDistance& obj2)
{
    return obj1.m_distance < obj2.m_distance; //从小到大排序
}

