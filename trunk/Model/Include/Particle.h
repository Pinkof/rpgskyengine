#pragma once
#include "Manager.h"
#include "matrix.h"
#include "Material.h"
#include "RenderNode.h"

struct Particle
{
	Vec3D vPos, vSpeed, vDown, vOrigin, vDir;
	Matrix mFace;
	float fSize, fLife, fMaxLife;
	int nTile;
	Color32 color;
};

typedef std::list<Particle> ParticleList;

class CParticleData;
class CParticleEmitter:public CRenderNode
{
public:
	CParticleEmitter()
		:m_pData(NULL)
		,m_nTime(0)
		,m_fRem(0)
	{
	}
	virtual int		getType		() {return NODE_PARTICLE;}
	void			init		(CParticleData* pData);
	virtual void	frameMove	(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void	render		(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType=MATERIAL_NORMAL)const;
	void			update		(const Matrix& mWorld, CParticleData& particleData, float fElapsedTime);
public:
	CParticleData*		m_pData;		// ���ӷ�����ָ��
	ParticleList		m_Particles;	// ���Ӷ�
	int					m_nTime;		// ʱ��
	float				m_fRem;			// �������ٵ�λ���� ������
};
