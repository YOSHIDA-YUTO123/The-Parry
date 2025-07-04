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
#include"particle.h"
#include"motion.h"

constexpr float SHADOW_ALEVEL = 0.7f;			// �e�̃A���t�@�l
constexpr float SHADOW_MAX_HEIGHT = 500.0f;		// �e��������ő�̍���
constexpr float SHADOW_SIZE = 200.0f;			// �e�̑傫��
constexpr int NUM_RUBBLE = 16;					// ���I���o����
constexpr int NUM_MATRIX = 8;					// ����ɂ���}�g���b�N�X�̐�
constexpr int NEXT_ACTION_TIME = 300;			// ���̍s���̒��I�܂ł̎���
constexpr float ROCKON_HEIGHT = 100.0f;			// ���b�N�I�����̌���ꏊ

using MOTION = CEnemyMotionController::TYPE;    // ���[�V�����̎�ނ̗񋓌^���g�p
using namespace math;							// ���O���math���g�p����
using namespace std;							// ���O���std���g�p

//===================================================
// �R���X�g���N�^
//===================================================
CEnemy::CEnemy()
{
	m_pMove = nullptr;
	m_nNumModel = NULL;
	D3DXMatrixIdentity(&m_weponMatrix);
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
		pEnemy->Uninit();

		// null�ɂ���
		pEnemy = nullptr;

		// �I�u�W�F�N�g������
		return nullptr;
	}

	if (pEnemy == nullptr) return nullptr;

	pEnemy->Init();
	pEnemy->m_pCharactor->Init();
	pEnemy->m_pCharactor->SetPosition(pos);
	pEnemy->m_pCharactor->GetRotation()->Set(rot);

	return pEnemy;
}

//===================================================
// ����������
//===================================================
HRESULT CEnemy::Init(void)
{
	// �ړ��N���X�̐���
	m_pMove = make_shared<CVelocity>();

	// ���[�V��������N���X�̐���
	m_pMotion = make_unique<CEnemyMotionController>();

	// ���[�h����
	m_pMotion->Load(m_apModel,&m_nNumModel);
	m_pMotion->Init(m_pMove);

	// �L�����N�^�[�̍쐬
	m_pCharactor = make_unique<CCharacter3D>();

	// �L�����N�^�[�̐ݒ菈��
	m_pCharactor->SetCharacter(10, 5.0f);

	// �G��AI�̐���
	m_pAI = make_unique<CEnemyAI>();

	// �G��AI����������
	m_pAI->Init(m_pMotion);

	// �ʒu�̎擾����
	D3DXVECTOR3 pos = m_pCharactor->GetPosition();

	// �e�̎擾����
	m_pShadow = CShadow::Create(pos, 100.0f, 100.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, SHADOW_ALEVEL));

	// �����蔻��̐���
	m_pSphere = CCollisionSphere::Create(pos,150.0f);
	
	return S_OK;
}

//===================================================
// �I������
//===================================================
void CEnemy::Uninit(void)
{
	for (int nCnt = 0; nCnt < (int)m_apModel.size(); nCnt++)
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

	if (m_pMotion != nullptr)
	{
		// ���[�V�����̏I������
		m_pMotion->Uninit();
	}

	if (m_pShadow != nullptr)
	{
		// �e�N���X�̔j��
		m_pShadow->Uninit();
	}

	if (m_pCharactor != nullptr)
	{
		// �L�����N�^�[�̏I������
		m_pCharactor->Uninit();
	}

	CObject::Release();
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

	D3DXVECTOR3 pos = m_pCharactor->GetPosition();

	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();

	// ���[�V�����̐���N���X�̎擾
	CPlayerMotionController* pPlayerMotion = pPlayer->GetMotionController();

	// �ړ��ʂ̌���
	m_pMove->SetInertia3D(0.25f);

	// �ړ��ʂ̎擾
	D3DXVECTOR3 move = m_pMove->Get();

	pos += move;

	// ���b�V���t�B�[���h�̎擾
	CMeshField* pMesh = CManager::GetMeshField();

	float fHeight = 0.0f;

	// �t�B�[���h�̓����蔻��
	if (pMesh->Collision(pos, &fHeight))
	{
		pos.y = fHeight;
		m_pCharactor->SetPosition(pos);
	}

	// �V�����_�[�̎擾
	CMeshCylinder* pCylinder = CManager::GetCylinder();

	// �V�����_�[�̓����蔻��
	pCylinder->Collision(&pos);

	// �d�͂̐ݒ�
	m_pMove->Gravity(-MAX_GRABITY);

	if (m_pShadow != nullptr)
	{
		pMesh = CManager::GetMeshField();

		D3DXVECTOR3 FieldNor = pMesh->GetNor(); 		   // �n�ʂ̖@���x�N�g���̎擾
		D3DXVECTOR3 VecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);  // ������x�N�g���̍쐬

		// �n�ʂ̊p�x�ɍ��킹���p�x��ݒ�
		m_pShadow->SetFieldAngle(FieldNor, VecU);

		// �e�̐ݒ菈��
		m_pShadow->Update(D3DXVECTOR3(pos.x, pos.y - fHeight, pos.z), D3DXVECTOR3(pos.x, fHeight + 5.0f, pos.z), SHADOW_SIZE, SHADOW_SIZE, SHADOW_MAX_HEIGHT, SHADOW_ALEVEL);
	}

	// ���[�V������null����Ȃ��Ȃ�
	if (m_pMotion != nullptr)
	{
		float fAngle = GetTargetAngle(PlayerPos, pos);		  // �^�[�Q�b�g�܂ł̊p�x�̎擾

		// �ҋ@����Ȃ�������
		if (m_pAI != nullptr && m_pAI->Wait() == false)
		{
			// ���̋����ɂ�����
			if (m_pAI->CheckDistance(PlayerPos, pos, 250.0f) == CEnemyAI::ACTION_MOVE)
			{
				m_pMotion->ConfigMove(m_pCharactor.get(), PlayerPos);
				m_pMotion->SetMotion(MOTION::TYPE_MOVE, true, 10);
			}
			// ���Ȃ�������
			else if (m_pAI->CheckDistance(PlayerPos, pos, 250.0f) == CEnemyAI::ACTION_IDLE)
			{
				m_pMotion->SetMotion(MOTION::TYPE_NEUTRAL, true, 10);
			}

			// �U���\��������
			if (m_pAI->IsAttack() == CEnemyAI::ACTION_ATTACK)
			{
				m_pMotion->SetMotion(MOTION::TYPE_ACTION, true, 10);

				// �ړI�̊p�x�̐ݒ�
				m_pCharactor->GetRotation()->SetDest(D3DXVECTOR3(0.0f, fAngle + D3DX_PI, 0.0f));
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
	// �C���p�N�g�̎擾
	CMeshFieldImpact* pImpact = pMesh->GetImpact();

	if (pImpact != nullptr)
	{
		// �C���p�N�g�Ƃ̔���
		const bool bCollision = pImpact->Collision(pos, 150.0f, pImpact->OBJ_ENEMY);

		// �C���p�N�g�̓����蔻��
		if (bCollision && m_pMotion->GetBlendType() != MOTION::TYPE_DAMAGE)
		{
			// �C���p�N�g�̈ʒu�̎擾
			D3DXVECTOR3 impactPos = pImpact->GetPosition();

			// ������я���
			BlowOff(impactPos, 150.0f, 10.0f);

			// ���[�V�����̐ݒ�
			m_pMotion->SetMotion(MOTION::TYPE_DAMAGE, true, 5);
		}
	}

	// ����̐�[�̈ʒu
	D3DXVECTOR3 WeponPos = GetPositionFromMatrix(m_weponMatrix);

	// ���̈ʒu�̎擾
	D3DXVECTOR3 chestpos = GetPositionFromMatrix(m_apModel[2]->GetMatrixWorld());

	// �G�̍U���̃J�E���^�[�̖ڈ��̕\��
	if (m_pMotion->IsEventFrame(50, 50, MOTION::TYPE_ACTION))
	{
		// �p�[�e�B�N���̐���
		CParticle3D::Create(chestpos, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), 240, 100.0f, 50, 120, 15.0f);
	}

	// �C�x���g�t���[���̔���
	if (m_pMotion->IsEventFrame(64, 71, MOTION::TYPE_ACTION) && m_pMotion->GetBlendType() != MOTION::TYPE_DAMAGE)
	{
		// �����蔻�肪null����Ȃ�������
		if (m_pSphere != nullptr)
		{
			// �v���C���[�̎��E�̒��ɂ���
			const bool bParry = pPlayer->IsParry(pos);

			// ����������
			if (CollisionWepon() && bParry)
			{
				//// �X���[���[�V�����̎擾
				//CSlow* pSlow = CManager::GetSlow();

				//// �X���[���[�V����
				//pSlow->Start(540, 12);

				// �p���B���[�V�����̍Đ�
				pPlayerMotion->SetMotion(pPlayerMotion->TYPE_PARRY, true, 2);
			}
			// �͈͓��Ŏ��E�ɓ����Ă��Ȃ��A�J�E���^�[���Ă��Ȃ�
			else if(CollisionWepon() && bParry == false)
			{
				// ������я���
				pPlayer->BlowOff(pos, 10.0f, 10.0f);

				// �_���[�W���[�V�����̍Đ�
				pPlayerMotion->SetMotion(pPlayerMotion->TYPE_DAMAGE, true, 5);
			}
		}
	}

	// �p���B���[�V�����̃p���`�ɂȂ�����
	if (pPlayerMotion->IsParryEvent(11, 11))
	{
		// �v���C���[�̉E��̈ʒu
		D3DXVECTOR3 playerHandR = pPlayer->GetModelPos(5);

		// �p�[�e�B�N���̐���
		CParticle3D::Create(playerHandR, D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f), 240, 20.0f, 50, 120, 15.0f);

		// �{�X�܂ł̊p�x���擾
		float fAngle = GetTargetAngle(pos, playerHandR);

		// ������я���
		BlowOff(PlayerPos, 150.0f, 12.0f);

		// �C���p�N�g�̐ݒ�
		CMeshCircle::Confing Circleconfig = { 35.0f,10.0f,0.0f,120.0f,30,false };

		// �C���p�N�g�𐶐�
		CMeshCircle::Create(Circleconfig, D3DXCOLOR(1.0f, 1.0f, 0.4f, 0.8f), playerHandR, 32, D3DXVECTOR3(D3DX_PI * 0.5f, fAngle, 0.0f));

		// ���[�V�������_���[�W�ɂ���
		m_pMotion->SetMotion(MOTION::TYPE_DAMAGE, true, 2);
	}

	// �U�����[�V�����̂��������ɂȂ�����
	if (m_pMotion->IsEventFrame(72,72, MOTION::TYPE_ACTION))
	{
		// �t�B�[���h�̔g�̐ݒ�
		CMeshFieldWave::Config config = { WeponPos,250.0f,380.0f,280.0f,12.0f,0.01f,120 };

		// �n�ʂɔg�𔭐�������
		pMesh->SetWave(config);

		// �C���p�N�g�̐ݒ�
		CMeshCircle::Confing Circleconfig = { 0.0f,35.0f,0.0f,50.0f,60,true };

		// ���b�V���T�[�N���̐���
		CMeshCircle::Create(Circleconfig, D3DXCOLOR(1.0f, 0.5f, 0.5f, 1.0f), WeponPos);

		// ���I�̐����o��
		for (int nCnt = 0; nCnt < NUM_RUBBLE; nCnt++)
		{
			// �����ɉ��������������߂�
			float fAngle = (D3DX_PI * 2.0f) / NUM_RUBBLE * nCnt;

			// ������їʂ�I�o
			float dir = rand() % 15 + 5.0f;
			float Jump = rand() % 15 + 25.0f;

			// �����ɉ�����������їʂ��v�Z
			float fMoveX = sinf(fAngle) * dir;
			float fMoveZ = cosf(fAngle) * dir;

			// ������I�o
			int nLife = rand() % 120 + 60;

			// ��ނ�I�o
			int nType = rand() % CRubble::TYPE_MAX;

			// ���I�𐶐�
			CRubble::Create(WeponPos, D3DXVECTOR3(fMoveX, Jump, fMoveZ), nLife, nType);
		}
	}

	// �Ռ��g�̐���
	if (m_pMotion->IsEventFrame(102, 102, MOTION::TYPE_ACTION2))
	{
		// �v���C���[�܂ł̕���
		D3DXVECTOR3 dir = PlayerPos - WeponPos;

		// �C���p�N�g�̐ݒ�
		CMeshFieldImpact::Config config = { WeponPos,chestpos,dir,D3DXCOLOR(1.0f,0.5f,0.5f,1.0f),CMeshFieldImpact::OBJ_ENEMY,100.0f,750.0f,26.0f,60 };

		// �C���p�N�g�̐���
		pMesh->SetImpact(config);
	}

	// ���[�V�����̑J��
	m_pMotion->TransitionMotion(m_pCharactor.get());

	// �ʒu�̐ݒ菈��
    m_pCharactor->SetPosition(pos);

	// �����̕��
	m_pCharactor->GetRotation()->SetSmoothAngle(0.1f);

	pos.y += ROCKON_HEIGHT;

	pCamera->Rockon(PlayerPos, pos);
}

//===================================================
// �`�揈��
//===================================================
void CEnemy::Draw(void)
{
	// �e�̕`�揈��
	if (m_pShadow != nullptr)
	{
		m_pShadow->Draw();
	}

	// ���[�V�����̃f�o�b�O�\��
	if (m_pMotion != nullptr)
	{
		//m_pMotion->Debug();
	}

	// �L�����N�^�[�̕`��
	if (m_pCharactor != nullptr)
	{
		m_pCharactor->Draw();
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
// �G�̐�����я���
//===================================================
void CEnemy::BlowOff(const D3DXVECTOR3 attacker, const float blowOff, const float jump)
{
	// �ʒu
	D3DXVECTOR3 pos = m_pCharactor->GetPosition();

	// �A�^�b�J�[����v���C���[�܂ł̍��������߂�
	D3DXVECTOR3 diff = pos - attacker;

	// �p�x�����߂�
	float fAngle = atan2f(diff.x, diff.z);

	// �ړI�̊p�x�̐ݒ�
	m_pCharactor->GetRotation()->SetDest(D3DXVECTOR3(0.0f, fAngle, 0.0f));

	// �ړ���
	D3DXVECTOR3 move;

	// �ړ��ʂ̐ݒ�
	move.x = sinf(fAngle) * blowOff;
	move.y = jump;
	move.z = cosf(fAngle) * blowOff;

	m_pMove->Set(move);
}

//===================================================
// ����Ƃ̓����蔻��
//===================================================
bool CEnemy::CollisionWepon(void)
{
	// �v���C���[�̎擾
	CPlayer* pPlayer = CManager::GetPlayer();

	// ����̐�̍��W
	D3DXVECTOR3 WeponTop = GetPositionFromMatrix(m_weponMatrix);

	// ����̍����̍��W
	D3DXVECTOR3 WeponBottom = GetPositionFromMatrix(m_apModel[15]->GetMatrixWorld());

	// ����̒��������߂�
	D3DXVECTOR3 diff = WeponTop - WeponBottom;

	// ����̃}�g���b�N�X����
	for (int nCnt = 0; nCnt < NUM_MATRIX; nCnt++)
	{
		// ���Βl
		float fRate = nCnt / (float)NUM_MATRIX;

		// ����̍���(�)�����܂œ_��ł�
		D3DXVECTOR3 pos = WeponBottom + diff * fRate;

		// �ʒu�̍X�V
		m_pSphere->SetPos(pos);

#ifdef _DEBUG

		// ����̃}�g���b�N�X�m�F�p
		CEffect3D::Create(pos, 50.0f, VEC3_NULL, WHITE, 10);
#endif // _DEBUG

		// �G�̕���ɓ���������
		if (m_pSphere->Collision(pPlayer->GetSphere()))
		{
			return true;
		}
	}

	return false;
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
// �G��AI�̃R���X�g���N�^
//===================================================
CEnemyAI::CEnemyAI()
{
	m_pMotion = nullptr;
	m_Action = ACTION_IDLE;
	m_nCounterAction = NULL;
	m_nNextCounter = NULL;
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
void CEnemyAI::Init(std::shared_ptr<CEnemyMotionController> pMotion, const int nBlendFrame, const int nFirstMotion)
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

	// ���̍s���̒��I�܂ł̃J�E���^�[
	m_nNextCounter++;

	int random = -1;

	// ���̍s���̒��I�^�C���ɒB������
	if (m_nNextCounter >= NEXT_ACTION_TIME)
	{
		// �s���𒊑I
		random = rand() % 100 + 1;

		// �J�E���^�[�����Z�b�g
		m_nNextCounter = 0;
	}

	// ���������߂�
	float fDistance = GetDistance(Diff);

	// �͈͓��ɓ�������
	if (fDistance >= fRadius)
	{
		// 55%�̊m���ōU��
		if (random <= 55 && random != -1)
		{
			m_Action = ACTION_ATTACK;
			m_pMotion->SetMotion(m_pMotion->TYPE_ACTION2, true, 10);
		}
		else
		{
			m_Action = ACTION_MOVE;
		}

		return m_Action;
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

	//int random = rand() % 100;

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

		// ���[�V�����̎�ނ��擾
		int moyiontype = m_pMotion->GetType();

		switch (moyiontype)
		{
		case MOTION::TYPE_ACTION:
			m_nCounterAction = 60;
			break;
		case MOTION::TYPE_DAMAGE:
			m_nCounterAction = 30;
			break;
		default:
			break;
		}

		m_pMotion->SetMotion(MOTION::TYPE_NEUTRAL, true, 10);


		return true;
	}

	return false;
}

//===================================================
// �R���X�g���N�^
//===================================================
CEnemyMotionController::CEnemyMotionController()
{
}

//===================================================
// �f�X�g���N�^
//===================================================
CEnemyMotionController::~CEnemyMotionController()
{
}

//===================================================
// ���[�h����
//===================================================
void CEnemyMotionController::Load(std::vector<CModel*>& pModel, int* pOutNumModel)
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
				m_pMotion = CMotion::Load(FILE_NAME, pModel, pOutNumModel, TYPE_MAX, m_pMotion->LOAD_TEXT);
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
// ����������
//===================================================
void CEnemyMotionController::Init(shared_ptr<CVelocity> pMove)
{
	// ���L�|�C���^�̎擾
	m_pMove = pMove;
}

//===================================================
// �I������
//===================================================
void CEnemyMotionController::Uninit(void)
{
	// ���[�V�����̔j��
	if (m_pMotion != nullptr)
	{
		m_pMotion->Uninit();
	}
}

//===================================================
// �X�V����
//===================================================
void CEnemyMotionController::Update(CModel** ppModel, const int nNumModel)
{
	if (m_pMotion != nullptr)
	{
		// ���[�V�����̍X�V����
		m_pMotion->Update(ppModel, nNumModel);
	}
}

//===================================================
// ���[�h�ł������ǂ���
//===================================================
bool CEnemyMotionController::IsLoad(void) const
{
	// ���[�h�ł������ǂ���
	if (m_pMotion->IsLoad())
	{
		return true;
	}
	return false;
}

//===================================================
// ���[�V�����̐ݒ菈��
//===================================================
void CEnemyMotionController::SetMotion(const int type, bool bBlend, const int nFrameBlend)
{
	m_pMotion->SetMotion(type, bBlend, nFrameBlend);
}

//===================================================
// �u�����h���[�V�����̎�ނ̎擾����
//===================================================
int CEnemyMotionController::GetBlendType(void) const
{
	return m_pMotion->GetBlendType();
}

//===================================================
// ���[�V�����̎�ނ̎擾����
//===================================================
int CEnemyMotionController::GetType(void) const
{
	return m_pMotion->GetType();
}

//===================================================
// �C�x���g�t���[���̔���
//===================================================
bool CEnemyMotionController::IsEventFrame(const int start, const int end, const TYPE type)
{
	// �C�x���g�t���[������������
	if (m_pMotion->IsEventFrame(start, end, type))
	{
		return true;
	}

	return false;
}

//===================================================
// 
//===================================================
bool CEnemyMotionController::IsFinishEndBlend(void)
{
	// �Ō�̃u�����h���I�������
	if (m_pMotion->IsFinishEndBlend())
	{
		return true;
	}
	return false;
}

//===================================================
// ���[�V�����̑J�ڏ���
//===================================================
void CEnemyMotionController::TransitionMotion(CCharacter3D *pCharacter)
{
	// ���[�V�����̎�ނ̎擾
	TYPE type = (TYPE)m_pMotion->GetBlendType();

	CPlayer* pPlayer = CManager::GetPlayer();

	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();

	D3DXVECTOR3 pos = pCharacter->GetPosition();

	switch (type)
	{
	case TYPE_NEUTRAL:
		break;
	case TYPE_MOVE:

		break;
	case TYPE_ACTION:
	{
		if (m_pMotion->IsEventFrame(1, 64, TYPE_ACTION))
		{
			// �v���C���[�܂ł̊p�x�����߂�
			float fAngle = GetTargetAngle(pos, PlayerPos);

			pCharacter->GetRotation()->SetDest(D3DXVECTOR3(0.0f, fAngle, 0.0f));
		}
	}
		break;
	case TYPE_DAMAGE:
		break;
	case TYPE_ACTION2:
	{
		if (m_pMotion->IsEventFrame(1, 91, TYPE_ACTION2))
		{
			// �v���C���[�܂ł̊p�x�����߂�
			float fAngle = GetTargetAngle(pos, PlayerPos);

			pCharacter->GetRotation()->SetDest(D3DXVECTOR3(0.0f, fAngle, 0.0f));
		}
	}
		break;
	default:
		break;
	}
}

//===================================================
// �ړ��̍\������
//===================================================
void CEnemyMotionController::ConfigMove(CCharacter3D* pCharacter, const D3DXVECTOR3 dest)
{
	// �ʒu�̎擾
	D3DXVECTOR3 pos = pCharacter->GetPosition();

	D3DXVECTOR3 TargetDir = GetVector(dest, pos);	  // �^�[�Q�b�g�܂ł̕������擾
	float fAngle = GetTargetAngle(dest, pos);		  // �ڕW�܂ł̊p�x�̎擾

	// �ړ��ʂ̎擾
	D3DXVECTOR3 move = m_pMove->Get();

	// ���x�̎擾
	float fSpeed = pCharacter->GetSpeed();

	// �ړ��ʂ̐ݒ�
	move.x = TargetDir.x * fSpeed;
	move.z = TargetDir.z * fSpeed;

	// �ړ��ʂ̐ݒ�
	m_pMove->Set(move);

	// �ړI�̊p�x�̐ݒ�
	pCharacter->GetRotation()->SetDest(D3DXVECTOR3(0.0f, fAngle + D3DX_PI, 0.0f));
}
