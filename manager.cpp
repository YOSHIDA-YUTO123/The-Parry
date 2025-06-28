//===================================================
//
// �}�l�[�W���[ [manager.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include "manager.h"
#include"renderer.h"
#include "testword.h"
#include "dome.h"
#include "enemy.h"

//***************************************************
// �ÓI�����o�ϐ��̐錾
//***************************************************
CRenderer* CManager::m_pRenderer = nullptr;				// �����_���[�ւ̃|�C���^
CInputKeyboard *CManager::m_pInputKeyboard = nullptr;	// �L�[�{�[�h�ւ̃|�C���^
CInputJoypad* CManager::m_pInputJoypad = nullptr;		// �p�b�h�ւ̃|�C���^
CInputMouse* CManager::m_pInputMouse = nullptr;			// �}�E�X�ւ̃|�C���^
CSound* CManager::m_pSound = nullptr;					// �T�E���h�̃|�C���^
int CManager::m_nFrameCounter = 0;						// �t���[���̃J�E���^�[
bool CManager::m_bShowDebug = true;						// �f�o�b�O�\�������邩���Ȃ���
CTextureManager* CManager::m_pTexture = nullptr;		// �e�N�X�`���N���X�ւ̃|�C���^
CCamera* CManager::m_pCamera = nullptr;					// �J�����̃|�C���^
CLight* CManager::m_pLight = nullptr;					// �J�����ւ̃|�C���^
CModelManager* CManager::m_pModel = nullptr;			// ���f���̃N���X�ւ̃|�C���^
CPlayer* CManager::m_pPlayer = nullptr;					// �v���C���[�N���X�ւ̃|�C���^
bool CManager::m_bPause = false;						// �|�[�Y
CMeshField* CManager::m_pMeshField = nullptr;			// ���b�V���t�B�[���h�̃|�C���^
CSlow* CManager::m_pSlow = nullptr;						// �X���[���[�V�����̃|�C���^
CMeshCylinder* CManager::m_pCylinder = nullptr;			// �V�����_�[�̃N���X�ւ̃|�C���^

//===================================================
// �R���X�g���N�^
//===================================================
CManager::CManager()
{
	m_fps = 0;
}
//===================================================
// �f�X�g���N�^
//===================================================
CManager::~CManager()
{
}
//===================================================
// ����������
//===================================================
HRESULT CManager::Init(HINSTANCE hInstance,HWND hWnd, BOOL bWindow)
{
	// �����_���[�𐶐�
	m_pRenderer = new CRenderer;

	// �T�E���h�̏���������
	m_pSound = new CSound;

	// �L�[�{�[�h�𐶐�
	m_pInputKeyboard = new CInputKeyboard;

	// �p�b�h�𐶐�
	m_pInputJoypad = new CInputJoypad;

	// �}�E�X�𐶐�
	m_pInputMouse = new CInputMouse;

	// ����������
	if (FAILED(m_pRenderer->Init(hWnd, bWindow))) return E_FAIL;

	// �T�E���h�̏���������
	if (FAILED(m_pSound->Init(hWnd))) return E_FAIL;

	// �L�[�{�[�h�̏���������
	if (FAILED(m_pInputKeyboard->Init(hInstance, hWnd))) return E_FAIL;

	// �p�b�h�̏���������
	if (FAILED(m_pInputJoypad->Init())) return E_FAIL;

	// �}�E�X�̏���������
	if (FAILED(m_pInputMouse->Init(hWnd))) return E_FAIL;

	// �e�N�X�`���̐���
	m_pTexture = new CTextureManager;

	// ���ׂẴe�N�X�`���̓ǂݍ��ݏ���
	m_pTexture->Load();

	// ���f���̐���
	m_pModel = new CModelManager;
	m_pModel->Load();

	// �J�����̐���
	m_pCamera = new CCamera;
	m_pCamera->Init();

	// ���C�g�̐���
	m_pLight = new CLight;
	m_pLight->Init();
	m_pLight->SetLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(1.0f,1.0f, 1.0f,1.0f), D3DXVECTOR3(-0.3f, -0.56f, 0.74f), D3DXVECTOR3(3000.0f, 0.0f, 0.0f));
	m_pLight->SetLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR3(-0.18f, -0.29f, -0.74f), D3DXVECTOR3(3000.0f, 0.0f, 0.0f));

	// �X���[���[�V�����̐�������
	m_pSlow = new CSlow;

	// �t�B�[���h�̐ݒ�
	m_pMeshField = CMeshField::Create(VEC3_NULL ,32,32, D3DXVECTOR2(5500.0f,5500.0f));

	// �V�����_�[�̐���
	m_pCylinder = CMeshCylinder::Create(D3DXVECTOR3(0.0f,0.0f,0.0f), 32, 1,1900.0f,1900.0f);

	// �h�[���̐���
	CMeshDome::Create(VEC3_NULL,10,10,60000.0f,20000.0f);

	//CObject3D::Create(D3DXVECTOR3(0.0f,250.0f,0.0f), VEC3_NULL, D3DXVECTOR3(500.0f, 500.0f, 500.0f), "data/TEXTURE/field.png");

	// �v���C���[�̐���
	m_pPlayer = CPlayer::Create();

	CObjectX::Create(VEC3_NULL, "data/MODEL/field/arena.x");

	CEnemy::Create(D3DXVECTOR3(0.0f,0.0f,1500.0f));

	// ���ʂ�Ԃ�
	return S_OK;
}
//===================================================
// �I������
//===================================================
void CManager::Uninit(void)
{
	// ���ׂẴT�E���h�̒�~
	m_pSound->StopSound();

	// �X���[�̔j��
	if (m_pSlow != nullptr)
	{
		delete m_pSlow;
		m_pSlow = nullptr;
	}

	// ���b�V���V�����_�[�̔j��
	if (m_pCylinder != nullptr)
	{
		m_pCylinder->Uninit();
		m_pCylinder = nullptr;
	}

	// ���b�V���t�B�[���h�̔j��
	if (m_pMeshField != nullptr)
	{
		m_pMeshField->Uninit();
		m_pMeshField = nullptr;
	}

	// �v���C���[�̔j��
	if (m_pPlayer != nullptr)
	{
		m_pPlayer = nullptr;
	}

	// ���f���̔j��
	if (m_pModel != nullptr)
	{
		m_pModel->UnLoad();
		delete m_pModel;
		m_pModel = nullptr;
	}

	// �J�����̔j��
	if (m_pCamera != nullptr)
	{
		// �I������
		m_pCamera->Uninit();

		delete m_pCamera;

		m_pCamera = nullptr;
	}

	// ���C�g�̔j��
	if (m_pLight != nullptr)
	{
		// �I������
		m_pLight->Uninit();

		delete m_pLight;

		m_pLight = nullptr;
	}

	// �e�N�X�`���̔j��
	if (m_pTexture != nullptr)
	{
		// �I������
		m_pTexture->UnLoad();

		delete m_pTexture;

		m_pTexture = nullptr;

	}

	// �T�E���h�̔j��
	if (m_pSound != nullptr)
	{
		// �L�[�{�[�h�̏I������
		m_pSound->Uninit();

		delete m_pSound;

		m_pSound = nullptr;
	}

	// �L�[�{�[�h�̔j��
	if (m_pInputKeyboard != nullptr)
	{
		// �L�[�{�[�h�̏I������
		m_pInputKeyboard->Uninit();

		delete m_pInputKeyboard;

		m_pInputKeyboard = nullptr;
	}

	// �p�b�h�̔j��
	if (m_pInputJoypad != nullptr)
	{
		// �p�b�h�̏I������
		m_pInputJoypad->Uninit();

		delete m_pInputJoypad;

		m_pInputJoypad = nullptr;
	}

	// �p�b�h�̔j��
	if (m_pInputMouse != nullptr)
	{
		// �}�E�X�̏I������
		m_pInputMouse->Uninit();

		delete m_pInputMouse;

		m_pInputMouse = nullptr;
	}

	// �����_���[�̔j��
	if (m_pRenderer != nullptr)
	{
		// �I�u�W�F�N�g�̏I������
		m_pRenderer->Uninit();

		delete m_pRenderer;

		m_pRenderer = nullptr;
	}
}
//===================================================
// �X�V����
//===================================================
void CManager::Update(void)
{
	// �t���[���̃J�E���^�[�𑝂₷
	m_nFrameCounter++;

	// 1�t���[���o�߂�����
	if (m_nFrameCounter > FRAME)
	{
		// �J�E���^�[�����Z�b�g
		m_nFrameCounter = 0;
	}

	EnablePause();

	if (m_bPause == false)
	{
		// �X�V����
		m_pRenderer->Update();
	}

	// �L�[�{�[�h�̍X�V����
	m_pInputKeyboard->Update();

	// �p�b�h�̍X�V����
	m_pInputJoypad->Update();

	// �}�E�X�̍X�V����
	m_pInputMouse->Update();

	// �J�����̍X�V����
	m_pCamera->Update();

	// ���C�g�̍X�V����
	m_pLight->Update();

	// �X���[���[�V�����̍X�V����
	m_pSlow->Update();

	if (m_pInputKeyboard->GetTrigger(DIK_F2))
	{
		m_bShowDebug = m_bShowDebug ? false : true;
	}

	// ���C���[�t���[���̏��
	static bool bWireFrame = false;

	if (m_pInputKeyboard->GetTrigger(DIK_F6))
	{
		bWireFrame = bWireFrame ? false : true;
	}

	if (bWireFrame == true)
	{
		m_pRenderer->onWireFrame();
	}
	else
	{
		m_pRenderer->offWireFrame();
	}

}
//===================================================
// �`�揈��
//===================================================
void CManager::Draw(void)
{
	// �X�V����
	m_pRenderer->Draw(m_fps);

}
//===================================================
// �����_���[�̎擾����
//===================================================
CRenderer* CManager::GetRenderer(void)
{
	// �����_���[��NULL��������NULL��Ԃ�
	if (m_pRenderer == nullptr) return nullptr;

	return m_pRenderer;
}
//===================================================
// �L�[�{�[�h�̎擾����
//===================================================
CInputKeyboard* CManager::GetInputKeyboard(void)
{
	// NULL��������
	if (m_pInputKeyboard == nullptr) return nullptr;

	return m_pInputKeyboard;
}
//===================================================
// �p�b�h�̎擾����
//===================================================
CInputJoypad* CManager::GetInputJoypad(void)
{
	// NULL��������
	if (m_pInputJoypad == nullptr) return nullptr;

	return m_pInputJoypad;
}
//===================================================
// �}�E�X�̎擾����
//===================================================
CInputMouse* CManager::GetInputMouse(void)
{
	// NULL��������
	if (m_pInputMouse == nullptr) return nullptr;

	return m_pInputMouse;
}
//===================================================
// �T�E���h�̎擾����
//===================================================
CSound* CManager::GetSound(void)
{
	// NULL��������
	if (m_pSound == nullptr) return nullptr;

	return m_pSound;
}

//===================================================
// �t���[���̃J�E���^�[
//===================================================
int CManager::GetFrameCounter(void)
{
	// ���݂̃t���[���̎擾
	return m_nFrameCounter;
}

//===================================================
// �e�N�X�`���N���X�ւ̃|�C���^
//===================================================
CTextureManager* CManager::GetTexture(void)
{
	// NULL��������
	if (m_pTexture == nullptr) return nullptr;

	return m_pTexture;
}

//===================================================
// �J�����̎擾
//===================================================
CCamera* CManager::GetCamera(void)
{
	// NULL��������
	if (m_pCamera == nullptr) return nullptr;

	return m_pCamera;
}

//===================================================
// ���C�g�̎擾
//===================================================
CLight* CManager::GetLight(void)
{
	// NULL��������
	if (m_pLight == nullptr) return nullptr;

	return m_pLight;
}

//===================================================
// ���f���N���X�̎擾
//===================================================
CModelManager* CManager::GetModel(void)
{
	// NULL��������
	if (m_pModel == nullptr) return nullptr;

	return m_pModel;
}

//===================================================
// �v���C���[�N���X�̎擾
//===================================================
CPlayer* CManager::GetPlayer(void)
{
	// NULL��������
	if (m_pPlayer == nullptr) return nullptr;

	return m_pPlayer;
}

//===================================================
// �|�[�Y�̐ݒ�
//===================================================
void CManager::EnablePause(void)
{
	if (m_pInputKeyboard->GetTrigger(DIK_P))
	{
		m_bPause = m_bPause ? false : true;
	}
}

//===================================================
// ���b�V���t�B�[���h�̎擾�̐ݒ�
//===================================================
CMeshField* CManager::GetMeshField(void)
{
	// NULL��������
	if (m_pMeshField == nullptr) return nullptr;

	return m_pMeshField;
}

