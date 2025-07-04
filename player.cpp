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
#include"renderer.h"
#include "impact.h"
#include"motion.h"
#include"score.h"
#include"shadow.h"
#include "explosion.h"
#include "dust.h"
#include "LoadManager.h"
#include "Wave.h"
#include "cylinder.h"

using namespace math; // ���O���math���g�p
using namespace std;  // ���O��Ԃ�std���g�p����

using MOTION = CPlayerMotionController::TYPE; // �񋓌^���g�p����

//***************************************************
// �}�N����`
//***************************************************
#define PLAYER_JUMP_HEIGHT (25.0f)  // �W�����v��
#define SHADOW_SIZE (50.0f)			// �e�̑傫��
#define SHADOW_MAX_HEIGHT (500.0f)  // �e��������ő�̍���
#define SHADOW_A_LEVEL (0.9f)       // �e�̃A���t�@�l�̃I�t�Z�b�g

//===================================================
// �R���X�g���N�^
//===================================================
CPlayer::CPlayer(int nPriority) : CObject(nPriority)
{
	m_pMove = nullptr;
	m_bJump = true;
	m_pScore = nullptr;
	m_nNumModel = NULL;
	m_bDash = false;
	m_posOld = VEC3_NULL;
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
	// �v���C���[�̃��[�V��������N���X�̐���
	m_pMotion = make_unique<CPlayerMotionController>();

	// �v���C���[�̃��[�h����
	m_pMotion->Load(m_apModel,&m_nNumModel);

	// �L�����N�^�[�N���X�̐���
	m_pCharacter3D = make_unique<CCharacter3D>();

	// �L�����N�^�[�̐ݒ菈��
	m_pCharacter3D->SetCharacter(10, 6.0f);

	// �L�����N�^�[�̈ʒu�̎擾
	D3DXVECTOR3 pos = m_pCharacter3D->GetPosition();

	// ���E����̐���
	m_pFOV = CCollisionFOV::Create(pos, 1000.0f);

	// �X�R�A�̐���
	m_pScore = (CScoreLerper*)CScore::Create(CScore::TYPE_LERPER,D3DXVECTOR3(1150.0f, 50.0f, 0.0f), 180.0f, 30.0f);

	// �e�̐���
	m_pShadow = CShadow::Create(VEC3_NULL, 50.0f, 50.0f, WHITE);

	// �ړ��N���X�̐���
	m_pMove = new CVelocity;

	return S_OK;
}

//===================================================
// �I������
//===================================================
void CPlayer::Uninit(void)
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

	// �X�R�A�̉��
	if (m_pScore != nullptr)
	{
		m_pScore = nullptr;
	}

	// �ړ��N���X�̔j��
	if (m_pMove != nullptr)
	{
		delete m_pMove;
		m_pMove = nullptr;
	}

	// ���[�V�����̏I������
	m_pMotion->Uninit();

	// �e�N���X�̔j��
	m_pShadow->Uninit();

	// �L�����N�^�[�̔j��
	m_pCharacter3D->Uninit();

	CObject::Release();
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
	if (m_pMotion->IsLoad() == false)
	{
		return;
	}

	// �ړ�����
	if (pJoypad->GetJoyStickL() == true)
	{
		// �p�b�h�̈ړ�����
		CPlayer::MoveJoypad(pJoypad);
	}
	// �L�[�{�[�h�̈ړ�����
	else if(CPlayer::MoveKeyboard(pKeyboard))
	{
		// �_�b�V�����[�V�������������[�V�������𔻒�
		int isDashMotion = (m_bDash ? MOTION::TYPE_DASH : MOTION::TYPE_MOVE);

		// �W�����v��jump����Ȃ����𔻒�
		int motiontype = m_bJump ? isDashMotion : MOTION::TYPE_JUMP;

		// ���[�V�����̐ݒ�
		m_pMotion->SetMotion(motiontype, true, 5);
	}
	
	// �_�b�V���{�^������������
	if ((pKeyboard->GetPress(DIK_LSHIFT) || pJoypad->GetPress(pJoypad->JOYKEY_RIGHT_SHOULDER)))
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
	m_posOld = m_pCharacter3D->GetPosition();

	// �ړ��ʂ̎擾
	D3DXVECTOR3 move = m_pMove->Get();

	D3DXVECTOR3 pos = m_pCharacter3D->GetPosition();

	// �ʒu�̍X�V
	pos += move;

	float fHeight = 0.0f;

	// ���b�V���t�B�[���h�̓����蔻��
	if (pMesh->Collision(pos,&fHeight))
	{
		// �����̐ݒ�
		pos.y = fHeight;

		// �W�����v�ł���
		m_bJump = true;
		
		// ���[�V�������W�����v��������
		if (m_pMotion->GetBlendType() == MOTION::TYPE_JUMP)
		{
			// ���n���[�V�����̍Đ�
			m_pMotion->SetMotion(MOTION::TYPE_LANDING, true, 5);

			// �C���p�N�g�̐ݒ�
			CMeshCircle::Confing Circleconfig = { 0.0f,10.0f,10.0f,50.0f,30,false };

			// �T�[�N���𐶐�
			CMeshCircle::Create(Circleconfig, D3DCOLOR_RGBA(220, 220, 220, 200),pos,32);
		}
	}
	else
	{
		m_bJump = false;
	}

	// ���b�V���V�����_�[�̎擾
	CMeshCylinder* pCylinder = CManager::GetCylinder();

	// �V�����_�[�̔���
	pCylinder->Collision(&pos);

	// �C���p�N�g�̎擾
	CMeshFieldImpact* pImpact = pMesh->GetImpact();

	if (pImpact != nullptr)
	{
		// �C���p�N�g�Ƃ̔���
		const bool bCollision = pImpact->Collision(pos, 150.0f, pImpact->OBJ_PLAYER);

		if (bCollision && m_pCharacter3D->GetState() == m_pCharacter3D->STATE_ACTION)
		{
			// �ŏ��̈ʒu
			D3DXVECTOR3 firstPos = pImpact->GetFirstPos();

			// �����̐ݒ�
			D3DXVECTOR3 dir = firstPos - pos;

			float fAngle = GetTargetAngle(firstPos, pos);

			D3DXVECTOR3 playerHandR = GetPositionFromMatrix(m_apModel[5]->GetMatrixWorld());

			// ���[�V�������_���[�W�ɂ���
			m_pMotion->SetMotion(MOTION::TYPE_PARRY, true, 2);

			// �p�[�e�B�N���̐���
			CParticle3D::Create(playerHandR, D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f), 240, 20.0f, 25, 120, 15.0f);

			// �C���p�N�g�̐ݒ�
			CMeshCircle::Confing Circleconfig = { 50.0f,10.0f,0.0f,50.0f,30,false };

			// �C���p�N�g�𐶐�
			CMeshCircle::Create(Circleconfig,D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f),pos,32, D3DXVECTOR3(D3DX_PI * 0.5f, fAngle, 0.0f));

			// �Đݒ�
			pImpact->Reset(dir,pImpact->OBJ_PLAYER, pos,D3DXCOLOR(1.0f,1.0f,0.5f,1.0f));
		}
		// �C���p�N�g�̓����蔻��
		else if (bCollision && m_pMotion->GetBlendType() != MOTION::TYPE_DAMAGE)
		{
			// �C���p�N�g�̈ʒu�̎擾
			D3DXVECTOR3 impactPos = pImpact->GetPosition();

			// ������я���
			BlowOff(impactPos, 50.0f, 10.0f);

			// ���[�V�����̐ݒ�
			m_pMotion->SetMotion(MOTION::TYPE_DAMAGE, true, 5);
		}
	}

	// �d�͂����Z
	m_pMove->Gravity(-MAX_GRABITY);

	// �e�̍X�V����
	if (m_pShadow != nullptr)
	{
		D3DXVECTOR3 FieldNor = pMesh->GetNor(); 	// �n�ʂ̖@���x�N�g���̎擾

		D3DXVECTOR3 PlayerRay = D3DXVECTOR3(0.0f, 1.0f, 0.0f);  // ������x�N�g���̍쐬

		// �e�̐ݒ菈��
		m_pShadow->Update(D3DXVECTOR3(pos.x,pos.y - fHeight,pos.z),D3DXVECTOR3(pos.x, fHeight + 2.0f, pos.z), SHADOW_SIZE, SHADOW_SIZE, SHADOW_MAX_HEIGHT,SHADOW_A_LEVEL);

		// �n�ʂ̊p�x�ɍ��킹���p�x��ݒ�
		m_pShadow->SetFieldAngle(FieldNor, PlayerRay);
	}

	// �W�����v�ł���Ȃ�
	if ((pKeyboard->GetPress(DIK_SPACE) == true || pJoypad->GetPress(pJoypad->JOYKEY_A) == true) && m_bJump == true)
	{
		m_pMotion->SetMotion(MOTION::TYPE_JUMP, true, 2);

		// �ړ��ʂ�������ɐݒ�
		m_pMove->Jump(PLAYER_JUMP_HEIGHT);
		m_bJump = false;
	}

#ifdef _DEBUG

	//if (pKeyboard->GetTrigger(DIK_B))
	//{
	//	m_pScore->SetDestScore(100000,120);

	//	// ���I�𐶐�
	//	CRubble::Create(pos, D3DXVECTOR3(15.0f, 15.0f, 15.0f), 120);

	//	// �X���[���[�V�����̎擾
	//	CSlow* pSlow = CManager::GetSlow();
	//	pSlow->Start(540, 4);
	//}

	//if (pKeyboard->GetTrigger(DIK_V))
	//{
	//	// �n�ʂɔg�𔭐�������
	//	pMesh->SetWave(pos, 20.0f, 300.0f, 380.0f, 15.0f, 0.01f, 120);
	//}

#endif // _DEBUG

	// �J�E���^�[���
	if (pKeyboard->GetTrigger(DIK_RETURN) && m_pMotion->GetBlendType() != MOTION::TYPE_DAMAGE)
	{
		m_pMotion->SetMotion(MOTION::TYPE_ACTION, true,6);
	
		m_pCharacter3D->SetState(m_pCharacter3D->STATE_ACTION, 25);
	}

	// ���b�N�I��
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
	if (m_pSphere != nullptr)
	{
		// �ʒu�̐ݒ菈��
		m_pSphere->SetPos(pos);
	}

	// ���E����
	if (m_pFOV != nullptr)
	{
		// �ʒu�̐ݒ�
		m_pFOV->SetPos(pos);
	}

	UpdateParry();

	// �L�����N�^�[�̍X�V����
	m_pCharacter3D->Update();

	// �ʒu�̐ݒ�
	m_pCharacter3D->SetPosition(pos);

	if (m_pMotion != nullptr)
	{
		// ���[�V�����̍X�V����
		m_pMotion->Update(&m_apModel[0], m_nNumModel);
	}

	// �ړI�̎��_�ɋ߂Â���
	m_pCharacter3D->GetRotation()->SetSmoothAngle(0.1f);

	D3DXVECTOR3 posRDest;

	D3DXVECTOR3 rot = m_pCharacter3D->GetRotation()->Get();

	D3DXVECTOR3 modelpos = math::GetPositionFromMatrix(m_apModel[2]->GetMatrixWorld());

	posRDest.x = modelpos.x + sinf(rot.y) * 1.0f;
	posRDest.y = (modelpos.y + 0.0f) + sinf(rot.y) * 1.0f;
	posRDest.z = modelpos.z + cosf(rot.y) * 1.0f;

	// �J�����̒Ǐ]����
	pCamera->SetTracking(posRDest,1.0f,0.1f);
}

//===================================================
// �`�揈��
//===================================================
void CPlayer::Draw(void)
{
	D3DXVECTOR3 pos = m_pCharacter3D->GetPosition();

	//m_pMotion->Debug();
	// �f�o�b�N
	CDebugProc::Print("�v���C���[�̈ʒu X = %.2f Y = %.2f Z = %.2f\n", pos.x, pos.y,pos.z);

#if 0

	LPDIRECT3DSURFACE9 pRenderDef, pZBuffer;
	D3DVIEWPORT9 viepowtDef;
	D3DXMATRIX mtxViewDef, mtxProjectionDef;

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ���݂̃����_�[�^�[�Q�b�g�̎擾
	pDevice->GetRenderTarget(0, &pRenderDef);

	// ���݂�Z�o�b�t�@�̎擾
	pDevice->GetDepthStencilSurface(&pZBuffer);

	// ���݂̃r���[�|�[�g�̎擾
	pDevice->GetViewport(&viepowtDef);

	// ���݂̃r���[�}�g���b�N�X�̎擾
	pDevice->GetTransform(D3DTS_VIEW, &mtxViewDef);

	// ���݂̃v���W�F�N�V�����}�g���b�N�X�̎擾
	pDevice->GetTransform(D3DTS_PROJECTION, &mtxProjectionDef);

	CCamera* pCamera = CManager::GetCamera();

	D3DXVECTOR3 posV, posR,vecU;

	posV = pCamera->GetPosV();

	posR = pCamera->GetPosR();

	vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	// �����_�����O�^�[�Q�b�g�̕ύX
	CManager::GetRenderer()->ChangeTarget(posV, posR, vecU);

	// �����_�����O�^�[�Q�b�g�p�e�N�X�`���̃N���A
	pDevice->Clear(0,
		NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(0, 255, 255, 255), 1.0f, 0);
#endif

	if (m_pShadow != nullptr)
	{
		// �e�̕`��
		m_pShadow->Draw();
	}

	// �L�����N�^�[�̕`��
	m_pCharacter3D->Draw();

	for (int nCnt = 0; nCnt < m_nNumModel; nCnt++)
	{
		if (m_apModel[nCnt] != nullptr)
		{
			// �`�揈��
			m_apModel[nCnt]->Draw();
		}
	}

#if 0

	// �����_�[�^�[�Q�b�g�����Ƃɖ߂�
	pDevice->SetRenderTarget(0, pRenderDef);

	// Z�o�b�t�@�����Ƃɖ߂�
	pDevice->SetDepthStencilSurface(pZBuffer);

	// �r���[�|�[�g�����Ƃɖ߂�
	pDevice->SetViewport(&viepowtDef);

	pDevice->SetTransform(D3DTS_VIEW, &mtxViewDef);

	pDevice->SetTransform(D3DTS_PROJECTION, &mtxProjectionDef);

#endif // 0
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
	float fSpeed = m_bDash ? m_pCharacter3D->GetSpeed() : 1.5f;

	// �ړ���
	D3DXVECTOR3 move = m_pMove->Get();

	// ���݂̖ړI�̌����̎擾
	D3DXVECTOR3 rotDest = m_pCharacter3D->GetRotation()->GetDest();

	if (m_pMotion->GetBlendType() == MOTION::TYPE_DAMAGE)
	{
		return false;
	}
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
		int motiontype = m_pMotion->GetBlendType();

		if (motiontype == MOTION::TYPE_MOVE || motiontype == MOTION::TYPE_DASH && m_pMotion != nullptr)
		{
			m_pMotion->SetMotion(MOTION::TYPE_NEUTRAL, true, 15);
		}
	}

	// �ړ��ʂ̐ݒ�
	m_pMove->Set(move);

	// �ړI�̌����̐ݒ�
	m_pCharacter3D->GetRotation()->SetDest(rotDest);

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
	float fSpeed = m_bDash ? m_pCharacter3D->GetSpeed() : 2.0f;

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
		D3DXVECTOR3 rotDest = m_pCharacter3D->GetRotation()->GetDest();

		rotDest.y = fDestAngle;

		// �ړI�̌����̐ݒ�
		m_pCharacter3D->GetRotation()->SetDest(rotDest);

		// �_�b�V�����[�V�������������[�V�������𔻒�
		int isDashMotion = (m_bDash ? MOTION::TYPE_DASH : MOTION::TYPE_MOVE);

		// �W�����v��jump����Ȃ����𔻒�
		int motiontype = m_bJump ? isDashMotion : MOTION::TYPE_JUMP;

		m_pMotion->SetMotion(motiontype, true, 5);
	}
	else
	{
		int motiontype = m_pMotion->GetBlendType();

		if ((motiontype == MOTION::TYPE_MOVE || motiontype == MOTION::TYPE_DASH))
		{
			m_pMotion->SetMotion(MOTION::TYPE_NEUTRAL, true, 15);
		}
	}
	
}


//===================================================
// �J�E���^�[
//===================================================
void CPlayer::UpdateParry(void)
{
	if (m_nParryCounter >= 0)
	{
		m_nParryCounter--;
	}
}


//===================================================
// ������я���
//===================================================
void CPlayer::BlowOff(const D3DXVECTOR3 attacker, const float blowOff, const float jump)
{
	// �ʒu
	D3DXVECTOR3 pos = m_pCharacter3D->GetPosition();

	// �A�^�b�J�[����v���C���[�܂ł̍��������߂�
	D3DXVECTOR3 diff = pos - attacker;

	// �p�x�����߂�
	float fAngle = atan2f(diff.x, diff.z);

	// �ړI�̊p�x�̐ݒ�
	m_pCharacter3D->GetRotation()->SetDest(D3DXVECTOR3(0.0f, fAngle, 0.0f));

	// �ړ���
	D3DXVECTOR3 move;

	// �ړ��ʂ̐ݒ�
	move.x = sinf(fAngle) * blowOff;
	move.y = jump;
	move.z = cosf(fAngle) * blowOff;

	m_pMove->Set(move);
}

//===================================================
// �v���C���[�̎��E��(�p���B��)����
//===================================================
bool CPlayer::IsParry(const D3DXVECTOR3 pos)
{
	// �����̎擾
	D3DXVECTOR3 rot = m_pCharacter3D->GetRotation()->Get();

	// ���E������Ԃ��U���̎�
	if (m_pCharacter3D->GetState() == CCharacter3D::STATE_ACTION && m_pFOV->Collision(pos, rot.y, D3DX_PI * 0.5f, -D3DX_PI * 0.5f))
	{
		return true;
	}
	return false;
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


	// �����蔻��̐���
	pPlayer->m_pSphere = CCollisionSphere::Create(pos, 50.0f);

	pPlayer->Init();
	pPlayer->m_pCharacter3D->Init();
	// ����������
	pPlayer->m_pCharacter3D->SetPosition(pos);
	pPlayer->m_pCharacter3D->GetRotation()->Set(rot);

	return pPlayer;
}

//===================================================
// �R���X�g���N�^
//===================================================
CPlayerMotionController::CPlayerMotionController()
{
}

//===================================================
// �f�X�g���N�^
//===================================================
CPlayerMotionController::~CPlayerMotionController()
{

}

//===================================================
// �v���C���[�̃p�����[�^�[�̃��[�h����
//===================================================
void CPlayerMotionController::Load(std::vector<CModel*>& pModel, int* pOutNumModel)
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
				m_pMotion = CMotion::Load(FILE_NAME, pModel, pOutNumModel, TYPE_MAX, CMotion::LOAD_TEXT);
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
// �I������
//===================================================
void CPlayerMotionController::Uninit(void)
{
	// ���[�V�����̏I������
	m_pMotion->Uninit();
}

//===================================================
// �X�V����
//===================================================
void CPlayerMotionController::Update(CModel** ppModel, const int nNumModel)
{
	if (m_pMotion != nullptr)
	{
		m_pMotion->Update(ppModel, nNumModel);
	}

	// ���[�V�����̑J��
	TransitionMotion();
}

//===================================================
// ���[�V���������[�h�ł������ǂ���
//===================================================
bool CPlayerMotionController::IsLoad(void) const
{
	// ���[�h�ł������ǂ���
	if (m_pMotion->IsLoad())
	{
		return true;
	}
	return false;
}
//===================================================
// �v���C���[�̃��[�V�����̃Z�b�g
//===================================================
void CPlayerMotionController::SetMotion(const int type, bool bBlend, const int nFrameBlend)
{
	// ���[�V�����̐ݒ�
	m_pMotion->SetMotion(type, bBlend, nFrameBlend);
}

//===================================================
// �v���C���[�̃��[�V�����̑J��
//===================================================
void CPlayerMotionController::TransitionMotion(void)
{
	// ���[�V�����̎��
	TYPE motiontype = (TYPE)m_pMotion->GetBlendType();

	// ���[�V�����̑J��
	switch (motiontype)
	{
	case TYPE_NEUTRAL:
		break;
	case TYPE_MOVE:
		break;
	case TYPE_ACTION:
		break;
	case TYPE_DAMAGE:
		break;
	default:
		break;
	}
}

//===================================================
// ���[�V�����^�C�v�̎擾
//===================================================
int CPlayerMotionController::GetBlendType(void) const
{
	return m_pMotion->GetBlendType();
}
//===================================================
// �p���B�̃C�x���g����̎擾
//===================================================
bool CPlayerMotionController::IsParryEvent(const int start, const int end)
{
	// �p���B�̃t���[���̔���
	if (m_pMotion->IsEventFrame(start, end, TYPE_PARRY))
	{
		return true;
	}

	return false;
}
