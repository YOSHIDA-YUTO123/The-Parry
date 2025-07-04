//===================================================
//
// �L�����N�^�[3D [character.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include "character3D.h"
#include "textureManager.h"
#include"manager.h"
#include"renderer.h"
#include"math.h"

//===================================================
// �R���X�g���N�^
//===================================================
CCharacter3D::CCharacter3D()
{
	m_pos = VEC3_NULL;
	m_pRot = nullptr;
	memset(m_mtxWorld, NULL, sizeof(m_mtxWorld));
	m_nLife = NULL;
	m_state = STATE::STATE_NORMAL;
	m_fSpeed = NULL;
}

//===================================================
// �f�X�g���N�^
//===================================================
CCharacter3D::~CCharacter3D()
{
}

//===================================================
// ����������
//===================================================
HRESULT CCharacter3D::Init(void)
{
	// �ʒu�A�����̐���
	m_pRot = new CRotation;

	return S_OK;
}

//===================================================
// �I������
//===================================================
void CCharacter3D::Uninit(void)
{
	// �����̔j��
	if (m_pRot != nullptr)
	{
		delete m_pRot;
		m_pRot = nullptr;
	}
}

//===================================================
// �X�V����
//===================================================
void CCharacter3D::Update(void)
{
	// ��Ԃ̑J��
	switch (m_state)
	{
	case STATE_NORMAL:
		break;
	case STATE_MOVE:
		break;
	case STATE_ACTION:

		// ��Ԃ̃J�E���^�[�����炷
		m_nCounterState--; 

		// �J�E���^�[��0�ɂȂ�����
		if (m_nCounterState <= 0)
		{
			// ��Ԃ��m�[�}���ɖ߂�
			m_state = STATE_NORMAL;
		}

		break;
	case STATE_DAMAGE:

		// ��Ԃ̃J�E���^�[�����炷
		m_nCounterState--; 

		// �J�E���^�[��0�ɂȂ�����
		if (m_nCounterState <= 0)
		{
			// ��Ԃ��m�[�}���ɖ߂�
			m_state = STATE_NORMAL;
		}

		break;
	default:
		break;
	}
}

//===================================================
// �`�揈��
//===================================================
void CCharacter3D::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//�v�Z�p�̃}�g���b�N�X
	D3DXMATRIX mtxRot, mtxTrans, mtxScal;

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxWorld);

	// �����̎擾
	D3DXVECTOR3 rot = m_pRot->Get();

	//�����𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//�ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	//���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);
}

//===================================================
// �L�����N�^�[�̐ݒ菈��
//===================================================
void CCharacter3D::SetCharacter(const int nLife, const float fSpeed)
{
	m_nLife = nLife;
	m_fSpeed = fSpeed;
}

//===================================================
// �L�����N�^�[�̃q�b�g����
//===================================================
bool CCharacter3D::Hit(int nDamage)
{
	// �̗͂����炷
	m_nLife -= nDamage;

	if (m_nLife <= 0)
	{
		// �I������
		Uninit();

		return true;
	}

	return false;
}

//===================================================
// ��Ԃ̐ݒ菈��
//===================================================
void CCharacter3D::SetState(const STATE state, const int nTime)
{
	m_state = state;
	m_nCounterState = nTime;
}
