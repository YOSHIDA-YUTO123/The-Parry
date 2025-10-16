//===================================================
//
// プレイヤーの状態 [playerstate.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "playerstate.h"
#include "player.h"
#include "motion.h"
#include "debugproc.h"
#include "impact.h"
#include "game.h"
#include "meshfield.h"
#include "MoveSmoke.h"
#include "GameCamera.h"
#include "manager.h"
#include "tutorial.h"
#include "sound.h"
#include "slow.h"
#include "ParryEffect.h"
#include "ParticleSpark.h"
#include "enemy.h"
#include "math.h"
#include "transform.h"
#include "PlayerMovement.h"

using MOTION = CPlayer::MOTIONTYPE; // プレイヤーの列挙型の使用
using namespace std;				// 名前空間stdの使用
using namespace Const;				// 名前空間Constの使用
using namespace math;				// 名前空間mathの使用

//===================================================
// コンストラクタ
//===================================================
CPlayerState::CPlayerState()
{
	m_ID = ID_BASE;
	m_pPlayer = nullptr;
}

//===================================================
// コンストラクタ
//===================================================
CPlayerState::CPlayerState(ID Id)
{
	m_ID = Id;
	m_pPlayer = nullptr;
}

//===================================================
// デストラクタ
//===================================================
CPlayerState::~CPlayerState()
{

}

//===================================================
// オーナーの設定
//===================================================
void CPlayerState::SetOwner(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
}

//===================================================
// コンストラクタ(Normla)
//===================================================
CPlayerNormal::CPlayerNormal() : CPlayerState(ID_NORMAL)
{
}

//===================================================
// デストラクタ(Normla)
//===================================================
CPlayerNormal::~CPlayerNormal()
{
}

//===================================================
// 更新処理(Normla)
//===================================================
void CPlayerNormal::Update(void)
{
}

//===================================================
// コンストラクタ(Move)
//===================================================
CPlayerMove::CPlayerMove() : CPlayerState(ID_MOVE)
{
}

//===================================================
// デストラクタ(Move)
//===================================================
CPlayerMove::~CPlayerMove()
{
}

//===================================================
// 更新処理(Move)
//===================================================
void CPlayerMove::Update(void)
{
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	// 音の取得
	CSound* pSound = CManager::GetSound();

	// 取得出来たら
	if (pMotion != nullptr)
	{
		// フレームの判定
		if(pMotion->IsEventFrame(CPlayer::MOTIONTYPE_MOVE,0))
		{
			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_WARK000);
			}
		}
		else if (pMotion->IsEventFrame(CPlayer::MOTIONTYPE_MOVE, 1))
		{
			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_WARK001);
			}
		}
	}
}

//===================================================
// コンストラクタ(Damage)
//===================================================
CPlayerDamage::CPlayerDamage() : CPlayerState(ID_DAMAGE)
{
	m_type = TYPE_NORMAL;
	m_nDamage = NULL;
}

//===================================================
// コンストラクタ(Damage)
//===================================================
CPlayerDamage::CPlayerDamage(int nDamage) : CPlayerState(ID_DAMAGE)
{
	m_type = TYPE_NORMAL;
	m_nDamage = nDamage;
}

//===================================================
// デストラクタ(Damage)
//===================================================
CPlayerDamage::~CPlayerDamage()
{
}

//===================================================
// 初期化処理(Damage)
//===================================================
void CPlayerDamage::Init(void)
{
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	// モードの取得
	CScene::MODE mode = CManager::GetMode();

	// カメラの取得
	CGameCamera* pCamera = nullptr;

	if (mode == CScene::MODE_GAME)
	{
		// カメラの取得
		pCamera = CGame::GetCamera();
	}
	else if (mode == CScene::MODE_TUTORIAL)
	{
		// カメラの取得
		pCamera = CTutorial::GetCamera();
	}

	// カメラの揺れ
	pCamera->SetShake(20, 40);

	pPlayer->Hit(m_nDamage);

	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(MOTION::MOTIONTYPE_DAMAGE, true, 2);
	}

	// 音の取得
	CSound* pSound = CManager::GetSound();

	// 種類がSPIKEだったら
	if (pSound != nullptr)
	{
		switch (m_type)
		{
		case TYPE_NORMAL:
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_NORMAL);
			break;
		case TYPE_SPIKE:
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_SPIKE);
			break;
		default:
			break;
		}
	}
}

//===================================================
// 更新処理(Damage)
//==================================================
void CPlayerDamage::Update(void)
{
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	if (pMotion->GetType() == MOTION::MOTIONTYPE_DAMAGE)
	{
		// モーションが終わったら
		if (pMotion != nullptr && pMotion->FinishMotion())
		{
			pPlayer->ChangeState(make_shared<CPlayerDownNeutral>());
		}
	}
}

//===================================================
// コンストラクタ(DownNeutral)
//===================================================
CPlayerDownNeutral::CPlayerDownNeutral() : CPlayerState(ID_DOWNNEUTRAL)
{
}

//===================================================
// デストラクタ(DownNeutral)
//===================================================
CPlayerDownNeutral::~CPlayerDownNeutral()
{
}

//===================================================
// 初期化処理(DownNeutral)
//===================================================
void CPlayerDownNeutral::Init(void)
{
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(MOTION::MOTIONTYPE_DOWN_NEUTRAL, false, 5);
	}
}

//===================================================
// 更新処理(DownNeutral)
//===================================================
void CPlayerDownNeutral::Update(void)
{

}

//===================================================
// コンストラクタ(回避)
//===================================================
CPlayerAvoid::CPlayerAvoid() : CPlayerState(ID_AVOID)
{
	m_fSpeed = NULL;
}

//===================================================
// コンストラクタ(回避)
//===================================================
CPlayerAvoid::CPlayerAvoid(const float fSpeed) : CPlayerState(ID_AVOID)
{
	m_fSpeed = fSpeed;
}

//===================================================
// デストラクタ(回避)
//===================================================
CPlayerAvoid::~CPlayerAvoid()
{
}

//===================================================
// 初期化処理(回避)
//===================================================
void CPlayerAvoid::Init(void)
{
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	// 移動制御の取得
	auto pMoveMent = pPlayer->GetMovement();

	if (pMoveMent != nullptr)
	{
		// 移動量の設定
		pMoveMent->Set(D3DXVECTOR3(0.0f, 10.0f, 0.0f));
	}

	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pSound != nullptr)
	{
		// 音の再生
		pSound->Play(CSound::SOUND_LABEL_AVOID);
	}
	if (pMotion != nullptr)
	{
		//// モーションの種類の取得
		//int nMotionType = pMotion->GetBlendType();

		//// 回避状態じゃないなら
		//if (nMotionType != CPlayer::MOTIONTYPE_AVOID)
		//{
		//	// 移動量の設定
		//	pMoveMent->Set(D3DXVECTOR3(0.0f, 10.0f, 0.0f));
		//}

		// モーションの再生
		pMotion->SetMotion(MOTION::MOTIONTYPE_AVOID, false, 5);
	}
}

//===================================================
// 更新処理(回避)
//===================================================
void CPlayerAvoid::Update(void)
{
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	// 移動制御の取得
	auto pMoveMent = pPlayer->GetMovement();

	// 向きの取得
	float fAnlge = pPlayer->GetRotation().y;

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// 移動クラスの取得
		if (pMoveMent != nullptr && pMotion->IsEventFrame(1,24,CPlayer::MOTIONTYPE_AVOID))
		{
			// 向いている方向に進む処理
			pMoveMent->MoveForward(m_fSpeed, fAnlge + D3DX_PI);
		}

		// モーションが終わったら
		if (pMotion->FinishMotion())
		{
			pPlayer->ChangeState(make_shared<CPlayerNormal>());

			pMotion->SetMotion(pPlayer->MOTIONTYPE_NEUTRAL, true, 5);

			return;
		}
	}
}

//===================================================
// コンストラクタ(回し蹴り)
//===================================================
CPlayerRoundKick::CPlayerRoundKick() : CPlayerState(ID_ROUNDKICK)
{

}

//===================================================
// デストラクタ(回し蹴り)
//===================================================
CPlayerRoundKick::~CPlayerRoundKick()
{
}

//===================================================
// 初期化処理(回し蹴り)
//===================================================
void CPlayerRoundKick::Init(void)
{
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(MOTION::MOTIONTYPE_ROUNDKICK,true, 2);
	}
}

//===================================================
// 更新処理(回し蹴り)
//===================================================
void CPlayerRoundKick::Update(void)
{
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	// 向きの取得
	float fAnlge = pPlayer->GetRotation().y;

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// 位置の取得
		D3DXVECTOR3 pos = pPlayer->GetPosition();

		if (pMotion->IsEventFrame(1, 10, pPlayer->MOTIONTYPE_ROUNDKICK))
		{
			// 向いている方向に進む
			pPlayer->GetMovement()->MoveForward(10.0f, fAnlge + D3DX_PI);
		}

		if (pMotion->IsEventFrame(15, 15, pPlayer->MOTIONTYPE_ROUNDKICK))
		{
			// モードの取得
			CScene::MODE mode = CManager::GetMode();

			// ジャンプ量の設定
			float fJumpHeight = (mode == CScene::MODE_GAME) ? 18.0f : 14.0f;

			// 移動量の設定
			pPlayer->GetMovement()->Set(D3DXVECTOR3(0.0f, fJumpHeight, 0.0f));

			// サークルの生成
			auto pCircle = CMeshCircle::Create(D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f), pos, 0.0f, 20.0f);
			pCircle->SetCircle(0.0f, 10.0f, 60, true);
		}

		if (pMotion->IsEventFrame(15, 17, pPlayer->MOTIONTYPE_ROUNDKICK))
		{
			// エフェクトの生成
			auto pEffect = CMoveSmoke::Create(D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z), 100.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

			// エフェクトの設定処理
			pEffect->SetEffect(35, VEC3_NULL);
		}

		// モーションが終わったら
		if (pMotion->FinishMotion())
		{
			pPlayer->ChangeState(make_shared<CPlayerNormal>());
		}
	}
}

//===================================================
// コンストラクタ(ダッシュ)
//===================================================
CPlayerDash::CPlayerDash() : CPlayerState(ID_DASH)
{
}

//===================================================
// デストラクタ(ダッシュ)
//===================================================
CPlayerDash::~CPlayerDash()
{
}

//===================================================
// 初期化処理(ダッシュ)
//===================================================
void CPlayerDash::Init(void)
{
}

//===================================================
// 更新処理(ダッシュ)
//===================================================
void CPlayerDash::Update(void)
{
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	// 向きの取得
	D3DXVECTOR3 rot = pPlayer->GetRotation();

	// プレイヤーの後ろ方向を設定
	float fMoveX = sinf(rot.y) * 2.0f;
	float fMoveZ = cosf(rot.y) * 2.0f;

	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pMotion != nullptr)
	{
		// 10フレーム目になったら
		if (pMotion->IsEventFrame(10, 10, CPlayer::MOTIONTYPE_DASH))
		{
			// 位置の取得
			D3DXVECTOR3 pos = pPlayer->GetModelPos(CPlayer::MODEL_FOOTR);

			// エフェクトの生成
			auto pEffect = CMoveSmoke::Create(D3DXVECTOR3(pos.x,pos.y,pos.z), 100.0f, WHITE);

			// エフェクトの設定処理
			pEffect->SetEffect(60, D3DXVECTOR3(fMoveX, 0.0f, fMoveZ));

			//if (pSound != nullptr)
			//{
			//	// 音の再生
			//	pSound->Play(CSound::SOUND_LABEL_WARK000);
			//}
		}
		// 30フレーム目になったら
		else if (pMotion->IsEventFrame(30, 30, CPlayer::MOTIONTYPE_DASH))
		{
			// 位置の取得
			D3DXVECTOR3 pos = pPlayer->GetModelPos(CPlayer::MODEL_FOOTL);

			// エフェクトの生成
			auto pEffect = CMoveSmoke::Create(D3DXVECTOR3(pos.x, pos.y, pos.z), 100.0f, WHITE);

			// エフェクトの設定処理
			pEffect->SetEffect(60, D3DXVECTOR3(fMoveX, 0.0f, fMoveZ));

		}

		if (pMotion->IsEventFrame(1, 1, CPlayer::MOTIONTYPE_DASH))
		{
			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_WARK000);
			}
		}
		if (pMotion->IsEventFrame(20, 20, CPlayer::MOTIONTYPE_DASH))
		{
			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_WARK001);
			}
		}
	}
}

//===================================================
// コンストラクタ(ジャンプ)
//===================================================
CPlayerJump::CPlayerJump() : CPlayerState(ID_JUMP)
{

}

//===================================================
// デストラクタ(ジャンプ)
//===================================================
CPlayerJump::~CPlayerJump()
{
}

//===================================================
// 初期化処理(ジャンプ)
//===================================================
void CPlayerJump::Init(void)
{
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(MOTION::MOTIONTYPE_JUMP, true, 2);
	}
}

//===================================================
// 更新処理(ジャンプ)
//===================================================
void CPlayerJump::Update(void)
{

}

//===================================================
// コンストラクタ(着地)
//===================================================
CPlayerLanding::CPlayerLanding() : CPlayerState(ID_LANDING)
{

}

//===================================================
// デストラクタ(着地)
//===================================================
CPlayerLanding::~CPlayerLanding()
{
}

//===================================================
// 初期化処理(着地)
//===================================================
void CPlayerLanding::Init(void)
{
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	if (pMotion != nullptr)
	{
		// 着地モーションの再生
		pMotion->SetMotion(MOTION::MOTIONTYPE_LANDING, true, 5);
	}

	// 位置の取得
	D3DXVECTOR3 pos = pPlayer->GetPosition();

	// サークルを生成
	auto pCircle = CMeshCircle::Create(D3DCOLOR_RGBA(220, 220, 220, 200), D3DXVECTOR3(pos.x, pos.y + 3.0f, pos.z), 0.0f, 50.0f, 32);

	// サークルの設定
	pCircle->SetCircle(0.0f, 10.0f, 30, true);
}

//===================================================
// 更新処理(着地)
//===================================================
void CPlayerLanding::Update(void)
{
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	if (pMotion != nullptr)
	{
		// モーションが終わったら
		if (pMotion->IsFinishEndBlend())
		{
			pPlayer->ChangeState(make_shared<CPlayerNormal>());
		}
	}
}

//===================================================
// コンストラクタ(絶対反撃)
//===================================================
CPlayerRevenge::CPlayerRevenge() : CPlayerState(ID_REVENGE)
{

}

//===================================================
// デストラクタ(絶対反撃)
//===================================================
CPlayerRevenge::~CPlayerRevenge()
{
}

//===================================================
// 初期化処理
//===================================================
void CPlayerRevenge::Init(void)
{
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	if (pMotion != nullptr)
	{
		// 着地モーションの再生
		pMotion->SetMotion(MOTION::MOTIONTYPE_REVENGE, true, 5);
	}
}

//===================================================
// 更新処理
//===================================================
void CPlayerRevenge::Update(void)
{
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	if (pMotion != nullptr)
	{
		if (pMotion->IsEventFrame(34, 34, MOTION::MOTIONTYPE_REVENGE))
		{
			// 演出処理
			pPlayer->SetRevengeEffect();
		}

		// モーションが終わったら
		if (pMotion->IsFinishEndBlend())
		{
			pPlayer->ChangeState(make_shared<CPlayerNormal>());
		}
	}
}

//===================================================
// コンストラクタ(絶対反撃中)
//===================================================
CPlayerRevengeAttack::CPlayerRevengeAttack() : CPlayerState(ID_REVENGEATTACK)
{
}

//===================================================
// デストラクタ(絶対反撃中)
//===================================================
CPlayerRevengeAttack::~CPlayerRevengeAttack()
{
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	pPlayer->EnableGravity(true);
}

//===================================================
// 初期化処理(絶対反撃中)
//===================================================
void CPlayerRevengeAttack::Init(void)
{
	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pSound != nullptr)
	{
		// 音の再生
		pSound->Play(CSound::SOUND_LABEL_SP);
	}

	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	// スローモーションの取得
	CSlow* pSlow = CManager::GetSlow();

	if (pSlow != nullptr)
	{
		// スローモーションの設定
		pSlow->Start(60, 8);
	}

	if (pMotion != nullptr)
	{
		// 着地モーションの再生
		pMotion->SetMotion(MOTION::MOTIONTYPE_REVENGEATTACK, false, 0);
	}

	// モーションの更新処理
	pPlayer->UpdateMotion();

	D3DXVECTOR3 playerPos = pPlayer->GetPosition();
	D3DXVECTOR3 HandPos = pPlayer->GetModelPos(CPlayer::MODEL_HANDL);

	D3DXVECTOR3 angle = pPlayer->GetRotation();

	// 右手の位置
	D3DXVECTOR3 ImpactPos =
	{
		playerPos.x + sinf(angle.y + D3DX_PI) * 50.0f,
		playerPos.y + HandPos.y,
		playerPos.z + cosf(angle.y + D3DX_PI) * 50.0f
	};

	// サークルの生成^
	auto pMeshCircle = CMeshCircle::Create(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), ImpactPos, 0.0f, 60.0f);
	pMeshCircle->SetCircle(0.0f, 8.0f, 120, true, D3DXVECTOR3(D3DX_PI * 0.5f, angle.y, 0.0f));

		// パリィエフェクトの生成
	CParryEffect::Create(ImpactPos, D3DXVECTOR3(150.0f, 150.0f, 0.0f), D3DXVECTOR3(0.0f, angle.y, 0.0f), 5, 2, 6, false, CParryEffect::TYPE_PARRY);

	// パーティクルの生成
	CParticleSpark* pSpark = CParticleSpark::Create(ImpactPos, D3DXVECTOR2(3.0f, 40.0f), D3DCOLOR_RGBA(255, 127, 80, 255));
	pSpark->SetParticle(15.0f, 60, 150, 1, -180);

	pSpark = CParticleSpark::Create(ImpactPos, D3DXVECTOR2(3.0f, 40.0f), D3DCOLOR_RGBA(106, 90, 205, 255));
	pSpark->SetParticle(15.0f, 60, 150, 1, -180);

	// パーティクルの生成
	CParticle3DNormal* pNormal = CParticle3DNormal::Create(ImpactPos, 35.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	// パーティクルの設定処理
	pNormal->SetParticle(15.0f, 120, 60, 1, 314);
	pNormal->SetParam(CEffect3D::TYPE_HIT,0);

	// 成功度の設定
	pPlayer->SetParryResult(CPlayer::PARRY_PARFECT);
}

//===================================================
// 更新処理(絶対反撃中)
//===================================================
void CPlayerRevengeAttack::Update(void)
{
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	// 位置の取得
	D3DXVECTOR3 pos = pPlayer->GetPosition();

	// パーティクルの生成
	auto pNormal = CParticle3DNormal::Create(pPlayer->GetModelPos(CPlayer::MODEL_CHEST), 10.0f, D3DXCOLOR(1.0f, 0.4f, 0.4f, 1.0f));

	// パーティクルの設定処理
	pNormal->SetParticle(5.0f, 120, 5, 1, 60);
	pNormal->SetParam(CEffect3D::TYPE_NORAML, 100);

	// パーティクルの生成
	pNormal = CParticle3DNormal::Create(pPlayer->GetModelPos(CPlayer::MODEL_CHEST), 10.0f, D3DXCOLOR(0.4f, 1.0f, 1.0f, 1.0f));

	// パーティクルの設定処理
	pNormal->SetParticle(5.0f, 120, 5, 1, 60);
	pNormal->SetParam(CEffect3D::TYPE_NORAML, 100);

	// 向きの取得
	float fAnlge = pPlayer->GetRotation().y;

	if (pMotion != nullptr)
	{
		// 現在のモードの取得
		CScene::MODE mode = CManager::GetMode();

		// 音の取得
		CSound* pSound = CManager::GetSound();

		if (pMotion->IsEventFrame(105, 105, CPlayer::MOTIONTYPE_REVENGEATTACK))
		{
			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_ENEMY_JUMP);
			}
			// ジャンプの高さ
			float fJumpHeight = (mode == CScene::MODE_TUTORIAL) ? 2.0f : 10.0f;

			// 少し前に進む
			pPlayer->GetMovement()->Set(D3DXVECTOR3(0.0f, fJumpHeight, 0.0f));
			pPlayer->GetMovement()->MoveForward(50.0f, fAnlge + D3DX_PI);

			// 重力を無効化
			pPlayer->EnableGravity(false);
		}

		// 120f目から終わりまで
		if (pMotion->IsEventFrame(120, 331, CPlayer::MOTIONTYPE_REVENGEATTACK))
		{
			// 重力を有効化
			pPlayer->EnableGravity(true);

			pPlayer->GetMovement()->Set(VEC3_NULL);
		}

		if (pMotion->IsEventFrame(350, 350, CPlayer::MOTIONTYPE_REVENGEATTACK))
		{
			pPlayer->SetRevengeEffect();
		}
		// モーションが終わったら
		if (pMotion->IsFinishEndBlend())
		{
			pPlayer->ChangeState(make_shared<CPlayerNormal>());
		}
	}
}

//===================================================
// コンストラクタ(後ろからのダメージ)
//===================================================
CPlayerDamageBack::CPlayerDamageBack() : CPlayerState(ID_DAMAGE_BACK)
{
	m_nDamage = NULL;
}

//===================================================
// コンストラクタ(後ろからのダメージ)
//===================================================
CPlayerDamageBack::CPlayerDamageBack(const int nDamage) : CPlayerState(ID_DAMAGE_BACK)
{
	m_nDamage = nDamage;
}

//===================================================
// デストラクタ(後ろからのダメージ)
//===================================================
CPlayerDamageBack::~CPlayerDamageBack()
{
}

//===================================================
// 初期化処理(後ろからのダメージ)
//===================================================
void CPlayerDamageBack::Init(void)
{
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	// モードの取得
	CScene::MODE mode = CManager::GetMode();

	// カメラの取得
	CGameCamera* pCamera = nullptr;

	if (mode == CScene::MODE_GAME)
	{
		// カメラの取得
		pCamera = CGame::GetCamera();
	}
	else if (mode == CScene::MODE_TUTORIAL)
	{
		// カメラの取得
		pCamera = CTutorial::GetCamera();
	}

	// カメラの揺れ
	pCamera->SetShake(20, 40);

	pPlayer->Hit(m_nDamage);

	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(MOTION::MOTIONTYPE_BACK_DAMAGE, true, 2);
	}

	// 音の取得
	CSound* pSound = CManager::GetSound();

	// 種類がSPIKEだったら
	if (pSound != nullptr)
	{
		// 音の再生
		pSound->Play(CSound::SOUND_LABEL_PERFECT);
	}
}
//===================================================
// 更新処理(後ろからのダメージ)
//===================================================
void CPlayerDamageBack::Update(void)
{
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();
	
	// モーションが終わったら
	if (pMotion->IsEventFrame(50,50,CPlayer::MOTIONTYPE_BACK_DAMAGE))
	{
		// 状態の変更
		pPlayer->ChangeState(make_shared<CPlayerDownNeutralBk>());
	}
}

//===================================================
// コンストラクタ(ダウン後ろ)
//===================================================
CPlayerDownNeutralBk::CPlayerDownNeutralBk() : CPlayerState(ID_DOWN_NEUTRAL_BACK)
{
}

//===================================================
// デストラクタ(ダウン後ろ)
//===================================================
CPlayerDownNeutralBk::~CPlayerDownNeutralBk()
{
}

//===================================================
// 初期化処理(ダウン後ろ)
//===================================================
void CPlayerDownNeutralBk::Init(void)
{
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	if (pMotion != nullptr)
	{
		// 着地モーションの再生
		pMotion->SetMotion(MOTION::MOTIONTYPE_DOWN_NEUTRA_BACK, true, 5);
	}
}

//===================================================
// 更新処理(ダウン後ろ)
//===================================================
void CPlayerDownNeutralBk::Update(void)
{

}

//===================================================
// コンストラクタ(ジャンプ攻撃)
//===================================================
CPlayerJumpAttack::CPlayerJumpAttack() : CPlayerState(ID_JUMP_ATTACK)
{
}

//===================================================
// デストラクタ(ジャンプ攻撃)
//===================================================
CPlayerJumpAttack::~CPlayerJumpAttack()
{
}

//===================================================
// 初期化処理(ジャンプ攻撃)
//===================================================
void CPlayerJumpAttack::Init(void)
{
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	if (pMotion != nullptr)
	{
		// 着地モーションの再生
		pMotion->SetMotion(MOTION::MOTIONTYPE_JUMP_ATTACK, true, 5);
	}
}

//===================================================
// 更新処理(ジャンプ攻撃)
//===================================================
void CPlayerJumpAttack::Update(void)
{
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	// 敵の取得
	CEnemy* pEnemy = CGame::GetEnemy();

	// 取得できなかったら処理しない
	if (pEnemy == nullptr)
	{
		// 状態の変更
		pPlayer->ChangeState(make_shared<CPlayerNormal>());
		return;
	}

	// 向きの取得
	float fAnlge = pPlayer->GetRotation().y;

	// プレイヤーの位置の取得
	D3DXVECTOR3 pos = pPlayer->GetPosition();

	if (pMotion != nullptr)
	{
		if (pMotion->IsEventFrame(CPlayer::MOTIONTYPE_JUMP_ATTACK, 0))
		{
			// ジャンプ
			pPlayer->GetMovement()->Set(D3DXVECTOR3(0.0f, JUMP_VALUE, 0.0f));
		}
		if (pMotion->IsEventFrame(CPlayer::MOTIONTYPE_JUMP_ATTACK, 1))
		{
			// 空中に少しとどまる
			pPlayer->GetMovement()->Set(VEC3_NULL);
		}
		if (pMotion->IsEventFrame(CPlayer::MOTIONTYPE_JUMP_ATTACK, 5))
		{
			// カメラの取得
			CGameCamera* pCamera = CGame::GetCamera();

			if (pCamera != nullptr)
			{
				// カメラの揺れ
				pCamera->SetShake(60, 15);
			}

			// メッシュフィールドの取得
			CMeshField* pMeshField = CGame::GetField();

			// フィールドの波の設定
			CMeshFieldWave::Config config = { pos,250.0f,380.0f,280.0f,12.0f,0.01f,120 };

			if (pMeshField != nullptr)
			{
				// 地面に波を発生させる
				pMeshField->SetWave(config);
			}

			// 瓦礫の設定
			pPlayer->SetRubble(pos);
		}
		if (pMotion->IsEventFrame(CPlayer::MOTIONTYPE_JUMP_ATTACK, 3))
		{
			// 敵の胸の位置
			D3DXVECTOR3 enemyChestPos = pEnemy->GetModelPos(CEnemy::MODEL_CHEST);

			// 敵の胸の大きさ
			D3DXVECTOR3 enemyChestSize = pEnemy->GetModelSize(CEnemy::MODEL_CHEST);

			// 敵の胸の中心を求める
			D3DXVECTOR3 enemyChestCenter = { enemyChestPos.x,enemyChestPos.y + enemyChestSize.y * 0.5f,enemyChestPos.z };

			// 角度の取得
			float fAngle = GetTargetAngle(enemyChestPos, pos);

			// 向きの設定
			pPlayer->SetAngle(fAngle + D3DX_PI);

			// 距離を求める
			float fDistance = GetDistance(enemyChestCenter - pos);

			// 移動量の設定
			pPlayer->GetMovement()->MoveForward(fDistance / JUMP_MOVE_FRAME, fAnlge + D3DX_PI);
		}
		// モーションがおわったら
		if (pMotion->IsFinishEndBlend())
		{
			// 状態の変更
			pPlayer->ChangeState(make_shared<CPlayerNormal>());
		}
	}
}
