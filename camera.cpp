//===================================================
//
// �J���� [camera.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include "camera.h"
#include "object.h"
#include"manager.h"
#include"renderer.h"
#include "player.h"
#include"math.h"

//***************************************************
// �}�N����`
//***************************************************

// �����̖��O��Ԃ��g�p
namespace
{
	constexpr float MAX_VIEW_TOP = 2.90f;	// �J�����̐���(��)
	constexpr float MAX_VIEW_BOTTOM = 0.1f; // �J�����̐���(��)
	constexpr float HEIGHT_OFFSET = 20.0f;	// �J�����̍����̃I�t�Z�b�g
	constexpr float ROCKON_HEIGHT = 300.0f;	// ���b�N�I���̎��̃J�����̍���
}

//===================================================
// �R���X�g���N�^
//===================================================
CCamera::CCamera()
{
	memset(m_mtxView, NULL, sizeof(D3DXMATRIX));
	memset(m_mtxProjection, NULL, sizeof(D3DXMATRIX));
	m_posR = VEC3_NULL;
	m_posV = VEC3_NULL;
	m_rot = VEC3_NULL;
	m_posRDest = VEC3_NULL;
	m_posVDest = VEC3_NULL;
	m_vecU = VEC3_NULL;
	m_fDistance = NULL;
	m_state = STATE_NONE;
}

//===================================================
// �f�X�g���N�^
//===================================================
CCamera::~CCamera()
{
}

//===================================================
// ����������
//===================================================
HRESULT CCamera::Init(void)
{
	m_posV = D3DXVECTOR3(0.0f, 250.0f, -350.0f);		// ���_

	m_rot = D3DXVECTOR3(D3DX_PI * 0.65f, 0.0f, 0.0f);	// �p�x

	float fRotX = m_posV.x - m_posR.x;
	float fRotY = m_posV.y - m_posR.y;
	float fRotZ = m_posV.z - m_posR.z;

	// ���������߂�
	m_fDistance = sqrtf((fRotX * fRotX) + (fRotY * fRotY) + (fRotZ * fRotZ));

	//�J�����̒����_�������W��ݒ�
	m_posR.x = m_posV.x + sinf(m_rot.x) * sinf(m_rot.y) * m_fDistance;
	m_posR.y = m_posV.y + cosf(m_rot.x) * m_fDistance;
	m_posR.z = m_posV.z + sinf(m_rot.x) * cosf(m_rot.y) * m_fDistance;

	// �J�����̎��_�̏��
	m_posV.x = m_posR.x - sinf(m_rot.x) * sinf(m_rot.y) * m_fDistance;
	m_posV.y = m_posR.y - cosf(m_rot.x) * m_fDistance;
	m_posV.z = m_posR.z - sinf(m_rot.x) * cosf(m_rot.y) * m_fDistance;

	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);				// ������x�N�g��

	m_state = STATE_TRACKING;

	return S_OK;
}

//===================================================
// �I������
//===================================================
void CCamera::Uninit(void)
{
}

//===================================================
// �X�V����
//===================================================
void CCamera::Update(void)
{
	// �}�E�X�̎��_�ړ�
	MouseView();
#if 0

	// �v���C���[�̎擾
	CPlayer* pPlayer = CManager::GetPlayer();

	// �v���C���[�̎擾
	D3DXVECTOR3 playerPos(pPlayer->GetPosition().x, pPlayer->GetPosition().y + 200.0f, pPlayer->GetPosition().z);
	D3DXVECTOR3 playerRot = pPlayer->GetRotation();

	m_posRDest.x = playerPos.x + sinf(playerRot.y) * 1.0f;
	m_posRDest.y = playerPos.y + sinf(playerRot.y) * 1.0f;
	m_posRDest.z = playerPos.z + cosf(playerRot.y) * 1.0f;

	m_posVDest.x = playerPos.x - sinf(m_rot.y) * m_fDistance;
	m_posVDest.y = playerPos.y - cosf(m_rot.y) * m_fDistance;
	m_posVDest.z = playerPos.z - cosf(m_rot.y) * m_fDistance;

	m_posR.x += ((m_posRDest.x - m_posR.x) * 0.07f);
	m_posR.y += ((m_posRDest.y - m_posR.y) * 0.07f);
	m_posR.z += ((m_posRDest.z - m_posR.z) * 0.07f);

	m_posV.x += ((m_posVDest.x - m_posV.x) * 0.07f);
	m_posV.z += ((m_posVDest.z - m_posV.z) * 0.07f);

#endif

	// ���b�V���V�����_�[�̎擾
	CMeshCylinder* pCylinder = CManager::GetCylinder();

	// ���b�V���t�B�[���h�̎擾
	CMeshField* pField = CManager::GetMeshField();

	float fHeight = 0.0f;

	if (pField->Collision(m_posV, &fHeight))
	{
		// ������ݒ�
		m_posV.y = fHeight + HEIGHT_OFFSET;
	}

	// �V�����_�[�̓����蔻��
	pCylinder->Collision(&m_posV);

	// �p�x�̐��K��
	NormalizeRot(&m_rot.x);
	NormalizeRot(&m_rot.y);
	NormalizeRot(&m_rot.z);
}

//===================================================
// �ݒ�,�`�揈��
//===================================================
void CCamera::SetCamera(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �r���[�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxView);

	// �r���[�}�g���b�N�X�̍쐬
	D3DXMatrixLookAtLH(&m_mtxView,
		&m_posV,
		&m_posR,
		&m_vecU);

	// �r���[�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_VIEW, &m_mtxView);

	// �v���W�F�N�V�����}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxProjection);

	// �v���W�F�N�V�����}�g���b�N�X�̍쐬
	D3DXMatrixPerspectiveFovLH(&m_mtxProjection,
		D3DXToRadian(45.0f),
		(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
		10.0f,
		100000.0f);

	// �v���W�F�N�V�����}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_PROJECTION, &m_mtxProjection);
}

//===================================================
// �}�E�X�z�C�[���̏���
//===================================================
void CCamera::MouseWheel(void)
{
	CInputMouse* pMouse = CManager::GetInputMouse();

	// �}�E�X�z�C�[���̉�]�ʂ��擾
	int nWheel = pMouse->GetMouseWeel();

	if (nWheel == 0)
	{
		return;
	}

	if (nWheel < 0)
	{
		m_fDistance += 100.0f;
	}
	else if (nWheel > 0)
	{
		m_fDistance -= 100.0f;
	}

	// �J�����̎��_�̏��
	m_posV.x = m_posR.x - sinf(m_rot.x) * sinf(m_rot.y) * m_fDistance;
	m_posV.y = m_posR.y - cosf(m_rot.x) * m_fDistance;
	m_posV.z = m_posR.z - sinf(m_rot.x) * cosf(m_rot.y) * m_fDistance;
}

//===================================================
// �}�E�X�̎��_�ړ�����
//===================================================
void CCamera::MouseView(void)
{
	if (m_state == STATE_ROCKON) return;

	// �}�E�X�̎擾
	CInputMouse* pMouse = CManager::GetInputMouse();

	D3DXVECTOR2 Move = pMouse->GetVelocity();
	D3DXVECTOR2 MoveOld = pMouse->GetOldVelocity();

	D3DXVECTOR2 fAngle = Move - MoveOld;

	CCamera::MouseWheel();

	CCamera::PadView();

	if (pMouse->OnMousePress(1))
	{
		//��]�ʂ��X�V
		m_rot.y += fAngle.x * 0.01f;
		m_rot.x += fAngle.y * 0.01f;

		//��]�ʂ𐧌�
		if (m_rot.x > MAX_VIEW_TOP)
		{
			m_rot.x -= fAngle.y * 0.01f;
		}
		else if (m_rot.x < MAX_VIEW_BOTTOM)
		{
			m_rot.x -= fAngle.y * 0.01f;
		}

		//�J�������W���X�V
		m_posR.x = m_posV.x + sinf(m_rot.x) * sinf(m_rot.y) * m_fDistance;
		m_posR.y = m_posV.y + cosf(m_rot.x) * m_fDistance;
		m_posR.z = m_posV.z + sinf(m_rot.x) * cosf(m_rot.y) * m_fDistance;

	}
	else if (pMouse->OnMousePress(0))
	{
		//��]�ʂ��X�V
		m_rot.y += fAngle.x * 0.01f;
		m_rot.x += fAngle.y * 0.01f;

		//��]�ʂ𐧌�
		if (m_rot.x > MAX_VIEW_TOP)
		{
			m_rot.x -= fAngle.y * 0.01f;
		}
		else if (m_rot.x < MAX_VIEW_BOTTOM)
		{
			m_rot.x -= fAngle.y * 0.01f;
		}

		// �J�����̎��_�̏��
		m_posV.x = m_posR.x - sinf(m_rot.x) * sinf(m_rot.y) * m_fDistance;
		m_posV.y = m_posR.y - cosf(m_rot.x) * m_fDistance;
		m_posV.z = m_posR.z - sinf(m_rot.x) * cosf(m_rot.y) * m_fDistance;
	}
}

//===================================================
// �p�b�h�̎��_�ړ�����
//===================================================
void CCamera::PadView(void)
{
	// �R���g���[���[�̎擾
	CInputJoypad* pJoypad = CManager::GetInputJoypad();

	// �X�e�B�b�N
	XINPUT_STATE* pStick = pJoypad->GetJoyStickAngle();

	if (pJoypad->GetJoyStickR() == true)
	{
		float RStickAngleY = pStick->Gamepad.sThumbRY;
		float RStickAngleX = pStick->Gamepad.sThumbRX;

		float DeadZone = 10920.0f;
		float fMag = sqrtf((RStickAngleX * RStickAngleX) + (RStickAngleY * RStickAngleY));

		if (fMag >= DeadZone)
		{
			float NormalizeX = RStickAngleX / fMag;
			float NormalizeY = RStickAngleY / fMag;

			float fAngle = fMag * 0.000003f;

			m_rot.y += NormalizeX * 0.5f * fAngle;
			m_rot.x -= NormalizeY * 0.5f * fAngle;
		}
	}

	// �J�����̎��_�̏��
	m_posV.x = m_posR.x - sinf(m_rot.x) * sinf(m_rot.y) * m_fDistance;
	m_posV.y = m_posR.y - cosf(m_rot.x) * m_fDistance;
	m_posV.z = m_posR.z - sinf(m_rot.x) * cosf(m_rot.y) * m_fDistance;
}

//===================================================
// �J�����̒Ǐ]����
//===================================================
void CCamera::SetTracking(const D3DXVECTOR3 posRDest, const float fSpeed, const float fcoef)
{
	if (m_state != STATE_TRACKING) return;

	// �v���C���[�̎擾
	CPlayer* pPlayer = CManager::GetPlayer();

	D3DXVECTOR3 pos = pPlayer->GetPosition()->Get();

	// �v���C���[�̎擾
	D3DXVECTOR3 playerPos(pos.x, pos.y + 200.0f, pos.z);
	D3DXVECTOR3 playerRot = pPlayer->GetRotation()->Get();

	m_posRDest.x = posRDest.x * fSpeed;
	m_posRDest.y = posRDest.y * fSpeed;
	m_posRDest.z = posRDest.z * fSpeed;

	m_posVDest.x = playerPos.x - sinf(m_rot.y) * m_fDistance;
	m_posVDest.y = playerPos.y - cosf(m_rot.y) * m_fDistance;
	m_posVDest.z = playerPos.z - cosf(m_rot.y) * m_fDistance;

	m_posR.x += ((m_posRDest.x - m_posR.x) * fcoef);
	m_posR.y += ((m_posRDest.y - m_posR.y) * fcoef);
	m_posR.z += ((m_posRDest.z - m_posR.z) * fcoef);

	m_posV.x += ((m_posVDest.x - m_posV.x) * fcoef);
	m_posV.z += ((m_posVDest.z - m_posV.z) * fcoef);
}

//===================================================
// ���b�N�I������
//===================================================
void CCamera::Rockon(D3DXVECTOR3 playerPos, D3DXVECTOR3 enemyPos)
{
	// �J�����̏�Ԃ����b�N�I������Ȃ��Ȃ�
	if (m_state != STATE_ROCKON) return;

	// �v���C���[�܂ł̕��������߂�
	D3DXVECTOR3 dir = playerPos - enemyPos;

	// �p�x�����߂�
	float fAngle = atan2f(dir.x, dir.z) + D3DX_PI;

	// �����_
	D3DXVECTOR3 posR;

	// �����_��G�̈ʒu�ɂ���
	posR.x = enemyPos.x;
	posR.y = enemyPos.y;
	posR.z = enemyPos.z;

	// �ړI�̒����_�̐ݒ�
	m_posRDest = posR;

	// �J������rot��ݒ�
	m_rot.y = fAngle;
	
	// y���W�͍l�����Ȃ�
	dir.y = 0.0f; 

	// �����x�N�g���ɂ���
	D3DXVec3Normalize(&dir, &dir);

	// �������|����
	dir *= m_fDistance;

	// y���W��ݒ�
	dir.y = playerPos.y + ROCKON_HEIGHT;

	// �ړI�̎��_�̐ݒ�
	m_posVDest = playerPos + dir;

	// �ړI�̒����_�ɋ߂Â���
	m_posR.x += ((m_posRDest.x - m_posR.x) * 0.07f);
	m_posR.y += ((m_posRDest.y - m_posR.y) * 0.07f);
	m_posR.z += ((m_posRDest.z - m_posR.z) * 0.07f);

	// �ړI�̎��_�ɋ߂Â���
	m_posV.x += ((m_posVDest.x - m_posV.x) * 0.07f);
	m_posV.y += ((m_posVDest.y - m_posV.y) * 0.07f);
	m_posV.z += ((m_posVDest.z - m_posV.z) * 0.07f);
}
