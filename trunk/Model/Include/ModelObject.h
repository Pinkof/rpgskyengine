#pragma once
#include "ModelData.h"
#include "AnimMgr.h"

class DLL_EXPORT CModelObject
{
public:
	CModelObject();
	~CModelObject();
public:
	void SkinAnim();	// ��������
	virtual void OnFrameMove(float fElapsedTime);
public:
	void SetLOD(uint32 uLodID);		// ����LodID
	void SetSkin(uint32 uSkinID);	// ����Ƥ��
	void SetAnim(const std::string& strAnimName);
	void SetLightMap(const std::string& strFilename);	// SetLightMap
public:
	void Register(const std::string& strFilename);
	bool load(const std::string& strFilename);
	bool Prepare()const;

	virtual void drawMesh			(E_MATERIAL_RENDER_TYPE eModelRenderType=MATERIAL_RENDER_NORMAL)const;
	virtual void renderMesh			(E_MATERIAL_RENDER_TYPE eModelRenderType=MATERIAL_RENDER_NORMAL)const;
	virtual void renderParticles	(E_MATERIAL_RENDER_TYPE eParticleRenderType=MATERIAL_RENDER_NORMAL)const;
	virtual void render				(E_MATERIAL_RENDER_TYPE eModelRenderType=MATERIAL_RENDER_NORMAL,E_MATERIAL_RENDER_TYPE eParticleRenderType=MATERIAL_RENDER_NORMAL)const;

	virtual void drawSkeleton()const;

	void updateEmitters(const Matrix& mWorld, float fElapsedTime);
	void SetLodLevel(int nLodLevel);

	//void setupAtt(int id);
	//void setupAtt2(int id);

	void Animate(const std::string& strAnimName);
	void CalcBones(const std::string& strAnim, int time);

	bool isCreated();
	void create();

	const BBox& getBBox()const;
	int getModelDataID()const;
	CModelData* getModelData()const;
protected:
	int m_nModelID;									// ģ��ID
	CModelData*		m_pModelData;					// ģ��Դ����
public:												// All the anim data.
	SingleAnimNode				m_AnimMgr;			// ����������
	CHardwareVertexBuffer*		m_pVB;				// ���㻺��
	std::vector<Matrix>			m_setBonesMatrix;	// ��������
	std::vector<CParticleGroup>	m_setParticleGroup;	// ����ID����
	std::string	m_strAnimName;						// Current Name Of Animate
	int		m_nAnimTime;							// ����ʱ��֡
	float	m_fTrans;								// ����͸����
	float	m_fAlpha;								// ����͸��
protected:											// Attribute.
	float	m_fRad;									// Radius
	uint32	m_uLodLevel;							// Current Lod Level
	uint32	m_uSkinID;								// Current Skin ID
	OBJID	m_idLightMapTex;						//
	bool	m_bLightmap;
	bool	m_bCreated;
protected:
	//LightAnim		*lights;
	//RibbonEmitter	*ribbons;
	//void lightsOn(int lbase);
	//void lightsOff(int lbase);
	BBox m_BBox;
public:
	const std::string& getModelFilename(){return m_strModelFilename;}
protected:
	std::string m_strModelFilename;
};