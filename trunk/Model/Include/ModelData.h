#pragma once
#include "Manager.h"
#include "Modelheaders.h"
#include "Animated.h"
//#include "enums.h"
#include "ParticleEmitter.h"

#include "Vec4D.h"
#include "Matrix.h"

#include "Skeleton.h"
#include "LodMesh.h"

class CModelData;
class CBone;

struct TexAnim
{
	Animated<Vec3D> trans, rot, scale;
	// ����
	void Calc(int nTime, Matrix& matrix);
};

struct ColorAnim
{
	Animated<Vec3D> color;
	Animated<Vec3D> specular;
	Animated<short> opacity;

	Vec4D GetColor(uint32 uTime)
	{
		return Vec4D(color.getValue(uTime), opacity.getValue(uTime)/32767.0f);
	}
};

struct TransAnim // Transparency
{
	Animated<short> trans;
};

struct ModelCamera
{
	bool ok;

	Vec3D pos, target;
	float nearclip, farclip, fov;
	Animated<Vec3D> tPos, tTarget;
	Animated<float> rot;

	void setup(int time=0);

	ModelCamera():ok(false) {}
};

struct LightAnim
{
	int type, parent;
	Vec3D pos, tpos, dir, tdir;
	Animated<Vec3D> diffColor, ambColor;
	Animated<float> diffIntensity, ambIntensity;
	void setup(int time, int l);
};



struct ModelAttachment
{
	int id;
	Vec3D pos;
	int bone;
	CModelData *model;

	void setup();
	void setupParticle();
};


class DLL_EXPORT CModelData:public iModelData
{
public:
	CModelData();
	~CModelData();
public:
	virtual void addAnimation(long timeStart, long timeEnd);
	virtual void setRenderPass(int nID, const std::string& strName,
		const std::string& strDiffuse, const std::string& strEmissive,
		const std::string& strSpecular, const std::string& strNormal,
		const std::string& strEnvironment, const std::string& strShader,
		int nChannel, bool bBlend, bool bAlphaTest, float fTexScaleU, float fTexScaleV);
	virtual	iLodMesh& getMesh(){return m_Mesh;}
	virtual iSkeleton& getSkeleton(){return m_Skeleton;}

	virtual bool LoadFile(const std::string& strFilename);
	virtual	bool loadMaterial(const std::string& strFilename,const std::string& strPath);
	virtual bool saveMaterial(const std::string& strFilename);
	virtual bool loadParticleMaterial(const std::string& strFilename,const std::string& strPath);
	virtual	bool loadParticleEmitters(const std::string& strFilename,const std::string& strPath);
	bool SaveFile(const std::string& strFilename);
	void InitSkins();
	void Init();
	int	GetOrder();
	bool isLoaded();
public:
	CLodMesh				m_Mesh;
 	CBoundMesh				m_BoundMesh;	// ��Χ��
public: // ����Դ
	CSkeleton		m_Skeleton;						// �Ǽ�
	ModelCamera		m_Camera;
	int				*globalSequences;
	
	std::vector<ModelAnimation>	m_AnimList;				// �������Դ
	std::vector<TexAnim>		m_TexAnims;				// ������Դ
	std::vector<ColorAnim>		m_ColorAnims;			// ��ɫ����Դ
	std::vector<TransAnim>		m_TransAnims;			// ͸������Դ
	std::vector<LightAnim>		m_LightAnims;			// �ƹ⶯��Դ

	std::map<int,ModelRenderPass>m_mapPasses;				// ��Ⱦ���̼�
	std::vector<CParticleEmitter>	m_setParticleEmitter;	// Particle Emitters
	//std::vector<CRibbonEmitter>	ribbons;			// ����Դ
public:
	bool m_bHasAlphaTex;	// �Ƿ���ALPHA͸������
public:
//	ModelType	m_ModelType;	// ģ�����
//public:
	std::vector<ModelAttachment> atts;
	int attLookup[40];
	int boneLookup[27];
public:
	int m_nOrder;
	bool bLoaded;
};

typedef struct{
	CModelPlugBase * pObj;
	HINSTANCE hIns;
}MODEL_PLUG_ST, * LPMODEL_PLUG_ST;

class CModelMgr: public CManager<CModelData>
{
public:
	CModelMgr();
	uint32 RegisterModel(const std::string& strFilename);
	CModelData* GetModel(uint32 uModelID);
	bool loadModel(CModelData& modelData,const std::string& strFilename);
private:
	bool loadPlugFromPath(const std::string& strPath);
	bool createPlug(const std::string& strFilename);

	std::vector<MODEL_PLUG_ST> m_arrPlugObj;
};

CModelMgr& GetModelMgr();
