#include "ModelDataMgr.h"
#include "IORead.h"
#include "FileSystem.h"

CModelDataMgr::CModelDataMgr()
{
#ifdef _DEBUG
	m_DataPlugsMgr.createPlugFromPath("Plugins\\debug\\","Model_Plug_CreateObject");
#else
	m_DataPlugsMgr.createPlugFromPath("Plugins\\","Model_Plug_CreateObject");
#endif
}

uint32 CModelDataMgr::RegisterModel(const std::string& strFilename)
{
	if (strFilename.length()==0)
	{
		return 0;
	}
	if(!IOReadBase::Exists(strFilename))// ����ļ��Ƿ����
	{
		return 0;
	}
	if (find(strFilename))// ��һ������������ �����ٴ�������
	{
		return addRef(strFilename);
	}

	CModelData* pModel = new CModelData();
	//if (strExt==".sm")
	//{
	//	pModel = new CModelData();
	//}
	//else if (strExt==".m2")
	//{
	//	pModel = new CM2Model();
	//}
	//else if (strExt==".bmd")
	//{
	//	pModel = new CBmdModel();
	//}
	return add(strFilename, pModel);
}

CModelData* CModelDataMgr::GetModel(uint32 uModelID)
{
	return getItem(uModelID);
}

bool CModelDataMgr::loadModel(CModelData& modelData,const std::string& strFilename)
{
	// �жϸ�ʽ--�����ļ���׺��
	std::string strExt = GetExtension(strFilename);
	CModelPlugBase* pModelPlug = (CModelPlugBase*)m_DataPlugsMgr.getPlugByExtension(strExt);
	if (pModelPlug)
	{
		return pModelPlug->importData(&modelData,strFilename);
	}
	return false;
}

CDataPlugsMgr& CModelDataMgr::getDataPlugsMgr()
{
	return m_DataPlugsMgr;
}