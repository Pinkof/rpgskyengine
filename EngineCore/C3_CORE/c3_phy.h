#pragma once
#include "c3_Common.h"
#include "HardwareIndexBuffer.h"
#include "HardwareVertexBuffer.h"
#include "Common.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "IORead.h"
#include "c3_key.h"
#include "Vec2D.h"
#include "Frustum.h"

/* �������嶥�㶨�� */
#define PHY_OUT_VERTEX (FVF_XYZ | FVF_DIFFUSE | FVF_TEX1)
struct PhyOutVertex
{
	Vec3D							p;
	//float							nx, ny, nz;
	uint32							color;
	Vec2D							t;
};

#define		_BONE_MAX_				2
#define		_MORPH_MAX_				4

struct PhyVertex
{
	Vec3D							pos[_MORPH_MAX_];
	Vec2D							t;
	//float							nx, ny, nz;
	uint32							color;
	uint32							index[_BONE_MAX_];
	float							weight[_BONE_MAX_];
};

struct C3KeyFrame
{
	uint32							pos;
	Matrix							*matrix;
};

class DLL_EXPORT C3Motion
{
public:
	uint32							dwBoneCount;		// ������
	uint32							dwFrames;			// ֡��

	uint32							dwKeyFrames;
	C3KeyFrame						*lpKeyFrame;

	Matrix							*matrix;

	uint32							dwMorphCount;
	float							*lpMorph;
	int								nFrame;				// ��ǰ֡
public:
	C3Motion();
	~C3Motion();
	static C3Motion* New(IOReadBase& file);
	bool Load(IOReadBase& file);
	bool Save(char *lpName, bool bNew);
	void GetMatrix(uint32 dwBone, Matrix *lpMatrix);
};

class DLL_EXPORT C3Phy
{
public:
	char							*lpName;			// ����
	uint32							dwBlendCount;		// ÿ�������ܵ����ٸ�����Ӱ��
	uint32							dwNVecCount;		// ������(��ͨ����)
	uint32							dwAVecCount;		// ������(͸������)
	PhyVertex						*lpVB;				// �����(��ͨ����/͸������)
	CHardwareVertexBuffer*			vb;
	uint32							dwNTriCount;		// �������(��ͨ�����)
	uint32							dwATriCount;		// �������(͸�������)
	uint16							*lpIB;				// ������(��ͨ�����/͸�������)
	CHardwareIndexBuffer*			ib;
	std::string						strTexName;			// [ be used by plugin ]
	int								nTex;
	int								nTex2;
	BBox							bbox;
	C3Motion						*lpMotion;
	float							fA, fR, fG, fB;
	C3Key							Key;
	bool							bDraw;
	uint32							dwTexRow;
	Matrix							InitMatrix;
	// 12-20-2002 : "STEP"
	Vec2D						uvstep;
public:
	C3Phy();
	~C3Phy();
	static C3Phy* New(IOReadBase& file, bool bTex = false);
	bool Load(IOReadBase& file, bool bTex = false);
	bool Save(char *lpName, bool bNew);
	static void Prepare();
	bool Calculate();
	bool DrawNormal();
	bool DrawAlpha(bool bZ = false, int nAsb=5, int nAdb=6);
	void NextFrame(int nStep);
	void SetFrame(uint32 dwFrame);
	void Muliply(int nBoneIndex, Matrix *matrix);
	void SetColor(float alpha, float red, float green, float blue);
	void ClearMatrix();
	void ChangeTexture(int nChangeTexID, int nChangeTexID2 = 0);
};
