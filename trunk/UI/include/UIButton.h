#pragma once

#include "UIStatic.h"
#include "UIDialog.h"

//-----------------------------------------------------------------------------
// Button control
//-----------------------------------------------------------------------------
class DLL_EXPORT CUIButton : public CUIStatic
{
public:
	CUIButton();
	virtual void OnPressDown();
	virtual void OnPressUp();
	// �������
	virtual bool HandleKeyboard(UINT uMsg, WPARAM wParam, LPARAM lParam);
	// �������
	virtual void OnLButtonDown(POINT point);
	virtual void OnLButtonDblClk(POINT point);
	virtual void OnLButtonUp(POINT point);
	virtual void OnHotkey();
	virtual bool ContainsPoint(POINT pt);
	// �Ƿ��ܻ�ȡ����
	virtual bool CanHaveFocus() { return (m_bVisible && m_bEnabled); }

	// ��Ⱦ
	virtual void OnFrameRender(double fTime, float fElapsedTime);

protected:
	virtual CONTROL_STATE GetState();
};