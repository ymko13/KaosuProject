#include "KaosuProjectPluginPCH.h"
#include "kMap.h"

V_IMPLEMENT_SERIAL ( kMap, kBasicEntity, 0, &g_myComponentModule );

START_VAR_TABLE( kMap, kBasicEntity, "Thy Map", 0, "")
	DEFINE_VAR_VSTRING_AND_NAME(kMap, currentTask, "Current Task", "State's current task of Generate method", "Doing nothing", 50, 0, 0);

	DEFINE_VAR_INT_AND_NAME(kMap, seed, "Generation seed", "Seed used to generate the object(Level Number)", "0", 0, 0);
	
	DEFINE_VAR_FLOAT_AND_NAME(kMap, m_enemySpawn.x, "Enemy Spawn Point X", "X Spawn Point", "0", 0, 0);
	DEFINE_VAR_FLOAT_AND_NAME(kMap, m_enemySpawn.y, "Enemy Spawn Point Y", "Y Spawn Point", "0", 0, 0);
	DEFINE_VAR_FLOAT_AND_NAME(kMap, m_enemySpawn.z, "Enemy Spawn Point Z", "Z Spawn Point", "0", 0, 0);

	DEFINE_VAR_FLOAT_AND_NAME(kMap, m_friendlySpawn.x, "Friendly Spawn Point X", "X Spawn Point", "0", 0, 0);
	DEFINE_VAR_FLOAT_AND_NAME(kMap, m_friendlySpawn.y, "Friendly Spawn Point Y", "Y Spawn Point", "0", 0, 0);
	DEFINE_VAR_FLOAT_AND_NAME(kMap, m_friendlySpawn.z, "Friendly Spawn Point Z", "Z Spawn Point", "0", 0, 0);

	DEFINE_VAR_INT_AND_NAME(kMap, total_rooms, "Total Rooms", "The amount of total rooms", "2", 0, 0);
	DEFINE_VAR_INT_AND_NAME(kMap, room_min, "Minimum room size", "The smallest possible room", "4", 0, 0);
	DEFINE_VAR_INT_AND_NAME(kMap, room_max, "Maximum room size", "The biggest possible room", "15", 0, 0);
	DEFINE_VAR_INT_AND_NAME(kMap, outer_ring, "Size of outer ring of the map", "Size of map coat", "1", 0, 0);
	
	DEFINE_VAR_FLOAT_AND_NAME(kMap, vMeshSize, "vMesh Size", "The size of the vMesh in float", "100.1", 0, 0);
	DEFINE_VAR_INT_AND_NAME(kMap, xoff, "X Space Between Rooms", "X SBR", "9", 0, 0);
	DEFINE_VAR_INT_AND_NAME(kMap, yoff, "Y Space Between Rooms", "Y SBR", "9", 0, 0);
	DEFINE_VAR_INT_AND_NAME(kMap, normal_textures, "Total Normal Textures", ".", "1", 0, 0);
	DEFINE_VAR_INT_AND_NAME(kMap, road_textures, "Total Road Textures", ".", "1", 0, 0);
	DEFINE_VAR_INT_AND_NAME(kMap, object_meshes, "Total Object Meshes", ".", "0", 0, 0);

	DEFINE_VAR_INT_AND_NAME(kMap, xC1, "xC1", ".", "9", 0, 0);
	DEFINE_VAR_INT_AND_NAME(kMap, yC1, "yC1", ".", "1", 0, 0);
	DEFINE_VAR_INT_AND_NAME(kMap, xC2, "xC2", ".", "1", 0, 0);
	DEFINE_VAR_INT_AND_NAME(kMap, yC2, "yC2", ".", "0", 0, 0);
	DEFINE_VAR_BOOL_AND_NAME(kMap, buildRoom, "Build Room", ".", "FALSE", 0,0);
	DEFINE_VAR_BOOL_AND_NAME(kMap, buildPath, "Build Path", ".", "FALSE", 0,0);
END_VAR_TABLE

int kMap::minX = 0, kMap::minY = 0, kMap::maxX = 0, kMap::maxY = 0;
SerialDynArray_cl<hkvVec3> kMap::m_full_path = SerialDynArray_cl<hkvVec3>(0);
DynObjArray_cl< VSmartPtr< kRoom > > kMap::rRooms = DynObjArray_cl< VSmartPtr < kRoom > >(0);

void kMap::InitializeProperties()
{
	kBasicEntity::InitializeProperties();
}

//Place the object, create the objects etc
void kMap::PostInitialize()
{
}

void kMap::InitMap()
{
	this->Generate(seed);
	if(outer_ring > 0)
	{
		for(int x = kMap::minX; x <= kMap::maxX; x++)
		{
			for(int y = kMap::minY; y <= kMap::maxY; y++)
			{
				VString key;
				key.FormatEx("C_%d_%d", x, y);
				VSmartPtr<kCell> eCell = (kCell *) Vision::Game.SearchEntity(key);
				if(eCell.m_pPtr != NULL)
				{
					CheckCloseCells(x, y, outer_ring);
				}
			}
		}
	}
	WasBuilt = true;
}

void kMap::ClearMap()
{
	if(WasBuilt)
	{
		for(int x = kMap::minX; x <= kMap::maxX; x++)
		{
			for(int y = kMap::minY; y <= kMap::maxY; y++)
			{
				VString key;
				key.FormatEx("C_%d_%d", x, y);
				VSmartPtr<kCell> eCell = (kCell *) Vision::Game.SearchEntity(key);
				if(eCell.m_pPtr != NULL)
				{
					eCell->Dispose();
				}
				else
				{
					key.FormatEx("C_%d_%d_SUPL", x, y);
					VSmartPtr<kCell> eCell1 = (kCell *) Vision::Game.SearchEntity(key);
					if(eCell.m_pPtr != NULL)
					{
						eCell1->Dispose();
					}
				}
			}
		}
		WasBuilt = false;
	}
}

//Send x-1 and y-1
void kMap::CheckCloseCells(int x, int y, int size)
{
	int sX = x - size, sY = y - size;
	int limX = x + size, limY = y + size;
	for(x = sX; x <= limX; x++)
	{
		for(y = sY; y <= limY; y++)
		{
			VString key;
			key.FormatEx("C_%d_%d", x, y);
			VSmartPtr<kCell> eCell = (kCell *) Vision::Game.SearchEntity(key);
			key.FormatEx("C_%d_%d_SUPL", x, y);
			VSmartPtr<kCell> eCell1 = (kCell *) Vision::Game.SearchEntity(key);
			if(eCell.m_pPtr == NULL && eCell1 == NULL)
			{
				int oPos = 5;
				kMap::addCell(0, 0, -132, x, y, oPos, vMeshSize);
			}
		}
	}
}

kMap::kMap(void)
{
	WasBuilt = false;
}

kMap::kMap(int rooms, int room_size_min, int room_size_max)
{
	this->total_rooms = ((rooms > 0) ? rooms : 1);
	this->room_min = ((room_size_min > 4) ? room_size_min : 4);
	this->room_max = ((room_size_max < 15) ? room_size_max : 15);
}

kMap::~kMap(void)
{
}

void kMap::SetSettings(int normal, int road, int objects)
{
	this->normal_textures = normal;
	this->road_textures = road;
	this->object_meshes = objects;
}

void kMap::Generate(int seed)
{
	bool hadProblems = false;
	int rndMax = this->total_rooms * 4;
	srand(seed);
	DynObjArray_cl<VSmartPtr<kRoom> > bRooms = DynObjArray_cl<VSmartPtr<kRoom> >(0);
	VSmartPtr<kRoom> EnemyRoom = GenerateRoom();
	VSmartPtr<kRoom> FriendlyRoom = GenerateRoom();
	hkvVec2 largest;
	//Step 1: Generate some random rooms
	currentTask = "Generating Random Rooms";
	for(int i = 0; i < rndMax; i++)
	{
		bRooms[i] = GenerateRoom();
	}
	//Step 2: Pick out the biggest rooms and make them main bases
	currentTask = "Picking out enemy and friendly room";
	EnemyRoom = bRooms[0];
	FriendlyRoom = bRooms[1];
	float eRArea = EnemyRoom->endPoint.x * EnemyRoom->endPoint.y;
	float fRArea = FriendlyRoom->endPoint.x * FriendlyRoom->endPoint.y;
	for(int i = 2; i < rndMax; i++)
	{
		float cRArea = bRooms[i]->endPoint.x * bRooms[i]->endPoint.y;
		if(cRArea > eRArea)
		{
			if(eRArea > fRArea)
			{
				FriendlyRoom = EnemyRoom;
				fRArea = eRArea;
			}
			EnemyRoom = bRooms[i];
			eRArea = cRArea;
		}
	}

	rRooms[rRooms.GetFreePos()] = FriendlyRoom;
	rRooms[rRooms.GetFreePos()] = EnemyRoom;

	bRooms.Remove(bRooms.GetElementPos(FriendlyRoom));
	bRooms.Remove(bRooms.GetElementPos(EnemyRoom));

	//Step 3: Add Room Connection point to each room
	FriendlyRoom->addConnectors(kMap::getRandom(0,4), -1);
	EnemyRoom->addConnectors(kMap::getRandom(0,4), -1);

	//Step 4:Select a random room, add a path to it then select another room
	currentTask = "Picking out other rooms and connecting them";
	VSmartPtr<kRoom> frokRoom = FriendlyRoom;
	frokRoom->Build(normal_textures,object_meshes, vMeshSize);
	int OffSetX = 0, OffSetY = 0;
	int fC = 1, tC = 1;
	for(int i = 0; i < total_rooms - 2; i++)
	{
		VSmartPtr<kRoom> room;
		int rnd = 0;
		while(room.m_pPtr == NULL)
		{
			rnd = kMap::getRandom(0, rndMax);
			room = bRooms[rnd];
		}
		rRooms[rRooms.GetFreePos()] = room;

		int Con1 = kMap::getRandom(0,4);
		int Con2 = kMap::getRandom(0,4);
		Con2 = (Con1 == Con2) ? (Con2 + 1) % 4 : Con2;

		room->addConnectors(Con1, Con2);

		OffSetX = (int)frokRoom->endPoint.x + xoff;
		OffSetY = (int)frokRoom->endPoint.y + yoff;
		room->OffSet(OffSetX, OffSetY);

		tC = (Con2 > Con1) ? 1 : 2;

		CreatePath(frokRoom, room, fC, tC).Build(vMeshSize);
		room->Build(normal_textures,object_meshes, vMeshSize);

		frokRoom = room;
		fC = (tC == 1) ? 2 : 1;
		bRooms.Remove(rnd);
	}
	
	OffSetX = (int)frokRoom->endPoint.x + xoff;
	OffSetY = (int)frokRoom->endPoint.y + yoff;
	EnemyRoom->OffSet(OffSetX, OffSetY);
	CreatePath(frokRoom, EnemyRoom, fC, 1).Build(vMeshSize);
	EnemyRoom->Build(normal_textures,object_meshes, vMeshSize);
	m_enemySpawn = EnemyRoom->Center();
	m_friendlySpawn = FriendlyRoom->Center();
	rRooms.GetValidSize();
	currentTask = "Finished Generating";
}

VSmartPtr<kRoom> kMap::GenerateRoom()
{
	hkvVec3 from(0);
	int dif = this->room_max - this->room_min;
	hkvVec3 to((float)(this->room_min + kMap::getRandom(1,dif)),(float)(this->room_min + kMap::getRandom(1,dif)),0);
	VSmartPtr<kRoom> m = (kRoom *) Vision::Game.CreateEntity("kRoom", from);
	m->startPoint = from;
	m->endPoint = to;
	return m;
}

kPath kMap::CreatePath(VSmartPtr<kRoom> from, VSmartPtr<kRoom> to, int fromCN, int toCN)
{
	kPath path = kPath();
	
	hkvVec3 fromCon;
	hkvVec3 toCon;

	if(toCN == 1)
		toCon = to->RC1;
	else
		toCon = to->RC2;

	if(fromCN == 1)
		fromCon = from->RC1;
	else
		fromCon = from->RC2;

	int fromZ = (int)fromCon.z;
	int toZ = (int)toCon.z;

	if(fromZ > toZ)
	{
		hkvVec3 tmp = toCon;
		toCon = fromCon;
		fromCon = tmp;
	}
		
	int xZ = (int)fromCon.z, yZ = (int)toCon.z;

	path.addPoint(hkvVec2(fromCon.x, fromCon.y), 3);
	
	int min = 0, max = 0;
	hkvVec2 currentPoint(0);

	int m = kMap::getRandom(0,2);

	if(xZ == 0 && yZ == 0) // Checked
	{
		currentPoint = hkvVec2(fromCon.x, fromCon.y - (float)kMap::getRandom(6, xoff / 2 + 2));
		path.addPoint(currentPoint, 3);
		if(m == 0)
		{
			currentPoint = hkvVec2(from->endPoint.x + (float)kMap::getRandom(6, xoff / 2 + 2), currentPoint.y);
			path.addPoint(currentPoint, 3);
			currentPoint = hkvVec2(currentPoint.x, toCon.y - (float)kMap::getRandom(6, xoff / 2 + 2));
			path.addPoint(currentPoint, 3);
		}
		currentPoint = hkvVec2(toCon.x, currentPoint.y);
		path.addPoint(currentPoint, 3);
	}
	else if(xZ == 0 && yZ == 1)  // Checked
	{
		currentPoint = hkvVec2(fromCon.x, fromCon.y - (float)kMap::getRandom(6, xoff / 2 + 2));
		path.addPoint(currentPoint, 3);
		currentPoint = hkvVec2(toCon.x - (float)kMap::getRandom(6, xoff / 2 + 2), currentPoint.y);
		path.addPoint(currentPoint, 3);
		currentPoint = hkvVec2(currentPoint.x, toCon.y);
		path.addPoint(currentPoint, 3);
	}
	else if(xZ == 0 && yZ == 2) // Checked
	{
		currentPoint = hkvVec2(fromCon.x, fromCon.y - (float)kMap::getRandom(8, xoff / 2 + 3));
		path.addPoint(currentPoint, 3);		
		float f = (from->endPoint.x - to->startPoint.x);
		currentPoint = hkvVec2(from->endPoint.x - f / 2, currentPoint.y);
		path.addPoint(currentPoint, 3);
		currentPoint = hkvVec2(currentPoint.x, toCon.y + (float)kMap::getRandom(8, xoff / 2 + 3));
		path.addPoint(currentPoint, 3);
		currentPoint = hkvVec2(toCon.x, currentPoint.y);
		path.addPoint(currentPoint, 3);
	}
	else if(xZ == 0 && yZ == 3) // Checked
	{
		currentPoint = hkvVec2(fromCon.x, fromCon.y - (float)kMap::getRandom(6, xoff / 2 + 5));
		path.addPoint(currentPoint, 3);	
		currentPoint = hkvVec2(toCon.x - (float)kMap::getRandom(6, xoff / 2 + 5), currentPoint.y);
		path.addPoint(currentPoint, 3);
		currentPoint = hkvVec2(currentPoint.x, toCon.y);
		path.addPoint(currentPoint, 3);
	}
	else if(xZ == 1 && yZ == 1) // Checked
	{
		currentPoint = hkvVec2(fromCon.x - (float)kMap::getRandom(6, xoff / 2 + 3), fromCon.y);
		path.addPoint(currentPoint, 3);	
		currentPoint = hkvVec2(currentPoint.x, from->startPoint.y + (float)kMap::getRandom(6, xoff / 2 + 3));
		path.addPoint(currentPoint, 3);
		currentPoint = hkvVec2(toCon.x - (float)kMap::getRandom(6, xoff / 2 + 3), currentPoint.y);
		path.addPoint(currentPoint, 3);
		currentPoint = hkvVec2(currentPoint.x, toCon.y);
		path.addPoint(currentPoint, 3);
	}
	else if(xZ == 1 && yZ == 2) // Checked
	{
		currentPoint = hkvVec2(fromCon.x - (float)kMap::getRandom(6, xoff / 2 + 3), fromCon.y);
		path.addPoint(currentPoint, 3);	
		currentPoint = hkvVec2(currentPoint.x, toCon.y + (float)kMap::getRandom(6, xoff / 2 + 3));
		path.addPoint(currentPoint, 3);
		currentPoint = hkvVec2(toCon.x, currentPoint.y);
		path.addPoint(currentPoint, 3);
	}
	else if(xZ == 1 && yZ == 3) // Checked
	{
		currentPoint = hkvVec2(fromCon.x - (float)kMap::getRandom(8, xoff / 2 + 3), fromCon.y);
		path.addPoint(currentPoint, 3);
		float f = (from->endPoint.y - to->startPoint.y);
		currentPoint = hkvVec2(currentPoint.x, from->endPoint.y - f / 2);
		path.addPoint(currentPoint, 3);	
		currentPoint = hkvVec2(toCon.x + (float)kMap::getRandom(8, xoff / 2 + 3), currentPoint.y);
		path.addPoint(currentPoint, 3);	
		currentPoint = hkvVec2(currentPoint.x, toCon.y);
		path.addPoint(currentPoint, 3);

	}
	else if(xZ == 2 && yZ == 2)
	{
		currentPoint = hkvVec2(fromCon.x, fromCon.y + (float)kMap::getRandom(6, xoff / 2 + 3));
		path.addPoint(currentPoint, 3);	
		currentPoint = hkvVec2(to->startPoint.x - (float)kMap::getRandom(4, 8), currentPoint.y);
		path.addPoint(currentPoint, 3);	
		currentPoint = hkvVec2(currentPoint.x, toCon.y + (float)kMap::getRandom(6, xoff / 2 + 3));
		path.addPoint(currentPoint, 3);	
		currentPoint = hkvVec2(toCon.x, currentPoint.y);
		path.addPoint(currentPoint, 3);	
	}
	else if(xZ == 2 && yZ == 3)
	{
		currentPoint = hkvVec2(fromCon.x, fromCon.y + (float)kMap::getRandom(6, xoff / 2 + 3));
		path.addPoint(currentPoint, 3);	
		currentPoint = hkvVec2(toCon.x + (float)kMap::getRandom(4, 8), currentPoint.y);
		path.addPoint(currentPoint, 3);	
		currentPoint = hkvVec2(currentPoint.x, toCon.y);
		path.addPoint(currentPoint, 3);
	}
	else if(xZ == 3 && yZ == 3)
	{
		currentPoint = hkvVec2(fromCon.x + (float)kMap::getRandom(6, xoff / 2 + 3), fromCon.y);
		path.addPoint(currentPoint, 3);	
		currentPoint = hkvVec2(currentPoint.x, from->endPoint.y + (float)kMap::getRandom(6, xoff / 2 + 3));
		path.addPoint(currentPoint, 3);
		currentPoint = hkvVec2(toCon.x + (float)kMap::getRandom(6, xoff / 2 + 3), currentPoint.y);
		path.addPoint(currentPoint, 3);
		currentPoint = hkvVec2(currentPoint.x, toCon.y);
		path.addPoint(currentPoint, 3);
	}
	
	if(xZ != -1 && yZ != -1)
	{
		currentPoint = hkvVec2(toCon.x, toCon.y);
		path.addPoint(currentPoint, 3);
	}

	return path;
}

const VSmartPtr<kCell> kMap::addCell(int tType, int tId, int objId, int x, int y, int oPos, float meshSize)
{
	hkvVec3 pos((float)x * meshSize,(float)y * meshSize, 0);
	VSmartPtr<kCell> s = (kCell *) Vision::Game.CreateEntity("kCell", pos);

	if(oPos != 5)
	{
		if(kMap::minX > x)
			kMap::minX = x;
		if(kMap::minY > y)
			kMap::minY = y;
		if(kMap::maxX < x)
			kMap::maxX = x;
		if(kMap::maxY < y)
			kMap::maxY = y;
	}

	s->x = x;
	s->y = y;

	VString key;

	switch(tType)
	{
	case 0:
		switch(tId)
		{
		case 0:
			s->SetMesh("Meshes/Cells/Cell00.vmesh");
			break;
		case 1:
			s->SetMesh("Meshes/Cells/Cell01.vmesh");
			break;
		case 2:
			s->SetMesh("Meshes/Cells/Cell02.vmesh");
			break;
		case 3:
			s->SetMesh("Meshes/Cells/Cell03.vmesh");
			break;
		default:
			s->SetMesh("Meshes/Cells/Cell00.vmesh");
			break;
		}
		break;
	case 1:
		switch(tId)
		{
		case 0:
			s->SetMesh("Meshes/Cells/Cell10.vmesh");
			break;
		case 1:
			s->SetMesh("Meshes/Cells/Cell11.vmesh");
			break;
		case 2:
			s->SetMesh("Meshes/Cells/Cell12.vmesh");
			break;
		case 3:
			s->SetMesh("Meshes/Cells/Cell13.vmesh");
			break;
		case 4:
			s->SetMesh("Meshes/Cells/Cell14.vmesh");
			break;
		case 5:
			s->SetMesh("Meshes/Cells/Cell15.vmesh");
			break;
		case 6:
			s->SetMesh("Meshes/Cells/Cell16.vmesh");
			break;
		case 7:
			s->SetMesh("Meshes/Cells/Cell17.vmesh");
			break;
		case 8:
			s->SetMesh("Meshes/Cells/Cell18.vmesh");
			break;
		case 9:
			s->SetMesh("Meshes/Cells/Cell19.vmesh");
			break;
		case 10:
			s->SetMesh("Meshes/Cells/Cell110.vmesh");
			break;
		case 11:
			s->SetMesh("Meshes/Cells/Cell111.vmesh");
			break;
		case 12:
			s->SetMesh("Meshes/Cells/Cell112.vmesh");
			break;
		}
		break;
	}

	SerialDynArray_cl<hkvVec2> treePos = SerialDynArray_cl<hkvVec2>(6);
	SerialDynArray_cl<hkvVec3> objPos = SerialDynArray_cl<hkvVec3>(10);
	
	for(int i = 0; i < (int)treePos.GetSize(); i ++)
	{
		if(i < (int)treePos.GetSize())
			treePos[i] = hkvVec2(-1000.f);
		objPos[i] = hkvVec3(-1000.f);
	}
	
	float fThird = meshSize / 18 * 5;
	float sCell = 12.f;
	float eCell = 27.f;
	
	int makeGrassUpTo = 4;

	switch(oPos)
	{
	case -3://-3 For no grass
		makeGrassUpTo = 0;
		break;
	case -2://-3 For no grass
		makeGrassUpTo = kMap::getRandom(1,4);
		break;
	case -1://-3 For no grass
		makeGrassUpTo = kMap::getRandom(3,7);
		break;
	case 0: //On Grass
		makeGrassUpTo = kMap::getRandom(1,9);
		break;
	case 1:
		treePos[0] = hkvVec2(x * meshSize + fThird, y * meshSize + fThird * (float)kMap::getRandom(1,4) - sCell);
		treePos[1] = hkvVec2(x * meshSize + fThird * 2.f, y * meshSize + fThird * (float)kMap::getRandom(1,4) - sCell);
		treePos[2] = hkvVec2(x * meshSize + fThird * 3.f, y * meshSize + fThird * (float)kMap::getRandom(1,4) - sCell);
		break;
	case 2:
		treePos[0] = hkvVec2(x * meshSize + fThird * (float)kMap::getRandom(0,3) + eCell, y * meshSize + fThird);
		treePos[1] = hkvVec2(x * meshSize + fThird * (float)kMap::getRandom(0,3) + eCell, y * meshSize + fThird * 2.f);
		treePos[2] = hkvVec2(x * meshSize + fThird * (float)kMap::getRandom(0,3) + eCell, y * meshSize + fThird * 3.f);
		break;
	case 3:
		treePos[0] = hkvVec2(x * meshSize + fThird, y * meshSize + fThird * (float)kMap::getRandom(0,3) + eCell);
		treePos[1] = hkvVec2(x * meshSize + fThird * 2.f, y * meshSize + fThird * (float)kMap::getRandom(0,3) + eCell);
		treePos[2] = hkvVec2(x * meshSize + fThird * 3.f, y * meshSize + fThird * (float)kMap::getRandom(0,3) + eCell);
		break;
	case 4:
		treePos[0] = hkvVec2(x * meshSize + fThird * (float)kMap::getRandom(1,4) - sCell, y * meshSize + fThird);
		treePos[1] = hkvVec2(x * meshSize + fThird * (float)kMap::getRandom(1,4) - sCell, y * meshSize + fThird * 2.f);
		treePos[2] = hkvVec2(x * meshSize + fThird * (float)kMap::getRandom(1,4) - sCell, y * meshSize + fThird * 3.f);
		break;
	case 5:
		treePos[0] = hkvVec2(x * meshSize + fThird * (float)kMap::getRandom(1,4) - sCell, y * meshSize + fThird);
		treePos[1] = hkvVec2(x * meshSize + fThird * (float)kMap::getRandom(1,4) - sCell, y * meshSize + fThird * 2.f);
		treePos[2] = hkvVec2(x * meshSize + fThird * (float)kMap::getRandom(1,4) - sCell, y * meshSize + fThird * 3.f);
		treePos[3] = hkvVec2(x * meshSize + fThird * (float)kMap::getRandom(0,3) + eCell, y * meshSize + fThird);
		treePos[4] = hkvVec2(x * meshSize + fThird * (float)kMap::getRandom(0,3) + eCell, y * meshSize + fThird * 2.f);
		treePos[5] = hkvVec2(x * meshSize + fThird * (float)kMap::getRandom(0,3) + eCell, y * meshSize + fThird * 3.f);
		break;
	}

	for(int i = 0; i < makeGrassUpTo; i ++)
	{
		float VarX = (float)kMap::getRandom(10, (int)meshSize - 10);
		float VarY = (float)kMap::getRandom(10, (int)meshSize - 10);
		objPos[i] = hkvVec3((float)(x * meshSize + VarX), (float)(y * meshSize + VarY), -1.f);
	}

	for(int i = 0; i < (int)treePos.GetSize(); i++)
	{
		hkvVec3 tPos = treePos[i].getAsVec3(0);

		if(tPos.x <= -999.f)
			continue;

		VSmartPtr<kStatObject> tree = (kStatObject *) Vision::Game.CreateEntity("kStatObject", tPos);
		tree->Set((int)tPos.x, (int)tPos.y, kMap::getRandom(0,2));
		key.FormatEx("C_%d_%d_Tree_%d", x, y, i);
		tree->SetEntityKey(key);
	}

	for(int i = 0; i < (int)objPos.GetSize(); i++)
	{
		hkvVec3 tPos = objPos[i];

		if(tPos.x <= -999.f)
			continue;

		VSmartPtr<kStatObject> object = (kStatObject *) Vision::Game.CreateEntity("kStatObject", tPos);
		object->Set((int)tPos.x, (int)tPos.y, (int)tPos.z);
		key.FormatEx("C_%d_%d_Obj_%d", x, y, i);
		object->SetEntityKey(key);
	}
	
	if(objId == -132)
		key.FormatEx("C_%d_%d_SUPL", x, y);
	else
		key.FormatEx("C_%d_%d", x, y);

	s->SetEntityKey(key);
	s->SetPosition(pos);
	s->ReComputeVisibility();
	return s;
}

const int kMap::getRandom(int min, int max)
{
	int x = max - min;
	int rnd;
	if(x < 2)
	{
		rnd = 0;
	}
	else
	{
		rnd = (rand() % x);
	}
	return rnd + min;
}

const hkvVec3 kMap::getRandomPoint(float vMeshSize)
{
	int size = kMap::rRooms.GetValidSize();
	int r = kMap::getRandom(0, size);
	VSmartPtr<kRoom> room = kMap::rRooms[r];
	return room->Center() * vMeshSize;
}

const int kMap::PrintToConsole(VString classFrom, VString text)
{
	text.InsertAt(0, classFrom);
	Vision::GetConsoleManager()->OutputTextLine(text);
	return 0;
}