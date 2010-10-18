#pragma once
#include "Manager.h"
#include "matrix.h"
#include "Material.h"

class CParticleEmitter;

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

class CParticleGroup
{
public:
	CParticleEmitter* m_pEmitter;	// ���ӷ�����ָ��
	ParticleList m_Particles;		// ���Ӷ�
	int  m_nTime;			// ʱ��
	float m_fRem;					// �������ٵ�λ���� ������
public:
	void Init(CParticleEmitter* pEmitter);
	void update(float fElapsedTime);
	void Setup(int nTime);
	//void SetTex(float dt);
	void draw()const;
	virtual void render(E_MATERIAL_RENDER_TYPE eRenderType=MATERIAL_NORMAL)const;
	CParticleGroup(): m_pEmitter(NULL)
	{
		m_nTime = 0;
		m_fRem	= 0;
	}
};
