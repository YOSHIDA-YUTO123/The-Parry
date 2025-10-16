//================================================
//
// 練習用の敵の状態 [TrainingEnemyState.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "TrainingEnemyState.h"
#include"TrainingEnemy.h"
#include"motion.h"
#include<memory>
#include"player.h"
#include"tutorial.h"
#include"playerstate.h"
#include "slow.h"
#include "manager.h"
#include"math.h"
#include"transform.h"
#include "sound.h"
#include "particle.h"
#include "impact.h"

using namespace math; // 名前空間mathの使用
using namespace std;  // 名前空間stdの使用

//************************************************
// 定数宣言
//************************************************
constexpr float ACTION_RANGE = 250.0f; // 攻撃してくる範囲

//================================================
// コンストラクタ
//================================================
CTrainingEnemyState::CTrainingEnemyState(ID Id)
{
	m_ID = Id;
	m_pEnemy = nullptr;
}

//================================================
// デストラクタ
//================================================
CTrainingEnemyState::~CTrainingEnemyState()
{
}

//================================================
// コンストラクタ(通常状態)
//================================================
CTrainingEnemyIdle::CTrainingEnemyIdle() : CTrainingEnemyState(ID_IDLE)
{
}

//================================================
// デストラクタ(通常状態)
//================================================
CTrainingEnemyIdle::~CTrainingEnemyIdle()
{
}

//================================================
// 初期化処理(通常状態)
//================================================
void CTrainingEnemyIdle::Init(void)
{
	// 敵の取得
	CTrainingEnemy* pEnemy = CTrainingEnemyState::GetEnemy();

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// 取得できなかったら処理しない
	if (pMotion == nullptr) return;

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(CTrainingEnemy::MOTIONTYPE_NEUTRAL, true, 10);
	}
}

//================================================
// 更新処理(通常状態)
//================================================
void CTrainingEnemyIdle::Update(void)
{
	// 敵の取得
	CTrainingEnemy* pEnemy = CTrainingEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// 範囲内にいたら
	if (pEnemy->CheckDistance(ACTION_RANGE))
	{
		// 攻撃状態
		pEnemy->ChangeState(make_shared<CTrainingEnemyAction>());
	}
}

//================================================
// コンストラクタ処理(攻撃状態)
//================================================
CTrainingEnemyAction::CTrainingEnemyAction() : CTrainingEnemyState(ID_ACTION)
{
}

//================================================
// デストラクタ処理(攻撃状態)
//================================================
CTrainingEnemyAction::~CTrainingEnemyAction()
{
}

//================================================
// 初期化処理(攻撃状態)
//================================================
void CTrainingEnemyAction::Init(void)
{
	// 敵の取得
	CTrainingEnemy* pEnemy = CTrainingEnemyState::GetEnemy();

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// 取得できなかったら処理しない
	if (pMotion == nullptr) return;

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(CTrainingEnemy::MOTIONTYPE_ACTION, true, 10);
	}
}

//================================================
// 更新処理(攻撃状態)
//================================================
void CTrainingEnemyAction::Update(void)
{
	// 敵の取得
	CTrainingEnemy* pEnemy = CTrainingEnemyState::GetEnemy();

	// プレイヤーの取得
	CPlayer* pPlayer = CTutorial::GetPlayer();

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// 取得できなかったら処理しない
	if (pMotion == nullptr) return;

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// 位置の取得
	D3DXVECTOR3 pos = pEnemy->GetPosition();
	D3DXVECTOR3 playerPos = pPlayer->GetPosition();

	if (pMotion->IsEventFrame(1, 50, CTrainingEnemy::MOTIONTYPE_ACTION))
	{
		pEnemy->DeleteOrbit();

		// プレイヤーまでの角度を求める
		float fAngle = GetTargetAngle(pos, playerPos);

		// 向きを設定
		pEnemy->SetRotDest(fAngle);
	}

	// 範囲外にいたらかつモーションが終わったら
	if (!pEnemy->CheckDistance(ACTION_RANGE) && pMotion->IsEndLoopMotion())
	{
		// 攻撃状態
		pEnemy->ChangeState(make_shared<CTrainingEnemyIdle>());
	}

	// 音の取得
	CSound* pSound = CManager::GetSound();

	// 敵の攻撃のカウンターの目安の表示
	if (pMotion->IsEventFrame(40, 40, CTrainingEnemy::MOTIONTYPE_ACTION))
	{
		// モデルの位置の取得
		D3DXVECTOR3 chestPos = pEnemy->GetModelPos(CTrainingEnemy::MODEL_CHEST);

		// パーティクルの生成
		auto pParticle = CParticle3DNormal::Create(chestPos, 100.0f, D3DXCOLOR(1.0f, 0.3f, 0.3f, 1.0f));

		// パーティクルの設定
		pParticle->SetParticle(15.0f, 240, 50, 1, 314);
	}

	if (pMotion->IsEventFrame(65, 65, CTrainingEnemy::MOTIONTYPE_ACTION))
	{
		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_SWING);
		}
	}
	// イベントフレームの判定
	if (pMotion->IsEventFrame(65, 88, CTrainingEnemy::MOTIONTYPE_ACTION))
	{
		// 軌跡の設定
		pEnemy->Orbit(16, D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f));

		// 攻撃の結果の取得
		auto result = pEnemy->GetAttackResult();

		// 絶対反撃
		if (result == CTrainingEnemy::RESULT_SPREVENGE)
		{
			// 角度を求める
			float fAngle = GetTargetAngle(playerPos, pos);

			// 角度を設定
			pPlayer->SetAngle(fAngle);

			// ヒット状態
			auto pHitState = make_shared<CTrainingEnemyHit>();

			// 連続ダメージ状態にする
			pHitState->SetCombDamage();

			// ヒット状態にする
			pEnemy->ChangeState(pHitState);

			// 状態の変更
			pPlayer->ChangeState(make_shared<CPlayerRevengeAttack>());
		}
		// パリィされた
		else if (result == CTrainingEnemy::RESULT_PARRY)
		{
			// 構えの設定処理
			pPlayer->SetStance(pos);

			// ヒットストップ
			pEnemy->SetHitStop(25);

			// ヒットストップ
			pPlayer->SetHitStop(25);

			// ヒット状態にする
			pEnemy->ChangeState(make_shared<CTrainingEnemyHit>());
		}
		// 回避だったら
		else if (result == CTrainingEnemy::RESULT_AVOID)
		{
		}
		// 攻撃があたった
		else if (result == CTrainingEnemy::RESULT_HIT)
		{
			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_SPIKE);
			}

			// ダメージモーション
			pPlayer->SetDamageMotion(pos, 0);
		}
	}
}

//================================================
// コンストラクタ(攻撃ヒット状態)
//================================================
CTrainingEnemyHit::CTrainingEnemyHit() : CTrainingEnemyState(ID_HIT)
{
}

//================================================
// デストラクタ(攻撃ヒット状態)
//================================================
CTrainingEnemyHit::~CTrainingEnemyHit()
{
}

//================================================
// 初期化処理(攻撃ヒット状態)
//================================================
void CTrainingEnemyHit::Init(void)
{
	// 敵の取得
	CTrainingEnemy* pEnemy = CTrainingEnemyState::GetEnemy();

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// 取得できなかったら処理しない
	if (pMotion == nullptr) return;

	// モーションの再生
	pMotion->SetMotion(CTrainingEnemy::MOTIONTYPE_HIT, true, 10);	
}

//================================================
// 更新処理(攻撃ヒット状態)
//================================================
void CTrainingEnemyHit::Update(void)
{
	// 敵の取得
	CTrainingEnemy* pEnemy = CTrainingEnemyState::GetEnemy();

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// プレイヤーの取得
	CPlayer* pPlayer = CTutorial::GetPlayer();

	// プレイヤーモーションクラスの取得
	CMotion* pPlayerMotion = pPlayer->GetMotion();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// 取得できなかったら処理しない
	if (pMotion == nullptr) return;

	// プレイヤーを取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// 連続ダメージ状態だったら
	if (m_type == TYPE_COMB_DAMAGE)
	{
		// プレイヤーのモーションが必殺攻撃の120フレーム目か判定
		if (pPlayerMotion != nullptr && pPlayerMotion->IsEventFrame(120, 120, CPlayer::MOTIONTYPE_REVENGEATTACK))
		{
			// 状態の変更
			pEnemy->ChangeState(make_shared<CTrainingEnemyCombDamage>());
		}
	}

	// モーションが終わったら
	if (pMotion->IsFinishEndBlend())
	{
		// 状態の変更
		pEnemy->ChangeState(make_shared<CTrainingEnemyIdle>());
	}
}

//================================================
// コンストラクタ(ダメージ状態)
//================================================
CTrainingEnemyDamage::CTrainingEnemyDamage() : CTrainingEnemyState(ID_DAMAGE)
{
	m_type = TYPE_DAMAGES;
}

//================================================
// デストラクタ(ダメージ状態)
//================================================
CTrainingEnemyDamage::~CTrainingEnemyDamage()
{
}

//================================================
// 初期化処理(ダメージ状態)
//================================================
void CTrainingEnemyDamage::Init(void)
{
	// 敵の取得
	CTrainingEnemy* pEnemy = CTrainingEnemyState::GetEnemy();

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// 取得できなかったら処理しない
	if (pMotion == nullptr) return;

	// 状態の遷移
	switch (m_type)
	{
	case TYPE_DAMAGELS:
		// モーションの再生
		pMotion->SetMotion(CTrainingEnemy::MOTIONTYPE_DAMAGELS, true, 10);
		break;
	case TYPE_DAMAGES:
		// モーションの再生
		pMotion->SetMotion(CTrainingEnemy::MOTIONTYPE_DAMAGES, true, 10);
		break;
	case TYPE_DAMAGE:
		// モーションの再生
		pMotion->SetMotion(CTrainingEnemy::MOTIONTYPE_DAMAGE, true, 10);
		break;
	default:
		break;
	}

	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pSound != nullptr)
	{
		// 音の再生
		pSound->Play(CSound::SOUND_LABEL_PERFECT);
	}
}

//================================================
// 更新処理(ダメージ状態)
//================================================
void CTrainingEnemyDamage::Update(void)
{
	// 敵の取得
	CTrainingEnemy* pEnemy = CTrainingEnemyState::GetEnemy();

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// 取得できなかったら処理しない
	if (pMotion == nullptr) return;

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションが終わったら
		if (pMotion->IsFinishEndBlend())
		{
			// 状態の変更
			pEnemy->ChangeState(make_shared<CTrainingEnemyIdle>());
		}
	}
}

//================================================
// コンストラクタ(連続ダメージ)
//================================================
CTrainingEnemyCombDamage::CTrainingEnemyCombDamage() : CTrainingEnemyState(ID_COMB_DAMAGE)
{
}

//================================================
// デストラクタ(連続ダメージ)
//================================================
CTrainingEnemyCombDamage::~CTrainingEnemyCombDamage()
{
}

//================================================
// 初期化処理(連続ダメージ)
//================================================
void CTrainingEnemyCombDamage::Init(void)
{
	// 敵の取得
	CTrainingEnemy* pEnemy = CTrainingEnemyState::GetEnemy();

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// 取得できなかったら処理しない
	if (pMotion == nullptr) return;

	// モーションの再生
	pMotion->SetMotion(CTrainingEnemy::MOTIONTYPE_COMB_DAMAGE, true, 10);
}

//================================================
// 更新処理(連続ダメージ)
//================================================
void CTrainingEnemyCombDamage::Update(void)
{
	// 敵の取得
	CTrainingEnemy* pEnemy = CTrainingEnemyState::GetEnemy();

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// プレイヤーの取得
	CPlayer* pPlayer = CTutorial::GetPlayer();

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// 取得できなかったら処理しない
	if (pMotion == nullptr) return;

	// 音の取得
	CSound* pSound = CManager::GetSound();

	// エフェクトの生成
	D3DXVECTOR3 HandR = pPlayer->GetModelPos(CTrainingEnemy::MODEL_HANDR);

	// プレイヤーの位置
	D3DXVECTOR3 playerPos = pPlayer->GetPosition();

	// プレイヤー向きの取得
	D3DXVECTOR3 angle = pPlayer->GetRotation();

	// インパクトの位置
	D3DXVECTOR3 ImpactPos =
	{
		playerPos.x + sinf(angle.y + D3DX_PI) * 100.0f,
		HandR.y,
		playerPos.z + cosf(angle.y + D3DX_PI) * 100.0f
	};

	// ランダムな位置の設定
	float fRandPosX = static_cast<float>(rand() % 50 - 25);
	float fRandPosY = static_cast<float>(rand() % 50 - 25);
	float fRandPosZ = static_cast<float>(rand() % 50 - 25);

	// 新しい位置の設定
	D3DXVECTOR3 NewImpactPos;
	NewImpactPos.x = ImpactPos.x + fRandPosX;
	NewImpactPos.y = ImpactPos.y + fRandPosY;
	NewImpactPos.z = ImpactPos.z + fRandPosZ;

	if (pMotion->IsEventFrame(CTrainingEnemy::MOTIONTYPE_COMB_DAMAGE))
	{
		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_PERFECT);
		}

		// エフェクトの生成
		auto pEffect = CParticle3DNormal::Create(ImpactPos, 20.0f, D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f));
		pEffect->SetParticle(35.0f, 360, 30, 1, 314);
		pEffect->SetParam(CEffect3D::TYPE_NORAML, 0,0.1f);

		// メッシュサークルの生成
		auto pMeshCircle = CMeshCircle::Create(D3DXCOLOR(1.0f, 1.0f, 0.4f, 0.5f), NewImpactPos, 10.0f, 50.0f);
		pMeshCircle->SetCircle(-50.0f, 8.0f, 30, false, D3DXVECTOR3(D3DX_PI * 0.5f, angle.y, 0.0f));
	}
	if (pMotion->IsEventFrame(159, 159, CTrainingEnemy::MOTIONTYPE_COMB_DAMAGE))
	{
		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_PERFECT);
		}

		// 足の位置
		D3DXVECTOR3 FootPosR = pPlayer->GetModelPos(CPlayer::MODEL_FOOTR);
		D3DXVECTOR3 FootPosL = pPlayer->GetModelPos(CPlayer::MODEL_FOOTL);

		// 足の間の位置を求める
		D3DXVECTOR3 FootPos = FootPosR + (FootPosR - FootPosL) * 0.5f;

		// メッシュサークルの生成
		auto pMeshCircle = CMeshCircle::Create(D3DXCOLOR(1.0f, 1.0f, 0.4f, 0.9f), FootPos, 10.0f, 50.0f);
		pMeshCircle->SetCircle(-50.0f, 8.0f, 30, false, D3DXVECTOR3(D3DX_PI * 0.5f, angle.y, 0.0f));

		// エフェクトの生成
		auto pEffect = CParticle3DNormal::Create(FootPos, 20.0f, D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f));
		pEffect->SetParticle(35.0f, 360, 50, 1, 314);
		pEffect->SetParam(CEffect3D::TYPE_NORAML, 0,0.1f);

		// ダメージ状態の生成
		auto pDamageState = make_shared<CTrainingEnemyDamage>();

		// 大ダメージ
		pDamageState->SetType(CTrainingEnemyDamage::TYPE_DAMAGE);

		// 状態の変更
		pEnemy->ChangeState(pDamageState);
	}
}
