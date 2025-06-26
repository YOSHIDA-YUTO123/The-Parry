//===================================================
//
// �v���C���[ [player.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include "player.h"
#include "input.h"
#include "manager.h"
#include "effect.h"
#include "particle.h"
#include"model.h"
#include"renderer.h"
#include "impact.h"
#include"motion.h"
#include"score.h"
#include"shadow.h"
#include "explosion.h"
#include "dust.h"
#include "LoadManager.h"

//***************************************************
// �}�N����`
//***************************************************
#define PLAYER_JUMP_HEIGHT (25.0f)  // �W�����v��
#define MOVE_SPEED (10.5f)			// �ړ����x
#define SHADOW_SIZE (100.0f)		// �e�̑傫��
#define SHADOW_MAX_HEIGHT (500.0f)  // �e��������ő�̍���
#define SHADOW_A_LEVEL (0.9f)       // �e�̃A���t�@�l�̃I�t�Z�b�g

//===================================================
// �R���X�g���N�^
//===================================================
CPlayer::CPlayer(int nPriority) : CCharacter3D(nPriority)
{
	m_pMove = nullptr;
	m_fSpeed = NULL;
	m_bJump = false;
	m_pScore = nullptr;
	memset(m_apModel, NULL, sizeof(m_apModel));
	m_nNumModel = NULL;
	m_pMotion = nullptr;
	m_bDash = false;
	m_posOld = VEC3_NULL;
	m_pCollision = nullptr;
}

//===================================================
// �f�X�g���N�^
//===================================================
CPlayer::~CPlayer()
{
}

//===================================================
// ����������
//===================================================
HRESULT CPlayer::Init(void)
{
	// �v���C���[�̃p�����[�^�[�̃��[�h����
	LoadPlayerParam();

	// �X�R�A�̐���
	m_pScore = (CScoreLerper*)CScore::Create(CScore::TYPE_LERPER,D3DXVECTOR3(1150.0f, 50.0f, 0.0f), 180.0f, 30.0f);

	// �e�̐���
	m_pShadow = CShadow::Create(VEC3_NULL, 50.0f, 50.0f, WHITE);

	// �����蔻��̐���
	m_pCollision = new CCollisionSphere;

	// �����̐ݒ�
	m_fSpeed = MOVE_SPEED;

	// �ړ��N���X�̐���
	m_pMove = new CVelocity;

	return S_OK;
}

//===================================================
// �I������
//===================================================
void CPlayer::Uninit(void)
{
	for (int nCnt = 0; nCnt < NUM_PARTS; nCnt++)
	{
		// ���f���̔j��
		if (m_apModel[nCnt] != nullptr)
		{
			// �I������
			m_apModel[nCnt]->Uninit();

			delete m_apModel[nCnt];

			m_apModel[nCnt] = nullptr;
		}
	}

	// ���[�V�����̏I������
	m_pMotion->Uninit();

	// ���[�V�����̔j��
	if (m_pMotion != nullptr)
	{
		delete m_pMotion;

		m_pMotion = nullptr;
	}

	// �X�R�A�̉��
	if (m_pScore != nullptr)
	{
		m_pScore = nullptr;
	}

	// �����蔻��̔j��
	if (m_pCollision != nullptr)
	{
		delete m_pCollision;

		m_pCollision = nullptr;
	}

	// �ړ��N���X�̔j��
	if (m_pMove != nullptr)
	{
		delete m_pMove;
		m_pMove = nullptr;
	}

	CCharacter3D::Uninit();
}

//===================================================
// �X�V����
//===================================================
void CPlayer::Update(void)
{
	// �L�[�{�[�h�̎擾
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	// �R���g���[���[�̎擾
	CInputJoypad* pJoypad = CManager::GetInputJoypad();

	// ���b�V���t�B�[���h�̎擾
	CMeshField* pMesh = CManager::GetMeshField();

	// �J�����̎擾����
	CCamera* pCamera = CManager::GetCamera();

	// �ǂݍ��߂Ă��Ȃ�������
	if (m_pMotion->GetLoadResult() == false)
	{
		return;
	}

	// �ړ�����
	if (pJoypad->GetJoyStickL() == true)
	{
		// �p�b�h�̈ړ�����
		CPlayer::MoveJoypad(pJoypad);
	}
	else
	{
		// �L�[�{�[�h�̈ړ�����
		if(CPlayer::MoveKeyboard(pKeyboard))
		{
			// �_�b�V�����[�V�������������[�V�������𔻒�
			int isDashMotion = (m_bDash ? MOTIONTYPE_DASH : MOTIONTYPE_MOVE);

			// �W�����v��jump����Ȃ����𔻒�
			int motiontype = m_bJump ? isDashMotion : MOTIONTYPE_JUMP;

			// ���[�V�����̐ݒ�
			m_pMotion->SetMotion(motiontype, true, 5);
		}
	}

	// �_�b�V���{�^������������
	if (pKeyboard->GetPress(DIK_LSHIFT) || pJoypad->GetPress(pJoypad->JOYKEY_RIGHT_SHOULDER))
	{
		m_bDash = true;
	}
	else
	{
		m_bDash = false;
	}

	// �ړ��ʂ̌���
	m_pMove->SetInertia3D(0.25f);

	// �O��̈ʒu�̎擾
	m_posOld = GetPosition()->Get();

	// �ړ��ʂ̎擾
	D3DXVECTOR3 move = m_pMove->Get();

	// �ʒu�̍X�V
	GetPosition()->UpdatePosition(move);

	D3DXVECTOR3 pos = GetPosition()->Get();

	float fHeight = 0.0f;

	// ���b�V���t�B�[���h�̓����蔻��
	if (pMesh->Collision(pos,&fHeight))
	{
		pos.y = fHeight;

		m_bJump = true;
		
		if (m_pMotion->GetBlendMotionType() == MOTIONTYPE_JUMP)
		{

			m_pMotion->SetMotion(MOTIONTYPE_LANDING, true, 5);
			CMeshCircle::Create(pos,10.0f,100.0f,10.0f,120);
		}
	}
	else
	{
		m_bJump = false;
	}

	// �d�͂����Z
	m_pMove->Gravity(-MAX_GRABITY);

	if (m_pShadow != nullptr)
	{
		pMesh = CManager::GetMeshField();

		D3DXVECTOR3 FieldNor = pMesh->GetNor(); 				// �n�ʂ̖@���x�N�g���̎擾
		D3DXVECTOR3 PlayerRay = D3DXVECTOR3(0.0f, 1.0f, 0.0f);  // ������x�N�g���̍쐬

		// �n�ʂ̊p�x�ɍ��킹���p�x���擾
		D3DXVECTOR3 rot = m_pShadow->GetFieldAngle(FieldNor, PlayerRay);

		// �e�̐ݒ菈��
		m_pShadow->Setting(D3DXVECTOR3(pos.x,pos.y - fHeight,pos.z),D3DXVECTOR3(pos.x, fHeight + 2.0f, pos.z), SHADOW_SIZE, SHADOW_SIZE, SHADOW_MAX_HEIGHT,SHADOW_A_LEVEL);

		m_pShadow->GetRotaition()->Set(rot);	
	}


	// �W�����v�ł���Ȃ�
	if ((pKeyboard->GetPress(DIK_SPACE) == true || pJoypad->GetPress(pJoypad->JOYKEY_A) == true) && m_bJump == true)
	{
		m_pMotion->SetMotion(MOTIONTYPE_JUMP, true, 5);

		// �ړ��ʂ�������ɐݒ�
		m_pMove->Jump(PLAYER_JUMP_HEIGHT);
		m_bJump = false;
	}

	if (pKeyboard->GetTrigger(DIK_B))
	{
		m_pScore->SetDestScore(100000,120);

		// ���I�𐶐�
		CDust::Create(pos, D3DXVECTOR3(15.0f, 15.0f, 15.0f), 120);
	}

	if (pKeyboard->GetTrigger(DIK_RETURN))
	{
		// ���[�V�����̏�񂪓ǂݍ��߂Ă�����
		if (m_pMotion != nullptr)
		{
			m_pMotion->SetMotion(MOTIONTYPE_ACTION, true, 5);
		}

		SetState(STATE_ACTION, 15);
	}

	if (pKeyboard->GetTrigger(DIK_V))
	{
		pMesh->SetWave(pos, 120, 15.0f, 20.0f, 300.0f, 380.0f,0.01f);
	}

	if (pKeyboard->GetTrigger(DIK_R) || pJoypad->GetTrigger(pJoypad->JOYKEY_RIGHT_THUMB))
	{
		CCamera::STATE state; // �J�����̏��

		// �J�����̏�Ԃ𔻒�
		const bool rockon = pCamera->GetState() == CCamera::STATE_ROCKON;

		// ���b�N�I������Ȃ������烍�b�N�I��
		state = rockon ? CCamera::STATE_TRACKING : CCamera::STATE_ROCKON;

		pCamera->SetState(state);
	}

	// �����蔻��̐ݒ菈��
	if (m_pCollision != nullptr)
	{
		m_pCollision->SetElement(pos);
		m_pCollision->SetRadius(50.0f);
	}

	Parry();

	// �ʒu�̐ݒ�
	GetPosition()->Set(pos);

	// �L�����N�^�[�̍X�V����
	CCharacter3D::Update();

	// �v���C���[�̃��[�V�����̑J��
	TransitionMotion();

	if (m_pMotion != nullptr)
	{
		// ���[�V�����̍X�V����
		m_pMotion->Update(&m_apModel[0], m_nNumModel);
	}

	GetRotation()->SetSmoothAngle(0.1f);

	D3DXVECTOR3 posRDest;

	D3DXVECTOR3 rot = GetRotation()->Get();

	posRDest.x = pos.x + sinf(rot.y) * 1.0f;
	posRDest.y = (pos.y + 200.0f) + sinf(rot.y) * 1.0f;
	posRDest.z = pos.z + cosf(rot.y) * 1.0f;

	// �J�����̒Ǐ]����
	pCamera->SetTracking(posRDest,1.0f,0.1f);
}

//===================================================
// �`�揈��
//===================================================
void CPlayer::Draw(void)
{
	// �L�����N�^�[�̕`��
	CCharacter3D::Draw();

	for (int nCnt = 0; nCnt < m_nNumModel; nCnt++)
	{
		if (m_apModel[nCnt] != nullptr)
		{
			// �`�揈��
			m_apModel[nCnt]->Draw();
		}
	}
}

//===================================================
// �L�[�{�[�h�̈ړ�����
//===================================================
bool CPlayer::MoveKeyboard(CInputKeyboard* pKeyboard)
{
	bool bMove = false;

	// �J�����̎擾
	CCamera* pCamera = CManager::GetCamera();

	// �J�����̌���
	D3DXVECTOR3 cameraRot = pCamera->GetRotaition();

	// ����
	float fSpeed = m_bDash ? MOVE_SPEED : 2.0f;

	// �ړ���
	D3DXVECTOR3 move = m_pMove->Get();

	// ���݂̖ړI�̌����̎擾
	D3DXVECTOR3 rotDest = GetRotation()->GetDest();

	if (pKeyboard->GetPress(DIK_A))
	{
		//�v���C���[�̈ړ�(��)
		if (pKeyboard->GetPress(DIK_W) == true)
		{
			move.x += sinf(cameraRot.y - D3DX_PI * 0.25f) * fSpeed;
			move.z += cosf(cameraRot.y - D3DX_PI * 0.25f) * fSpeed;

			rotDest.y = cameraRot.y + D3DX_PI * 0.75f;

			bMove = true;
		}
		//�v���C���[�̈ړ�(��)
		else if (pKeyboard->GetPress(DIK_S))
		{
			move.x += sinf(cameraRot.y - D3DX_PI * 0.75f) * fSpeed;
			move.z += cosf(cameraRot.y - D3DX_PI * 0.75f) * fSpeed;

			rotDest.y = cameraRot.y + D3DX_PI * 0.25f;

			bMove = true;
		}
		// �v���C���[�̈ړ�(��)
		else
		{
			move.z += sinf(cameraRot.y) * fSpeed;
			move.x -= cosf(cameraRot.y) * fSpeed;

			rotDest.y = cameraRot.y + D3DX_PI * 0.5f;

			bMove = true;
		}
	}
	//�v���C���[�̈ړ�(�E)
	else if (pKeyboard->GetPress(DIK_D))
	{
		//�v���C���[�̈ړ�(��)
		if (pKeyboard->GetPress(DIK_W))
		{
			move.x += sinf(cameraRot.y + D3DX_PI * 0.25f) * fSpeed;
			move.z += cosf(cameraRot.y + D3DX_PI * 0.25f) * fSpeed;

			rotDest.y = cameraRot.y - D3DX_PI * 0.75f;

			bMove = true;
		}
		//�v���C���[�̈ړ�(��)
		else if (pKeyboard->GetPress(DIK_S))
		{
			move.x += sinf(cameraRot.y + D3DX_PI * 0.75f) * fSpeed;
			move.z += cosf(cameraRot.y + D3DX_PI * 0.75f) * fSpeed;

			rotDest.y = cameraRot.y - D3DX_PI * 0.25f;

			bMove = true;
		}
		// �v���C���[�̈ړ�(�E)
		else
		{
			move.z -= sinf(cameraRot.y) * fSpeed;
			move.x += cosf(cameraRot.y) * fSpeed;

			rotDest.y = cameraRot.y - D3DX_PI * 0.5f;

			bMove = true;
		}
	}
	//�v���C���[�̈ړ�(��)
	else if (pKeyboard->GetPress(DIK_W) == true)
	{
		move.x += sinf(cameraRot.y) * fSpeed;
		move.z += cosf(cameraRot.y) * fSpeed;

		rotDest.y = cameraRot.y + D3DX_PI;

		bMove = true;
	}
	//�v���C���[�̈ړ�(��)
	else if (pKeyboard->GetPress(DIK_S) == true)
	{
		move.x -= sinf(cameraRot.y) * fSpeed;
		move.z -= cosf(cameraRot.y) * fSpeed;

		rotDest.y = cameraRot.y;

		bMove = true;
	}
	else
	{
		int motiontype = m_pMotion->GetBlendMotionType();

		if (motiontype == MOTIONTYPE_MOVE || motiontype == MOTIONTYPE_DASH && m_pMotion != nullptr)
		{
			m_pMotion->SetMotion(MOTIONTYPE_NEUTRAL, true, 15);
		}
	}

	// �ړ��ʂ̐ݒ�
	m_pMove->Set(move);

	// �ړI�̌����̐ݒ�
	GetRotation()->SetDest(rotDest);

	return bMove;
}

//===================================================
// �p�b�h�̈ړ�����
//===================================================
void CPlayer::MoveJoypad(CInputJoypad* pJoypad)
{
	XINPUT_STATE* pStick;

	pStick = pJoypad->GetJoyStickAngle();

	// �J�����̎擾
	CCamera* pCamera = CManager::GetCamera();

	// �J�����̌���
	D3DXVECTOR3 cameraRot = pCamera->GetRotaition();

	// ����
	float fSpeed = m_bDash ? MOVE_SPEED : 2.0f;

	// L�X�e�B�b�N�̊p�x
	float LStickAngleY = pStick->Gamepad.sThumbLY;
	float LStickAngleX = pStick->Gamepad.sThumbLX;

	// �f�b�h�]�[��
	float deadzone = 32767.0f * 0.25f;

	// �X�e�B�b�N�̌X�����p�x�����߂�
	float magnitude = sqrtf((LStickAngleX * LStickAngleX) + (LStickAngleY * LStickAngleY));

	// ��������
	if (magnitude > deadzone)
	{
		// �A���O���𐳋K��
		float normalizeX = (LStickAngleX / magnitude);
		float normalizeY = (LStickAngleY / magnitude);

		// �v���C���[�̈ړ���
		float moveX = normalizeX * cosf(-cameraRot.y) - normalizeY * sinf(-cameraRot.y);
		float moveZ = normalizeX * sinf(-cameraRot.y) + normalizeY * cosf(-cameraRot.y);

		// �ړ��ʂ��X�e�B�b�N�̊p�x�ɂ���ĕύX
		float speedWk = fSpeed * ((magnitude - deadzone) / (32767.0f - deadzone));

		// �ړ���
		D3DXVECTOR3 moveWk = VEC3_NULL;

		// �v���C���[�̈ړ�
		moveWk.x += moveX * speedWk;
		moveWk.z += moveZ * speedWk;

		// �ړ��ʂ̐ݒ�
		m_pMove->Set(moveWk);

		// �v���C���[�̊p�x���ړ������ɂ���
		float fDestAngle = atan2f(-moveX, -moveZ);

		// ���݂̖ړI�̌����̎擾
		D3DXVECTOR3 rotDest = GetRotation()->GetDest();

		rotDest.y = fDestAngle;

		// �ړI�̌����̐ݒ�
		GetRotation()->SetDest(rotDest);

		// �_�b�V�����[�V�������������[�V�������𔻒�
		int isDashMotion = (m_bDash ? MOTIONTYPE_DASH : MOTIONTYPE_MOVE);

		// �W�����v��jump����Ȃ����𔻒�
		int motiontype = m_bJump ? isDashMotion : MOTIONTYPE_JUMP;

		m_pMotion->SetMotion(motiontype, true, 5);
	}
	else
	{
		int motiontype = m_pMotion->GetBlendMotionType();

		if (motiontype == MOTIONTYPE_MOVE || motiontype == MOTIONTYPE_DASH)
		{
			m_pMotion->SetMotion(MOTIONTYPE_NEUTRAL, true, 15);
		}
	}
	
}

//===================================================
// �v���C���[�̃p�����[�^�[�̃��[�h����
//===================================================
void CPlayer::LoadPlayerParam(void)
{
	fstream file("data/system.ini"); // �t�@�C�����J��
	string line; // �t�@�C���̕�����ǂݎ��p
	string input; // �l��������

	// �t�@�C�����J������
	if (file.is_open())
	{
		// ���[�h�̃}�l�[�W���̐���
		CLoadManager* pLoadManager = new CLoadManager;

		// �Ōザ��Ȃ��Ȃ�
		while (getline(file,line))
		{
			// �v���C���[�̃��[�V�����t�@�C����ǂݎ������
			if (line.find("PLAYER_MOTION_FILE") != string::npos)
			{
				size_t equal_pos = line.find("="); // =�̈ʒu

				// [=] ���������߂�
				input = line.substr(equal_pos + 1);

				// �t�@�C���̖��O���擾
				string file_name = pLoadManager->GetString(input);

				// �t�@�C���̖��O����
				const char* FILE_NAME = file_name.c_str();

				// ���[�V�����̃��[�h����
				m_pMotion = CMotion::Load(FILE_NAME, &m_apModel[0], NUM_PARTS, &m_nNumModel, MOTIONTYPE_MAX, CMotion::LOAD_TEXT);
			}
		}

		// ���[�h�̃}�l�[�W���[�̔j��
		if (pLoadManager != nullptr)
		{
			delete pLoadManager;
			pLoadManager = nullptr;
		}
		// �t�@�C�������
		file.close();
	}
	else
	{
		MessageBox(NULL, "system.ini���J���܂���", "�t�@�C�������݂��܂���B", MB_OK | MB_ICONWARNING);
		return;
	}
}

//===================================================
// �~�̔���̎擾
//===================================================
CCollisionSphere* CPlayer::GetSphere(void)
{
	if (m_pCollision == nullptr) return nullptr;

	return m_pCollision;
}

//===================================================
// �J�E���^�[
//===================================================
void CPlayer::Parry(void)
{
	if (m_nParryCounter >= 0)
	{
		m_nParryCounter--;
	}
}

//===================================================
// �v���C���[�̃��[�V�����̑J��
//===================================================
void CPlayer::TransitionMotion(void)
{
	// ���[�V�����̎��
	MOTIONTYPE motiontype = (MOTIONTYPE)m_pMotion->GetBlendMotionType();

	// ���[�V�����̑J��
	switch (motiontype)
	{
	case MOTIONTYPE_NEUTRAL:
		break;
	case MOTIONTYPE_MOVE:
		break;
	case MOTIONTYPE_ACTION:
		break;
	default:
		break;
	}
}


//===================================================
// ��������
//===================================================
CPlayer* CPlayer::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot)
{
	CPlayer* pPlayer = nullptr;

	// 3D�I�u�W�F�N�g�̐���
	pPlayer = new CPlayer;

	// �D�揇�ʂ̎擾
	int nPriority = pPlayer->GetPriority();

	// ���݂̃I�u�W�F�N�g�̍ő吔
	const int nNumAll = CObject::GetNumObject(nPriority);

	// �I�u�W�F�N�g���ő吔�܂ł�������
	if (nNumAll >= MAX_OBJECT && pPlayer != nullptr)
	{
		// �����̃|�C���^�̉��
		pPlayer->Release();

		// null�ɂ���
		pPlayer = nullptr;

		// �I�u�W�F�N�g������
		return nullptr;
	}

	if (pPlayer == nullptr) return nullptr;

	pPlayer->CCharacter3D::Init();
	pPlayer->GetPosition()->Set(pos);
	pPlayer->GetRotation()->Set(rot);
	pPlayer->Init();

	return pPlayer;
}
