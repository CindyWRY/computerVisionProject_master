//
// Created by wangry on 2019/9/9.
//

#include "CentroidTracker.h"
#include "common.h"

#define  LOG_TAG    "CentroidTracker"

CentroidTracker::CentroidTracker(int maxDisappeared, int maxDistance) 
:m_maxDisappeared(maxDisappeared),
 m_maxDistance(maxDistance),
 m_nextObjectID(0),
 m_disappeared(), 
 m_unUsedRows(),
 m_unUsedCols(),
 m_objects()
{
    
}

CentroidTracker::~CentroidTracker(){

}

void CentroidTracker::Register(centroid objcentroid){
	m_objects[m_nextObjectID] = objcentroid;
	m_disappeared[m_nextObjectID] = 0;
	m_nextObjectID += 1;
}

void CentroidTracker::deregister(int objectID){
	//m_objects.erase(objectID);
	//m_disappeared.erase(objectID);
	auto search2 = m_objects.find(objectID);
    if (search2 != m_objects.end()){
		m_objects.erase(search2);
	}
	auto search1 = m_disappeared.find(objectID);
	if(search1 != m_disappeared.end()){
		 m_disappeared.erase(search1);
	}
}



int CentroidTracker::distance(centroid &objectCentroid, centroid &inputCentroid){
	int dis = sqrt((objectCentroid.getX()-inputCentroid.getX())*
					(objectCentroid.getX() -inputCentroid.getX())+
					(objectCentroid.getY()-inputCentroid.getY())*
					(objectCentroid.getY()-inputCentroid.getY())
				  );
	return dis;

}
void CentroidTracker::setIdex(int row,int col){	
	m_unUsedRows.insert(row);
	m_unUsedCols.insert(col);
}
void CentroidTracker::setIdex(std::map<int, centroid> &inputCentroids){
	for(int j=0; j<inputCentroids.size(); j++){
		m_unUsedCols.insert(j);
	}
	
	for(auto itobj = m_objects.begin(); itobj != m_objects.end(); ++itobj){
		m_unUsedRows.insert(itobj->first);
	}
}

void CentroidTracker::Dimendistance(std::map<int, centroid> &inputCentroids,std::vector<IndexOfDistance> & in_IndexOfDistance){
	for(auto itobj = m_objects.begin(); itobj != m_objects.end(); ++itobj){
		IndexOfDistance objctDis;
		int minDistanceInLine=60000;
		int dis =0;
		for(int j=0; j<inputCentroids.size(); j++){
			dis = distance(m_objects[itobj->first], inputCentroids[j]);
			if(dis < minDistanceInLine){
				minDistanceInLine = dis;
				objctDis.setRow(itobj->first);
				objctDis.setCol(j);
				objctDis.setDistance(minDistanceInLine);
			}
			//��������set ����
		}
		//LOGI("in_IndexOfDistance.push_back objctDis  col:%d", objctDis.getCol());
	    //LOGI("in_IndexOfDistance.push_back objctDis  Row:%d", objctDis.getRow());
		//LOGI("in_IndexOfDistance.push_back objctDis  Distance:%d", objctDis.getDistance());
		in_IndexOfDistance.push_back(objctDis);
	}
}
bool CentroidTracker::findIdex(IndexOfDistance &objDis,std::set<int> &usedRows, std::set<int> &usedCols){
	bool find = false;
	int col = objDis.getCol();
	int row = objDis.getRow();
	
	auto searchRow = usedRows.find(row);
	auto searchCol = usedCols.find(col);
	if((searchCol != usedCols.end())||
		(searchRow != usedRows.end())
	){
		find = true;
	}
	else {
        find = false;
    }
	return find;
}
void CentroidTracker::deleteUsed(int row , int col){
	auto search = m_unUsedRows.find(row);    
	if( search != m_unUsedRows.end()) m_unUsedRows.erase(search);    
	search = m_unUsedCols.find(col);    
	if( search != m_unUsedCols.end()) m_unUsedCols.erase(search);
}
void CentroidTracker::deleteObjects(){
	m_objects.erase(m_objects.begin(), m_objects.end());
}

struct Less
{
    bool operator()(const IndexOfDistance& obj1, const IndexOfDistance& obj2)
    {
        return  obj1.m_distance < obj2.m_distance; //��С��������
    }
};

void CentroidTracker::update(std::vector<dlib::drectangle> rects){//ÿһ֡�������
   // 更新m_objects;
   int objectID = 0;
   int disappeare=0;
  // ����ǰrectsΪ��,���Ѵ��ڵ�ObjcetID�е�disappeared��ʧ֡����������
  // ,���ж��Ƿ��Ѿ����������ʧ֡��?����ɾ����
   if (rects.size()==0){
		for (auto it = m_disappeared.begin(); it != m_disappeared.end();){
			objectID = it->first;
			disappeare = it->second;
			m_disappeared[objectID] += 1;
			if (m_disappeared[objectID] > m_maxDisappeared){
				it = m_disappeared.erase(it);
				auto search = m_objects.find(objectID);
    			if (search != m_objects.end()){
					m_objects.erase(search);
				}
			}
			else{
				++it;
			}
		}
		return;     
   }
   
   std::map<int, centroid> inputCentroids;
   int i=0;
   for (auto itr = rects.cbegin(); itr != rects.cend(); itr++){
		dlib::drectangle rect = (*itr);
		double startX = rect.left();
		double startY = rect.top();
		double endX = rect.right();
		double endY = rect.bottom();

		int cX = (int)(startX + endX) / 2.0;
		int	cY = (int)(startY + endY) / 2.0;

		centroid obj(cX,cY);
		inputCentroids[i] = obj;
		i++;
   }
   //�жϵ�ǰ�Ƿ����ע�����,��û��,�����inputCentroidsע���¶���
   if(m_objects.size()==0){
	   for(auto itput = inputCentroids.begin(); itput != inputCentroids.end(); itput++){
		   Register(inputCentroids[itput->first]);
	   }
   }else{
	   //�����ڸ��ٶ���,���û������ĸ����㷨��������(����ŷʽ����,���������ԭ����µ�ǰ�������������?
		// ��ȡ���и��ٶ����objectID�Լ���������ֵ
		std::set<int> usedRows;
	    std::set<int> usedCols;
		IndexOfDistance obj;
	    int rowNum = m_objects.size();
	    int colNum = inputCentroids.size();
		
		
		//����õ�ÿ�е���С����?
		 std::vector<IndexOfDistance> in_IndexOfDistance;
		Dimendistance(inputCentroids, in_IndexOfDistance);
		std::sort(in_IndexOfDistance.begin(), in_IndexOfDistance.end(), Less());
		//�������и�������������
		setIdex(inputCentroids);
		for (auto it = in_IndexOfDistance.cbegin(); it != in_IndexOfDistance.cend(); it++){
			obj = (*it);
			if (findIdex(obj, usedRows, usedCols)){
				continue;
			}
			if (obj.getDistance() > m_maxDistance){
				continue;
			}
			// ���º󽫵�ǰ�������������usedRows,usedCols��
			m_objects[obj.getRow()]= inputCentroids[obj.getCol()];
			m_disappeared[obj.getRow()]=0;
			usedRows.insert(obj.getRow());
			usedCols.insert(obj.getCol());
			deleteUsed(obj.getRow(), obj.getCol());
		}
		//LOGI("row Num :%d",rowNum);
		//LOGI("col num :%d", colNum);
		//LOGI("m_unUsedRows size:%d", m_unUsedRows.size());
		//LOGI("m_unUsedCols size:%d", m_unUsedCols.size());
		if (rowNum >= colNum){
			for (auto it = m_unUsedRows.begin(); it != m_unUsedRows.end(); it++){
				m_disappeared[*it] += 1;
				if(m_disappeared[*it] > m_maxDisappeared)
				deregister(*it);
			}
		}else{
			for (auto it = m_unUsedCols.begin(); it != m_unUsedCols.end(); it++){
				Register(inputCentroids[*it]);
			}
		}
   }
   m_unUsedCols.clear();
   m_unUsedRows.clear();
}


