#include "Terrain.h"
#include <fstream>
#include "RenderSystem.h"
#include "Graphics.h"

CTerrain::CTerrain():
m_pVB(NULL),
m_pIB(NULL),
m_pGrassVB(NULL),
m_pGrassIB(NULL),
m_bShowBox(false),
m_nLightMap(0),
m_uShowTileIBCount(0)
{
}

CTerrain::~CTerrain()
{
	S_DEL(m_pVB);
	S_DEL(m_pIB);
	S_DEL(m_pGrassVB);
	S_DEL(m_pGrassIB);
}

void CTerrain::setTileMaterial(int nTileID, const std::string& strMaterialName)
{
	m_Tiles[nTileID] = strMaterialName;
}

CMaterial& CTerrain::getMaterial(const std::string& strMaterialName)
{
	return GetRenderSystem().getMaterialMgr().getItem(strMaterialName);
}

void CTerrain::updateIB()
{
	m_RenderTileSubsLayer[0].clear();
	m_RenderTileSubsLayer[1].clear();
	// 写IB
	if (m_pIB)
	{
		for (LIST_CUBES::iterator it=m_RenderCubesList.begin(); it!=m_RenderCubesList.end(); it++)
		{
			const BBox& bbox = (*it)->bbox;
			for (size_t y = bbox.vMin.z; y<bbox.vMax.z; ++y)
			{
				for (size_t x = bbox.vMin.x; x<bbox.vMax.x; ++x)
				{
					Pos2D posCell=Pos2D(x,y);
					uint32 uIndex = m_TerrainData.GetVertexIndex(posCell);
					for (int nLayer=0; nLayer<2; nLayer++)
					{
						const uint8 uTileID = m_TerrainData.GetCellTileID(posCell,nLayer);
						const uint8 uAttribute = m_TerrainData.getCellAttribute(posCell);
						if (uTileID!=0xFF&&(uAttribute&0x8)==0)
						{
							m_RenderTileSubsLayer[nLayer][uTileID].myVertexIndex(uIndex);
						}
					}
				}
			}
		}

		// resize grass vertex.
		//size_t uGrassCount = m_RenderTileSubsLayer[0][0].icount/6;
		//if (uGrassCount>0)
		{
			size_t uGrassCount = 0;
			for (LIST_CUBES::iterator it=m_RenderCubesList.begin(); it!=m_RenderCubesList.end(); it++)
			{
				const BBox& bbox = (*it)->bbox;
				for (size_t y = bbox.vMin.z; y<bbox.vMax.z; ++y)
				{
					for (size_t x = bbox.vMin.x; x<bbox.vMax.x; ++x)
					{
						if (m_TerrainData.hasGrass(x,y))
						{
							uGrassCount++;
						}
					}
				}
			}
			if (uGrassCount)
			{
				CreateGrassVB(uGrassCount);
				CreateGrassIB(uGrassCount);
				m_GrassSub.vbase=0;	// base vertex
				m_GrassSub.vstart=0;	// first vertex
				m_GrassSub.vcount=m_pGrassVB->getNumVertices();	// num vertices
				m_GrassSub.istart=0;	// first index
				m_GrassSub.icount=m_pGrassIB->getNumIndexes();	// num indices
			}
		}

		m_uShowTileIBCount = 0;
		for (int nLayer=0; nLayer<2; nLayer++)
		{
			for (std::map<uint8,TerrainSub>::iterator it = m_RenderTileSubsLayer[nLayer].begin(); it!=m_RenderTileSubsLayer[nLayer].end(); it++)
			{
				it->second.istart = m_uShowTileIBCount;
				m_uShowTileIBCount += it->second.icount;
				it->second.icount = 0;
				it->second.vcount=it->second.vcount-it->second.vstart+1+m_TerrainData.GetVertexXCount()+1;
			}
		}

		uint32 uTempVertexXCount = m_TerrainData.GetVertexXCount();
		uint16* index = (uint16*)m_pIB->lock(0, m_uShowTileIBCount*sizeof(uint16),CHardwareBuffer::HBL_NO_OVERWRITE);
		for (LIST_CUBES::iterator it=m_RenderCubesList.begin(); it!=m_RenderCubesList.end(); it++)
		{
			const BBox& bbox = (*it)->bbox;
			for (size_t y = bbox.vMin.z; y<bbox.vMax.z; ++y)
			{
				for (size_t x = bbox.vMin.x; x<bbox.vMax.x; ++x)
				{
					Pos2D posCell=Pos2D(x,y);
					uint32 uIndex = m_TerrainData.GetVertexIndex(posCell);
					for (int nLayer=0; nLayer<2; nLayer++)
					{
						const uint8 uTileID = m_TerrainData.GetCellTileID(posCell,nLayer);
						const uint8 uAttribute = m_TerrainData.getCellAttribute(posCell);
						if (uTileID!=0xFF&&(uAttribute&0x8)==0)
						{
							TerrainSub& sub = m_RenderTileSubsLayer[nLayer][uTileID];
							{
								// 2	 3
								//	*---*
								//	| / |
								//	*---*
								// 0	 1
								uint16* p = index+sub.istart+sub.icount;
								sub.icount+=6;
								*p = uIndex;
								p++;
								*p = uIndex+uTempVertexXCount;
								p++;
								*p = uIndex+uTempVertexXCount+1;
								p++;

								*p = uIndex;
								p++;
								*p = uIndex+uTempVertexXCount+1;
								p++;
								*p = uIndex+1;
								p++;
							}
						}
					}
				}
			}
		}
		m_pIB->unlock();
	}
}

void CTerrain::UpdateCubeBBox(Cube& cube)
{
	if (cube.pChildCube==NULL)
	{
		float fInitHeight = m_TerrainData.getVertexHeight(Pos2D(cube.bbox.vMin.x,cube.bbox.vMin.z));
		cube.bbox.vMin.y = fInitHeight;
		cube.bbox.vMax.y = fInitHeight;
		for (size_t y = cube.bbox.vMin.z; y<=cube.bbox.vMax.z; ++y)
		{
			for (size_t x = cube.bbox.vMin.x; x<=cube.bbox.vMax.x; ++x)
			{
				float fHeight = m_TerrainData.getVertexHeight(Pos2D(x,y));
				cube.bbox.vMin.y = min(cube.bbox.vMin.y,fHeight);
				cube.bbox.vMax.y = max(cube.bbox.vMax.y,fHeight);
			}
		}
	}
	else
	{
		Cube* pChildCube = cube.pChildCube;
		UpdateCubeBBox(*pChildCube);
		cube.bbox.vMin.y = pChildCube->bbox.vMin.y;
		cube.bbox.vMax.y = pChildCube->bbox.vMax.y;
		pChildCube++;
		for (size_t i=1; i<2; ++i)
		{
			UpdateCubeBBox(*pChildCube);
			cube.bbox.vMin.y = min(cube.bbox.vMin.y, pChildCube->bbox.vMin.y);
			cube.bbox.vMax.y = max(cube.bbox.vMax.y, pChildCube->bbox.vMax.y);
			pChildCube++;
		}
	}
}

void CTerrain::updateVB(const Pos2D& posBegin, const Pos2D& posEnd)
{
	if (m_pVB==NULL)
	{
		return;
	}
	uint32 uOffset	= sizeof(TerrainVertex)*m_TerrainData.GetVertexIndex(posBegin);
	uint32 uSize	= sizeof(TerrainVertex)*m_TerrainData.GetVertexIndex(posEnd)-uOffset;
	if (uSize>0)
	{
		TerrainVertex*	pV = (TerrainVertex*)m_pVB->lock(uOffset, uSize, CHardwareBuffer::HBL_NO_OVERWRITE/*CHardwareBuffer::HBL_NORMAL*/);
		for (int y = posBegin.y; y <= posEnd.y; y++)//+1?
		{
			for (int x = posBegin.x; x <= posEnd.x; x++)//+1?
			{
				m_TerrainData.getVertexByCell(x,y,*pV);
				pV++;
			}
		}
		m_pVB->unlock();
	}
}

void CTerrain::CreateVB()
{
	S_DEL(m_pVB);
	m_pVB = GetRenderSystem().GetHardwareBufferMgr().CreateVertexBuffer(m_TerrainData.GetVertexCount(),sizeof(TerrainVertex));
	updateVB(Pos2D(0,0),Pos2D(m_TerrainData.GetVertexXCount()-1,m_TerrainData.GetVertexYCount()-1));/*CHardwareBuffer::HBL_NORMAL*/
}

void CTerrain::CreateIB()
{
	S_DEL(m_pIB);
	m_pIB = GetRenderSystem().GetHardwareBufferMgr().CreateIndexBuffer(m_TerrainData.GetCellCount()*2*6*sizeof(uint16),CHardwareIndexBuffer::IT_16BIT,CHardwareBuffer::HBU_WRITE_ONLY);
}

void CTerrain::CreateGrassVB(size_t uGrassCount)
{
	S_DEL(m_pGrassVB);
	m_pGrassVB = GetRenderSystem().GetHardwareBufferMgr().CreateVertexBuffer(uGrassCount*4,sizeof(TerrainVertex),CHardwareBuffer::HBU_DYNAMIC);
	if (m_pGrassVB)
	{
		TerrainVertex*	pV = (TerrainVertex*)m_pGrassVB->lock(CHardwareBuffer::HBL_NORMAL);
		for (LIST_CUBES::iterator it=m_RenderCubesList.begin(); it!=m_RenderCubesList.end(); it++)
		{
			const BBox& bbox = (*it)->bbox;
			for (size_t y = bbox.vMin.z; y<bbox.vMax.z; ++y)
			{
				for (size_t x = bbox.vMin.x; x<bbox.vMax.x; ++x)
				{
					if (m_TerrainData.hasGrass(x,y))
					{
						m_TerrainData.getGrassVertexByCell(x,y,pV);
						pV+=4;
					}
				}
			}
		}
		m_pGrassVB->unlock();
	}
}

void CTerrain::CreateGrassIB(size_t uGrassCount)
{
	S_DEL(m_pGrassIB);
	m_pGrassIB = GetRenderSystem().GetHardwareBufferMgr().CreateIndexBuffer(uGrassCount*6*sizeof(uint16),CHardwareIndexBuffer::IT_16BIT,CHardwareBuffer::HBU_WRITE_ONLY);
	uint16* index = (uint16*)m_pGrassIB->lock(0, uGrassCount*6*sizeof(uint16),CHardwareBuffer::HBL_NO_OVERWRITE);
	int i=0;
	for (LIST_CUBES::iterator it=m_RenderCubesList.begin(); it!=m_RenderCubesList.end(); it++)
	{
		const BBox& bbox = (*it)->bbox;
		for (size_t y = bbox.vMin.z; y<bbox.vMax.z; ++y)
		{
			for (size_t x = bbox.vMin.x; x<bbox.vMax.x; ++x)
			{
				if (m_TerrainData.hasGrass(x,y))
				{
					// 1	 2
					//	*---*
					//	| / |
					//	*---*
					// 0	 3
					*index = i;
					index++;
					*index = i+1;
					index++;
					*index = i+2;
					index++;

					*index = i;
					index++;
					*index = i+2;
					index++;
					*index = i+3;
					index++;
					i+=4;
				}
			}
		}
	}
	m_pGrassIB->unlock();
}

void CTerrain::create(size_t width, size_t height, size_t cubeSize)
{
	m_TerrainData.Create(width, height, cubeSize);
	create();
}

void CTerrain::resize(size_t width, size_t height, size_t cubeSize)
{
	m_TerrainData.resize(width, height, cubeSize);
	create();
}

void CTerrain::setLightMapTexture(const std::string& strFilename)
{
	GetRenderSystem().GetTextureMgr().del(m_nLightMap);
	m_nLightMap = GetRenderSystem().GetTextureMgr().RegisterTexture(strFilename);
}

bool CTerrain::create()
{
	m_RenderTileSubsLayer[0].clear();
	m_RenderTileSubsLayer[1].clear();
	m_RenderCubesList.clear();
	m_RenderChunkCubesList.clear();

	m_TerrainData.CalcTexUV();
	CreateVB();
	CreateIB();
	{
		S_DELS(m_RootCube.pChildCube);

		m_RootCube.bbox.vMin = Vec3D(0.0f,0.0f,0.0f);
		m_RootCube.bbox.vMax = Vec3D(m_TerrainData.GetWidth(),0.0f,m_TerrainData.GetHeight());
		m_RootCube.createChildCube(m_TerrainData.GetCubeSize());
		UpdateCubeBBox(m_RootCube);
	}
	m_LightMapDecal.createBySize(GetData().GetVertexXCount(), m_TerrainData.GetCubeSize(), m_TerrainData.GetCubeSize());
	return true;
}

bool CTerrain::Prepare()
{
	if (m_pVB==NULL||m_pIB==NULL)
	{
		return false;
	}
	CRenderSystem& R = GetRenderSystem();
	R.SetLightingEnabled(false);
	R.SetCullingMode(CULL_ANTI_CLOCK_WISE);
	R.SetDepthBufferFunc(true,true);

	R.SetFVF(TERRAIN_VERTEX_FVF);
	R.SetStreamSource(0, m_pVB, 0, sizeof(TerrainVertex));
	R.SetIndices(m_pIB);
	return true;
}

bool CTerrain::LightMapPrepare()
{
	CRenderSystem& R = GetRenderSystem();
	R.SetLightingEnabled(false);
	R.SetCullingMode(CULL_ANTI_CLOCK_WISE);
	//R.SetRenderState(D3DRS_FOGENABLE, false);
	R.SetDepthBufferFunc(true,false);
	R.SetAlphaTestFunc(false);
	R.SetBlendFunc(true,BLENDOP_ADD,SBF_DEST_COLOUR,SBF_ZERO);

	R.SetTextureColorOP(0,TBOP_SOURCE1,TBS_TEXTURE,TBS_DIFFUSE);
	R.SetTextureAlphaOP(0,TBOP_DISABLE);
	R.SetTextureColorOP(1,TBOP_DISABLE);
	R.SetTexCoordIndex(0, 2);
	//bump
	//static int nTexID = GetRenderSystem().GetTextureMgr().RegisterTexture("Data/Textures/Tile/road.dds");
	R.SetTexture(0,m_nLightMap);

	R.SetIndices(m_LightMapDecal.GetIB());
	return true;
}

void CTerrain::LightMapFinish()
{
	CRenderSystem& R = GetRenderSystem();
	//R.BlendFunc(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
	//R.SetTexCoordIndex(0, 0);
	R.setTextureMatrix(0,TTF_DISABLE);
	R.setTextureMatrix(1,TTF_DISABLE);
	R.SetTextureColorOP(1,TBOP_DISABLE);
	R.SetTextureColorOP(2,TBOP_DISABLE);
}

void CTerrain::UpdateRender(const CFrustum& frustum)
{
	//
	m_RenderCubesList.clear();
	getCubesByFrustum(frustum, m_RenderCubesList);
	//
	m_RenderChunkCubesList.clear();
	for (size_t i=0; i<m_RenderCubesList.size(); ++i)
	{
		m_RenderCubesList[i]->getChildCrunodeCubes(m_RenderChunkCubesList);
	}
	//
	updateIB();
}

void CTerrain::DrawCubeBoxes(Color32 color)
{
	CRenderSystem& R = GetRenderSystem();
	R.SetDepthBufferFunc(true,true);
	for (size_t i=0; i<m_RenderCubesList.size(); ++i)
	{
		GetGraphics().drawBBox(m_RenderCubesList[i]->bbox,color);
	}
}

void CTerrain::drawLayer0()
{
	CRenderSystem& R = GetRenderSystem();
	for (std::map<uint8,TerrainSub>::iterator it = m_RenderTileSubsLayer[0].begin(); it!=m_RenderTileSubsLayer[0].end(); it++)
	{
		if (R.prepareMaterial(m_Tiles[it->first]))
		{
			R.SetTexCoordIndex(0, 0);	// Diffuse
			R.SetTexCoordIndex(1, 1);	// lightmap
			R.SetAlphaTestFunc(false);
			R.SetBlendFunc(false);
			R.SetTextureAlphaOP(0,TBOP_DISABLE);

			R.drawIndexedSubset(it->second);
		}
		R.finishMaterial();
	}
}

void CTerrain::drawLayer1()
{
	CRenderSystem& R = GetRenderSystem();
	for (std::map<uint8,TerrainSub>::iterator it = m_RenderTileSubsLayer[1].begin(); it!=m_RenderTileSubsLayer[1].end(); it++)
	{
		if (R.prepareMaterial(m_Tiles[it->first]))
		{
			R.SetTexCoordIndex(0, 0);	// Diffuse
			R.SetTexCoordIndex(1, 1);	// lightmap
			R.drawIndexedSubset(it->second);
		}
		R.finishMaterial();
	}
}

void CTerrain::renderGrass()
{
	CRenderSystem& R = GetRenderSystem();
	{
		if (m_pGrassVB==NULL||m_pGrassIB==NULL)
		{
			return;
		}
		R.SetLightingEnabled(false);
		R.SetCullingMode(CULL_NONE);
		R.SetDepthBufferFunc(true,true);
		R.SetAlphaTestFunc(true,CMPF_GREATER_EQUAL,0x80);

		R.SetFVF(TERRAIN_VERTEX_FVF);
		R.SetStreamSource(0, m_pGrassVB, 0, sizeof(TerrainVertex));
		R.SetIndices(m_pGrassIB);
	}
	if (R.prepareMaterial("Terrain.Grass"))
	{
		R.drawIndexedSubset(m_GrassSub);
	}
	R.finishMaterial();
}

void CTerrain::Render()
{
	if (m_bShowBox)
	{
		DrawCubeBoxes();
	}
	// 地形 VB设置一次
	if (Prepare())
	{
		drawLayer0();
		drawLayer1();
		// 阴影
		//if (LightMapPrepare())
		//{
		//	drawTerrain();
		//}
		//LightMapFinish();
	}
	renderGrass();		// Show grass.
}

void CTerrain::draw()
{
	if (m_LightMapDecal.setIB())
	{
		for (size_t i=0;i<m_RenderChunkCubesList.size();++i)
		{
			DrawChunk(*m_RenderChunkCubesList[i]);
		}
	}
}

void CTerrain::DrawChunk(const Cube& cube)
{
	uint32 uBaseVertexIndex = GetData().GetVertexIndex(cube.getMinCellPos());
	m_LightMapDecal.Draw(uBaseVertexIndex);
}

void CTerrain::getCubesByFrustum(const CFrustum& frustum, LIST_CUBES& CubeList)const
{
	m_RootCube.getChildCubesByFrustum(frustum,CubeList);
}

void CTerrain::getCrunodeCubes(LIST_CUBES& CubeList)const
{
	m_RootCube.getChildCrunodeCubes(CubeList);
}

void CTerrain::clearAllTiles()
{
	m_Tiles.clear();
}