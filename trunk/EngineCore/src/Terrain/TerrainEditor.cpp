#include "TerrainEditor.h"
#include "RenderSystem.h"
#include "ModelObject.h"
#include "Scene.h"

CTerrainEditor::CTerrainEditor() : CTerrain(),
m_bShowLayer0(true),
m_bShowLayer1(true),
m_bShowAttribute(false),
m_bShowGrid(false),
m_bShowBrushDecal(false)
{
}

CTerrainEditor::~CTerrainEditor()
{
}

bool CTerrainEditor::create()
{
	CTerrain::create();
	createBrush();
	return true;
}

#define  BRUSH_SIZE 8
void CTerrainEditor::createBrush()
{
	m_BrushDecal.create(m_TerrainData.GetVertexXCount(),BRUSH_SIZE);
}

void CTerrainEditor::drawAttribute()
{
	CRenderSystem& R = GetRenderSystem();
	{
		R.SetAlphaTestFunc(false);
		R.SetBlendFunc(true);
		R.SetTextureColorOP(0,TBOP_SOURCE1,TBS_TFACTOR);
		R.SetTextureAlphaOP(0,TBOP_SOURCE1,TBS_TFACTOR);
	}
	for (std::map<uint8,TerrainSub>::iterator it = m_mapRenderAttributeSubs.begin(); it!=m_mapRenderAttributeSubs.end(); it++)
	{
		Color32 c=0x80000000;
		if (it->first&(1<<0))
		{
			c.g=255;
		}
		if (it->first&(1<<2))
		{
			c.r=255;
		}
		if (it->first&(1<<3))
		{
			c.b=255;
		}
		if (it->first&(1<<1))
		{
			c=0x80778899;
		}
		if (it->first&(1<<4))
		{
			c=0x80778899;
		}
		R.SetTextureFactor(c);
		//switch(it->first)
		//{
		//case 0:// unknown
		//	R.SetTextureFactor(0x80FFFFFF);
		//	break;
		//case 1:// safe
		//	R.SetTextureFactor(0x8000FF00);
		//	break;
		//case 4:// break
		//	R.SetTextureFactor(0x80FF0000);
		//	break;
		//case 5:// safe&break
		//	R.SetTextureFactor(0x80FFFF00);
		//	break;
		//case 0xC:// break&unvisible
		//	R.SetTextureFactor(0x80FF00FF);
		//	break;
		//case 0xFF:
		//	R.SetTextureFactor(0x80000000);
		//	break;
		//default:// unknown
		//	
		//	break;
		//}
		R.drawIndexedSubset(it->second);
	}
}

void CTerrainEditor::Render()
{
	//CTerrain::Render();

	if (m_bShowBox)
	{
		DrawCubeBoxes();
	}
	// 地形 VB设置一次
	if (Prepare())
	{
		// 地块
		if (m_bShowLayer0)
		{
			drawLayer0();
		}
		if (m_bShowLayer1)
		{
			drawLayer1();
		}
		if (m_bShowAttribute)
		{
			drawAttribute();
		}
		if (m_bShowGrid)
		{
			drawGrid();
		}

		// 阴影
		//if (LightMapPrepare())
		//{
		//	
		//}
		//LightMapFinish();
		if (m_bShowBrushDecal)
		{
			m_BrushDecal.Render();
		}
	}

	renderGrass();
	//RenderGrid();
}

void CTerrainEditor::drawGrid()
{
	CRenderSystem& R = GetRenderSystem();
	R.setTextureMatrix(0,TTF_DISABLE);
	if (R.prepareMaterial("grid"))
	{
		draw();
		R.finishMaterial();
	}
}

void CTerrainEditor::markEdit()
{
	if(m_setReback.size()>0)
	{
		if(m_setReback.back().empty())
		{
			return;
		}
	}
	m_setReback.resize(m_setReback.size()+1);
}

void CTerrainEditor::doEdit(MAP_EDIT_RECORD& recordIn,MAP_EDIT_RECORD& mapEditRecordOut)
{
	if(recordIn.empty())
	{
		return;
	}

	Pos2D posBegin = recordIn.begin()->first.pos;
	bool bUpdateCube = false;
	bool bUpdateIB = false;

	EditValue editValue;
	for(std::map<EditTarget,EditValue>::iterator it=recordIn.begin();it!=recordIn.end();it++)
	{
		switch(it->first.type)
		{
		case CTerrainBrush::BRUSH_TYPE_TERRAIN_HEIGHT:
			if(mapEditRecordOut.find(it->first)==mapEditRecordOut.end())
			{
				editValue.fHeight = m_TerrainData.getVertexHeight(it->first.pos);
				mapEditRecordOut[it->first] = editValue;
			}
			m_TerrainData.setVertexHeight(it->first.pos,it->second.fHeight);
			break;
		case CTerrainBrush::BRUSH_TYPE_TERRAIN_ATT:
			if(mapEditRecordOut.find(it->first)==mapEditRecordOut.end())
			{
				editValue.uAtt = m_TerrainData.getCellAttribute(it->first.pos);
				mapEditRecordOut[it->first] = editValue;
			}
			m_TerrainData.setCellAttribute(it->first.pos,it->second.uAtt);
			break;
		case CTerrainBrush::BRUSH_TYPE_TERRAIN_TEXTURE_ALPHA_PAINT:
			if(mapEditRecordOut.find(it->first)==mapEditRecordOut.end())
			{
				editValue.color = m_TerrainData.getVertexColor(it->first.pos).c;
				mapEditRecordOut[it->first] = editValue;
			}
			m_TerrainData.setVertexColor(it->first.pos,it->second.color);
			break;
		case CTerrainBrush::BRUSH_TYPE_TERRAIN_TEXTURE_PAINT:
			if(mapEditRecordOut.find(it->first)==mapEditRecordOut.end())
			{
				editValue.tile.id = m_TerrainData.GetCellTileID(it->first.pos,it->second.tile.layer);
				mapEditRecordOut[it->first] = editValue;
			}
			m_TerrainData.SetCellTileID(it->first.pos,it->second.tile.id, it->second.tile.layer);
			break;
		case CTerrainBrush::BRUSH_TYPE_MAX:
			break;
		default:
			break;
		}

		std::map<EditTarget,EditValue>::iterator itNext = it;
		itNext++;

		if(recordIn.end()==itNext||it->first.type!=itNext->first.type||it->first.pos.y!=itNext->first.pos.y)
		{
			switch(it->first.type)
			{
			case CTerrainBrush::BRUSH_TYPE_TERRAIN_HEIGHT:
			case CTerrainBrush::BRUSH_TYPE_TERRAIN_TEXTURE_ALPHA_PAINT:
				if(posBegin.x<it->first.pos.x)
				{
					updateVB(posBegin,it->first.pos);
				}
				else
				{
					updateVB(it->first.pos,posBegin);
				}
				break;
			default:
				break;
			}

			switch(it->first.type)
			{
			case CTerrainBrush::BRUSH_TYPE_TERRAIN_HEIGHT:
				bUpdateCube = true;
				bUpdateIB = true;
				break;
			case CTerrainBrush::BRUSH_TYPE_TERRAIN_ATT:
			case CTerrainBrush::BRUSH_TYPE_TERRAIN_TEXTURE_PAINT:
				bUpdateIB = true;
				break;
			default:
				break;
			}

			if(recordIn.end()!=itNext)
			{
				posBegin = itNext->first.pos;
			}
		}
	}
	if(bUpdateCube)
	{
		UpdateCubeBBox(m_RootCube);
	}
	if(bUpdateIB)
	{
		updateIB();
	}
}

void CTerrainEditor::doEdit(MAP_EDIT_RECORD& mapEditRecordIn)
{
	if(mapEditRecordIn.empty())
	{
		return;
	}
	if(m_setReback.empty())
	{
		m_setReback.resize(m_setReback.size()+1);
	}
	doEdit(mapEditRecordIn,*m_setReback.rbegin());
	m_setRedo.clear();
}

void CTerrainEditor::doEdit(std::vector<MAP_EDIT_RECORD>& mapEditRecordIn,std::vector<MAP_EDIT_RECORD>& mapEditRecordOut)
{
	if(mapEditRecordIn.empty())
	{
		return;
	}
	mapEditRecordOut.resize(mapEditRecordOut.size()+1);
	doEdit(mapEditRecordIn.back(),mapEditRecordOut.back());
	mapEditRecordIn.pop_back();
}

void CTerrainEditor::rebackEdit()
{
	doEdit(m_setReback,m_setRedo);
}

void CTerrainEditor::redoEdit()
{
	doEdit(m_setRedo,m_setReback);
}

void CTerrainEditor::Brush(float fPosX, float fPosY)
{
	MAP_EDIT_RECORD mapEditRecord;
	if (m_TerrainData.isCellIn(Pos2D(fPosX, fPosY)))
	{
		// 给更新纹理用的
		float fRadius = m_BrushDecal.GetRadius()*m_BrushDecal.GetSize();
		if (CTerrainBrush::BRUSH_TYPE_TERRAIN_TEXTURE_PAINT == m_BrushDecal.GetBrushType()||
			CTerrainBrush::BRUSH_TYPE_TERRAIN_ATT == m_BrushDecal.GetBrushType())
		{
			fPosX -=0.5f;
			fPosY -=0.5f;
		}
		EditTarget editTarget;
		EditValue editValue;
		editTarget.type = m_BrushDecal.GetBrushType();
		for (int y=fPosY-fRadius; y<fPosY+fRadius; y++)
		{
			for (int x=fPosX-fRadius; x<fPosX+fRadius; x++)
			{
				Pos2D posCell(x,y);
				editTarget.pos = posCell;
				if (m_TerrainData.isCellIn(posCell))
				{
					Vec2D vLength(fPosX-x, fPosY-y);
					float fOffset = 1.0f-vLength.length()/fRadius;
					fOffset = min(fOffset/(1.0f-m_BrushDecal.GetHardness()),1.0f);
					if (fOffset>0)
					{
						switch(m_BrushDecal.GetBrushType())
						{
						case CTerrainBrush::BRUSH_TYPE_TERRAIN_HEIGHT:
							{
								float fHeight = m_TerrainData.getVertexHeight(posCell);
								switch(m_BrushDecal.getBrushHeightType())
								{
								case CTerrainBrush::BHT_NORMAL:
									{
										fHeight +=fOffset*m_BrushDecal.GetStrength();
									}
									break;
								case CTerrainBrush::BHT_SMOOTH:
									{
										float fRate = fOffset/fRadius;
										fHeight = fHeight*fRate+
											(m_TerrainData.getVertexHeight(Pos2D(x+1,y))+
											m_TerrainData.getVertexHeight(Pos2D(x-1,y))+
											m_TerrainData.getVertexHeight(Pos2D(x,y+1))+
											m_TerrainData.getVertexHeight(Pos2D(x,y-1)))
											*0.25f*(1.0f-fRate);
									}
									break;
								}
								editValue.fHeight = min(max(fHeight,m_BrushDecal.getHeightMin()),m_BrushDecal.getHeightMax());
								mapEditRecord[editTarget]=editValue;
							}
							break;
						case CTerrainBrush::BRUSH_TYPE_TERRAIN_ATT:
							{
								editValue.uAtt = m_BrushDecal.GetAtt();
								mapEditRecord[editTarget]=editValue;
							}
							break;
						case CTerrainBrush::BRUSH_TYPE_TERRAIN_TEXTURE_ALPHA_PAINT:
							{
								Color32 color = m_TerrainData.getVertexColor(posCell);
								float fRate=fOffset*m_BrushDecal.GetStrength();
								color.a = min(max(color.a+fRate*255,0),255);
								editValue.color = color.c;
								mapEditRecord[editTarget]=editValue;
							}
							break;
						case CTerrainBrush::BRUSH_TYPE_TERRAIN_TEXTURE_PAINT:
							{
								int nTileID = 0xFF;
								if(m_BrushDecal.GetStrength()>0)
								{
									nTileID = m_BrushDecal.GetTileID(0,0);
								}
								editValue.tile.id=nTileID;
								editValue.tile.layer=m_BrushDecal.GetTileLayer();
								mapEditRecord[editTarget]=editValue;

								if(m_BrushDecal.GetTileLayer()>0)
								{
									Color32 color = m_TerrainData.getVertexColor(posCell);
									float fRate=fOffset*m_BrushDecal.GetStrength();
									color.a = min(max(color.a+fRate*255,0),255);
									m_TerrainData.setVertexColor(posCell,color);
								}
							}
							break;
						default:
							break;
						}
					}
				}
			}
		}
	}
	doEdit(mapEditRecord);
}


void CTerrainEditor::CreateIB()
{
	S_DEL(m_pIB);
	m_pIB = GetRenderSystem().GetHardwareBufferMgr().CreateIndexBuffer(m_TerrainData.GetCellCount()*3*6*sizeof(uint16),CHardwareIndexBuffer::IT_16BIT,CHardwareBuffer::HBU_WRITE_ONLY);
}

void CTerrainEditor::updateIB()
{
	CTerrain::updateIB();
	m_mapRenderAttributeSubs.clear();
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

					const uint8 uAttribute = m_TerrainData.getCellAttribute(posCell);
					if (uAttribute!=0x0)
					{
						m_mapRenderAttributeSubs[uAttribute].myVertexIndex(uIndex);
					}
				}
			}
		}
		uint32 m_uShowAttributeIBCount = 0;

		for (std::map<uint8,TerrainSub>::iterator it = m_mapRenderAttributeSubs.begin(); it!=m_mapRenderAttributeSubs.end(); it++)
		{
			it->second.istart = m_uShowTileIBCount+m_uShowAttributeIBCount;
			m_uShowAttributeIBCount += it->second.icount;
			it->second.icount = 0;
			it->second.vcount=it->second.vcount-it->second.istart+1+m_TerrainData.GetVertexXCount()+1;
		}

		uint32 uTempVertexXCount = m_TerrainData.GetVertexXCount();
		uint16* index = (uint16*)m_pIB->lock(m_uShowTileIBCount*sizeof(uint16), m_uShowAttributeIBCount*sizeof(uint16),CHardwareBuffer::HBL_NO_OVERWRITE);
		for (LIST_CUBES::iterator it=m_RenderCubesList.begin(); it!=m_RenderCubesList.end(); it++)
		{
			const BBox& bbox = (*it)->bbox;
			for (size_t y = bbox.vMin.z; y<bbox.vMax.z; ++y)
			{
				for (size_t x = bbox.vMin.x; x<bbox.vMax.x; ++x)
				{
					Pos2D posCell=Pos2D(x,y);
					uint32 uIndex = m_TerrainData.GetVertexIndex(posCell);
		
					const uint8 uAttribute = m_TerrainData.getCellAttribute(posCell);
					if (uAttribute!=0x0)
					{
						TerrainSub& sub = m_mapRenderAttributeSubs[uAttribute];
						{
							// 2	 3
							//	*---*
							//	| / |
							//	*---*
							// 0	 1
							uint16* p = index+sub.istart+sub.icount-m_uShowTileIBCount;
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
		m_pIB->unlock();
	}
}