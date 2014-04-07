#include "KaosuProjectPluginPCH.h"
#include "kRoom.h"

V_IMPLEMENT_SERIAL ( kRoom, kBasicEntity, 0, &g_myComponentModule );

START_VAR_TABLE( kRoom, kBasicEntity, "The Cell", 0, "")
END_VAR_TABLE

kRoom::kRoom(void)
{
}

kRoom::~kRoom(void)
{

}

void kRoom::addConnectors(int z1, int z2)
{
	this->RC1 = hkvVec3(0,0,-1);
	this->RC2 = hkvVec3(0,0,-1);

	for(int i = 0; i < 2; i++)
	{
		int side = (i == 0) ? z1 : z2;
		switch(side)
		{
		default: break;
		case 0:
			if(this->RC1.z < 0.f)
			{
				this->RC1 = hkvVec3((float)kMap::getRandom(3,(int)endPoint.x - 3), startPoint.y, (float)side);
			}
			else
			{
				this->RC2 = hkvVec3((float)kMap::getRandom(3,(int)endPoint.x - 3), startPoint.y, (float)side);
			}
			break;
		case 1:
			if(this->RC1.z < 0.f)
			{
				this->RC1 = hkvVec3(startPoint.x, (float)kMap::getRandom(3,(int)endPoint.y - 3), (float)side);
			}
			else
			{
				this->RC2 = hkvVec3(startPoint.x, (float)kMap::getRandom(3,(int)endPoint.y - 3), (float)side);
			}
			break;
		case 2:
			if(this->RC1.z < 0.f)
			{
				this->RC1 = hkvVec3((float)kMap::getRandom(3,(int)endPoint.x - 3), endPoint.y, (float)side);
			}
			else
			{
				this->RC2 = hkvVec3((float)kMap::getRandom(3,(int)endPoint.x - 3), endPoint.y, (float)side);
			}
			break;
		case 3:
			if(this->RC1.z < 0.f)
			{
				this->RC1 = hkvVec3(endPoint.x, (float)kMap::getRandom(3,(int)endPoint.y - 3), (float)side);
			}
			else
			{
				this->RC2 = hkvVec3(endPoint.x, (float)kMap::getRandom(3,(int)endPoint.y - 3), (float)side);
			}
			break;
		}
	}
}

void kRoom::OffSet(int x, int y)
{
	this->startPoint.x += x;
	this->startPoint.y += y;
	this->endPoint.x += x;
	this->endPoint.y += y;
	this->RC1.x += x;
	this->RC1.y += y;
	if(this->RC2.z >= 0)
	{
		this->RC2.x += x;
		this->RC2.y += y;
	}
}

hkvVec3 kRoom::Center()
{
	return (startPoint + endPoint) / 2.f;
}
hkvVec3 kRoom::TopLeft()
{
	float X = (startPoint.x >= endPoint.x) ? endPoint.x : startPoint.x;
	float Y = (startPoint.y >= endPoint.y) ? startPoint.y : endPoint.y;

	return hkvVec3(X,Y,0);
}
hkvVec3 kRoom::BottomRight()
{
	float X = (startPoint.x >= endPoint.x) ? startPoint.x : endPoint.x;
	float Y = (startPoint.y >= endPoint.y) ? endPoint.y : startPoint.y;

	return hkvVec3(X,Y,0);
}
int kRoom::Width()
{
	return (int)(endPoint.x - startPoint.x);
}
int kRoom::Height()
{
	return (int)(endPoint.y - startPoint.y);
}

void kRoom::Build(int normal_textures, int object_meshes, float meshSize)
{
	int sPx = (int)startPoint.x;
	int sPy = (int)startPoint.y;
	int ePx = (int)endPoint.x;
	int ePy = (int)endPoint.y;

	int RC1x = (int)this->RC1.x;
	int RC1y = (int)this->RC1.y;
	int RC1z = (int)this->RC1.z;
	int RC2x = (int)this->RC2.x;
	int RC2y = (int)this->RC2.y;
	int RC2z = (int)this->RC2.z;

	for(int x = sPx; x <= ePx; x++)
	{
		for(int y = sPy; y <= ePy; y++)
		{
			bool cont = CheckForContinue(x, y, RC1x, RC1y, RC1z) || CheckForContinue(x, y, RC2x, RC2y, RC2z);

			if(cont)
				continue;

			int texId = kMap::getRandom(0, normal_textures);
			int objId = kMap::getRandom(0, object_meshes);
			int objPos = kMap::getRandom(0,2) - 3;

			if(x == sPx)
			{
				objPos = 3;
			}
			if(x == ePx)
			{
				objPos = 1;
			}
			if(y == sPy)
			{
				objPos = 4;
			}
			if(y == ePy)
			{
				objPos = 2;
			}

			//TODO: Figure out object position
			VSmartPtr<kCell> c = kMap::addCell(0, texId, objId, x, y, objPos, meshSize);
			VASSERT(c);
		}
	}

	hkvVec3 ext = this->Center() * meshSize;
	if(RC2z >= 0)
	{
		if((RC1z + RC2z) % 2 == 0)
		{
			if(RC1z == 0 || RC2z == 0)
			{
				ext.x += 100.f;
			}
			else
			{
				ext.y += 100.f;
			}
		}
		else
		{
			ext = hkvVec3((float)((RC1x + RC2x) / 2),(float)((RC1y + RC2y) / 2),0);
		}
	}
	else
	{
		if((RC1z) % 2 == 0)
		{
			ext = ext + hkvVec3(100.f,0,0);
		}
		else
		{
			ext = ext + hkvVec3(0,100.f,0);			
		}
	}

	kMap::m_full_path[kMap::m_full_path.GetFreePos()] = ext + hkvVec3(0.5f, 0.5f, 0.f) * meshSize;

	this->BuildConnector(RC1x, RC1y, RC1z, meshSize);
	if(RC2z >= 0)
		this->BuildConnector(RC2x, RC2y, RC2z, meshSize);
}

void kRoom::BuildConnector(int x, int y, int side, float meshSize)
{
	(kMap::addCell(1, 0, -1, x, y, 0, meshSize)); //R1 2
	switch(side)
	{
	case 0:
		(kMap::addCell(0, 0, -2, x - 2, y + 1, 0, meshSize)); //R2 1
		(kMap::addCell(1, 8, -1, x - 1, y + 1, 0, meshSize)); //R2 2
		(kMap::addCell(1, 1, -1, x, y + 1, 0, meshSize)); //R2 3
		(kMap::addCell(1, 5, -1, x + 1, y + 1, 0, meshSize)); //R2 4
		(kMap::addCell(0, 0, -2, x + 2, y + 1, 0, meshSize)); //R2 5
		break;
	case 1: 
		(kMap::addCell(0, 0, -1, x + 1, y + 2, 0, meshSize)); //R1 1
		(kMap::addCell(1, 5, -1, x + 1, y + 1, 0, meshSize)); //R1 1
		(kMap::addCell(1, 2, -1, x + 1, y, 0, meshSize)); //R2 1
		(kMap::addCell(1, 6, -1, x + 1, y - 1, 0, meshSize)); //R3 1
		(kMap::addCell(0, 0, -1, x + 1, y - 2, 0, meshSize)); //R3 1
		break;
	case 2:
		(kMap::addCell(0, 0, -1, x - 2, y - 1, 0, meshSize)); //R1 1
		(kMap::addCell(1, 7, -1, x - 1, y - 1, 0, meshSize)); //R1 1
		(kMap::addCell(1, 3, -1, x, y - 1, 0, meshSize)); //R1 2
		(kMap::addCell(1, 6, -1, x + 1, y - 1, 0, meshSize)); //R1 3
		(kMap::addCell(0, 0, -1, x + 2, y - 1, 0, meshSize)); //R1 3
		break;
	case 3:
		(kMap::addCell(0, 0, -1, x - 1, y + 2, 0, meshSize)); //R1 1
		(kMap::addCell(1, 8, -1, x - 1, y + 1, 0, meshSize)); //R1 1
		(kMap::addCell(1, 4, -1, x - 1, y, 0, meshSize)); //R2 1
		(kMap::addCell(1, 7, -1, x - 1, y - 1, 0, meshSize)); //R3 1
		(kMap::addCell(0, 0, -1, x - 1, y - 2, 0, meshSize)); //R3 1
		break;
	}
}

bool kRoom::CheckForContinue(int x, int y, int Cx, int Cy, int Cz)
{
	//Construct a box
	int minX = -1, minY = -1, maxX = -1, maxY = -1;
	switch(Cz)
	{
	case 0:
		minX = Cx - 2;
		maxX = Cx + 2;
		minY = Cy;
		maxY = Cy + 1;
		break;
	case 1:
		minX = Cx;
		maxX = Cx + 1;
		minY = Cy - 2;
		maxY = Cy + 2;
		break;
	case 2:
		minX = Cx - 2;
		maxX = Cx + 2;
		minY = Cy - 1;
		maxY = Cy;
		break;
	case 3:
		minX = Cx - 1;
		maxX = Cx;
		minY = Cy - 2;
		maxY = Cy + 2;
		break;
	}

	if(x >= minX && x <= maxX && y >= minY && y <= maxY)
		return true;

	return false;
}