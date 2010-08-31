#include "TerrainData.h"
#include <fstream>
//#include "tinyxml.h"
#include "interpolation.h"
#include "IORead.h"
#include "Intersect.h"

#define		MAX_PICK_COUNT	200
CTerrainData::CTerrainData():
m_nWidth(0),
m_nHeight(0),
m_nCubeSize(8),
m_nCellCount(0),
m_posTileCount(0,0),
m_nVertexXCount(0),
m_nVertexYCount(0),
m_nVertexCount(0),
m_vLightDir(0,-1,0)
{
	m_vLightDir = Vec3D(-1,-1,-1).normalize();
}

CTerrainData::~CTerrainData()
{
}

void CTerrainData::clear()
{
	m_nWidth = 0;
	m_nHeight = 0;
	m_nCubeSize = 0;
	m_nCellCount = 0;
	m_nVertexXCount = 0;
	m_nVertexYCount = 0;
	m_nVertexCount = 0;
	m_Cells.clear();
}

void CTerrainData::create(size_t width, size_t height, size_t cubeSize)
{
	clear();
	resize(width,height,cubeSize);
}
template <class T>
inline void resizeVector(std::vector<T>& v, size_t srcWidth, size_t srcHeight,size_t destWidth, size_t destHeight,T val=0)
{
	std::vector<T> srcV=v;
	v.resize(destWidth*destHeight,val);
	if (srcV.empty())
	{
		return;
	}
	size_t width = min(srcWidth,destWidth);
	size_t height = min(srcHeight,destHeight);
	for (size_t y = 0; y < height; ++y)
	{
		for (size_t x = 0; x < width; ++x)
		{
			v[y*destWidth+x] = srcV[y*srcWidth+x];
		}
	}
}

bool CTerrainData::resize(size_t width, size_t height, size_t cubeSize)
{
	if (cubeSize==0)
	{
		cubeSize = 8;
		for (size_t i=8; i<30; ++i)
		{
			if (width%i==0&&width%i==0)
			{
				cubeSize = i;
				break;
			}
		}
	}
	width=(width/cubeSize)*cubeSize;
	height=(height/cubeSize)*cubeSize;

	if (65536<(width+1)*(height+1))
	{
		return false;
	}

	TerrainCell val={0,255,0xFFFFFFFF,0xFFFFFFFF,0.0f,Vec3D(0.0f,1.0f,0.0f),0,false};
	resizeVector(m_Cells,m_nVertexXCount,m_nVertexYCount,width+1,height+1,val);

	m_nWidth = width;
	m_nHeight = height;
	m_nCubeSize = cubeSize;
	m_nCellCount = m_nWidth * m_nHeight;
	m_nVertexXCount = m_nWidth + 1;
	m_nVertexYCount = m_nHeight + 1;
	m_nVertexCount = m_nVertexXCount * m_nVertexYCount;
	return true;
}

TerrainCell* CTerrainData::getCell(int x, int y)
{
	int nCellIndex = y*m_nVertexXCount+x;
	return &m_Cells[nCellIndex];
}

const TerrainCell* CTerrainData::getCell(int x, int y)const
{
	int nCellIndex = y*m_nVertexXCount+x;
	return &m_Cells[nCellIndex];
}

void CTerrainData::getVertexByCell(int nCellX, int nCellY, TerrainVertex& vertex)const
{
	const TerrainCell* cell = getCell(nCellX,nCellY);
	vertex.p = Vec3D((float)nCellX, cell->fHeight, (float)nCellY);
	vertex.n = cell->vNormals;

	/*Vec3D vBinormal = vertex.n.cross(Vec3D(0,0,1)).normalize();
	Vec3D vTangent =  Vec3D(1,0,0).cross(vertex.n).normalize();
	Matrix mTangent;
	mTangent.Zero();
	mTangent._11=vBinormal.x;
	mTangent._21=vBinormal.y;
	mTangent._31=vBinormal.z;

	mTangent._12=vertex.n.x;
	mTangent._22=vertex.n.y;
	mTangent._32=vertex.n.z;

	mTangent._13=vTangent.x;
	mTangent._23=vTangent.y;
	mTangent._33=vTangent.z;

	vertex.n = mTangent*GetLightDir();
	vertex.n=vertex.n.normalize();*/

	vertex.c = cell->color;
	//vertex.t = m_EquableTexUV[nCellIndex];
	vertex.t0.x = (float)nCellX;
	vertex.t0.y = (float)nCellY;
	vertex.t1.x = (float)nCellX/(float)GetWidth();
	vertex.t1.y = (float)nCellY/(float)GetHeight();
}

void CTerrainData::getGrassVertexByCell(int nCellX, int nCellY, TerrainVertex*& vertex)const
{
	const TerrainCell* cell1 = getCell(nCellX,nCellY);
	const TerrainCell* cell2 = getCell(nCellX+1,nCellY+1);
	int	nRand = (((nCellY*m_nVertexXCount+nCellX+nCellX*nCellY)*214013L+2531011L)>>16)&0x7fff;   
	float fTexU = (nRand%4)*0.25f;
	vertex[0].p = Vec3D((float)nCellX, cell1->fHeight, (float)nCellY);
	//vertex[0].n = Vec3DGetCellNormal(posCell1);
	vertex[0].c = cell1->color;
	vertex[0].t0 = Vec2D(fTexU,1.0f);

	vertex[1].p = vertex[0].p+Vec3D(0.0f,1.5f,0.0f);
	//vertex[1].n = vertex[0].n;
	vertex[1].c = vertex[0].c;
	vertex[1].t0 = Vec2D(fTexU,0.0f);

	vertex[3].p = Vec3D((float)(nCellX+1), cell2->fHeight, (float)(nCellY+1));
	//vertex[3].n = GetCellNormal(posCell2);
	vertex[3].c = cell2->color;
	vertex[3].t0 = Vec2D(fTexU+0.25f,1.0f);

	vertex[2].p = vertex[3].p+Vec3D(0.0f,1.5f,0.0f);
	//vertex[2].n = vertex[3].n;
	vertex[2].c = vertex[3].c;
	vertex[2].t0 = Vec2D(fTexU+0.25f,0.0f);
}

bool CTerrainData::hasGrass(int nCellX, int nCellY)const
{
	const TerrainCell* cell = getCell(nCellX,nCellY);
	const uint8 uAttribute = getCellAttribute(nCellX,nCellY);
	return 0==cell->uTileID[0]&&255==cell->uTileID[1]&&0==(cell->uAttribute&0x8);
}

bool CTerrainData::PickCell(int nCellX, int nCellY, const Vec3D& vRayPos, const Vec3D& vRayDir, Vec3D* pPos)const
{
	if (isCellIn(nCellX,nCellY))
	{
		const TerrainCell* cell = getCell(nCellX,nCellY);
		Vec3D v0((float)(nCellX),	cell->fHeight,						(float)(nCellY));
		Vec3D v1((float)(nCellX),	(cell+m_nVertexXCount)->fHeight,	(float)(nCellY+1));
		Vec3D v2((float)(nCellX+1),	(cell+m_nVertexXCount+1)->fHeight,	(float)(nCellY+1));
		Vec3D v3((float)(nCellX+1),	(cell+1)->fHeight,					(float)(nCellY));
		Vec3D vOut;
		if(IntersectTri(v1,v2,v0,vRayPos,vRayDir,vOut))
		{
			if (pPos)
			{
				*pPos = vOut;
			}
			return true;
		}
		else if (IntersectTri(v3,v0,v2,vRayPos,vRayDir,vOut))
		{
			if (pPos)
			{
				*pPos = vOut;
			}
			return true;
		}
	}
	return false;
}

bool CTerrainData::Pick(const Vec3D& vRayPos, const Vec3D& vRayDir, Vec3D* pPos)const
{
	// 格子地图的快速PICK
	int nIncX = vRayDir.x>0?1:-1;
	int nIncY = vRayDir.z>0?1:-1;
	float fK = vRayDir.z/vRayDir.x;
	float fB = vRayPos.z - fK*vRayPos.x;
	int nX = int(vRayPos.x)-nIncX;
	int nY = int(vRayPos.z)-nIncY;

	if (fK<1&&fK>-1)
	{
		for (int i = 0; i < MAX_PICK_COUNT; i++)
		{
			int nCellX = nX + nIncX;
			int nCellY = (int)(fK*(float)nCellX+fB);
			if (nCellY!=nY)
			{
				nY = nCellY;
				nCellX = nX - (nIncX<0);
				nCellY = nY - nIncY*(nIncX<0);
			}
			else
			{
				nX = nCellX;
			}
			if (PickCell(nCellX, nCellY, vRayPos, vRayDir, pPos))
			{
				return true;
			}
		}
	}
	else
	{
		for (int i = 0; i < MAX_PICK_COUNT; i++)
		{
			int nCellY = nY + nIncY;
			int nCellX = (int)(((float)nCellY-fB)/fK);
			if (nCellX!=nX)
			{
				nX = nCellX;
				nCellX = nX - nIncX*(nIncY<0);
				nCellY = nY - (nIncY<0);
			}
			else
			{
				nY = nCellY;
			}
			if (PickCell(nCellX, nCellY, vRayPos, vRayDir, pPos))
			{
				return true;
			}
		}
	}
	return false;
}

uint32 CTerrainData::GetVertexIndex(int nCellX, int nCellY)const
{
	return nCellX*m_nVertexXCount + nCellY;
}

int CTerrainData::GetCellXByVertexIndex(uint32 uVertexIndex)const
{
	return uVertexIndex%m_nVertexXCount;
}

int CTerrainData::GetCellYByVertexIndex(uint32 uVertexIndex)const
{ 
	return uVertexIndex/m_nVertexXCount;
}

Pos2D CTerrainData::GetCellPosByVertexIndex(uint32 uVertexIndex)const
{
	return Pos2D(uVertexIndex%m_nVertexXCount,uVertexIndex/m_nVertexXCount);
}

bool CTerrainData::isCellIn(int nCellX, int nCellY)const
{
	if (nCellX>=0 && nCellX<m_nWidth && nCellY>=0 && nCellY<m_nHeight)
	{
		return true;
	}
	return false;
}

bool CTerrainData::isPointIn(int nCellX, int nCellY)const
{
	if (nCellX>=0 && nCellX<m_nVertexXCount && nCellY>=0 && nCellY<m_nVertexYCount)
	{
		return true;
	}
	return false;
}

uint8 CTerrainData::GetCellTileID(int nCellX, int nCellY, size_t layer)const
{
	if (isCellIn(nCellX,nCellY))
	{
		if (2>layer)
		{
			return getCell(nCellX,nCellY)->uTileID[layer];
		}
	}
	return 0;
}

void CTerrainData::SetCellTileID(int nCellX, int nCellY, uint8 uTileID, size_t layer)
{
	if (isCellIn(nCellX,nCellY))
	{
		if (2>layer)
		{
			getCell(nCellX,nCellY)->uTileID[layer] = uTileID;
		}
	}
}

uint8 CTerrainData::getCellAttribute(int nCellX, int nCellY)const
{
	if (isCellIn(nCellX,nCellY))
	{
		return getCell(nCellX,nCellY)->uAttribute;
	}
	return 0;
}

void CTerrainData::setCellAttribute(int nCellX, int nCellY, uint8 uAtt)
{
	if (isCellIn(nCellX,nCellY))
	{
		getCell(nCellX,nCellY)->uAttribute = uAtt;
	}
}

bool CTerrainData::isCellSearched(int nCellX, int nCellY)const
{
	if (isCellIn(nCellX,nCellY))
	{
		return getCell(nCellX,nCellY)->bSearched;
	}
	return true;
}

void CTerrainData::setCellSearched(int nCellX, int nCellY, bool bSearched)
{
	if (isCellIn(nCellX,nCellY))
	{
		getCell(nCellX,nCellY)->bSearched = bSearched;
	}
}

float CTerrainData::getVertexHeight(int nCellX, int nCellY)const
{
	if (isPointIn(nCellX,nCellY))
	{
		return getCell(nCellX,nCellY)->fHeight;
	}
	return 0.0f;
}

void CTerrainData::setVertexHeight(int nCellX, int nCellY, float fHeight)
{
	if (isPointIn(nCellX,nCellY))
	{
		getCell(nCellX,nCellY)->fHeight = fHeight;
	}
}

Vec3D CTerrainData::getVertexNormal(int nCellX, int nCellY)const
{
	float a = getVertexHeight(nCellX,	nCellY);
	float b = getVertexHeight(nCellX,	nCellY+1);
	float c = getVertexHeight(nCellX+1,	nCellY);
	Vec3D vVector0(0,(b-a),1);
	Vec3D vVector1(1,(c-a),0);
	Vec3D vN = vVector0.cross(vVector1);
	return vN.normalize();
}

Color32 CTerrainData::getVertexColor(int nCellX, int nCellY)const
{
	if (isPointIn(nCellX,nCellY))
	{
		return getCell(nCellX,nCellY)->color;
	}
	return 0xFFFFFFFF;
}

void CTerrainData::setVertexColor(int nCellX, int nCellY, Color32 color)
{
	if (isPointIn(nCellX,nCellY))
	{
		getCell(nCellX,nCellY)->color = color;
	}
}

float CTerrainData::GetHeight(const Vec2D& vPos)const
{
	int nCellX = int(vPos.x);
	int nCellY = int(vPos.y);
	float u = vPos.x - nCellX;
	float v = vPos.y - nCellY;
	float a = getVertexHeight(nCellX,	nCellY);
	float b = getVertexHeight(nCellX+1,	nCellY);
	float c = getVertexHeight(nCellX,	nCellY+1);
	float d = getVertexHeight(nCellX+1,	nCellY+1);
	return bilinearInterpolation(a,b,c,d,u,v);
}

Vec4D CTerrainData::GetColor(const Vec2D& vPos)const
{
	int nCellX = int(vPos.x);
	int nCellY = int(vPos.y);
	float u = vPos.x - nCellX;
	float v = vPos.y - nCellY;
	Vec4D a = getVertexColor(nCellX,	nCellY);
	Vec4D b = getVertexColor(nCellX+1,	nCellY);
	Vec4D c = getVertexColor(nCellX,	nCellY+1);
	Vec4D d = getVertexColor(nCellX+1,	nCellY+1);
	return bilinearInterpolation(a,b,c,d,u,v);
}