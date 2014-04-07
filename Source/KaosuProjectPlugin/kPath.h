#pragma once
#include "kCell.h"

class kPath
{
public:
	kPath(void);
	~kPath(void);

	SerialDynArray_cl<hkvVec3> paths;

	void addPoint(hkvVec2 path, int size);
	
	void Build(float meshSize);
};

