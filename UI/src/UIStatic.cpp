#include "UIStatic.h"
#include "UIDialog.h"
#include "FileSystem.h"
#include "tinyxml.h"

CUIStatic::CUIStatic()
{
	m_Type = UI_CONTROL_STATIC;
}

void CUIStatic::OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime)
{
	if (m_rcOffset.getWidth()==0&&m_rcScale.getWidth()==0)
	{
		CRect<float> rect = m_rcBoundingBox.getRECT();
		UIGraph::CalcTextRect(m_wstrText,rect);
		m_rcBoundingBox=rect.getRECT();
	}
	m_Style.draw(mTransform,m_rcRelativeBox,m_wstrText,GetState(), fElapsedTime);
}

void CUIStatic::SetText(const std::wstring& wstrText)
{
	m_wstrText = wstrText;
}

void CUIStatic::SetText(int nValue)
{
	m_wstrText = i2ws(nValue);
}