#include "KaosuProjectPluginPCH.h"
#include "kPath.h"

kPath::kPath(void)
{
}

kPath::~kPath(void)
{
}

void kPath::addPoint(hkvVec2 path, int size)
{
	if(!paths.IsInitialized())
		paths = SerialDynArray_cl<hkvVec3>(15, hkvVec3(0));
	paths[paths.GetFreePos()] = (hkvVec3(path.x, path.y, (float)size));
}


void kPath::Build(float meshSize)
{
	hkvVec3 startPt = this->paths[0];
	SerialDynArray_cl<hkvVec3> pts = SerialDynArray_cl<hkvVec3>();
	pts[pts.GetFreePos()] = startPt;
	int Length = (int)this->paths.GetValidSize();

	for(int i = 1; i < Length; i++)
	{
		hkvVec3 endPt = this->paths[i];
		pts[pts.GetFreePos()] = endPt;

		int sX = (int)startPt.x, sY = (int)startPt.y;
		int	eX = (int)endPt.x,  eY = (int)endPt.y;
		bool Left = eX > sX, Right = eX < sX; //Vert
		bool Up = eY < sY, Down = eY > sY; //Horiz

		bool makeCorner = false;
		int cornerType = -1;

		int xMin = 0, xMax = 0, yMin = 0, yMax = 0;
		int vDifa = 3, vDifb = 3;

		if(i == 1)
			vDifa = 0;

		if(i == (Length - 1))
			vDifb = 0;
		else //TURN
		{
			hkvVec3 nPoint = this->paths[i + 1];
			int asX = (int)endPt.x, asY = (int)endPt.y;
			int	aeX = (int)nPoint.x,  aeY = (int)nPoint.y;
				
			bool aLeft = aeX > asX, aRight = aeX < asX; //Vert
			bool aUp = aeY < asY, aDown = aeY > asY; //Horiz
			
			(kMap::addCell(1, 0, -1, asX, asY, -3, meshSize)); //Common Cell

			int NE = 0, NW = 0, SE = 0, SW = 0;
			if(Up || aDown)
			{
				(kMap::addCell(1, 0, -1, asX, asY + 1, kMap::getRandom(0,2) - 3, meshSize)); //Common Cell
				(kMap::addCell(1, 0, -1, asX, asY + 2, kMap::getRandom(0,2) - 3, meshSize)); //Common Cell

				(kMap::addCell(0, 0, -1, asX, asY - 2, 2, meshSize)); //SIDE
				(kMap::addCell(0, 0, -1, asX + 1, asY - 2, 2, meshSize)); //SIDE
				(kMap::addCell(0, 0, -1, asX - 1, asY - 2, 2, meshSize)); //SIDE

				(kMap::addCell(1, 4, 0, asX - 1, asY + 2, kMap::getRandom(0,2) - 3, meshSize)); //Common Cell
				(kMap::addCell(1, 2, -1, asX + 1, asY + 2, kMap::getRandom(0,2) - 3, meshSize)); //Common Cell

			}
			if(Down || aUp)
			{
				(kMap::addCell(1, 0, -1, asX, asY - 1, kMap::getRandom(0,2) - 3, meshSize)); //Main
				(kMap::addCell(1, 0, -1, asX, asY - 2, kMap::getRandom(0,2) - 3, meshSize)); //Main

				(kMap::addCell(0, 0, -1, asX, asY + 2, 4, meshSize)); //Side
				(kMap::addCell(0, 0, -1, asX + 1, asY + 2, 4, meshSize)); //Side
				(kMap::addCell(0, 0, -1, asX - 1, asY + 2, 4, meshSize)); //Side

				(kMap::addCell(1, 4, 0, asX - 1, asY - 2, kMap::getRandom(0,2) - 3, meshSize)); //Common Cell
				(kMap::addCell(1, 2, -1, asX + 1, asY - 2, kMap::getRandom(0,2) - 3, meshSize)); //Common Cell
			}
			if(Left || aRight)
			{
				(kMap::addCell(1, 0, -1, asX - 1, asY, kMap::getRandom(0,2) - 3, meshSize)); //Common Cell
				(kMap::addCell(1, 0, -1, asX - 2, asY, kMap::getRandom(0,2) - 3, meshSize)); //Common Cell

				(kMap::addCell(0, 0, -1, asX + 2, asY, 3, meshSize)); //Side
				(kMap::addCell(0, 0, -1, asX + 2, asY + 1, 3, meshSize)); //Side
				(kMap::addCell(0, 0, -1, asX + 2, asY - 1, 3, meshSize)); //Side

				(kMap::addCell(1, 3, -1, asX - 2, asY - 1, kMap::getRandom(0,2) - 3, meshSize)); //Common Cell
				(kMap::addCell(1, 1, -1, asX - 2, asY + 1, kMap::getRandom(0,2) - 3, meshSize)); //Common Cell
			}
			if(Right || aLeft)
			{
				(kMap::addCell(1, 0, -1, asX + 1, asY, kMap::getRandom(0,2) - 3, meshSize)); //Common Cell
				(kMap::addCell(1, 0, -1, asX + 2, asY, kMap::getRandom(0,2) - 3, meshSize)); //Common Cell

				(kMap::addCell(0, 0, -1, asX - 2, asY, 1, meshSize)); //Side
				(kMap::addCell(0, 0, -1, asX - 2, asY + 1, 1, meshSize)); //Side
				(kMap::addCell(0, 0, -1, asX - 2, asY - 1, 1, meshSize)); //Side

				(kMap::addCell(1, 3, -1, asX + 2, asY - 1, kMap::getRandom(0,2) - 3, meshSize)); //Common Cell
				(kMap::addCell(1, 1, -1, asX + 2, asY + 1, kMap::getRandom(0,2) - 3, meshSize)); //Common Cell
			}
			//.x

			if(Down && aRight || Left && aUp)
			{
				(kMap::addCell(1, 2, -1, asX + 1, asY - 1, kMap::getRandom(0,2) - 3, meshSize)); //Side
				(kMap::addCell(1, 2, -1, asX + 1, asY, kMap::getRandom(0,2) - 3, meshSize)); //Side

				(kMap::addCell(1, 5, -1, asX + 1, asY + 1, -3, meshSize)); //Outer Corner
				(kMap::addCell(1, 9, -1, asX - 1, asY - 1, -3, meshSize)); //Inner Corner
				
				(kMap::addCell(1, 1, -1, asX, asY + 1, kMap::getRandom(0,2) - 3, meshSize)); //Side
				(kMap::addCell(1, 1, -1, asX - 1, asY + 1, kMap::getRandom(0,2) - 3, meshSize)); //Side
				NW = 4; NE = 2; SE = 3; SW = 1;
			}
			if(Down && aLeft || Right && aUp)
			{
				(kMap::addCell(1, 4, 0, asX - 1, asY - 1, kMap::getRandom(0,2) - 3, meshSize)); //Side
				(kMap::addCell(1, 4, 0, asX - 1, asY, kMap::getRandom(0,2) - 3, meshSize)); //Side

				(kMap::addCell(1, 8, -1, asX - 1, asY + 1, -3, meshSize)); //Outer Corner
				(kMap::addCell(1, 12, -1, asX + 1, asY - 1, -3, meshSize)); //Inner Corner
				
				(kMap::addCell(1, 1, -1, asX + 1, asY + 1, kMap::getRandom(0,2) - 3, meshSize)); //Side
				(kMap::addCell(1, 1, -1, asX, asY + 1, kMap::getRandom(0,2) - 3, meshSize)); //Side
				NW = 2; NE = 4; SE = 3; SW = 1;
			}
			if(Up && aLeft || Right && aDown)
			{
				(kMap::addCell(1, 3, -1, asX + 1, asY - 1, kMap::getRandom(0,2) - 3, meshSize)); //Common Cell
				(kMap::addCell(1, 3, -1, asX, asY - 1, kMap::getRandom(0,2) - 3, meshSize)); //Common Cell

				(kMap::addCell(1, 7, -1, asX - 1, asY - 1, -3, meshSize)); //Outer Corner
				(kMap::addCell(1, 11, -1, asX + 1, asY + 1, -3, meshSize)); //Inner Corner
				
				(kMap::addCell(1, 4, 0, asX - 1, asY + 1, kMap::getRandom(0,2) - 3, meshSize)); //Side
				(kMap::addCell(1, 4, 0, asX - 1, asY, kMap::getRandom(0,2) - 3, meshSize)); //Side
				NW = 1; NE = 4; SE = 2; SW = 3;
			}

			if(Up && aRight || Left && aDown)
			{
				(kMap::addCell(1, 3, -1, asX - 1, asY - 1, kMap::getRandom(0,2) - 3, meshSize)); //Common Cell
				(kMap::addCell(1, 3, -1, asX, asY - 1, kMap::getRandom(0,2) - 3, meshSize)); //Common Cell

				(kMap::addCell(1, 6, -1, asX + 1, asY - 1, -3, meshSize)); //Outer Corner
				(kMap::addCell(1, 10, -1, asX - 1, asY + 1, -3, meshSize)); //Inner Corner
				
				(kMap::addCell(1, 2, -1, asX + 1, asY + 1, kMap::getRandom(0,2) - 3, meshSize)); //Side
				(kMap::addCell(1, 2, -1, asX + 1, asY, kMap::getRandom(0,2) - 3, meshSize)); //Side
				NW = 3; NE = 1; SE = 2; SW = 4;
			}
			
			(kMap::addCell(0, 0, -1, asX + 2, asY + 2, NE, meshSize)); //Corner
			(kMap::addCell(0, 0, -1, asX + 2, asY - 2, NW, meshSize)); //Corner
			(kMap::addCell(0, 0, -1, asX - 2, asY + 2, SE, meshSize)); //Corner
			(kMap::addCell(0, 0, -1, asX - 2, asY - 2, SW, meshSize)); //Corner
		}

		if(Left)
		{
			xMin = vDifa;
			xMax = -vDifb;
		}
		else if(Right)
		{
			xMin = -vDifa;
			xMax = vDifb;
		}
		else if(Up)
		{
			yMin = -vDifa;
			yMax = vDifb;
		}
		else if(Down)
		{
			yMin = vDifa;
			yMax = -vDifb;
		}

		int sMinX = (sX > eX) ? eX + xMax : sX + xMin;
		int eMaxX = (sX > eX) ? sX + xMin : eX + xMax;

		int sMinY = (sY > eY) ? eY + yMax : sY + yMin;
		int eMaxY = (sY > eY) ? sY + yMin : eY + yMax;
		
		for(int x = sMinX; x <= eMaxX; x++)
		{
			for(int y = sMinY; y <= eMaxY; y++)
			{
				(kMap::addCell(1, 0, -1, x, y, -3, meshSize)); //Common Cell
				if(Left || Right) //Add Sides of the path
				{
					(kMap::addCell(0, 0, -2, x, y - 2, 3, meshSize)); //Common Cell
					(kMap::addCell(1, 3, -1, x, y - 1, kMap::getRandom(0,2) - 3, meshSize)); //Common Cell
					(kMap::addCell(1, 1, -1, x, y + 1, kMap::getRandom(0,2) - 3, meshSize)); //Common Cell
					(kMap::addCell(0, 0, -2, x, y + 2, 1, meshSize)); //Common Cell
				}
				else
				{
					(kMap::addCell(0, 0, -2, x - 2, y, 4, meshSize)); //Common Cell
					(kMap::addCell(1, 4, 0, x - 1, y, kMap::getRandom(0,2) - 3, meshSize)); //Common Cell
					(kMap::addCell(1, 2, -1, x + 1, y, kMap::getRandom(0,2) - 3, meshSize)); //Common Cell
					(kMap::addCell(0, 0, -2, x + 2, y, 2, meshSize)); //Common Cell
				}
			}
		}
		startPt = endPt;
	}
	
	if(paths[0].x > paths[paths.GetValidSize()-1].x)
	{
		for(int i = (int)pts.GetValidSize() - 1; i >= 0; i--)
		{
			startPt = pts[i];
			kMap::m_full_path[kMap::m_full_path.GetFreePos()] = (startPt - hkvVec3(0,0, startPt.z)) * meshSize + hkvVec3(0.5f, 0.5f, 0.f) * meshSize;
		}
	}
	else
	{
		for(int i = 0; i < (int)pts.GetValidSize(); i++)
		{
			startPt = pts[i];
			kMap::m_full_path[kMap::m_full_path.GetFreePos()] = (startPt - hkvVec3(0,0, startPt.z)) * meshSize + hkvVec3(0.5f, 0.5f, 0.f) * meshSize;
		}
	}

	paths.Reset();
}
