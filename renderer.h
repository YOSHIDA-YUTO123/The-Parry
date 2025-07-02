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

	void ChangeTarget(D3DXVECTOR3 posV, D3DXVECTOR3 posR, D3DXVECTOR3 vecU); // �����_�����O�^�[�Q�b�g�̕ύX

	LPDIRECT3DDEVICE9 GetDevice(void);
	LPDIRECT3DTEXTURE9 GetTextureMT(void) const { return m_pTextureMT; }

	void onWireFrame();
	void offWireFrame();

private:
	LPDIRECT3DTEXTURE9 m_pTextureMT;		// �����_�����O�^�[�Q�b�g�p�e�N�X�`��
	LPDIRECT3DSURFACE9 m_pRenderMT;			// �����_�����O�^�[�Q�b�g�p�C���^�[�t�F�[�X
	LPDIRECT3DSURFACE9 m_pZBuffMT;			// �����_�����O�^�[�Q�b�g�pZ�o�b�t�@
	D3DVIEWPORT9 m_viewport;				
	LPDIRECT3D9 m_pD3D;						// Directx�f�o�C�X�ւ̃|�C���^
	LPDIRECT3DDEVICE9 m_pD3DDevice;			// Directx�f�o�C�X�ւ̃|�C���^
	static CDebugProc* m_pDebug;			// �f�o�b�O�t�H���g�ւ̃|�C���^
};

#endif