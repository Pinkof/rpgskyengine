#pragma once
#include "RenderWindow.h"
#include "RenderSystem.h"

class DLL_EXPORT CRoot
{
public:
	CRoot();
	~CRoot();
public:
	// �����豸
	// ��direct3d�豸����λ������������������ã�������÷���D3DPOOL_DEFAULT ��Դ���룬��Ϊ��
	// ��Щ��Դ���豸��ʧ����Ҫ����װ�ء������ﴴ������ԴӦ����OnLostDevice �������ͷ�

	// ��ʧ�豸
	// ��Direct3D�豸����lost״̬����IDirect3DDevice9::Reset ����֮ǰ���ô˺�������OnResetDevice 
	// �д�������Դ�����������ͷţ�ͨ���������е�D3DPOOL_DEFAULT ��Դ��


	// �����豸
	//�˻ص�������direct3d�豸������ʱ���ã�ͨ�������ڳ�����ֹ����OnCreateDevice �д�������Դ��Ҫ
	//�������ͷţ�ͨ���������е�D3DPOOL_MANAGED��Դ
	//IDirect3DDevice9::Present ����ʾ��ת������һ�����������ݡ�

	// ��ʼ��
	bool CreateRenderWindow(const std::wstring& wstrWindowTitle, int32 nWidth, int32 nHeight, bool bFullScreen = false);
	//
	virtual void	OnFrameMove(double fTime, float fElapsedTime);

	//�˻ص�������ÿFrame��󱻵��ã��ڳ�����ִ�����е���Ⱦ���ã���������Ҫ�ػ棨����WM_PAINT��
	//Ϣ��ʱ�˺���Ҳ�ᱻ���ã���ʱ������OnFrameMove�����ڴ˺������غ�DXUT������
	//IDirect3DDevice9::Present ����ʾ��ת������һ�����������ݡ�
	virtual void	OnFrameRender(double fTime, float fElapsedTime);
	// ������Ϣ
	// Handle messages to the application 
	virtual bool	MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)= 0;
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	//virtual void	KeyboardProc(UINT nChar, bool bKeyDown, bool bAltDown);
	//virtual void	MouseProc(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
	//	bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos);
	virtual void Run();
	bool MainUpdate();
	//bool			ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps);
	CRenderWindow* GetRenderWindow(){return m_pRenderWindow;}
	CRenderSystem* GetRenderSystem(){return m_pRenderSystem;}
protected:
	CRenderWindow* m_pRenderWindow;
	CRenderSystem* m_pRenderSystem;
};
DLL_EXPORT void SetRoot(CRoot* pDisPlay);
DLL_EXPORT CRoot& GetRoot();

