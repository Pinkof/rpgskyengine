#pragma once
#include "Manager.h"
#include "matrix.h"
#include "Material.h"
#include "RenderNode.h"

struct Particle
{
	Vec3D vPos, vSpeed, vDown, vOrigin, vDir;
	Matrix mFace;
	//Vec3D tpos;
	float fSize, fLife, fMaxLife;
	int nTile;
	Color32 color;
};
typedef std::list<Particle> ParticleList;

class CParticleEmitter;
class CParticleGroup:public CRenderNode
{
public:
	CParticleGroup()
		:m_pEmitter(NULL)
		,m_nTime(0)
		,m_fRem(0)
	{
	}
	virtual int		getType		() {return NODE_PARTICLE;}
	void			init		(CParticleEmitter* pEmitter);
	virtual void	frameMove	(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void	render		(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType=MATERIAL_NORMAL)const;
public:
	CParticleEmitter*	m_pEmitter;		// ���ӷ�����ָ��
	ParticleList		m_Particles;	// ���Ӷ�
	int					m_nTime;		// ʱ��
	float				m_fRem;			// �������ٵ�λ���� ������
};
