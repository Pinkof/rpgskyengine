#pragma once
#include "ModelData.h"

class CBmdModel: public CModelData
{
public:
	CBmdModel();
	~CBmdModel();
public:
	// ����
	virtual bool LoadFile(const std::string& strFilename);
protected:
};