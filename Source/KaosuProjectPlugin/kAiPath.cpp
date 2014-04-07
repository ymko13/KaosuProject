#include "KaosuProjectPluginPCH.h"
#include "kAiPath.h"


kAiPath::kAiPath(void)
{
}


kAiPath::~kAiPath(void)
{
}

void kAiPath::SetPath(SerialDynArray_cl<hkvVec3> m_Pts)
{
	m_pts_on_path = m_Pts;
	m_current_point = m_pts_on_path.GetValidSize() - 1;
}

void kAiPath::GetClosestPoint(int &indexPoint, hkvVec3 fromPoint, hkvVec3 &closestPoint)
{
	indexPoint = m_current_point;
	closestPoint = m_pts_on_path[m_current_point];
	int distance = (int)closestPoint.getDistanceToSquared(fromPoint);

	for(int i = 0; i < (int)m_pts_on_path.GetValidSize(); i++)
	{
		int d = (int)m_pts_on_path[i].getDistanceToSquared(fromPoint);
		if(d < distance)
		{
			closestPoint = m_pts_on_path[i];
			indexPoint = i;
			distance = d;
		}
	}
}

hkvVec3 kAiPath::GetNextPoint()
{
	m_current_point = hkvMath::Max(0, m_current_point - 1);
	return m_pts_on_path.Get(m_current_point);
}

hkvVec3 kAiPath::GetCurrentPoint()
{
	return m_pts_on_path.Get(m_current_point);
}

hkvVec3 kAiPath::GetPreviousPoint()
{
	m_current_point++;
	m_current_point = hkvMath::Min(m_current_point, (int)m_pts_on_path.GetValidSize() - 1);
	return m_pts_on_path.Get(m_current_point);
}

hkvVec3 kAiPath::GetAt(const int i)
{
	return m_pts_on_path[i];
}