#include "UIMainDialog.h"
#include "IME.h"

CUIMainDialog::CUIMainDialog()
{
}

CUIMainDialog::~CUIMainDialog()
{
	CUniBuffer::Uninitialize();
	CIME::Uninitialize();
}

bool CUIMainDialog::OnInitDialog()
{
	CIME::StaticOnCreateDevice();
	CUIControl::s_TipStyle.SetStyle("tip");
	return CUIDialog::OnInitDialog();
}

void CUIMainDialog::OnFrameRender(double fTime, float fElapsedTime)
{
	CUIDialog::OnFrameRender(fTime, fElapsedTime);
	// ��ʾTips
	if (s_pControlMouseOver)
	{
		s_pControlMouseOver->drawTip(m_rcBoundingBox, fTime, fElapsedTime);
	}
	else
	{
		CUIControl::s_TipStyle.Blend(m_rcBoundingBox, CONTROL_STATE_HIDDEN,fElapsedTime);
	}
}

bool CUIMainDialog::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UISetHWND(hWnd);
	// Let the CUIIMEEditBox's static message proc handle the msg.
	// This is because some IME messages must be handled to ensure
	// proper functionalities and the static msg proc ensures that
	// this happens even if no control has the input focus.
	if(CIME::StaticMsgProc(uMsg, wParam, lParam))
	{
		//return true;
	}

	switch(uMsg)
	{
	case WM_SIZE:
		{
			RECT rc={0,0,LOWORD(lParam), HIWORD(lParam)};
			OnSize(rc);
		}
		break;
	case WM_ACTIVATEAPP:
		// Call OnFocusIn()/OnFocusOut() of the control that currently has the focus
		// as the application is activated/deactivated.  This matches the Windows
		// behavior.
		if(s_pControlFocus && 
			s_pControlFocus->GetEnabled())
		{
			if(wParam)
				s_pControlFocus->OnFocusIn();
			else
				s_pControlFocus->OnFocusOut();
		}
		break;
		// Keyboard messages
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		{
			// If a control is in focus, it belongs to this dialog, and it's enabled, then give
			// it the first chance at handling the message.
			if(s_pControlFocus && 
				s_pControlFocus->GetEnabled())
			{
				if(s_pControlFocus->HandleKeyboard(uMsg, wParam, lParam))
					return true;
			}
		}
		break;
	case WM_CAPTURECHANGED:
		{
			// The application has lost mouse capture.
			// The dialog object may not have received
			// a WM_MOUSEUP when capture changed. Reset
			// m_bDrag so that the dialog does not mistakenly
			// think the mouse button is still held down.
			if((HWND)lParam != hWnd)
			{
				SetPressed(false);
			}
			break;
		}
	}

	// ������Ŀؼ� Ҫ���ȴ�����Ϣ ��dialog����
	if(s_pControlFocus && 
		s_pControlFocus->GetEnabled())
	{
		// If the control MsgProc handles it, then we don't.
		if(s_pControlFocus->MsgProc(uMsg, wParam, lParam))
			return true;
	}

	if(CUIDialog::MsgProc(hWnd, uMsg, wParam, lParam))
	{
		return true;
	}

	//ClearState();
	return false;
}

void CUIMainDialog::OnMouseMove(POINT point)
{
	if(s_pControlPressed)
	{
		s_pControlPressed->OnMouseMove(point);
		return;
	}
	CUIDialog::OnMouseMove(point);
}

void CUIMainDialog::OnLButtonUp(POINT point)
{
	// ��ס�Ŀؼ�
	if(s_pControlPressed)
	{
		s_pControlPressed->OnLButtonUp(point);
		s_pControlPressed->SetPressed(false);
		s_pControlPressed = NULL;
		return;
	}
	CUIDialog::OnLButtonUp(point);
}

void CUIMainDialog::OnMouseWheel(POINT point,short wheelDelta)
{
	CUIDialog::OnMouseWheel(point,wheelDelta);
	if(false)
	{
		// ����ؼ� for what? I can't remanber.
		if(s_pControlFocus)
		{
			s_pControlFocus->OnMouseWheel(point,wheelDelta);
			//return true;
		}
	}
}