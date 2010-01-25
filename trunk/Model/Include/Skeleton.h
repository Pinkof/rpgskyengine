#pragma once
#include "modelheaders.h"
#include "Vec4D.h"
#include "Matrix.h"
#include "Animated.h"

struct BoneAnim
{
public:
	BoneAnim():
	parent(0xFF),
	billboard(false)
	{
		mSkin.unit();
	}
	std::string				strName;
	Animated<Vec3D>			trans;
	Animated<Quaternion>	rot;
	Animated<Vec3D>			scale;
	Matrix					mSkin;
	Vec3D pivot;
	uint8 parent;
	bool billboard;
};

// �����ࣺ����������������Ϣ�����㼰����
class CBone
{
public:
	CBone* m_pParent;
	// ����������Ϣָ��
	BoneAnim* m_pBoneAnim;
	// ����ת������
	Matrix m_mat;
	// ��ת�仯����--���ڷ�������
	Matrix m_mRot;
	// ת��������ĵ�
	Vec3D m_vTransPivot;
	// �Ƿ�������
	bool m_bCalc;
	// ���㶯��֡����
	void CalcMatrix(int time, bool bRotate=true);
};

class CSkeleton
{
public:
	std::vector<BoneAnim>			m_BoneAnims;	// ��������Դ
	std::vector<ModelAnimation>		m_AnimList;		// �������Դ
	uint8 getIDByName(const std::string& strName);
	bool CreateBones(std::vector<CBone>& bones);
	//void GetBoneMatrix(int nBoneID, int time, bool bRotate=true);
	void CalcBonesMatrix(int time, std::vector<CBone>& bones);
	void Render(const std::vector<CBone>& bones)const;
	CLumpNode* Save(CLumpNode& lump, const char* name);
	CLumpNode* Load(CLumpNode& lump, const char* name);
};