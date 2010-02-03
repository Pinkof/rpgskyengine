#pragma once

class RibbonEmitter;

#include <list>
#include "Animated.h"
#include "Vec2D.h"
#include "Matrix.h"
#include "Material.h"

float frand();
float randfloat(float lower, float upper);
int randint(int lower, int upper);

struct	Particle;
class	CParticleGroup;
class	CBone;
//class ParticleEmitter {
//protected:
//
//public:
//	ParticleEmitter(CParticleGroup *sys): sys(sys) {}
//	virtual Particle newParticle(int anim, int time, float w, float l, float spd, float var, float spr, float spr2) = 0;
//};
//
//class PlaneParticleEmitter: public ParticleEmitter {
//public:
//	PlaneParticleEmitter(CParticleGroup *sys): ParticleEmitter(sys) {}
//	Particle newParticle(int anim, int time, float w, float l, float spd, float var, float spr, float spr2);
//};
//
//class SphereParticleEmitter: public ParticleEmitter {
//public:
//	SphereParticleEmitter(CParticleGroup *sys): ParticleEmitter(sys) {}
//	Particle newParticle(int anim, int time, float w, float l, float spd, float var, float spr, float spr2);
//};

struct TexCoordSet
{
    Vec2D tc[4];
};

class DLL_EXPORT CParticleEmitter
{
public:
	// �ٶȣ� �仯����չ��lat�� ���������ڣ������ʣ�����һ�������ģ�ͨ�������ٶ�
	Animated<float> m_Speed, m_Variation, m_Spread, m_Lat, m_Gravity, m_Lifespan, m_Rate, m_Areal, m_Areaw, m_Deacceleration;
	Animated<uint8> m_Enabled;
	Color32 m_Colors[3];	// ��ɫx3

	float m_Sizes[3];		// ��Сx3
	// �������е㣬���٣���ת
	float m_fLifeMid, m_fSlowdown, m_fRotation;
	Vec3D m_vPos;			// ����
	int m_nTexChannel;		// ����ͨ��
	// ���ģʽ��ûɶ�õ�m_nOrder��
	//int m_nBlend;
	int m_nOrder, type;
	// ��������Tile��Ϣ
	int m_nRows, m_nCols;
	std::vector<TexCoordSet> m_Tiles;
	void InitTile(Vec2D *tc, int nID);

	bool m_bBillboard;			// �����

	//bool transform;

	// unknown parameters omitted for now ...
	int32 flags;
	int16 pType;

	int	m_nBoneID;

	float tofs;

	uint32	uTexID;

	CMaterial m_Material;
public:
	CParticleEmitter(): m_nBoneID(0), m_nTexChannel(0), m_fLifeMid(0)
	{
//		m_nBlend = 0;
		m_nOrder = 0;
		type = 0;

		m_nRows = 0;
		m_nCols = 0;

		m_fSlowdown = 0;
		m_fRotation = 0;
		tofs = 0;
		pType =1;

		m_Tiles.resize(1);
		m_Tiles[0].tc[0]=Vec2D(1,0);
		m_Tiles[0].tc[1]=Vec2D(1,1);
		m_Tiles[0].tc[2]=Vec2D(0,1);
		m_Tiles[0].tc[3]=Vec2D(0,0);
	}
	// ����
	void update(const Matrix& mWorld, CParticleGroup& particleGroup, float dt);
	Particle NewPlaneParticle(const Matrix& mWorld, int time, float w, float l, float spd, float var, float spr, float spr2);
	Particle NewSphereParticle(const Matrix& mWorld, int time, float w, float l, float spd, float var, float spr, float spr2);
};

struct RibbonSegment
{
	Vec3D pos, up, back;
	float len,len0;
};

class RibbonEmitter
{
public:
	Animated<Vec3D> color;
	Animated<short> opacity;
	Animated<float> above, below;

	CBone* m_pParentBone;
	float f1, f2;

	Vec3D pos;

	// ����ID,ʱ��
	int m_nTime;
	float length, seglen;
	int numsegs;

	Vec3D tpos;
	Vec4D tcolor;
	float tabove, tbelow;

	int m_nTexChannel;

	std::list<RibbonSegment> segs;

public:
	void setup(int time);
	void draw();
};