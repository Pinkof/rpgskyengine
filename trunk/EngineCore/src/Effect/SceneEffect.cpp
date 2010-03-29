#include "SceneEffect.h"
#include "RenderSystem.h"
#include "Vec3D.h"
#include "Color.h"

CSceneEffect::CSceneEffect():
m_fBloomVal(0.2f),
m_fAdaptedLum(0.5f),
m_fHDRKey(0.3f),
m_bHDR(false),
m_bInitialized(false),
m_pDepthRenderTarget(NULL),
m_pGlowRenderTarget(NULL),
m_pSceneTexture(NULL),
m_pExposureTexture(NULL),
m_pExposureTexture2(NULL),
m_pBackTexture(NULL),
m_pSceneCopyTexture(NULL),
m_nWidth(0),
m_nHeight(0)
{
}

CSceneEffect::~CSceneEffect()
{
	clearTextures();
}

void CSceneEffect::clearTextures()
{
	S_DEL(m_pGlowRenderTarget);
	S_DEL(m_pDepthRenderTarget);
	S_DEL(m_pSceneTexture);
	S_DEL(m_pExposureTexture);
	S_DEL(m_pExposureTexture2);
	S_DEL(m_pBackTexture);
	// new 
	{ // Fixed the scene texture release.
		CRenderSystem& R = GetRenderSystem();
		CShader* pShader = R.GetShaderMgr().getSharedShader();
		pShader->setTexture("g_texScene",(CTexture*)NULL);
	}

	S_DEL(m_pSceneCopyTexture);
}

void CSceneEffect::Reset(const CRect<int>& rc)
{
	clearTextures();
	int nWidth = rc.getWidth();
	int nHeight = rc.getHeight();

	m_nWidth = nWidth;
	m_nHeight= nHeight;

	CRenderSystem& R = GetRenderSystem();
	//m_pDepthRenderTarget = R.GetTextureMgr().CreateRenderTarget(512, 512);

	m_pGlowRenderTarget = R.GetTextureMgr().CreateRenderTarget(nWidth,nHeight);

	m_pSceneTexture = R.GetTextureMgr().CreateRenderTarget(nWidth*2,nHeight*2);

	m_pSceneCopyTexture = R.GetTextureMgr().CreateRenderTarget(nWidth,nHeight);

	//m_pExposureTexture = R.GetTextureMgr().CreateRenderTarget(1,1);
	//m_pBackTexture = R.GetTextureMgr().CreateRenderTarget(512, 512);//.CreateDynamicTexture(512,512);

	m_QuadVB[0].p = Vec4D(-0.5f,				(float)nHeight-0.5f,	0.0f, 1.0f);
	m_QuadVB[1].p = Vec4D(-0.5f,				-0.5f,					0.0f, 1.0f);
	m_QuadVB[2].p = Vec4D((float)nWidth-0.5f,	(float)nHeight-0.5f,	0.0f, 1.0f);
	m_QuadVB[3].p = Vec4D((float)nWidth-0.5f,	- 0.5f,					0.0f, 1.0f);
	//
	m_BloomClearVB[0].p = Vec4D((float)nWidth,		(float)nHeight,		0.0f, 1.0f);
	m_BloomClearVB[1].p = Vec4D(0.0f,				(float)nHeight,		0.0f, 1.0f);
	m_BloomClearVB[2].p = Vec4D(0.0f,				(float)nHeight*2.0f,0.0f, 1.0f);
	m_BloomClearVB[3].p = Vec4D((float)nWidth*2.0f,	(float)nHeight*2,	0.0f, 1.0f);
	m_BloomClearVB[4].p = Vec4D((float)nWidth*2.0f,	0.0f,				0.0f, 1.0f);
	m_BloomClearVB[5].p = Vec4D((float)nWidth,		0.0f,				0.0f, 1.0f);
	m_BloomClearVB[0].c = 0;
	m_BloomClearVB[1].c = 0;
	m_BloomClearVB[2].c = 0;
	m_BloomClearVB[3].c = 0;
	m_BloomClearVB[4].c = 0;
	m_BloomClearVB[5].c = 0;

	float fU0 = 0.0f;
	float fV0 = 0.0f;
	float fU1 = 0.5f;
	float fV1 = 0.5f;

	float fX0 = - 0.5f;
	float fY0 = - 0.5f;
	float fX1 =(float)nWidth - 0.5f;
	float fY1 =(float)nHeight - 0.5f;
	float fOffset = 1;

	m_FloodLumVB[0].p = Vec4D(fX0, fY1, 0.0f, 1.0f);
	m_FloodLumVB[1].p = Vec4D(fX0, fY0, 0.0f, 1.0f);
	m_FloodLumVB[2].p = Vec4D(fX1, fY1, 0.0f, 1.0f);
	m_FloodLumVB[3].p = Vec4D(fX1, fY0, 0.0f, 1.0f);

	m_BloomVB[0].t = Vec2D(fU0, fV1);
	m_BloomVB[1].t = Vec2D(fU0, fV0);
	m_BloomVB[2].t = Vec2D(fU1, fV1);
	m_BloomVB[3].t = Vec2D(fU1, fV0);

	m_BloomVB[0].p = Vec4D(fX0, fY1, 0.0f, 1.0f);
	m_BloomVB[1].p = Vec4D(fX0, fY0, 0.0f, 1.0f);
	m_BloomVB[2].p = Vec4D(fX1, fY1, 0.0f, 1.0f);
	m_BloomVB[3].p = Vec4D(fX1, fY0, 0.0f, 1.0f);

	DWORD color = (51<<24)+0x808080;
	m_BloomVB[0].c = color;
	m_BloomVB[1].c = color;
	m_BloomVB[2].c = color;
	m_BloomVB[3].c = color;

	DWORD BloomWeights[7] = 
	{
		0,
		225,
		155,
		83,
		35,
		11,
		3,
	};

	float fBloomWeights[7] = 
	{
		0.199471f,
		0.176033f,
		0.120985f,
		0.064759f,
		0.026995f,
		0.008764f,
		0.002216f,
	};

	int nBaseID = 0;
	for(int i = 0; i < 7; i++)
	{
		DWORD Alpha =254.0f*fBloomWeights[i]+1;
		DWORD color = (255<<24)+(Alpha<<16)+(Alpha<<8)+Alpha;
		if (i == 0)
		{

			for (int n = 0; n < 6; n++)
			{
				m_BloomHVB[nBaseID+n].c = color;
				m_BloomVVB[nBaseID+n].c = color;
			}

			fU0 = 0.0;
			fV0 = 0.0;
			fU1 = 0.5;
			fV1 = 0.5;

			m_BloomHVB[nBaseID+0].t = Vec2D(fU0, fV1);
			m_BloomHVB[nBaseID+1].t = Vec2D(fU0, fV0);
			m_BloomHVB[nBaseID+2].t = Vec2D(fU1, fV0);
			m_BloomHVB[nBaseID+3].t = Vec2D(fU0, fV1);
			m_BloomHVB[nBaseID+4].t = Vec2D(fU1, fV0);
			m_BloomHVB[nBaseID+5].t = Vec2D(fU1, fV1);

			fU0 = 0.5;
			fV0 = 0.0;
			fU1 = 1.0;
			fV1 = 0.5;

			m_BloomVVB[nBaseID+0].t = Vec2D(fU0, fV1);
			m_BloomVVB[nBaseID+1].t = Vec2D(fU0, fV0);
			m_BloomVVB[nBaseID+2].t = Vec2D(fU1, fV0);
			m_BloomVVB[nBaseID+3].t = Vec2D(fU0, fV1);
			m_BloomVVB[nBaseID+4].t = Vec2D(fU1, fV0);
			m_BloomVVB[nBaseID+5].t = Vec2D(fU1, fV1);

			fX0 = nWidth - 0.5f;
			fY0 = - 0.5f;
			fX1 = nWidth*2 - 0.5f;
			fY1 = nHeight - 0.5f;

			m_BloomHVB[nBaseID+0].p = Vec4D(fX0-fOffset*i,  fY1, 0.0f, 1.0f);
			m_BloomHVB[nBaseID+1].p = Vec4D(fX0-fOffset*i,  fY0, 0.0f, 1.0f);
			m_BloomHVB[nBaseID+2].p = Vec4D(fX1-fOffset*i,  fY0, 0.0f, 1.0f);
			m_BloomHVB[nBaseID+3].p = Vec4D(fX0-fOffset*i,  fY1, 0.0f, 1.0f);
			m_BloomHVB[nBaseID+4].p = Vec4D(fX1-fOffset*i,  fY0, 0.0f, 1.0f);
			m_BloomHVB[nBaseID+5].p = Vec4D(fX1-fOffset*i,  fY1, 0.0f, 1.0f);

			fX0 = - 0.5f;
			fY0 = (float)nHeight - 0.5f;
			fX1 = (float)nWidth - 0.5f;
			fY1 = (float)nHeight*2.0f - 0.5f;

			m_BloomVVB[nBaseID+0].p = Vec4D(fX0,  fY1-fOffset*i, 0.0f, 1.0f);
			m_BloomVVB[nBaseID+1].p = Vec4D(fX0,  fY0-fOffset*i, 0.0f, 1.0f);
			m_BloomVVB[nBaseID+2].p = Vec4D(fX1,  fY0-fOffset*i, 0.0f, 1.0f);
			m_BloomVVB[nBaseID+3].p = Vec4D(fX0,  fY1-fOffset*i, 0.0f, 1.0f);
			m_BloomVVB[nBaseID+4].p = Vec4D(fX1,  fY0-fOffset*i, 0.0f, 1.0f);
			m_BloomVVB[nBaseID+5].p = Vec4D(fX1,  fY1-fOffset*i, 0.0f, 1.0f);

			nBaseID += 6;
		}
		else
		{

			for (int n = 0; n < 12; n++)
			{
				m_BloomHVB[nBaseID+n].c = color;
				m_BloomVVB[nBaseID+n].c = color;
			}
			fU0 = 0.0;
			fV0 = 0.0;
			fU1 = 0.5;
			fV1 = 0.5;

			m_BloomHVB[nBaseID+0].t = Vec2D(fU0, fV1);
			m_BloomHVB[nBaseID+1].t = Vec2D(fU0, fV0);
			m_BloomHVB[nBaseID+2].t = Vec2D(fU1, fV0);
			m_BloomHVB[nBaseID+3].t = Vec2D(fU0, fV1);
			m_BloomHVB[nBaseID+4].t = Vec2D(fU1, fV0);
			m_BloomHVB[nBaseID+5].t = Vec2D(fU1, fV1);
			m_BloomHVB[nBaseID+6].t = Vec2D(fU0, fV1);
			m_BloomHVB[nBaseID+7].t = Vec2D(fU0, fV0);
			m_BloomHVB[nBaseID+8].t = Vec2D(fU1, fV0);
			m_BloomHVB[nBaseID+9].t = Vec2D(fU0, fV1);
			m_BloomHVB[nBaseID+10].t = Vec2D(fU1, fV0);
			m_BloomHVB[nBaseID+11].t = Vec2D(fU1, fV1);

			fU0 = 0.5;
			fV0 = 0.0;
			fU1 = 1.0;
			fV1 = 0.5;

			m_BloomVVB[nBaseID+0].t = Vec2D(fU0, fV1);
			m_BloomVVB[nBaseID+1].t = Vec2D(fU0, fV0);
			m_BloomVVB[nBaseID+2].t = Vec2D(fU1, fV0);
			m_BloomVVB[nBaseID+3].t = Vec2D(fU0, fV1);
			m_BloomVVB[nBaseID+4].t = Vec2D(fU1, fV0);
			m_BloomVVB[nBaseID+5].t = Vec2D(fU1, fV1);
			m_BloomVVB[nBaseID+6].t = Vec2D(fU0, fV1);
			m_BloomVVB[nBaseID+7].t = Vec2D(fU0, fV0);
			m_BloomVVB[nBaseID+8].t = Vec2D(fU1, fV0);
			m_BloomVVB[nBaseID+9].t = Vec2D(fU0, fV1);
			m_BloomVVB[nBaseID+10].t = Vec2D(fU1, fV0);
			m_BloomVVB[nBaseID+11].t = Vec2D(fU1, fV1);

			fX0 = (float)nWidth - 0.5f;
			fY0 = - 0.5f;
			fX1 = (float)nWidth*2.0f - 0.5f;
			fY1 = (float)nHeight - 0.5f;

			m_BloomHVB[nBaseID+0].p = Vec4D(fX0-fOffset*i,  fY1, 0.0f, 1.0f);
			m_BloomHVB[nBaseID+1].p = Vec4D(fX0-fOffset*i,  fY0, 0.0f, 1.0f);
			m_BloomHVB[nBaseID+2].p = Vec4D(fX1-fOffset*i,  fY0, 0.0f, 1.0f);
			m_BloomHVB[nBaseID+3].p = Vec4D(fX0-fOffset*i,  fY1, 0.0f, 1.0f);
			m_BloomHVB[nBaseID+4].p = Vec4D(fX1-fOffset*i,  fY0, 0.0f, 1.0f);
			m_BloomHVB[nBaseID+5].p = Vec4D(fX1-fOffset*i,  fY1, 0.0f, 1.0f);
			m_BloomHVB[nBaseID+6].p = Vec4D(fX0+fOffset*i,  fY1, 0.0f, 1.0f);
			m_BloomHVB[nBaseID+7].p = Vec4D(fX0+fOffset*i,  fY0, 0.0f, 1.0f);
			m_BloomHVB[nBaseID+8].p = Vec4D(fX1+fOffset*i,  fY0, 0.0f, 1.0f);
			m_BloomHVB[nBaseID+9].p = Vec4D(fX0+fOffset*i,  fY1, 0.0f, 1.0f);
			m_BloomHVB[nBaseID+10].p = Vec4D(fX1+fOffset*i,  fY0, 0.0f, 1.0f);
			m_BloomHVB[nBaseID+11].p = Vec4D(fX1+fOffset*i,  fY1, 0.0f, 1.0f);

			fX0 = - 0.5f;
			fY0 = (float)nHeight - 0.5f;
			fX1 = (float)nWidth - 0.5f;
			fY1 = (float)nHeight*2 - 0.5f;

			m_BloomVVB[nBaseID+0].p = Vec4D(fX0,  fY1-fOffset*i, 0.0f, 1.0f);
			m_BloomVVB[nBaseID+1].p = Vec4D(fX0,  fY0-fOffset*i, 0.0f, 1.0f);
			m_BloomVVB[nBaseID+2].p = Vec4D(fX1,  fY0-fOffset*i, 0.0f, 1.0f);
			m_BloomVVB[nBaseID+3].p = Vec4D(fX0,  fY1-fOffset*i, 0.0f, 1.0f);
			m_BloomVVB[nBaseID+4].p = Vec4D(fX1,  fY0-fOffset*i, 0.0f, 1.0f);
			m_BloomVVB[nBaseID+5].p = Vec4D(fX1,  fY1-fOffset*i, 0.0f, 1.0f);
			m_BloomVVB[nBaseID+6].p = Vec4D(fX0,  fY1+fOffset*i, 0.0f, 1.0f);
			m_BloomVVB[nBaseID+7].p = Vec4D(fX0,  fY0+fOffset*i, 0.0f, 1.0f);
			m_BloomVVB[nBaseID+8].p = Vec4D(fX1,  fY0+fOffset*i, 0.0f, 1.0f);
			m_BloomVVB[nBaseID+9].p = Vec4D(fX0,  fY1+fOffset*i, 0.0f, 1.0f);
			m_BloomVVB[nBaseID+10].p = Vec4D(fX1,  fY0+fOffset*i, 0.0f, 1.0f);
			m_BloomVVB[nBaseID+11].p = Vec4D(fX1,  fY1+fOffset*i, 0.0f, 1.0f);

			nBaseID += 12;
		}
	}

	fU0 = 0.0;
	fV0 = 0.5;
	fU1 = 0.5;
	fV1 = 1.0;

	m_QuadVB[0].t = Vec2D(fU0, fV1);
	m_QuadVB[1].t = Vec2D(fU0, fV0);
	m_QuadVB[2].t = Vec2D(fU1, fV1);
	m_QuadVB[3].t = Vec2D(fU1, fV0);

	m_FloodLumVB[0].t = Vec2D(fU0, fV1);
	m_FloodLumVB[1].t = Vec2D(fU0, fV0);
	m_FloodLumVB[2].t = Vec2D(fU1, fV1);
	m_FloodLumVB[3].t = Vec2D(fU1, fV0);
	m_bInitialized = true;
}

CTexture* CSceneEffect::getSceneTexture()
{
	CRenderSystem& R = GetRenderSystem();
	CTexture* m_pRenderSystemTarget = R.GetRenderTarget();
	CRect<int> rcSrc;
	R.getViewport(rcSrc);
	R.StretchRect(m_pRenderSystemTarget,&rcSrc,m_pSceneCopyTexture,NULL, TEXF_LINEAR);
	S_DEL(m_pRenderSystemTarget);
	return m_pSceneCopyTexture;
}

void CSceneEffect::RenderTemporalBloom()
{
	if (false==m_bInitialized)
	{
		return;
	}
	//return;
	// 1:copy�������� 2:����һ֡����ϱȻ�ϲ���� 3:�����֡�����´λ��
	CRenderSystem& R = GetRenderSystem();

	CTexture* m_pRenderSystemTarget = R.GetRenderTarget();
	R.SetupRenderState();
	R.StretchRect(m_pRenderSystemTarget,NULL,m_pBackTexture,NULL, TEXF_LINEAR);
	//if(R.BeginFrame())
	{
		int nAlpha =227; min(m_fBloomVal*255,255);
		DWORD dwFactor = (nAlpha<<24)+0xFF0000;
		R.SetTextureFactor(dwFactor);
		R.SetCullingMode(CULL_NONE);// CULL_CCW
		R.SetLightingEnabled(false);
		R.SetBlendFunc(false,BLENDOP_ADD,SBF_SOURCE_ALPHA,SBF_ONE_MINUS_SOURCE_ALPHA);
		R.SetDepthBufferFunc(false,false);
		R.SetTextureColorOP(0,TBOP_SOURCE1,TBS_TEXTURE);
		R.SetTextureAlphaOP(0,TBOP_SOURCE1,TBS_TFACTOR);

		R.SetTextureColorOP(1,TBOP_DISABLE);
		R.SetTextureAlphaOP(1,TBOP_DISABLE);

		R.SetTexture(0,m_pBackTexture) ;
		R.SetFVF(QuadVertex::FVF);
		R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_QuadVB, sizeof(QuadVertex));
	//	R.EndFrame();
	}
	//���浱ǰ�Ļ��� �Ա���һ֡����
	//R.EndFrame();
	R.StretchRect(m_pRenderSystemTarget,NULL,m_pBackTexture,NULL, TEXF_LINEAR);
	//m_pBackTexture->SaveToFile("D:/BackCopy.bmp");
	//R.BeginFrame();
	S_DEL(m_pRenderSystemTarget);
}

float CSceneEffect::GetSceneExposure()
{	/*const RECT rect0 = {0,0,255,255};
const RECT rect1 = {256,0,256+64,64};
const RECT rect2 = {256+64,0,256+64+16,16};
const RECT rect3 = {256+64+16,0,256+64+16+4,4};
const RECT rect4 = {0,0,1,1};

R.StretchRect(m_pSceneTexture, &rect0, m_pSceneTexture, &rect1, TEXF_LINEAR);
R.StretchRect(m_pSceneTexture, &rect1, m_pSceneTexture, &rect2, TEXF_LINEAR);
R.StretchRect(m_pSceneTexture, &rect2, m_pSceneTexture, &rect3, TEXF_LINEAR);
R.StretchRect(m_pSceneTexture, &rect3, m_pExposureTexture, &rect4, TEXF_LINEAR);

D3DXSaveSurfaceToFileA("F:/��Ŀ/NewGame/cc.bmp", D3DXIFF_BMP, m_pSceneTexture, NULL, NULL);*/
	///RECT rect;
	//rect.left = rand()%250 + 2;
	//rect.top = rand()%250 + 2;
	//rect.right = rect.left+1;
	//rect.bottom = rect.top+1;

	//R.StretchRect(m_pSceneTexture, &rect, m_pExposureTexture, NULL, TEXF_NONE);

	//R.GetRenderTargetData(m_pExposureTexture, m_pExposureTexture2);

	//D3DLOCKED_RECT lockedRect;
	//HRESULT hr = m_pExposureTexture2->LockRect(&lockedRect,
	//	0/*lock entire tex*/, D3DLOCK_DONOTWAIT | D3DLOCK_NO_DIRTY_UPDATE | D3DLOCK_READONLY/*flags*/);

	//if (hr == S_OK)
	//{
	//	Color32 color = *((Color32*)lockedRect.pBits);
	//	// �������ص�����
	//	float fLum = (0.27f*color.a + 0.67f*color.r + 0.06f * color.g)/255.0f;
	//	// ���㵱ǰ��Ӧ������
	//	m_fAdaptedLum += (fLum - m_fAdaptedLum) * (1 - pow(0.98f, 10 * DXUTGetElapsedTime()));
	//}
	//else if (hr == D3DERR_WASSTILLDRAWING)
	//{
	//	hr = S_OK;
	//}
	//m_pExposureTexture2->UnlockRect();
	return m_fAdaptedLum;
}

void CSceneEffect::RenderBloom()
{
	CRenderSystem& R = GetRenderSystem();

	renderTargetBegin();
	R.ClearBuffer(true, true, 0xFFFFFFFF);
	renderTargetGlow();
	renderTargetBloom();
	renderTargetEnd();
	compose();
	// test
	//R.SetRenderTarget(m_pDepthRenderTarget);
	//if (R.BeginFrame())
	//{
	//	
	//}
	//R.EndFrame();
	//m_pDepthRenderTarget->SaveToFile(L"D:/Depth.bmp");

	//R.GetBackBuffer()
	//void *pContainer = NULL;
	//IDirect3DTexture9 *pTexture = NULL;
	//HRESULT hr = pSurface->GetContainer(IID_IDirect3DTexture9, &pContainer);
//	if (SUCCEEDED(hr) && pContainer)
	//{
	//	pTexture = (IDirect3DTexture9 *)pContainer;
	//}

	//int nAlpha = min(m_fBloomVal*255,255);
	//DWORD dwFactor = nAlpha<<24;

	//R.SetShader(-1);


	//D3DXSaveSurfaceToFileW(L"D:/m_pSceneTexture.bmp", D3DXIFF_BMP, m_pSceneTexture, NULL, NULL);
}

void CSceneEffect::glowRenderTargetBegin()
{
	CRenderSystem& R = GetRenderSystem();
	m_pRenderSystemTarget = R.GetRenderTarget();
	R.SetRenderTarget(m_pGlowRenderTarget);

	//RECT rect={0,0,255,255};
	//R.setViewport(rect);
}

void CSceneEffect::glowRenderTargetEnd()
{
	CRenderSystem& R = GetRenderSystem();
	// Restore
	R.SetRenderTarget(m_pRenderSystemTarget);
	S_DEL(m_pRenderSystemTarget);
}

void CSceneEffect::renderTargetBegin()
{
	CRenderSystem& R = GetRenderSystem();
	m_pRenderSystemTarget = R.GetRenderTarget();
	R.SetRenderTarget(m_pSceneTexture);

	R.SetCullingMode(CULL_NONE);
	R.SetLightingEnabled(false);
	R.SetDepthBufferFunc(false,false);
}

void CSceneEffect::renderTargetGlow()// not good
{
	CRenderSystem& R = GetRenderSystem();
	// first: copy system render target to my render target.

	CRect<int> rect(0,0,m_nWidth,m_nHeight);
	R.StretchRect(m_pGlowRenderTarget, NULL, m_pSceneTexture, &rect, TEXF_POINT);

	return;
	if (m_bHDR)
	{
		GetSceneExposure();
	}

	//if(R.BeginFrame())
	// Get the lum texture
	{
		
		// һ��Ҫ�ѿհ�����ֵΪ��ɫ
		R.SetTexture(0, m_pSceneTexture);
		R.SetFVF(SceneBloomVertex::FVF);


		R.SetBlendFunc(false);
		// Increase the contrast
		//R.SetTextureColorOP(0,TBOP_DOTPRODUCT3,TBS_TEXTURE,TBS_DIFFUSE);
		R.SetTextureColorOP(0,TBOP_MODULATE,TBS_TEXTURE,TBS_TEXTURE);
		R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_BloomVB, sizeof(SceneBloomVertex));

		{
			int nInvExposure= (int)(m_fAdaptedLum * m_fAdaptedLum * 255.0f);
			Color32 clrFactor(nInvExposure,nInvExposure,nInvExposure,nInvExposure);
			R.SetTextureFactor(clrFactor);
		}

		// To reduce the brightness
		R.SetTextureColorOP(0,TBOP_SUBTRACT,TBS_TEXTURE,TBS_TFACTOR);
		R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_BloomVB, sizeof(SceneBloomVertex));


		//R.SetBlendFunc(true,BLENDOP_ADD,SBF_ONE,SBF_ONE);
		//R.SetTextureFactor(0x40404040)
		//R.SetTextureColorOP(0,TBOP_MODULATE,TBS_TEXTURE,TBS_TFACTOR);
		//r->DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_FloodLumVB, sizeof(SceneBloomVertex));

		//	R.EndFrame();
	}
}

void CSceneEffect::renderTargetBloom()
{
	CRenderSystem& R = GetRenderSystem();
	R.SetBlendFunc(false);

	R.SetTexture(0, m_pSceneTexture);
	R.SetFVF(SceneBloomVertex::FVF);

	// ������ɾ��ĵط�
	R.SetTextureColorOP(0,TBOP_SOURCE2,TBS_TEXTURE,TBS_DIFFUSE);
	R.DrawPrimitiveUP(VROT_TRIANGLE_FAN, 4, m_BloomClearVB, sizeof(SceneBloomVertex));

	// �����������ɹ���-�ݺ�ģ��
	for (int nBloomCount = 0; nBloomCount<1; nBloomCount++)
	{
		R.SetBlendFunc(true,BLENDOP_ADD,SBF_ONE,SBF_ONE);
		R.SetTextureColorOP(0,TBOP_MODULATE,TBS_TEXTURE,TBS_DIFFUSE);
		R.SetTextureAlphaOP(0,TBOP_DISABLE);

		R.DrawPrimitiveUP(VROT_TRIANGLE_LIST, 26, m_BloomHVB, sizeof(SceneBloomVertex));
		R.DrawPrimitiveUP(VROT_TRIANGLE_LIST, 26, m_BloomVVB, sizeof(SceneBloomVertex));
	}
}

void CSceneEffect::renderTargetEnd()
{
	CRenderSystem& R = GetRenderSystem();
	// Restore
	R.SetRenderTarget(m_pRenderSystemTarget);
	S_DEL(m_pRenderSystemTarget);
}

void CSceneEffect::compose()
{
	CRenderSystem& R = GetRenderSystem();
	static bool bcan = true;
	R.SetCullingMode(CULL_NONE);
	R.SetLightingEnabled(false);
	R.SetDepthBufferFunc(false,false);
	R.SetBlendFunc(bcan,BLENDOP_ADD,SBF_ONE,SBF_ONE);

	R.SetTextureColorOP(0,TBOP_SOURCE1);

	R.SetTexCoordIndex(0, 0);

	//if(R.BeginFrame())
	{
		R.SetTexture(0, m_pSceneTexture);
		R.SetFVF(QuadVertex::FVF);

		if (m_bHDR)
		{
			// ��������
			R.SetBlendFunc(bcan,BLENDOP_REVSUBTRACT,SBF_ONE,SBF_ONE);
			//r->BlendOP(BLENDOP_REVSUBTRACT);BLENDOP_REVSUBTRACT
			float fLumScale = m_fHDRKey/m_fAdaptedLum;


			if (fLumScale > 1.0f)// ������Ļ����
			{
				fLumScale -= 1.0f;
				unsigned char ucFactor = min(255,fLumScale*255);
				Color32 clrFactor(ucFactor,255,255,255);
				R.SetTextureFactor(clrFactor);
				R.SetBlendFunc(bcan,BLENDOP_ADD,SBF_DEST_COLOUR,SBF_SOURCE_ALPHA);

				R.SetTextureColorOP(0,TBOP_SOURCE1,TBS_TFACTOR);
				R.SetTextureAlphaOP(0,TBOP_SOURCE1,TBS_TFACTOR);

				R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_QuadVB, sizeof(QuadVertex));
			}
			else// ������Ļ����
			{
				unsigned char ucFactor = min(255,fLumScale*255);
				Color32 clrFactor(ucFactor,ucFactor,ucFactor,ucFactor);
				R.SetTextureFactor(clrFactor);
				R.SetBlendFunc(bcan,BLENDOP_ADD,SBF_DEST_COLOUR,SBF_ZERO);

				R.SetTextureColorOP(0,TBOP_SOURCE1,TBS_TFACTOR);
				R.SetTextureAlphaOP(0,TBOP_DISABLE);
				R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_QuadVB, sizeof(QuadVertex));
			}
		}

		// ���ӹ���
		R.SetBlendFunc(bcan,BLENDOP_ADD,SBF_ONE,SBF_ONE);
		R.SetTextureColorOP(0,TBOP_SOURCE1,TBS_TEXTURE);
		R.SetTextureAlphaOP(0,TBOP_DISABLE);
		R.DrawPrimitiveUP(VROT_TRIANGLE_STRIP, 2, m_QuadVB, sizeof(QuadVertex));
		//	R.EndFrame();
	}
}