//===================================================
//
// ���I [dust.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include "dust.h"
#include "meshfield.h"
#include "manager.h"
#include"math.h"

//***************************************************
// �}�N����`
//***************************************************
#define SHADOW_SIZE_000 (50.0f)	// �e�̑傫��000
#define SHADOW_SIZE_001 (25.0f)	// �e�̑傫��001
#define SHADOW_SIZE_002 (10.0f)	// �e�̑傫��002

#define SHADOW_MAX_HEIGHT (700.0f)  // �e��������ő�̍���
#define SHADOW_A_LEVEL (0.9f)       // �e�̃A���t�@�l�̃I�t�Z�b�g

//===================================================
// �R���X�g���N�^
//===================================================
CRubble::CRubble(int nPriority) : CObject(nPriority)
{
	m_nLife = NULL;
	m_pObjectX = nullptr;
	ZeroMemory(&m_pos, sizeof(m_pos));
	ZeroMemory(&m_rot, sizeof(m_rot));
	ZeroMemory(&m_move, sizeof(m_move));
	m_nMaxLife = NULL;
	m_fDecAlv = NULL;
	m_fShadowSize = NULL;
}

//===================================================
// �f�X�g���N�^
//===================================================
CRubble::~CRubble()
{
}

//===================================================
// ��������
//===================================================
CRubble* CRubble::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 dir, const int nLife,int nType)
{
	CRubble* pDust = nullptr;

	// ���I�̐���
	pDust = new CRubble;

	// �D�揇�ʂ̎擾
	int nPriority = pDust->GetPriority();

	// ���݂̃I�u�W�F�N�g�̍ő吔
	const int nNumAll = CObject::GetNumObject(nPriority);

	// �I�u�W�F�N�g���ő吔�܂ł�������
	if (nNumAll >= MAX_OBJECT && pDust != nullptr)
	{
		// �����̃|�C���^�̉��
		pDust->Release();

		// null�ɂ���
		pDust = nullptr;

		// �I�u�W�F�N�g������
		return nullptr;
	}

	if (pDust == nullptr) return nullptr;

	switch (nType)
	{
	case TYPE::TYPE_ONE:
		pDust->m_pObjectX = CObjectX::Create(pos, "data/MODEL/dust/dust000.x");
		pDust->m_fShadowSize = SHADOW_SIZE_000;
		break;
	case TYPE::TYPE_TWO:
		pDust->m_pObjectX = CObjectX::Create(pos, "data/MODEL/dust/dust001.x");
		pDust->m_fShadowSize = SHADOW_SIZE_001;
		break;
	case TYPE::TYPE_THREE:
		pDust->m_pObjectX = CObjectX::Create(pos, "data/MODEL/dust/dust002.x");
		pDust->m_fShadowSize = SHADOW_SIZE_002;
		break;
	default:
		break;
	}
	

	pDust->Init();
	pDust->m_nLife = nLife;
	pDust->m_nMaxLife = nLife;
	pDust->m_fDecAlv = 1.0f / nLife;

	pDust->m_move.Set(dir);

	pDust->m_pos.Set(pos);

	return pDust;
}

//===================================================
// ����������
//===================================================
HRESULT CRubble::Init(void)
{
	// �e�̐���
	m_pShadow = CShadow::Create(m_pos.Get(), m_fShadowSize, m_fShadowSize, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.9f));

	return S_OK;
}

//===================================================
// �I������
//===================================================
void CRubble::Uninit(void)
{
	// �I�u�W�F�N�gX�̔j��
	if (m_pObjectX != nullptr)
	{
		m_pObjectX = nullptr;
	}

	// �e�̔j��
	m_pShadow->Uninit();

	// �������g�̔j��
	Release();
}

//===================================================
// �X�V����
//===================================================
void CRubble::Update(void)
{
	CMeshField* pMesh = CManager::GetMeshField();

	//// �X�V����
	//m_pObjectX->Update();

	// �ʒu�̍X�V����
	m_pos.UpdatePosition(m_move.Get());

	// �ʒu�̎擾
	D3DXVECTOR3 pos = m_pos.Get();

	float fHeight = 0.0f;

	// �n�ʂ̓����蔻��
	if (pMesh->Collision(pos, &fHeight))
	{
		pos.y = fHeight;

		// �n�ʂ̖@���̎擾
		D3DXVECTOR3 nor = pMesh->GetNor();

		// �o�E���h�̐ݒ�
		m_move.Bound(nor);
	}

	D3DXVECTOR3 up = pMesh->GetNor(); // ������x�N�g��

	D3DXVECTOR3 dir = m_move.Get();

	D3DXVec3Normalize(&dir, &dir);

	D3DXVECTOR3 axis;
	D3DXVec3Cross(&axis, &up, &dir);

	float dot = D3DXVec3Dot(&up, &dir);
	float fAngle = acosf(dot);

	D3DXMATRIX mtxRot; // �v�Z�p��]�s��

	// �C�ӂ̎������]�s������߂�
	D3DXMatrixRotationAxis(&mtxRot, &axis, fAngle);

	// ��]�s����I�C���[�p�ɕϊ�
	D3DXVECTOR3 rot = math::MatrixToEulerXYZ(mtxRot);

	// �����̐ݒ�
	m_rot.Set(rot);

	// �d�͂̐ݒ�
	m_move.Gravity(-MAX_GRABITY);

	// �e�̍X�V����
	if (m_pShadow != nullptr)
	{
		pMesh = CManager::GetMeshField();

		D3DXVECTOR3 FieldNor = pMesh->GetNor(); 				// �n�ʂ̖@���x�N�g���̎擾
		D3DXVECTOR3 PlayerRay = D3DXVECTOR3(0.0f, 1.0f, 0.0f);  // ������x�N�g���̍쐬

		// �n�ʂ̊p�x�ɍ��킹���p�x��ݒ�
		m_pShadow->SetFieldAngle(FieldNor, PlayerRay);

		// �e�̐ݒ菈��
		m_pShadow->Update(D3DXVECTOR3(pos.x, pos.y - fHeight, pos.z), D3DXVECTOR3(pos.x, fHeight + 2.0f, pos.z), m_fShadowSize, m_fShadowSize, SHADOW_MAX_HEIGHT, SHADOW_A_LEVEL);
	}

	if (m_pObjectX != nullptr)
	{
		// �ʒu�̐ݒ菈��
		m_pObjectX->GetPosition()->Set(pos);

		// �����̐ݒ菈��
		m_pObjectX->GetRotaition()->Set(rot);
	}

	// �ʒu�̐ݒ菈��
	m_pos.Set(pos);

	m_nLife--;

	if (m_nLife <= 0)
	{
		// �I�u�W�F�N�g�̔j��
		m_pObjectX->Uninit();

		Uninit();
	}
}

//===================================================
// �`�揈��
//===================================================
void CRubble::Draw(void)
{
	// ���������߂�
	float fRate = m_nLife / (float)m_nMaxLife;

	// �e�̕`�揈��
	if (m_pShadow != nullptr)
	{
		m_pShadow->Draw();
	}

	if (m_pObjectX != nullptr)
	{
		// �`�揈��(�����x�ݒ�)
		m_pObjectX->Draw(fRate);
	}
}
