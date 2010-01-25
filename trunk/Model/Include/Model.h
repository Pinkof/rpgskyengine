#pragma once
#include "Manager.h"
#include "Modelheaders.h"
#include "Animated.h"
#include "enums.h"
#include "ParticleEmitter.h"


#include "Vec4D.h"
#include "Matrix.h"


#include "Skeleton.h"
#include "LodMesh.h"

class CModel;
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

#pragma pack(push) // ����ǰpack����ѹջ���� 
#pragma pack(1) // �����ڽṹ�嶨��֮ǰʹ�� 
struct CMaterial
{
	std::string strDiffuse;
	std::string strEmissive;
	std::string strSpecular;
	std::string strBump;
	std::string strReflection;
	std::string strEffect;
	std::string strLightMap;

	uint32	uDiffuse;
	uint32	uEmissive;
	uint32	uSpecular;
	uint32	uBump;
	uint32	uReflection;
	uint32	uLightMap;
	uint32	uEffect;
	bool	bAlphaTest;
	uint8	uAlphaTestValue;
	bool	bBlend;
	bool	bCull;
	Vec2D	vTexAnim;
	float	m_fOpacity;
	Color32 cEmissive;

	CMaterial():
	uDiffuse(0),
	uEmissive(0),
	uSpecular(0),
	uBump(0),
	uReflection(0),
	uLightMap(0),
	uEffect(0),
	bAlphaTest(false),
	uAlphaTestValue(0x80),
	bBlend(false),
	bCull(true),
	m_fOpacity(1.0f),
	cEmissive(255,255,255,255)
	{
	}

	void SetEmissiveColor(Color32 color)
	{
		cEmissive = color;
	}

	bool isGlow();

	bool Begin(float fOpacity);
};

struct ModelRenderPass
{
	ModelRenderPass():
nRenderFlag(0),
nBlendMode(0),
nTransID(-1),
nTexanimID(-1),
nColorID(-1),
nOrder(0),
p(0)
{
}
	// RenderFlag;
	uint16 nRenderFlag;	//
	uint16 nBlendMode;	//
	int nTransID,nTexanimID,nColorID;
	int nOrder;
	float p;

	// Geoset ID
	int nSubID;
	//
	bool bUseTex2, bUseEnvMap, bCull, bTrans, bUnlit, bNoZWrite;
	bool bHasAlphaTex;
	// texture wrapping
	bool bSwrap, bTwrap;

	// colours
	Vec4D ocol, ecol;

	CMaterial material;
	bool operator< (const ModelRenderPass &m) const
	{
		// sort order method
		if (nOrder!=m.nOrder)
			return nOrder<m.nOrder;
		else
			return nBlendMode == m.nBlendMode ? (p<m.p) : (nBlendMode<m.nBlendMode);
	}
};
#pragma pack(pop) // �ָ���ǰ��pack���� 

struct ModelAttachment
{
	int id;
	Vec3D pos;
	int bone;
	CModel *model;

	void setup();
	void setupParticle();
};

class DLL_EXPORT CModel
{
public:
	CModel();
	~CModel();
public:
	virtual bool LoadFile(const std::string& strFilename);
	virtual	bool loadMaterial(const std::string& strFilename,const std::string& strPath);
	virtual bool saveMaterial(const std::string& strFilename);
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

	std::vector<ModelRenderPass>m_Passes;				// ��Ⱦ���̼�
	std::vector<CParticleEmitter>	m_setParticleEmitter;	// Particle Emitters
	//std::vector<CRibbonEmitter>	ribbons;			// ����Դ
public:
	bool m_bHasAlphaTex;	// �Ƿ���ALPHA͸������
public:
	ModelType	m_ModelType;	// ģ�����
 	BBox		m_bbox;				//
public:
	std::vector<ModelAttachment> atts;
	int attLookup[40];
	int boneLookup[27];
public:
	int m_nOrder;
	bool bLoaded;
};

class CModelMgr: public CManager<CModel>
{
public:
	uint32 RegisterModel(const std::string& strFilename);
	CModel* GetModel(uint32 uModelID);
};

CModelMgr& GetModelMgr();
