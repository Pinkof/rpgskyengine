//	---------------------------------------------------------------------------
//
//	@file		TwDirect3D9.cpp
//	@author		Philippe Decaudin - http://www.antisphere.com
//  @license    This file is part of the AntTweakBar library.
//              For conditions of distribution and use, see License.txt
//
//	note:		TAB=4
//
//	---------------------------------------------------------------------------
#include "TwPrecomp.h"
#include "TwDirect3D9.h"
#include "TwMgr.h"
#include "RenderSystem.h"

#include <d3d9.h>
#ifdef _DEBUG
	#include <dxerr9.h>
	#pragma comment(lib, "dxerr9")
#endif // _DEBUG


////using namespace std;

const char *g_ErrCantLoadD3D9	= "Cannot load Direct3D9 library dynamically";
const char *g_ErrCantUnloadD3D9	= "Cannot unload Direct3D9 library";


//	---------------------------------------------------------------------------

static IDirect3DTexture9 *BindFont(IDirect3DDevice9 *_Dev, const CTexFont *_Font)
{
	assert(_Font!=NULL);

	IDirect3DTexture9 *Tex = NULL;
	HRESULT hr = _Dev->CreateTexture(_Font->m_TexWidth, _Font->m_TexHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &Tex, NULL);
	if(FAILED(hr))
		return NULL;

	D3DLOCKED_RECT r;
	hr = Tex->LockRect(0, &r, NULL, 0);
	if(SUCCEEDED(hr))
	{
		color32 *p = static_cast<color32 *>(r.pBits);
		for(int i=0; i<_Font->m_TexWidth*_Font->m_TexHeight; ++i, ++p)
			*p = 0x00ffffff | (((color32)(_Font->m_TexBytes[i]))<<24);
		Tex->UnlockRect(0);
	}
	return Tex;
}

//	---------------------------------------------------------------------------

static void UnbindFont(IDirect3DDevice9 *_Dev, IDirect3DTexture9 *_Tex)
{
	(void)_Dev;

	if(_Tex)
		_Tex->Release();
}

//	---------------------------------------------------------------------------

struct CState
{
	IDirect3DStateBlock9 *m_StateBlock;

	// DeviceCaps (filled by constructor)
	D3DCAPS9		m_Caps;

	void			Save();
	void			Restore();
					CState(IDirect3DDevice9 *_Dev);
					~CState();
private:
	IDirect3DDevice9 *m_D3DDev;
};

CState::CState(IDirect3DDevice9 *_Dev)
{
	ZeroMemory(this, sizeof(CState));
	m_D3DDev = _Dev;

	m_D3DDev->GetDeviceCaps(&m_Caps);
}

CState::~CState()
{
	if(m_StateBlock)
	{
		UINT rc = m_StateBlock->Release();
		assert(rc==0); (void)rc;
		m_StateBlock = NULL;
	}
}

void CState::Save()
{
	if(!m_StateBlock && m_D3DDev)
		m_D3DDev->CreateStateBlock(D3DSBT_ALL, &m_StateBlock);

	if(m_StateBlock)
		m_StateBlock->Capture();
}

void CState::Restore()
{
	if(m_StateBlock)
		m_StateBlock->Apply();
}

//	---------------------------------------------------------------------------

int CTwGraphDirect3D9::Init()
{
	assert(g_TwMgr->m_Device!=NULL);

	m_D3DDev = static_cast<IDirect3DDevice9 *>(g_TwMgr->m_Device);
	m_Drawing = false;
	m_FontTex = NULL;
	m_FontD3DTex = NULL;
	D3DDEVICE_CREATION_PARAMETERS cp;
	m_D3DDev->GetCreationParameters(&cp);
	m_PureDevice = (cp.BehaviorFlags & D3DCREATE_PUREDEVICE) ? true : false;
	m_WndWidth = 0;
	m_WndHeight = 0;
	m_State = new CState(m_D3DDev);

	return 1;
}

//	---------------------------------------------------------------------------

int CTwGraphDirect3D9::Shut()
{
	assert(m_Drawing==false);

	UnbindFont(m_D3DDev, m_FontD3DTex);
	m_FontD3DTex = NULL;
	delete m_State;
	m_State = NULL;
	m_D3DDev = NULL;

	return 1;
}

//	---------------------------------------------------------------------------

void CTwGraphDirect3D9::BeginDraw(int _WndWidth, int _WndHeight)
{
	assert(m_Drawing==false && _WndWidth>0 && _WndHeight>0);
	m_Drawing = true;

	m_WndWidth  = _WndWidth;
	m_WndHeight = _WndHeight;

	// save context
	if(!m_PureDevice)
		m_State->Save();

	if(m_WndWidth>0 && m_WndHeight>0)
	{
		D3DVIEWPORT9 Vp;
		Vp.X = 0;
		Vp.Y = 0;
		Vp.Width = m_WndWidth;
		Vp.Height = m_WndHeight;
		Vp.MinZ = 0;
		Vp.MaxZ = 1;
		m_D3DDev->SetViewport(&Vp);

		//D3DMATRIX Transfo = { 2.0f/_WndWidth,0,0,0, 0,2.0f/_WndHeight,0,0, 0,0,-1,0, 0,0,0,1 };
		//r.SetProjectionMatrix(Transfo);
	}
	//  const D3DMATRIX id = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	//	r.SetTransform(D3DTS_VIEW, &id);
	//	r.SetViewMatrix(id);
	//  r.SetTextureMatrix(0, &id);

	CRenderSystem& R = GetRenderSystem();
	R.SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	R.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	R.SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	R.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	R.SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	R.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	R.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	R.SetRenderState(D3DRS_CLIPPLANEENABLE, 0);
	R.SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	R.SetRenderState(D3DRS_LASTPIXEL, FALSE);
	R.SetRenderState(D3DRS_FOGENABLE, FALSE);
	R.SetRenderState(D3DRS_STENCILENABLE, FALSE);
	R.SetRenderState(D3DRS_COLORWRITEENABLE, 0x0000000F);
	R.SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
	if(m_State->m_Caps.PrimitiveMiscCaps & D3DPMISCCAPS_SEPARATEALPHABLEND)
		R.SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE);
	//if(m_State->m_Caps.LineCaps & D3DLINECAPS_ANTIALIAS)
		R.SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);

	R.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	R.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	R.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	R.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	R.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	R.SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);
	R.SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	R.SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	R.SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	R.SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	R.SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	R.SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	R.SetVertexShader(NULL);
	R.SetPixelShader(NULL);
}

//	---------------------------------------------------------------------------

void CTwGraphDirect3D9::EndDraw()
{
	assert(m_Drawing==true);
	m_Drawing = false;

	// restore context
	if(!m_PureDevice)
		m_State->Restore();
}

//	---------------------------------------------------------------------------

bool CTwGraphDirect3D9::IsDrawing()
{
	return m_Drawing;
}

//	---------------------------------------------------------------------------

void CTwGraphDirect3D9::Restore()
{
	if(m_State)
		if(m_State->m_StateBlock)
		{
			UINT rc = m_State->m_StateBlock->Release();
			assert(rc==0); (void)rc;
			m_State->m_StateBlock = NULL;
		}

	UnbindFont(m_D3DDev, m_FontD3DTex);
	m_FontD3DTex = NULL;

	m_FontTex = NULL;
}


//	---------------------------------------------------------------------------

void CTwGraphDirect3D9::DrawLine(int _X0, int _Y0, int _X1, int _Y1, color32 _Color0, color32 _Color1, bool _AntiAliased)
{
	assert(m_Drawing==true);

	struct CVtx
	{
		float m_Pos[4];
		DWORD m_Color;
	};
	CVtx p[2];

	p[0].m_Pos[0] = (float)_X0;
	p[0].m_Pos[1] = (float)_Y0;
	p[0].m_Pos[2] = 0;
	p[0].m_Pos[3] = 0;
	p[0].m_Color  = _Color0;

	p[1].m_Pos[0] = (float)_X1;
	p[1].m_Pos[1] = (float)_Y1;
	p[1].m_Pos[2] = 0;
	p[1].m_Pos[3] = 0;
	p[1].m_Color  = _Color1;

	CRenderSystem& R = GetRenderSystem();
	//if(m_State->m_Caps.LineCaps & D3DLINECAPS_ANTIALIAS)
		R.SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, (_AntiAliased ? TRUE : FALSE));
	R.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
	R.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	R.SetFVF(D3DFVF_XYZRHW|D3DFVF_DIFFUSE);
	R.DrawPrimitiveUP(D3DPT_LINELIST, 1, p, sizeof(CVtx));
	//if(m_State->m_Caps.LineCaps & D3DLINECAPS_ANTIALIAS)
		R.SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
}

//	---------------------------------------------------------------------------

void CTwGraphDirect3D9::DrawRect(int _X0, int _Y0, int _X1, int _Y1, color32 _Color00, color32 _Color10, color32 _Color01, color32 _Color11)
{
	assert(m_Drawing==true);

	// border adjustment
	if(_X0<_X1)
		++_X1;
	else if(_X0>_X1)
		++_X0;
	if(_Y0<_Y1)
		++_Y1;
	else if(_Y0>_Y1)
		++_Y0;

	struct CVtx
	{
		float m_Pos[4];
		DWORD m_Color;
	};
	CVtx p[4];

	p[0].m_Pos[0] = (float)_X1;
	p[0].m_Pos[1] = (float)_Y0;
	p[0].m_Pos[2] = 0;
	p[0].m_Pos[3] = 1;
	p[0].m_Color  = _Color10;

	p[1].m_Pos[0] = (float)_X0;
	p[1].m_Pos[1] = (float)_Y0;
	p[1].m_Pos[2] = 0;
	p[1].m_Pos[3] = 1;
	p[1].m_Color  = _Color00;

	p[2].m_Pos[0] = (float)_X1;
	p[2].m_Pos[1] = (float)_Y1;
	p[2].m_Pos[2] = 0;
	p[2].m_Pos[3] = 1;
	p[2].m_Color  = _Color11;

	p[3].m_Pos[0] = (float)_X0;
	p[3].m_Pos[1] = (float)_Y1;
	p[3].m_Pos[2] = 0;
	p[3].m_Pos[3] = 1;
	p[3].m_Color  = _Color01;

	CRenderSystem& R = GetRenderSystem();
	R.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
	R.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	R.SetFVF(D3DFVF_XYZRHW|D3DFVF_DIFFUSE);
	R.DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, p, sizeof(CVtx));
}

//	---------------------------------------------------------------------------

void *CTwGraphDirect3D9::NewTextObj()
{
	return new CTextObj;
}

//	---------------------------------------------------------------------------

void CTwGraphDirect3D9::DeleteTextObj(void *_TextObj)
{
	assert(_TextObj!=NULL);
	delete static_cast<CTextObj *>(_TextObj);
}

//	---------------------------------------------------------------------------

void CTwGraphDirect3D9::BuildText(void *_TextObj, const std::string *_TextLines, color32 *_LineColors, color32 *_LineBgColors, int _NbLines, const CTexFont *_Font, int _Sep, int _BgWidth)
{
	assert(m_Drawing==true);
	assert(_TextObj!=NULL);
	assert(_Font!=NULL);

	if(_Font != m_FontTex)
	{
		UnbindFont(m_D3DDev, m_FontD3DTex);
		m_FontD3DTex = BindFont(m_D3DDev, _Font);
		m_FontTex = _Font;
	}

	CTextObj *TextObj = static_cast<CTextObj *>(_TextObj);
	TextObj->m_TextVerts.resize(0);
	TextObj->m_BgVerts.resize(0);
	TextObj->m_LineColors = (_LineColors!=NULL);
	TextObj->m_LineBgColors = (_LineBgColors!=NULL);

	int x, x1, y, y1, i, Len;
	unsigned char ch;
	const unsigned char *Text;
	color32 LineColor = COLOR32_RED;
	CTextVtx Vtx;
	Vtx.m_Pos[2] = 0;
	Vtx.m_Pos[3] = 1;
	CBgVtx BgVtx;
	BgVtx.m_Pos[2] = 0;
	BgVtx.m_Pos[3] = 1;
	for(int Line=0; Line<_NbLines; ++Line)
	{
		x = 0;
		y = Line * (_Font->m_CharHeight+_Sep);
		y1 = y+_Font->m_CharHeight;
		Len = (int)_TextLines[Line].length();
		Text = (const unsigned char *)(_TextLines[Line].c_str());
		if(_LineColors!=NULL)
			LineColor = _LineColors[Line];

		for(i=0; i<Len; ++i)
		{
			ch = Text[i];
			x1 = x + _Font->m_CharWidth[ch];

			Vtx.m_Color  = LineColor;

			Vtx.m_Pos[0] = (float)x;
			Vtx.m_Pos[1] = (float)y;
			Vtx.m_UV [0] = _Font->m_CharU0[ch];
			Vtx.m_UV [1] = _Font->m_CharV0[ch];
			TextObj->m_TextVerts.push_back(Vtx);

			Vtx.m_Pos[0] = (float)x1;
			Vtx.m_Pos[1] = (float)y;
			Vtx.m_UV [0] = _Font->m_CharU1[ch];
			Vtx.m_UV [1] = _Font->m_CharV0[ch];
			TextObj->m_TextVerts.push_back(Vtx);

			Vtx.m_Pos[0] = (float)x;
			Vtx.m_Pos[1] = (float)y1;
			Vtx.m_UV [0] = _Font->m_CharU0[ch];
			Vtx.m_UV [1] = _Font->m_CharV1[ch];
			TextObj->m_TextVerts.push_back(Vtx);

			Vtx.m_Pos[0] = (float)x1;
			Vtx.m_Pos[1] = (float)y;
			Vtx.m_UV [0] = _Font->m_CharU1[ch];
			Vtx.m_UV [1] = _Font->m_CharV0[ch];
			TextObj->m_TextVerts.push_back(Vtx);

			Vtx.m_Pos[0] = (float)x1;
			Vtx.m_Pos[1] = (float)y1;
			Vtx.m_UV [0] = _Font->m_CharU1[ch];
			Vtx.m_UV [1] = _Font->m_CharV1[ch];
			TextObj->m_TextVerts.push_back(Vtx);

			Vtx.m_Pos[0] = (float)x;
			Vtx.m_Pos[1] = (float)y1;
			Vtx.m_UV [0] = _Font->m_CharU0[ch];
			Vtx.m_UV [1] = _Font->m_CharV1[ch];
			TextObj->m_TextVerts.push_back(Vtx);

			x = x1;
		}
		if(_BgWidth>0)
		{
			if(_LineBgColors!=NULL)
				BgVtx.m_Color = _LineBgColors[Line];
			else
				BgVtx.m_Color  = COLOR32_BLACK;

			BgVtx.m_Pos[0] = -1;
			BgVtx.m_Pos[1] = (float)y;
			TextObj->m_BgVerts.push_back(BgVtx);

			BgVtx.m_Pos[0] = (float)(_BgWidth+1);
			BgVtx.m_Pos[1] = (float)y;
			TextObj->m_BgVerts.push_back(BgVtx);

			BgVtx.m_Pos[0] = -1;
			BgVtx.m_Pos[1] = (float)y1;
			TextObj->m_BgVerts.push_back(BgVtx);

			BgVtx.m_Pos[0] = (float)(_BgWidth+1);
			BgVtx.m_Pos[1] = (float)y;
			TextObj->m_BgVerts.push_back(BgVtx);

			BgVtx.m_Pos[0] = (float)(_BgWidth+1);
			BgVtx.m_Pos[1] = (float)y1;
			TextObj->m_BgVerts.push_back(BgVtx);

			BgVtx.m_Pos[0] = -1;
			BgVtx.m_Pos[1] = (float)y1;
			TextObj->m_BgVerts.push_back(BgVtx);
		}
	}

}

//	---------------------------------------------------------------------------
void CTwGraphDirect3D9::DrawTextObj(void *_TextObj, int _X, int _Y, color32 _Color, color32 _BgColor)
{
	assert(m_Drawing==true);
	assert(_TextObj!=NULL);
	CRenderSystem& R = GetRenderSystem();
	CTextObj *TextObj = static_cast<CTextObj *>(_TextObj);
	float x = (float)_X;
	float y = (float)_Y;

	int i;
	int nv = (int)TextObj->m_TextVerts.size();
	int nb = (int)TextObj->m_BgVerts.size();

	if(nb>=4)
	{
		for(i=0; i<nb; ++i)
		{
			TextObj->m_BgVerts[i].m_Pos[0] += x;
			TextObj->m_BgVerts[i].m_Pos[1] += y;
			if(_BgColor!=0 || !TextObj->m_LineBgColors)
				TextObj->m_BgVerts[i].m_Color = _BgColor;
		}

		R.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
		R.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		R.SetFVF(D3DFVF_XYZRHW|D3DFVF_DIFFUSE);
		R.DrawPrimitiveUP(D3DPT_TRIANGLELIST, nb/3, &(TextObj->m_BgVerts[0]), sizeof(CBgVtx));

		for(i=0; i<nb; ++i)
		{
			TextObj->m_BgVerts[i].m_Pos[0] -= x;
			TextObj->m_BgVerts[i].m_Pos[1] -= y;
		}
	}

	if(nv>=4)
	{
		for(i=0; i<nv; ++i)
		{
			TextObj->m_TextVerts[i].m_Pos[0] += x;
			TextObj->m_TextVerts[i].m_Pos[1] += y;
		}
		if(_Color!=0 || !TextObj->m_LineColors)
			for(i=0; i<nv; ++i)
				TextObj->m_TextVerts[i].m_Color = _Color;

		R.SetTexture(0, m_FontD3DTex);
		R.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		R.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		R.SetFVF(D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE2(0));
		R.DrawPrimitiveUP(D3DPT_TRIANGLELIST, nv/3, &(TextObj->m_TextVerts[0]), sizeof(CTextVtx));

		for(i=0; i<nv; ++i)
		{
			TextObj->m_TextVerts[i].m_Pos[0] -= x;
			TextObj->m_TextVerts[i].m_Pos[1] -= y;
		}
	}
}

//	---------------------------------------------------------------------------