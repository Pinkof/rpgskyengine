#pragma once
#include "Manager.h"
#include "Modelheaders.h"
#include "Animated.h"
#include "ParticleEmitter.h"
#include "Vec4D.h"
#include "Matrix.h"
#include "Skeleton.h"
#include "LodMesh.h"
#include "Material.h"

class CModelData;
class CBone;

struct TexAnim
{
	Animated<Vec3D> trans, rot, scale;
	// ����
	void Calc(int nTime, Matrix& matrix)const;
};

struct ColorAnim
{
	Animated<Vec3D> color;
	Animated<Vec3D> specular;
	Animated<short> opacity;

	Vec4D GetColor(uint32 uTime)const
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


//////////////////////////////////////////////////////////////////////////
#pragma pack(push) // ����ǰpack����ѹջ���� 
#pragma pack(1) // �����ڽṹ�嶨��֮ǰʹ�� 
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
bool bUseTex2, bTrans, bUnlit, bNoZWrite;

// colours
Vec4D ocol, ecol;

std::string strMaterialName;

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

//////////////////////////////////////////////////////////////////////////

class DLL_EXPORT CModelData:public iModelData
{
public:
	CModelData();
	~CModelData();
public:
	virtual const std::string& getItemName(){return m_strModelFilename;}

	virtual void addAnimation(long timeStart, long timeEnd);
	virtual size_t getRenderPassCount();
	virtual void setRenderPass(int nID, int nSubID, const std::string& strMaterialName);
	virtual bool getRenderPass(int nID, int& nSubID, std::string& strMaterialName)const;
	virtual bool delRenderPass(int nID);
	virtual CMaterial& getMaterial(const std::string& strMaterialName);
	virtual	iLodMesh& getMesh(){return m_Mesh;}
	virtual iSkeleton& getSkeleton(){return m_Skeleton;}

	virtual bool LoadFile(const std::string& strFilename);
	virtual bool saveMaterial(const std::string& strFilename);
	virtual bool initParticleMaterial();
	virtual	bool loadParticleEmitters(const std::string& strFilename);
	bool SaveFile(const std::string& strFilename);
	void InitSkins();
	void Init();
	int	GetOrder();
	bool isLoaded();
private:
	bool passBegin(const ModelRenderPass& pass, float fOpacity, int nAnimTime)const;
	void passEnd()const;
public:
	void renderMesh(E_MATERIAL_RENDER_TYPE eModelRenderType, size_t uLodLevel, CHardwareVertexBuffer* pSkinVB, float fOpacity, int nAnimTime)const;
	void drawMesh(E_MATERIAL_RENDER_TYPE eModelRenderType, size_t uLodLevel, CHardwareVertexBuffer* pSkinVB=NULL)const;
	void drawMesh(CHardwareVertexBuffer* pSkinVB=NULL)const;
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

	std::vector<CParticleEmitter>	m_setParticleEmitter;	// Particle Emitters
	//std::vector<CRibbonEmitter>	ribbons;			// ����Դ
private:
	std::map<int,ModelRenderPass>m_mapPasses;				// ��Ⱦ���̼�
public:
	std::vector<ModelAttachment> atts;
	int attLookup[40];
	int boneLookup[27];
public:
	int m_nOrder;
	bool bLoaded;
private:
	std::string m_strModelFilename;
};