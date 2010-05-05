#pragma once
#include "modelheaders.h"
#include "InterfaceModel.h"

// �����ࣺ����������������Ϣ�����㼰����
class CBone
{
public:
	CBone* m_pParent;
	// ����ת������
	Matrix m_mat;
	// ��ת�仯����--���ڷ�������
	Matrix m_mRot;
	// ת��������ĵ�
	Vec3D m_vTransPivot;
	// �Ƿ�������
	bool m_bCalc;
	// ���㶯��֡����
	void CalcMatrix1(const BoneInfo& boneInfo,const BoneAnim& boneAnim,int time);
	void CalcMatrix2();
};

class CSkeleton:public iSkeleton
{
public:
	virtual size_t getAnimationCount();
	//virtual void setAnimation(const std::string& strName, long timeCount);
	virtual bool getAnimation(const std::string& strName, long& timeCount)const;
	virtual bool getAnimation(size_t index, std::string& strName, long& timeCount)const;
	virtual bool delAnimation(const std::string& strName);

	uint8 getIDByName(const std::string& strName);
	bool CreateBones(std::vector<CBone>& bones);
	//void GetBoneMatrix(int nBoneID, int time, bool bRotate=true);
	void CalcBonesMatrix(const std::string& strAnim, int time, std::vector<CBone>& bones);
	void Render(const std::vector<CBone>& bones)const;
	CNodeData* Save(CNodeData& lump, const char* name);
	CNodeData* Load(CNodeData& lump, const char* name);
};