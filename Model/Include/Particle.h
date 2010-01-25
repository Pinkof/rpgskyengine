#pragma once
#include "Manager.h"
#include "matrix.h"
#include "Color.h"

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
	CBone* m_pBone;					// �󶨵Ĺ���
	ParticleList m_Particles;		// ���Ӷ�
	uint32 m_uTexID;					// ��������ID
	int  m_nTime;			// ʱ��
	float m_fRem;					// �������ٵ�λ���� ������
public:
	void Init(CParticleEmitter* pEmitter, CBone* pBone);
	void update(float fElapsedTime);
	void SetTex(int nTexID);
	void Setup(int nTime);
	//void SetTex(float dt);
	void draw()const;
	CParticleGroup(): m_pEmitter(NULL), m_pBone(NULL), m_uTexID(0)
	{
		m_nTime = 0;
		m_fRem	= 0;
	}
};
