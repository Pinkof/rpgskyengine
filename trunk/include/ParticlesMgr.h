#pragma once

// Mesh ����������������,������������ʵ� �����ظ�����
#include "GameStdafx.h"
#include "Particle.h"


typedef struct ATTRIBUTERANGE
{
	char  szName[32];
	DWORD AttribId;
	DWORD IndexStart;
	DWORD IndexCount;
	DWORD VertexStart;
	DWORD VertexCount;
	DWORD dwFlags;
};
typedef ATTRIBUTERANGE* LPATTRIBUTERANGE;

class CParticlesMgr
{
public:
	CParticlesMgr(void);
	~CParticlesMgr(void);

	// �����豸
	HRESULT OnCreateDevice( LPDIRECT3DDEVICE9 pd3dDevice ){ m_pDevice = pd3dDevice; }
	// ����D3D�豸
	void OnResetDevice();
	void OnLostDevice();
	void OnDestroyDevice();

	//
	void Render();
	// ʵʱ���
	void Update();
public:
	//
	LPDIRECT3DVERTEXBUFFER9 GetVB(){ return m_pVB; }
	//
	LPDIRECT3DINDEXBUFFER9 GetIB(){ return m_pIB; }
public:
	//
	void SetVB( LPDIRECT3DVERTEXBUFFER9 pVB ){ m_pVB = pVB; }
	//
	void SetIB( LPDIRECT3DINDEXBUFFER9 pIB ){ m_pIB = pIB; }
public:
	vector<ATTRIBUTERANGE>		m_Attribs;
protected:
	LPDIRECT3DDEVICE9			m_pDevice;
	LPDIRECT3DVERTEXBUFFER9		m_pVB;			// ���㻺��
	LPDIRECT3DINDEXBUFFER9		m_pIB;			// ��������
};

CParticlesMgr* GetParticlesMgr();