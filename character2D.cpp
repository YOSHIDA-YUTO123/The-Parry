//===================================================
//
// �L�����N�^�[2D [character.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include "character2D.h"
#include "textureManager.h"
#include"manager.h"
#include"renderer.h"

//===================================================
// �R���X�g���N�^
//===================================================
CCharacter2D::CCharacter2D(int nPriority) : CObject2D(nPriority)
{
	m_nIdxTexture = NULL;
	m_nLife = NULL;
	m_col = WHITE;
}

//===================================================
// �f�X�g���N�^
//===================================================
CCharacter2D::~CCharacter2D()
{

}

//===================================================
// ����������
//===================================================
HRESULT CCharacter2D::Init(void)
{
	// ������
	if (FAILED(CObject2D::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===================================================
// �I������
//===================================================
void CCharacter2D::Uninit(void)
{
	// �I������
	CObject2D::Uninit();
}

//===================================================
// �X�V����
//===================================================
void CCharacter2D::Update(void)
{
	// ���_���W�̍X�V����
	CObject2D::Update();
}

//===================================================
// �`�揈��
//===================================================
void CCharacter2D::Draw(void)
{
	CTextureManager* pTexture = CManager::GetTexture();

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	pDevice->SetTexture(0, pTexture->GetAdress(m_nIdxTexture));

	// �`�揈��
	CObject2D::Draw();
}


//===================================================
// �e�N�X�`���̐ݒ�
//===================================================
void CCharacter2D::SetTexture(const char* pFilePath)
{
	// �e�N�X�`���N���X�̎擾
	CTextureManager* pTexture = CManager::GetTexture();

	// �e�N�X�`���̔ԍ������炤
	m_nIdxTexture = pTexture->Register(pFilePath);
}

//===================================================
// �_�ŏ���
//===================================================
void CCharacter2D::Flash(const int nCount, const float fSpeed)
{
	// �_�Ŏ��Ԃ��I�������
	if (nCount <= 0)
	{
		// �F�����Ƃɖ߂�
		m_col.a = 1.0f;

		// �F�̐ݒ�
		CObject2D::SetColor(m_col);

		return;
	}

	// �_�ŏ���
	m_col.a = fabsf(sinf(nCount * fSpeed));

	// �F�̐ݒ�
	CObject2D::SetColor(m_col);
}

//===================================================
// �L�����N�^�[�̃q�b�g��̏���
//===================================================
bool CCharacter2D::HitCharacter(const int nDamage)
{
	// �̗͂����炷
	m_nLife -= nDamage;

	if (m_nLife <= 0)
	{
		// �I������
		Uninit();

		return false;
	}
	else
	{
		return true;
	}
}

//===================================================
// �R���X�g���N�^
//===================================================
CAnimCharacter2D::CAnimCharacter2D()
{
	m_fPosTexU = m_fPosTexH = NULL;
	m_nCounterAnim = NULL;
	m_nPatternAnim = NULL;
	m_nPosX = NULL;
	m_nPosY = NULL;
	m_nAnimSpeed = NULL;
	m_nOffTexPosH = NULL;
}

//===================================================
// �f�X�g���N�^
//===================================================
CAnimCharacter2D::~CAnimCharacter2D()
{
}

//===================================================
// ����������
//===================================================
HRESULT CAnimCharacter2D::Init(void)
{
	// ����������
	if (FAILED(CCharacter2D::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===================================================
// �I������
//===================================================
void CAnimCharacter2D::Uninit(void)
{
	// �I������
	CCharacter2D::Uninit();
}

//===================================================
// �X�V����
//===================================================
void CAnimCharacter2D::Update(void)
{
	// �A�j���[�V�����̃J�E���^�[��i�߂�
	m_nCounterAnim++;

	m_fPosTexU = 1.0f / m_nPosX;
	m_fPosTexH = 1.0f / m_nPosY;

	if (m_nCounterAnim >= m_nAnimSpeed)
	{
		m_nCounterAnim = 0;

		m_nPatternAnim++;

		float UV = m_fPosTexU * m_nPatternAnim;
		float HV = m_nOffTexPosH * m_fPosTexH;

		// �e�N�X�`�����W�̍X�V
		CObject2D::SetUvPos(D3DXVECTOR2(UV, HV), D3DXVECTOR2(m_fPosTexU, m_fPosTexH));

		// �ő�܂ōs������
		if (m_nPatternAnim >= (m_nPosX * m_nPosY))
		{
			m_nPatternAnim = 0;
		}
	}

	// �X�V����
	CCharacter2D::Update();
}

//===================================================
// �`�揈��
//===================================================
void CAnimCharacter2D::Draw(void)
{
	// �`�揈��
	CCharacter2D::Draw();
}

//===================================================
// �e�N�X�`���̕������̐ݒ�
//===================================================
void CAnimCharacter2D::SetTexPos(const int nPosX, const int nPosY)
{
	m_nPosX = nPosX;
	m_nPosY = nPosY;
}
