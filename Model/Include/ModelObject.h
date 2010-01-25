#pragma once

#include "Model.h"
#include "AnimMgr.h"

enum E_MODEL_RENDER_TYPE
{
	MODEL_RENDER_NOTHING		= 0,
	MODEL_RENDER_MESH_GEOMETRY	= 1<<0,
	MODEL_RENDER_MESH_ALPHA		= 1<<1,
	MODEL_RENDER_MESH_GLOW		= 1<<2,
	MODEL_RENDER_PARTICLES_ALPHA= 1<<3,
	MODEL_RENDER_PARTICLES_GLOW	= 1<<4,
	MODEL_RENDER_NORMAL			= 0xFF,
	MODEL_RENDER_ALL			= 0x7FFFFFFF,
};

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
	void SetAnim(uint32 uAnimID);	// ���ö���ID
	void SetLightMap(const std::string& strFilename);	// SetLightMap
	void SetMaterial(const Vec4D& vAmbient, const Vec4D& vDiffuse);
public:
	void Register(const std::string& strFilename);
	bool load(const std::string& strFilename);
	bool Prepare()const;
	bool PassBegin(ModelRenderPass& pass)const;	// ��Ⱦ����
	void PassEnd()const;

	virtual void DrawSubsHasNoAlphaTex()const;
	bool PrepareEdge()const;
	void FinishEdge()const;
	virtual void DrawModelEdge()const;
	virtual void render(E_MODEL_RENDER_TYPE eModelRenderType=MODEL_RENDER_NORMAL)const;

	virtual void draw()const;

	virtual void DrawBones()const;		// ��������

	void updateEmitters(const Matrix& mWorld, float fElapsedTime);	// ����������
	void SetLodLevel(int nLodLevel);	// ����LOD�ȼ�

	//void setupAtt(int id);
	//void setupAtt2(int id);
	//void AnimReset() { 
	//	m_bAnimCalc = false; 
	//}
	void Animate(int anim);
	void CalcBones(int time);

	bool isCreated();
	void create();
public: // ģ������Դ
	int m_nModelID;		// ģ��ID
	CModel*			m_pModel;				// ģ��Դ����
	CSkeleton*		m_pSkeleton;			// �Ǽ�
	CLodMesh*		m_pMesh;
public: // ģ�Ͷ�����������
	AnimManager*			m_AnimMgr;	// ����������
	CHardwareVertexBuffer*	m_pVB;	// ���㻺��
	std::vector<CBone>		m_Bones;	// ����
	std::vector<CParticleGroup>	m_setParticleGroup;	// ����ID����
	int m_nCurrentAnimID;	// ��ǰ����ID
	int m_nAnim;			// ��ǰ����ID
	int m_nAnimTime;		// ����ʱ��֡
	float m_fTrans;			// ����͸����
	float m_fAlpha;			// ����͸��
public:// ����
	virtual void renderParticles()const;	// ������
	// ������ʾ����
	void ShowGeoset(uint32 uID, bool bShow = true){ if (uID<m_setShowSubset.size()) m_setShowSubset[uID] = bShow;}
	// ������ʾ����
	void ShowParticle(uint32 uID, bool bShow = true){ if (uID<m_setShowParticle.size()) m_setShowParticle[uID] = bShow;}
protected: 
	ModelType m_ModelType;		// ģ�����
	float	m_fRad;				// �뾶
	uint32	m_uLodID;			// ��ǰLodID
	int		m_nLodLevel;		// LOD �ȼ�
	uint32	m_uSkinID;			// Ƥ��I
	OBJID	m_idLightMapTex;	//
	bool	m_bLightmap;
	std::vector<ModelRenderPass>* m_pPasses;	// ��Ⱦ���̼�
	std::vector<bool> m_setShowParticle;			// ��ʾ����

	bool	m_bCreated;
protected:
	Vec4D m_vAmbient;
	Vec4D m_vDiffuse;
	bool m_bCartoonRender;	// ���
	bool m_bRenderEdge;		// ��Ⱦ��ͨ��Ե
	bool m_bRenderCartoon;	// ��Ⱦ��ͨ��Ե

	//LightAnim		*lights;

	//RibbonEmitter	*ribbons;

	//void lightsOn(int lbase);
	//void lightsOff(int lbase);
public:
	const std::string& getModelFilename(){return m_strModelFilename;}
protected:
	std::string m_strModelFilename;
	std::vector<bool> m_setShowSubset;	// ��ʾ����
};