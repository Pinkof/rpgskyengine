#pragma once
#include "modelheaders.h"
#include "InterfaceModel.h"

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

class CSkeleton:public iSkeleton
{
public:
	uint8 getIDByName(const std::string& strName);
	bool CreateBones(std::vector<CBone>& bones);
	//void GetBoneMatrix(int nBoneID, int time, bool bRotate=true);
	void CalcBonesMatrix(int time, std::vector<CBone>& bones);
	void Render(const std::vector<CBone>& bones)const;
	CNodeData* Save(CNodeData& lump, const char* name);
	CNodeData* Load(CNodeData& lump, const char* name);
};