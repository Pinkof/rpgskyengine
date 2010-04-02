#pragma once

#include "Skeleton.h"

class CHardwareVertexBuffer;
class CHardwareIndexBuffer;
class CVertexDeclaration;

struct SkinnedVertex
{
	Vec3D	p;
	Vec3D	n;
};

struct SkinVertex: public SkinnedVertex
{
	SkinVertex()
	{
		w4=0;
		b4=0;
	}
	union{
		uint8	w[4];
		uint32	w4;
	};
	union{
		uint8	b[4];
		uint32	b4;
	};
};

struct ModelLod
{
	ModelLod()
	{
		pIB = NULL;
		nLevel = 0;
	}
	CHardwareIndexBuffer*		pIB;			// 索引缓冲
	std::vector<uint16>			IndexLookup;	// 使用索引查找表可以优化骨骼动画（通过索引表去更新必要的顶点） Vertices in this model (index into vertices[])
	std::vector<uint16>			Indices;		// indices
	std::vector<IndexedSubset>	setSubset;		// 部件
	int32						nLevel;			// LOD等级
	void Load(CLumpNode& lump)
	{
		lump.getVector("IndexLookup",	IndexLookup);
		lump.getVector("Indices",		Indices);
		lump.getVector("Geoset",		setSubset);
	}
	void Save(CLumpNode& lump)
	{
		lump.SetVector("IndexLookup",	IndexLookup);
		lump.SetVector("Indices",		Indices);
		lump.SetVector("Geoset",		setSubset);
	}
};

class CBoundMesh
{
public:
	std::vector<Vec3D>			pos;		// 包围盒
	std::vector<uint16>			indices;	// 包围盒
	void draw()const;
};

class DLL_EXPORT CSubMesh:public iSubMesh
{
public:
	virtual size_t getVertexIndexCount();
	virtual void addVertexIndex(const VertexIndex& faceIndex);
	virtual bool getVertexIndex(size_t n, VertexIndex& faceIndex);

	virtual size_t getPosCount();
	virtual size_t getBoneCount();
	virtual size_t getWeightCount();
	virtual size_t getNormalCount();
	virtual size_t getTexcoordCount();

	virtual void addPos(const Vec3D& vPos);
	virtual void addBone(uint32 uBone);
	virtual void addWeight(uint32 uWeight);
	virtual void addNormal(const Vec3D& vNormal);
	virtual void addColor(const Color32& clr);
	virtual void addTexcoord(const Vec2D& vUV);

	virtual void setPos(size_t n, const Vec3D& vPos);
	virtual void setBone(size_t n, uint32 uBone);
	virtual void setWeight(size_t n, uint32 uWeight);
	virtual void setNormal(size_t n, const Vec3D& vNormal);
	virtual void setTexcoord(size_t n, const Vec2D& vUV);

	virtual void getPos(size_t n, Vec3D& vPos);
	virtual void getBone(size_t n, uint32& uBone);
	virtual void getWeight(size_t n, uint32& uWeight);
	virtual void getNormal(size_t n, Vec3D& vNormal);
	virtual void getTexcoord(size_t n, Vec2D& vUV);

	bool intersect(const Vec3D& vRayPos , const Vec3D& vRayDir, Vec3D& vOut)const;

	// Vertex Data
	std::vector<Vec3D>	pos;
	std::vector<uint32>	weight;
	std::vector<uint32>	bone;
	std::vector<Vec3D>	normal;
	std::vector<Color32>color;
	std::vector<Vec2D>	texcoord;
	std::vector<Vec2D>	texcoord2;

	std::vector<VertexIndex> m_setVertexIndex;
};

class DLL_EXPORT CLodMesh:public iLodMesh
{
public:
	CLodMesh();
	~CLodMesh();
public:
	virtual int getSubCount();
	virtual iSubMesh& addSubMesh();
	virtual iSubMesh* getSubMesh(size_t n);
	virtual const BBox& getBBox();
	virtual void update();

	void Init();
	uint32 GetSkinVertexSize();
	bool SetMeshSource(int nLodLevel=0, CHardwareVertexBuffer* pSkinVB=NULL)const;
	void drawSub(size_t uSubID, size_t uLodLevel=0)const;
	void draw(size_t uLodLevel=0)const;
	void skinningMesh(CHardwareVertexBuffer* pVB, std::vector<CBone>& bones)const;
	void InitBBox();
	void Clear();
	size_t GetSkinVertexCount(){return m_setSkinVertex.size();}

	void load(CLumpNode& lump);
	void save(CLumpNode& lump);
	bool intersect(const Vec3D& vRayPos , const Vec3D& vRayDir, Vec3D& vOut, int& nSubID)const;
	bool intersect(const Vec3D& vRayPos , const Vec3D& vRayDir)const;
protected:
public:
	std::vector<CSubMesh> m_setSubMesh;

	CHardwareVertexBuffer*	m_pShareBuffer;	// Share Vertex Buffer
	std::vector<ModelLod>	m_Lods;			// the lods

	uint16					m_uSkinVertexSize;
	uint16					m_uShareVertexSize;

	CVertexDeclaration*		m_pVertexDeclHardware;	// FVF

	bool		m_bSkinMesh;		// is skin mesh?
	float		m_fRad;				// 半径
	BBox		m_bbox;				//
protected:
	std::vector<SkinVertex>	m_setSkinVertex;
};

class DLL_EXPORT CMeshCoordinate:public CLodMesh
{
public:
	CMeshCoordinate();
	~CMeshCoordinate();
public:
	void init();
	void setPos(const Vec3D& vPos);
	void setScale(float fScale);
	void render(const Vec3D& vCoordShow);
	bool intersect(const Vec3D& vRayPos , const Vec3D& vRayDir,Vec3D& vCoord)const;
protected:
	Matrix getWorldMatrix()const;
	enum CoordLineType
	{
		CLT_X,
		CLT_X_Y,
		CLT_X_Z,

		CLT_Y,
		CLT_Y_X,
		CLT_Y_Z,

		CLT_Z,
		CLT_Z_X,
		CLT_Z_Y,

		CLT_MAX,
	};
	struct CoordLine
	{
		Vec3D vBegin,vEnd;
	};
	CoordLine m_CoordLines[CLT_MAX];
	Vec3D m_vPos;
	float m_fScale;
};