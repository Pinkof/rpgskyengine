#pragma once
#include "RenderNode.h"
#include "AnimMgr.h"

class CTextRender;
class CSkeletonData;
class CSkeletonNode:public CRenderNode
{
public:
	CSkeletonNode();
	~CSkeletonNode();
public:
	virtual int			getType			() {return NODE_SKELETON;}
	virtual void		frameMove		(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void		render			(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType=MATERIAL_NORMAL)const;
	CSkeletonData*		getSkeletonData	(){return m_pSkeletonData;}
	void				setSkeletonData	(CSkeletonData* pSkeletonData);
	virtual void		animate			(float fElapsedTime);
	void				setAnim			(int nID);
	void				setAnim			(const char* szAnimName);
	void				Animate			(const char* szAnimName);
	void				CalcBones		(const char* szAnim, int time);
	virtual void		drawSkeleton	(CTextRender* pTextRender)const;
	std::vector<Matrix>&getBonesMatrix	(){return m_setBonesMatrix;}
protected:
	SingleAnimNode		m_AnimMgr;			// ����������
	std::vector<Matrix>	m_setBonesMatrix;	// ��������
	std::string			m_strAnimName;		// Current Name Of Animate
	int					m_nAnimTime;		// ����ʱ��֡
	CSkeletonData*		m_pSkeletonData;	// 
};