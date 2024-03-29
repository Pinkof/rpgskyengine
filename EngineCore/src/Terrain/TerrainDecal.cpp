#include "TerrainDecal.h"
#include "RenderSystem.h"

CTerrainDecal::CTerrainDecal():
m_nBufferWidth(0),
m_fRadius(0),
m_pIB(NULL),
m_bVisible(true),
m_uWidth(0),
m_uHeight(0)
{
}

void CTerrainDecal::setMaterial(const std::string& strMaterial)
{
	m_strMaterial=strMaterial;
}

int CTerrainDecal::GalcLogicalLengthByRadius(float fRadius)
{
	return fRadius*2 + 1 + (fRadius*2!=floorf(fRadius*2));
}
void CTerrainDecal::create(size_t uBufferWidth, float fRadius)
{
	m_fRadius = fRadius;
	size_t uLength = GalcLogicalLengthByRadius(m_fRadius);
	createBySize(uBufferWidth,uLength,uLength);
}

void CTerrainDecal::createBySize(size_t uBufferWidth, size_t uWidth, size_t uHeight)
{
	m_nBufferWidth = uBufferWidth;
	m_uWidth = uWidth;
	m_uHeight = uHeight;
	m_nVertexCount = m_uHeight*m_nBufferWidth + m_uWidth+1;
	m_nIBCount = m_uWidth*m_uHeight*6;
	assert(m_nVertexCount<65535);
	CreateIB();
}

bool CTerrainDecal::setIB()
{
	if (!m_pIB)
	{
		return false;
	}
	GetRenderSystem().SetIndices(m_pIB);
	return true;
}

void CTerrainDecal::CreateIB()
{
	S_DEL(m_pIB);
	m_pIB = GetRenderSystem().GetHardwareBufferMgr().CreateIndexBuffer(m_nIBCount);
	if (m_pIB)
	{
		unsigned short* index = 0;
		index = (unsigned short*)m_pIB->lock(CHardwareBuffer::HBL_NORMAL);
		for (size_t nY = 0; nY < m_uHeight; ++nY)
		{
			for (size_t nX = 0; nX < m_uWidth; ++nX)
			{
				// 2	 3
				//	*---*
				//	| / |
				//	*---*
				// 0	 1
				unsigned short uID = nY * m_nBufferWidth + nX;
				*index = uID;
				index++;
				*index = uID+m_nBufferWidth;
				index++;
				*index = uID+m_nBufferWidth+1;
				index++;

				*index = uID;
				index++;
				*index = uID+m_nBufferWidth+1;
				index++;
				*index = uID+1;
				index++;
			}
		}
		m_pIB->unlock();
	}
}

unsigned long CTerrainDecal::GetBaseVertexIndex(float fX, float fY)
{
	if (m_nBufferWidth<=0)
	{
		return 0;
	}
	int nX = fX-m_uWidth/2;
	int nY = fY-m_uHeight/2;
	if (nX<0)
	{
		nX = 0;
	}
	if (nY<0)
	{
		nY = 0;
	}
	int nMaxWidth = m_nBufferWidth-m_uWidth-1;
	if (nX > nMaxWidth)
	{
		nX = nMaxWidth;
	}
	if (nY > nMaxWidth)
	{
		nY = nMaxWidth;
	}
	return nY* m_nBufferWidth + nX;
}

void CTerrainDecal::Draw(float fX, float fY)
{
	unsigned long uBaseVertexIndex = GetBaseVertexIndex(fX, fY);
	Draw(uBaseVertexIndex);
}

void CTerrainDecal::Draw(unsigned long uBaseVertexIndex)
{
	GetRenderSystem().DrawIndexedPrimitive(
		VROT_TRIANGLE_LIST,
		uBaseVertexIndex,
		0,
		m_nVertexCount,
		0,
		m_nIBCount/3);
}