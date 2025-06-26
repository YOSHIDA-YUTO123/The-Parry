//===================================================
//
// �G [enemy.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// �C���N���[�h�t�@�C��
//***************************************************
#include "enemy.h"
#include "player.h"
#include"manager.h"
#include"renderer.h"
#include "math.h"
#include"shadow.h"
#include "dust.h"
#include"explosion.h"
#include "impact.h"
#include "LoadManager.h"

//***************************************************
// �}�N����`
//***************************************************
#define SHADOW_A_LEVEL (0.9f)		// �e�̃A���t�@�l
#define SHADOW_SIZE (300.0f)		// �e�̑傫��
#define SHADOW_MAX_HEIGHT (500.0f)	// �e��������ő�̍���
#define NUM_DUST (16)				// ���I���o����

using namespace math;

//===================================================
// �R���X�g���N�^
//===================================================
CEnemy::CEnemy(int nPriority) : CCharacter3D(nPriority)
{
	memset(m_apModel, NULL, sizeof(m_apModel));
	m_fSpeed = NULL;
	m_pMove = nullptr;
	m_nNumModel = NULL;
	m_pMotion = nullptr;
	m_pAI = nullptr;
	m_pShadow = nullptr;
	D3DXMatrixIdentity(&m_weponMatrix);
	m_pCollision = nullptr;
}

//===================================================
// �f�X�g���N�^
//===================================================
CEnemy::~CEnemy()
{
}

//===================================================
// ��������
//===================================================
CEnemy* CEnemy::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot)
{
	CEnemy* pEnemy = nullptr;

	// �G�I�u�W�F�N�g�̐���
	pEnemy = new CEnemy;

	// �D�揇�ʂ̎擾
	int nPriority = pEnemy->GetPriority();

	// ���݂̃I�u�W�F�N�g�̍ő吔
	const int nNumAll = CObject::GetNumObject(nPriority);

	// �I�u�W�F�N�g���ő吔�܂ł�������
	if (nNumAll >= MAX_OBJECT && pEnemy != nullptr)
	{
		// �����̃|�C���^�̉��
		pEnemy->Release();

		// null�ɂ���
		pEnemy = nullptr;

		// �I�u�W�F�N�g������
		return nullptr;
	}

	if (pEnemy == nullptr) return nullptr;

	pEnemy->CCharacter3D::Init();
	pEnemy->GetPosition()->Set(pos);
	pEnemy->GetRotation()->Set(rot);
	pEnemy->Init();

	return pEnemy;
}

//===================================================
// ����������
//===================================================
HRESULT CEnemy::Init(void)
{
	Load();
	//m_pMotion = CMotion::Load("data/MOTION/motionEnemy000.txt", &m_apModel[0], ENEMY_MAX_PARTS, &m_nNumModel, MOTIONTYPE_MAX,CMotion::LOAD_TEXT);

	// �G��AI�̐���
	m_pAI = new CEnemyAI;

	// �G��AI����������
	m_pAI->Init(m_pMotion);

	// �e�̎擾����
	m_pShadow = CShadow::Create(GetPosition()->Get(), 100.0f, 100.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, SHADOW_A_LEVEL));

	// �����蔻��̐���
	m_pCollision = new CCollisionSphere;
	
	// �ړ��N���X�̐���
	m_pMove = new CVelocity;

	return S_OK;
}

//===================================================
// �I������
//===================================================
void CEnemy::Uninit(void)
{
	for (int nCnt = 0; nCnt < ENEMY_MAX_PARTS; nCnt++)
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

	if (m_pAI != nullptr)
	{
		delete m_pAI;
		m_pAI = nullptr;
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
void CEnemy::Update(void)
{
	// �v���C���[�̎擾
	CPlayer* pPlayer = CManager::GetPlayer();

	// �J�����̎擾����
	CCamera* pCamera = CManager::GetCamera();

	//D3DXVECTOR3 pos = GetPosition();

	D3DXVECTOR3 PlayerPos = pPlayer->GetPosition()->Get();

	// �ړ��ʂ̌���
	m_pMove->SetInertia3D(0.25f);

	// �ړ��ʂ̎擾
	D3DXVECTOR3 move = m_pMove->Get();

	GetPosition()->UpdatePosition(move);

	// ���b�V���t�B�[���h�̎擾
	CMeshField* pMesh = CManager::GetMeshField();

	float fHeight = 0.0f;

	D3DXVECTOR3 pos = GetPosition()->Get();

	if (pMesh->Collision(pos, &fHeight))
	{
		pos.y = fHeight;
		GetPosition()->Set(pos);
	}
	
	// �d�͂̐ݒ�
	m_pMove->Gravity(-MAX_GRABITY);

	if (m_pShadow != nullptr)
	{
		pMesh = CManager::GetMeshField();

		D3DXVECTOR3 FieldNor = pMesh->GetNor(); 		   // �n�ʂ̖@���x�N�g���̎擾
		D3DXVECTOR3 VecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);  // ������x�N�g���̍쐬

		// �n�ʂ̊p�x�ɍ��킹���p�x���擾
		D3DXVECTOR3 rot = m_pShadow->GetFieldAngle(FieldNor, VecU);

		// �e�̐ݒ菈��
		m_pShadow->Setting(D3DXVECTOR3(pos.x, pos.y - fHeight, pos.z), D3DXVECTOR3(pos.x, fHeight + 5.0f, pos.z), SHADOW_SIZE, SHADOW_SIZE, SHADOW_MAX_HEIGHT, SHADOW_A_LEVEL);

		m_pShadow->GetRotaition()->Set(rot);
	}

	if (m_pMotion != nullptr)
	{
		float fAngle = GetTargetAngle(PlayerPos, pos);		  // �^�[�Q�b�g�܂ł̊p�x�̎擾

		// �ҋ@����Ȃ�������
		if (m_pAI != nullptr && m_pAI->Wait() == false)
		{
			// ���̋����ɂ�����
			if (m_pAI->CheckDistance(PlayerPos, pos, 1000.0f) == CEnemyAI::ACTION_MOVE)
			{
				m_pMotion->SetMotion(MOTIONTYPE_MOVE, true, 10);
			}
			// ���Ȃ�������
			else if (m_pAI->CheckDistance(PlayerPos, pos, 1000.0f) == CEnemyAI::ACTION_IDLE)
			{
				m_pMotion->SetMotion(MOTIONTYPE_NEUTRAL, true, 10);
			}

			// �U���\��������
			if (m_pAI->IsAttack() == CEnemyAI::ACTION_ATTACK)
			{
				m_pMotion->SetMotion(MOTIONTYPE_ACTION, true, 10);

				// �ړI�̊p�x�̐ݒ�
				GetRotation()->SetDest(D3DXVECTOR3(0.0f, fAngle + D3DX_PI, 0.0f));
			}
		}

		// AI�̍X�V����
		if (m_pAI != nullptr)
		{
			m_pAI->Update();
		}

		// ���[�V�����̍X�V����
		m_pMotion->Update(&m_apModel[0], m_nNumModel);
	}

	D3DXVECTOR3 modelpos = GetPositionFromMatrix(m_weponMatrix);

	// �U�����[�V�����̂��������ɂȂ�����
	if (m_pMotion->IsIventFrame(72,72,MOTIONTYPE_ACTION))
	{
		// �n�ʂɔg�𔭐�������
		pMesh->SetWave(modelpos,20.0f,300.0f,380.0f,25.0f,0.01f,120);

		// ���b�V���T�[�N���̐���
		CMeshCircle::Create(modelpos, 0.0f, 50.0f, 35.0f, 60,32,D3DXCOLOR(1.0f,0.5f,0.5f,1.0f));

		// ���I�̐����o��
		for (int nCnt = 0; nCnt < NUM_DUST; nCnt++)
		{
			// �����ɉ��������������߂�
			float fAngle = (D3DX_PI * 2.0f) / NUM_DUST * nCnt;

			// ������їʂ�I�o
			float dir = rand() % 15 + 5.0f;
			float Jump = rand() % 15 + 25.0f;

			// �����ɉ�����������їʂ��v�Z
			float fMoveX = sinf(fAngle) * dir;
			float fMoveZ = cosf(fAngle) * dir;

			// ������I�o
			int nLife = rand() % 120 + 60;

			// ���I�𐶐�
			CRubble::Create(modelpos, D3DXVECTOR3(fMoveX, Jump, fMoveZ), nLife);
		}
	}

	// �C�x���g�t���[���̔���
	if (m_pMotion->IsIventFrame(64, 78, MOTIONTYPE_ACTION))
	{
		// �����蔻�肪null����Ȃ�������
		if (m_pCollision != nullptr)
		{
			m_pCollision->SetRadius(250.0f);
			m_pCollision->SetElement(modelpos);

			// ��Ԃ̎擾
			STATE PlayerState = pPlayer->GetState();

			// ����������
			if (m_pCollision->Collision(pPlayer->GetSphere()) && PlayerState == STATE_ACTION)
			{
				m_pMove->Jump(55.0f);
			}
		}
	}

	// ���[�V�����̑J��
	TransitionMotion();

	// �ʒu�̐ݒ菈��
	GetPosition()->Set(pos);

	// �����̕��
	GetRotation()->SetSmoothAngle(0.1f);

	D3DXVECTOR3 modelPos = math::GetPositionFromMatrix(m_apModel[2]->GetMatrixWorld());

	modelPos.y *= 0.5f;

	pCamera->Rockon(PlayerPos, modelPos);
}

//===================================================
// �`�揈��
//===================================================
void CEnemy::Draw(void)
{
	// �L�����N�^�[�̕`��
	CCharacter3D::Draw();

	if (m_pMotion != nullptr)
	{
		m_pMotion->Debug();
	}
	for (int nCnt = 0; nCnt < m_nNumModel; nCnt++)
	{
		if (m_apModel[nCnt] != nullptr)
		{
			// �`�揈��
			m_apModel[nCnt]->Draw();

			// ���f�������킾������
			if (nCnt == 15)
			{
				// �e�q�֌W�̐ݒ菈��
				SetParent(nCnt);
			}
		}
	}
}

//===================================================
// �G�̈ړ�����
//===================================================
void CEnemy::UpdateMoveMotion(void)
{
	CPlayer* pPlayer = CManager::GetPlayer();

	D3DXVECTOR3 pos = GetPosition()->Get();

	D3DXVECTOR3 PlayerPos = pPlayer->GetPosition()->Get();

	D3DXVECTOR3 TargetDir = GetVector(PlayerPos, pos); // �^�[�Q�b�g�܂ł̕������擾
	float fAngle = GetTargetAngle(PlayerPos, pos);		  // �^�[�Q�b�g�܂ł̊p�x�̎擾

	// �ړ��ʂ̎擾
	D3DXVECTOR3 move = m_pMove->Get();

	// �ړ��ʂ̐ݒ�
	move.x = TargetDir.x * 10.0f;
	move.z = TargetDir.z * 10.0f;

	// �ړ��ʂ̐ݒ�
	m_pMove->Set(move);

	// �ړI�̊p�x�̐ݒ�
	GetRotation()->SetDest(D3DXVECTOR3(0.0f, fAngle + D3DX_PI, 0.0f));
}

//===================================================
// ���[�V�����̑J��
//===================================================
void CEnemy::TransitionMotion(void)
{
	// ���[�V�����̎�ނ̎擾
	MOTIONTYPE type = (MOTIONTYPE)m_pMotion->GetBlendMotionType();

	switch (type)
	{
	case MOTIONTYPE_NEUTRAL:
		break;
	case MOTIONTYPE_MOVE:

		// �G�̈ړ�����
		UpdateMoveMotion();

		break;
	case MOTIONTYPE_ACTION:
		break;
	default:
		break;
	}
}

//===================================================
// �e�q�֌W�̐ݒ菈��
//===================================================
void CEnemy::SetParent(const int nCnt)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//�v�Z�p�̃}�g���b�N�X
	D3DXMATRIX mtxRot, mtxTrans, mtxParent;

	//���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_weponMatrix);

	// �e�̈ʒu�A�����̐ݒ�
	D3DXVECTOR3 ParentPos = m_apModel[nCnt]->GetPosition();
	D3DXVECTOR3 ParentRot = m_apModel[nCnt]->GetRotaition();

	//�����𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, ParentRot.y, ParentRot.x, ParentRot.z);
	D3DXMatrixMultiply(&m_weponMatrix, &m_weponMatrix, &mtxRot);

	// �傫���̎擾
	D3DXVECTOR3 Size = m_apModel[nCnt]->GetSize();

	//�ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, ParentPos.x, ParentPos.y + Size.y, ParentPos.z);
	D3DXMatrixMultiply(&m_weponMatrix, &m_weponMatrix, &mtxTrans);

	// �e�̃��[���h�}�g���b�N�X�̎擾
	pDevice->GetTransform(D3DTS_WORLD, &mtxParent);

	// �e�̃��[���h�}�g���b�N�X�Ɗ|�����킹��
	D3DXMatrixMultiply(&m_weponMatrix, &m_weponMatrix, &mtxParent);

	// ���[���h�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_weponMatrix);
}

//===================================================
// �G�̃��[�h����
//===================================================
void CEnemy::Load(void)
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
		while (getline(file, line))
		{
			// �v���C���[�̃��[�V�����t�@�C����ǂݎ������
			if (line.find("ENEMY000_MOTION_FILE") != string::npos)
			{
				size_t equal_pos = line.find("="); // =�̈ʒu

				// [=] ���������߂�
				input = line.substr(equal_pos + 1);

				// �t�@�C���̖��O���擾
				string file_name = pLoadManager->GetString(input);

				// �t�@�C���̖��O����
				const char* FILE_NAME = file_name.c_str();

				// ���[�V�����̃��[�h����
				m_pMotion = CMotion::Load(FILE_NAME, &m_apModel[0], ENEMY_MAX_PARTS, &m_nNumModel, MOTIONTYPE_MAX, CMotion::LOAD_TEXT);
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
// �G��AI�̃R���X�g���N�^
//===================================================
CEnemyAI::CEnemyAI()
{
	m_pMotion = nullptr;
	m_Action = ACTION_IDLE;
	m_nCounterAction = NULL;
}

//===================================================
// �G��AI�̃f�X�g���N�^
//===================================================
CEnemyAI::~CEnemyAI()
{

}

//===================================================
// �G��AI�̏���������
//===================================================
void CEnemyAI::Init(CMotion* pMotion, const int nBlendFrame, const int nFirstMotion)
{
	// ���[�V�����̏��̎擾
	m_pMotion = pMotion;

	m_pMotion->SetMotion(nFirstMotion, true, nBlendFrame);
}

//===================================================
// �X�V����
//===================================================
void CEnemyAI::Update(void)
{
	switch (m_Action)
	{
	case ACTION_WAIT:
		m_nCounterAction--;

		if (m_nCounterAction <= 0)
		{
			m_Action = ACTION_IDLE;
		}
		break;
	default:
		break;
	}
}

//===================================================
// �G�̈ړ���AI
//===================================================
int CEnemyAI::CheckDistance(const D3DXVECTOR3 dest, const D3DXVECTOR3 pos,const float fRadius)
{
	if (m_Action != ACTION_IDLE && m_Action != ACTION_MOVE) return -1;

	// ���������߂�
	D3DXVECTOR3 Diff = dest - pos;

	// ���������߂�
	float fDistance = GetDistance(Diff);

	if (fDistance >= fRadius)
	{
		m_Action = ACTION_MOVE;

		return ACTION_MOVE;
	}

	m_Action = ACTION_IDLE;

	return ACTION_IDLE;
}

//===================================================
// �U���J�n����
//===================================================
int CEnemyAI::IsAttack(void)
{
	if (m_Action != ACTION_IDLE) return -1;

	int random = rand() % 100;

	m_Action = ACTION_ATTACK;

	return ACTION_ATTACK;
}

//===================================================
// �ҋ@���
//===================================================
bool CEnemyAI::Wait(void)
{
	if (m_pMotion->IsFinishEndBlend() == true)
	{
		m_Action = ACTION_WAIT;

		m_pMotion->SetMotion(CEnemy::MOTIONTYPE_NEUTRAL, true, 10);

		m_nCounterAction = 60;

		return true;
	}

	return false;
}
