//===================================================
//
// �}�l�[�W���[ [manager.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// ���d�C���N���[�h�h�~
//***************************************************
#ifndef _MANAGER_H_
#define _MANAGER_H_

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include "main.h"
#include "input.h"
#include "sound.h"
#include "textureManager.h"
#include"camera.h"
#include"light.h"
#include"modelManager.h"
#include"player.h"
#include"meshfield.h"

//***************************************************
// �}�l�[�W���[�N���X�̒�`
//***************************************************
class CManager
{
public:
	CManager();
	~CManager();

	HRESULT Init(HINSTANCE hInstance,HWND hWnd, BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	static CRenderer* GetRenderer(void);
	static CInputKeyboard* GetInputKeyboard(void);
	static CInputJoypad* GetInputJoypad(void);
	static CInputMouse* GetInputMouse(void);
	static CSound* GetSound(void);
	void SetFps(const int fps) { m_fps = fps; }
	static int GetFrameCounter(void);
	static bool GetShowDebug(void) { return m_bShowDebug; }
	static CTextureManager* GetTexture(void);
	static CCamera* GetCamera(void);
	static CLight* GetLight(void);
	static CModelManager* GetModel(void);
	static CPlayer* GetPlayer(void);
	static void EnablePause(void);
	static void SetPause(void) { m_bPause = true; }
	static CMeshField* GetMeshField(void);
private:
	static CRenderer* m_pRenderer;				// �����_���[�̃|�C���^
	static CInputKeyboard* m_pInputKeyboard;	// �L�[�{�[�h�̃|�C���^
	static CInputJoypad* m_pInputJoypad;		// �p�b�h�̃|�C���^
	static CInputMouse* m_pInputMouse;			// �}�E�X�̃|�C���^
	static CSound* m_pSound;					// �T�E���h�̃|�C���^
	int m_fps;									// fps�i�[�p�ϐ�
	static int m_nFrameCounter;					// �t���[���J�E���^�[
	static bool m_bShowDebug;					// �f�o�b�O�\�������邩���Ȃ���
	static CTextureManager* m_pTexture;			// �e�N�X�`���N���X�ւ̃|�C���^	
	static CCamera* m_pCamera;					// �J�����̃|�C���^
	static CLight* m_pLight;					// ���C�g�ւ̃|�C���^
	static CModelManager* m_pModel;				// ���f���N���X�ւ̃|�C���^
	static CPlayer* m_pPlayer;					// �v���C���[�N���X�ւ̃|�C���^
	static bool m_bPause;						// �|�[�Y
	static CMeshField* m_pMeshField;			// ���b�V���t�B�[���h�ւ̃|�C���^
};
#endif