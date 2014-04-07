#pragma once
#include "KaosuProjectPluginPCH.h"

class kAiPath
{
public:
	kAiPath(void);
	~kAiPath(void);

	void SetPath(SerialDynArray_cl<hkvVec3> m_pts_on_path);

	void GetClosestPoint(int &indexPoint, hkvVec3 fromPoint, hkvVec3 &closestPoint);
	hkvVec3 GetNextPoint();
	hkvVec3 GetCurrentPoint();
	hkvVec3 GetPreviousPoint();
	hkvVec3 GetAt(const int i);
protected:
	int m_current_point;
	SerialDynArray_cl<hkvVec3> m_pts_on_path;
};

