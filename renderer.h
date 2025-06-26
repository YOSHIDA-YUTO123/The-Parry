//===================================================
//
// �|���S���̕`�揈�� [renderer.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// ���d�C���N���[�h�h�~
//***************************************************
#ifndef _RENDERER_H_
#define _RENDERER_H_

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include"main.h"
#include "debugproc.h"

//***************************************************
// �����_���[�N���X�̒�`
//***************************************************
class CRenderer
{
public:
	CRenderer();
	~CRenderer();

	HRESULT Init(HWND hWnd, BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(const int fps);
	LPDIRECT3DDEVICE9 GetDevice(void);

	void onWireFrame();
	void offWireFrame();

private:
	LPDIRECT3D9 m_pD3D;						// Directx�f�o�C�X�ւ̃|�C���^
	LPDIRECT3DDEVICE9 m_pD3DDevice;			// Directx�f�o�C�X�ւ̃|�C���^
	static CDebugProc* m_pDebug;			// �f�o�b�O�t�H���g�ւ̃|�C���^
};

#endif