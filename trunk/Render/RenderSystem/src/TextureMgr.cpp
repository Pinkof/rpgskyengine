#include "TextureMgr.h"
#include "IORead.h"

CTextureMgr::CTextureMgr():m_pLoadingTexture(NULL)
{
}

CTextureMgr::~CTextureMgr()
{
}

CTexture* CTextureMgr::createTexture()
{
	CTexture* pTex = newTexture();
	if (pTex)
	{
		m_setTextureList.insert(pTex);
	}
	return pTex;
}

CTexture* CTextureMgr::CreateTextureFromFile(const std::string& strFilename, int nLevels)
{
	CTexture* pTex = createTexture();
	if (pTex)
	{
		pTex->m_eTexType = CTexture::TEX_TYPE_FORM_FILE;
		pTex->m_bPoolManaged = true;
		pTex->createTextureFromFile(strFilename, nLevels);
	}
	return pTex;
}

CTexture* CTextureMgr::CreateTexture(int nWidth, int nHeight, int nLevels, bool bPoolManaged)
{
	CTexture* pTex = createTexture();
	if (pTex)
	{
		pTex->m_eTexType = CTexture::TEX_TYPE_NORMAL;
		pTex->m_nWidth = nWidth;
		pTex->m_nHeight = nHeight;
		pTex->m_bPoolManaged = bPoolManaged;
		pTex->createTexture(nWidth, nHeight, nLevels, bPoolManaged);
	}
	return pTex;
}

CTexture* CTextureMgr::CreateDynamicTexture(int nWidth, int nHeight)
{
	CTexture* pTex = createTexture();
	if (pTex)
	{
		pTex->m_eTexType = CTexture::TEX_TYPE_DYNAMIC;
		pTex->m_nWidth = nWidth;
		pTex->m_nHeight = nHeight;
		pTex->m_bPoolManaged = false;
		pTex->createDynamicTexture(nWidth, nHeight);
	}
	return pTex;
}

CTexture* CTextureMgr::CreateTextureFromFileInMemory(void* pBuf, int nSize, int nLevels)
{
	CTexture* pTex = createTexture();
	if (pTex)
	{
		pTex->m_eTexType = CTexture::TEX_TYPE_NORMAL;
		pTex->m_bPoolManaged = true;
		pTex->createTextureFromFileInMemory(pBuf, nSize, nLevels);
	}
	return pTex;
}

CTexture* CTextureMgr::CreateTextureFromMemory(void* pBuf, int nSize, int nWidth, int nHeight, int nLevels)
{
	CTexture* pTex = createTexture();
	if (pTex)
	{
		pTex->m_eTexType = CTexture::TEX_TYPE_NORMAL;
		pTex->m_nWidth = nWidth;
		pTex->m_nHeight = nHeight;
		pTex->m_bPoolManaged = true;
		pTex->createTextureFromMemory(pBuf, nSize, nWidth, nHeight, nLevels);
	}
	return pTex;
}

CTexture* CTextureMgr::CreateCubeTextureFromFile(const std::string& strFilename)
{
	CTexture* pTex = createTexture();
	if (pTex)
	{
		pTex->m_eTexType = CTexture::TEX_TYPE_CUBETEXTURE;
		pTex->m_strFilename = strFilename;
		pTex->m_bPoolManaged = true;
		pTex->createCubeTextureFromFile(strFilename);
	}
	return pTex;
}

CTexture* CTextureMgr::CreateRenderTarget(int nWidth, int nHeight)
{
	CTexture* pTex = createTexture();
	if (pTex)
	{
		pTex->m_eTexType = CTexture::TEX_TYPE_RENDERTARGET;
		pTex->m_nWidth = nWidth;
		pTex->m_nHeight = nHeight;
		pTex->m_bPoolManaged = false;
		pTex->createRenderTarget(nWidth, nHeight);
	}
	return pTex;
}

CTexture* CTextureMgr::CreateDepthStencil(int nWidth, int nHeight)
{
	CTexture* pTex = createTexture();
	if (pTex)
	{
		pTex->m_eTexType = CTexture::TEX_TYPE_DEPTHSTENCIL;
		pTex->m_nWidth = nWidth;
		pTex->m_nHeight = nHeight;
		pTex->m_bPoolManaged = false;
		pTex->createDepthStencil(nWidth, nHeight);
	}
	return pTex;
}

uint32 CTextureMgr::RegisterTexture(const std::string& strFilename, int nLevels)
{
	if(!IOReadBase::Exists(strFilename))
	{
		return 0;
	}
	if (find(strFilename))
	{
		return addRef(strFilename);
	}
	//
	CTexture* pTex = newTexture();
	pTex->SetLevels(nLevels);
	pTex->SetFilename(strFilename);
	return add(strFilename, pTex);
}

CTexture* CTextureMgr::getLoadedTexture(uint32 uTexID)
{
	CTexture* pTexture = getItem(uTexID);
	if (pTexture&&!pTexture->isLoaded())
	{
		//pushLoadingTexture(pTexture);
		pTexture->load();
	}
	return pTexture;
}

void CTextureMgr::releaseBuffer(int nIndex)
{
	CTexture* pTex = getItem(nIndex);
	if (pTex)
	{
		pTex->releaseBuffer();
	}
}

void CTextureMgr::releaseBuffer(const std::string& strFilename)
{
	releaseBuffer(find(strFilename));
}

//void CTextureMgr::SetLoad(int nID)
//{
//	m_pLoadTex = GetItem(nID);
//}

void CTextureMgr::pushLoadingTexture(CTexture* pTexture)
{
	if (m_pLoadingTexture==NULL)
	{
		m_pLoadingTexture = pTexture;
	}
}

void CTextureMgr::Update()
{
	if (m_pLoadingTexture)
	{
		m_pLoadingTexture->load();
		m_pLoadingTexture = NULL;
	}
}

void CTextureMgr::remove(CTexture* pTexture)
{
	for (std::set<CTexture*>::iterator it=m_setTextureList.begin(); it!=m_setTextureList.end(); it++)
	{
		if (*it==pTexture)
		{
			m_setTextureList.erase(it);
			return;
		}
	}
}