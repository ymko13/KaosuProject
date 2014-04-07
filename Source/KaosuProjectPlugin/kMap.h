#pragma once
#include "KaosuProjectPluginModule.h"
#include <Vision/Runtime/Base/String/VString.hpp> 

class kMap  : public kBasicEntity
{
public:
	kMap(void);
	kMap(int rooms, int room_size_min, int room_size_max);
	~kMap(void);
	///Amount of Normal Textures, Road Textures, Object Textures
	void SetSettings(int NormalTextures, int RoadTextures, int ObjectMeshes);
	void Generate(int seed);

	///Provides info on what Generate is currently doing
	VString currentTask;

	hkvVec3 m_enemySpawn;
	hkvVec3 m_friendlySpawn;
	
	void InitializeProperties() HKV_OVERRIDE;
	void PostInitialize() HKV_OVERRIDE;
	void InitMap();
	void ClearMap();
	static int minX, minY, maxX, maxY;
	static SerialDynArray_cl< hkvVec3 > m_full_path;
private:	
	V_DECLARE_SERIAL_DLLEXP ( kMap, SAMPLEPLUGIN_IMPEXP );
	V_DECLARE_VARTABLE ( kMap, SAMPLEPLUGIN_IMPEXP );
	/*map specs*/
	int seed;

	int total_rooms;
	int room_min;
	int room_max;
	int outer_ring;

	int xoff;
	int yoff;
	float vMeshSize;

	int normal_textures;
	int road_textures;
	int object_meshes;
	/*map specs*/

	static DynObjArray_cl< VSmartPtr < kRoom > > rRooms;

	VSmartPtr<kRoom> GenerateRoom();
	kPath CreatePath(VSmartPtr<kRoom> from, VSmartPtr<kRoom> to, int toCN, int fromCN);

	static const VSmartPtr<kCell> addCell(int tType, int tId, int objId, int x, int y, int objectPos, float meshSize);
	static const int getRandom(int min, int max);
	static const hkvVec3 getRandomPoint(float vMeshSize);
	static const int PrintToConsole(VString classfrom, VString text);
	
	void CheckCloseCells(int x, int y, int size);

	/*Old Data*/
	int xC1, yC1, xC2, yC2;
	VBool buildRoom, buildPath;
	
	bool WasBuilt;
};

