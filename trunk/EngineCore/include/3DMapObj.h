#pragma once
#include "MapObj.h"
#include "ModelObject.h"

class C3DMapObj : public CMapObj
{
public:
	C3DMapObj();
	~C3DMapObj();
protected:
	Pos2D m_posCell;
	CModelObject	*m_pModelObject;
public:// for mapobj
	virtual void GetCellPos( Pos2D& posCell );
	virtual void SetCellPos( Pos2D& posCell );

	virtual void Process(void* pInfo){;}
	virtual int  GetObjType(){return MAP_3DOBJ;}
	virtual BOOL IsFocus(){return false;}

	virtual Matrix getWorldMatrix()const;
	virtual BBox getBBox()const;
	virtual void render(int flag)const;
	virtual void renderFocus()const;
	virtual void renderDebug()const;
	virtual void OnFrameMove(float fElapsedTime);

	virtual void drawWithoutMaterial()const;
	virtual bool intersect(const Vec3D& vRayPos , const Vec3D& vRayDir, float &tmin ,float &tmax);

	bool isSkinMesh();
protected:
	void renderFocus(Color32 color)const;
};