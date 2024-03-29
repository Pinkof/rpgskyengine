#pragma once
#include "Manager.h"
#include "matrix.h"
#include "Material.h"
#include "RenderNode.h"
#include "InterfaceModel.h"

struct Particle
{
	Vec3D vPos, vSpeed, vDown, vOrigin, vDir;
	Matrix mFace;
	float fSize, fLife, fMaxLife;
	int nTile;
	Color32 color;
};

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
	void			init		(ParticleData* pData);
	virtual void	frameMove	(const Matrix& mWorld, double fTime, float fElapsedTime);
	virtual void	render		(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType=MATERIAL_NORMAL)const;
	void			update		(const Matrix& mWorld, ParticleData& particleData, float fElapsedTime);
public:
	ParticleData*		m_pData;		// 粒子发射器指针
	std::list<Particle>	m_Particles;	// 粒子堆
	int					m_nTime;		// 时间
	float				m_fRem;			// 人体伦琴单位当量 辐射量
};
