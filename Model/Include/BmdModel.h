#pragma once
#include "Model.h"

class CBmdModel: public CModel
{
public:
	CBmdModel();
	~CBmdModel();
public:
	// ����
	virtual bool LoadFile(const std::string& strFilename);
protected:
};