#pragma once
#include "kCell.h"

class kRoom : public kBasicEntity
{
public:
	kRoom(void);
	~kRoom(void);

	hkvVec3 startPoint, endPoint; //Easier for map building

	/***************************
	Returns the smallest x, y, z
	****************************/
	hkvVec3 TopLeft();
	/***************************
	Returns the Biggest x, y, z
	****************************/
	hkvVec3 BottomRight();
	/*****************************
	Returns the center of the area
	******************************/
	hkvVec3 Center();

	int Width();
	int Height();

	void addConnectors(int connectors, int z);
	void OffSet(int X, int Y);

	void Build(int normal_textures, int object_meshes, float meshSize);

	hkvVec3 RC1; //x,y coords ; z is type 0,1,2,3 for N,E,S,W(simplier for texturing)
	hkvVec3 RC2; //x,y coords ; z is type 0,1,2,3 for N,E,S,W(simplier for texturing)

private:	
	V_DECLARE_SERIAL_DLLEXP ( kRoom, SAMPLEPLUGIN_IMPEXP );
	V_DECLARE_VARTABLE ( kRoom, SAMPLEPLUGIN_IMPEXP );
	void BuildConnector(int x, int y, int side, float meshSize);
	bool CheckForContinue(int x, int y, int Cx, int Cy, int Cz);
};

