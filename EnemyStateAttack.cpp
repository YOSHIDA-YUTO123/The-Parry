//===================================================
//
// 敵の状態攻撃 [enemyStateAttack.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "EnemyStateAttack.h"
#include "enemy.h"
#include "motion.h"
#include "game.h"
#include "player.h"
#include <memory>
#include "playerstate.h"
#include "sound.h"
#include "manager.h"
#include "math.h"
#include "particle.h"
#include "Wave.h"
#include "slow.h"
#include "dust.h"
#include <memory>
#include "EnemyStateMovement.h"
#include "EnemyMovement.h"

using namespace std;	// 名前空間stdの使用
using namespace Const;	// 名前空間Constの使用
using namespace math;	// 名前空間mathの使用

//***************************************************
// 定数宣言
//***************************************************
namespace
{
	constexpr int INIT_NEXT_ACTION = 999;			// 次の行動に移る確率
	constexpr int ABSSPIN_TIME = 30;				// 絶対回転する時間
	constexpr float JUMPATTACK_MOVE_FRAME = 20.0f;	// ジャンプ攻撃の移動フレーム
}

//===================================================
// コンストラクタ
//===================================================
CEnemyAttackSmash::CEnemyAttackSmash() : CEnemyState(ID_SMASH)
{
	m_nNextAction = INIT_NEXT_ACTION;
}

//===================================================
// デストラクタ
//===================================================
CEnemyAttackSmash::~CEnemyAttackSmash()
{
}

//===================================================
// 更新処理(攻撃)
//===================================================
void CEnemyAttackSmash::Init(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 剣の軌跡のリセット
	pEnemy->DeleteOrbit();

	// 次の行動を選出
	m_nNextAction = rand() % 100;

	if (pMotion != nullptr)
	{
		// 攻撃モーションの設定
		pMotion->SetMotion(CEnemy::MOTIONTYPE_SMASH, true, 10);
	}
}

//===================================================
// 更新処理(攻撃)
//===================================================
void CEnemyAttackSmash::Update(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// プレイヤーの取得
	CPlayer* pPlayer = CGame::GetPlayer();

	// プレイヤーが使われていないなら
	if (pPlayer == nullptr)
	{
		return;
	}

	// 位置の取得
	D3DXVECTOR3 pos = pEnemy->GetPosition();

	// 攻撃モーションのたたきつけになったら
	if (pMotion->IsEventFrame(65, 65, CEnemy::MOTIONTYPE_SMASH))
	{
		// 瓦礫の設定処理
		pEnemy->SetRubble();
	}

	// イベントフレームの判定
	if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_SMASH) && pEnemy->IsDamageMotion() == false)
	{
		// 状態の設定
		pEnemy->SetState(CCharacter3D::STATE_ACTION, 5);

		// 攻撃の結果を取得
		CEnemy::RESULT result = pEnemy->WeponAttackResult(pPlayer);

		// パリィされた
		if (result == CEnemy::RESULT_PARRY)
		{
			// 構えの設定
			pPlayer->SetStance(pos);

			// ヒットストップ
			pEnemy->SetHitStop(20);

			// ヒットストップ
			pPlayer->SetHitStop(20);

			// ヒット状態にする
			pEnemy->ChangeState(make_shared<CEnemyHit>());
		}
		//// 回避だったら
		//else if (pEnemy->CollisionWepon() && pPlayerMotion->GetBlendType() == pPlayer->TYPE_AVOID)
		//{
		//	CSlow *pSlow = CManager::GetSlow();

		//	pSlow->Start(60,4);
		//}
		// 攻撃があたった
		else if (result == CEnemy::RESULT_HIT)
		{
			// プレイヤーのダメージモーションの設定
			pPlayer->SetDamageMotion(pos, 8);
		}
		// 絶対反撃
		else if (result == CEnemy::RESULT_SPREVENGE)
		{
			// プレイヤーの位置の取得
			D3DXVECTOR3 playerPos = pPlayer->GetPosition();

			// 角度を求める
			float fAngle = GetTargetAngle(playerPos, pos);

			// 角度を設定
			pPlayer->SetAngle(fAngle);

			// ヒット状態にする
			pEnemy->ChangeState(make_shared<CEnemySuperHit>());

			// 状態の変更
			pPlayer->ChangeState(make_shared<CPlayerRevengeAttack>());
		}
	}

	// 状態マネージャーの取得
	auto pStateManager = pEnemy->GetStateManager();

	// 敵の攻撃のカウンターの目安の表示
	if (pMotion->IsEventFrame(50, 50, CEnemy::MOTIONTYPE_SMASH))
	{
		// 音の取得
		CSound* pSound = CManager::GetSound();

		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_ENEMY_ATTACK);
		}

		// モデルの位置の取得
		D3DXVECTOR3 chestPos = pEnemy->GetModelPos(CEnemy::MODEL_CHEST);

		// パーティクルの生成
		auto pParticle = CParticle3DNormal::Create(chestPos, 100.0f, D3DXCOLOR(1.0f, 0.3f, 0.3f, 1.0f));

		// パーティクルの設定
		pParticle->SetParticle(15.0f, 240, 50, 1, 314);
	}

	if (pMotion->IsEventFrame(0, 30, CEnemy::MOTIONTYPE_SMASH))
	{
		// プレイヤーの方向を見る処理
		pEnemy->AngleToPlayer();
	}

	if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_SMASH))
	{
		// 軌跡の処理
		pEnemy->Orbit(16, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f));
	}

	// モーションが終わったら
	if (pMotion->FinishMotion())
	{
		// プレイヤーの立ち位置でモーションを設定
		if (pStateManager != nullptr && pStateManager->SetMotionByPlayerPosition())
		{
			return;
		}
		// 20%の確率
		if (m_nNextAction <= 20)
		{
			// バックステップする
			pEnemy->ChangeState(make_shared<CEnemyBackStep>());
		}
	}
	// 攻撃モーションが終わったら
	else if (pMotion->IsFinishEndBlend())
	{
		// IDLEにする
		pEnemy->ChangeState(make_shared<CEnemyIdle>(10));

		return;
	}

	if (pMotion->IsEventFrame(60, 60, CEnemy::MOTIONTYPE_SMASH))
	{
		// 音の取得
		CSound* pSound = CManager::GetSound();

		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_SWING);
		}
	}
}

//===================================================
// コンストラクタ(衝撃波攻撃)
//===================================================
CEnemyAttackImpact::CEnemyAttackImpact() : CEnemyState(ID_IMPACT)
{

}

//===================================================
// デストラクタ(衝撃波攻撃)
//===================================================
CEnemyAttackImpact::~CEnemyAttackImpact()
{
}

//===================================================
// 初期化処理(衝撃波攻撃)
//===================================================
void CEnemyAttackImpact::Init(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 剣の軌跡のリセット
	pEnemy->DeleteOrbit();

	if (pMotion != nullptr)
	{
		// 攻撃モーションの設定
		pMotion->SetMotion(CEnemy::MOTIONTYPE_IMPACT, true, 10);
	}
}

//===================================================
// 更新処理(衝撃波攻撃)
//===================================================
void CEnemyAttackImpact::Update(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// プレイヤーの取得
	CPlayer* pPlayer = CGame::GetPlayer();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 攻撃モーションの設定
	pMotion->SetMotion(CEnemy::MOTIONTYPE_IMPACT, true, 10);

	// 状態を攻撃にする
	pEnemy->SetState(CCharacter3D::STATE_ACTION, 5);

	if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_IMPACT, 0))
	{
		// プレイヤーの方向を見る処理
		pEnemy->AngleToPlayer();
	}

	if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_IMPACT, 1))
	{
		// 軌跡の処理
		pEnemy->Orbit(16, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f));
	}

	// 位置の取得
	D3DXVECTOR3 pos = pEnemy->GetPosition();

	if (pMotion->IsEventFrame(50, 50, CEnemy::MOTIONTYPE_IMPACT))
	{
		// ウェーブの生成
		auto pWave = CMeshWave::Create(pos, 50.0f, 50.0f, WHITE);

		// ウェーブの設定処理
		pWave->SetWave(30, 50.0f);
	}

	// モーションが終わったら
	if (pMotion->FinishMotion())
	{
		// 状態マネージャーの取得
		auto pStateManager = pEnemy->GetStateManager();

		// プレイヤーの立ち位置でモーションを設定
		if (pStateManager != nullptr && pStateManager->SetMotionByPlayerPosition())
		{
			return;
		}
	}
	// 攻撃モーションが終わったら
	else if (pMotion->IsFinishEndBlend())
	{
		// IDLEにする
		pEnemy->ChangeState(make_shared<CEnemyIdle>(10));

		return;
	}
}

//===================================================
// コンストラクタ(回転攻撃)
//===================================================
CEnemySpin::CEnemySpin() : CEnemyState(ID_SPIN)
{
	m_ProbAttack = NULL;
	m_nTime = NULL;
	m_nMaxTime = NULL;
}

//===================================================
// コンストラクタ(回転攻撃)
//===================================================
CEnemySpin::CEnemySpin(const int nTime) : CEnemyState(ID_SPIN)
{
	m_ProbAttack = NULL;
	m_nTime = nTime;
	m_nMaxTime = nTime;
}

//===================================================
// デストラクタ(回転攻撃)
//===================================================
CEnemySpin::~CEnemySpin()
{
}

//===================================================
// 初期化処理(回転攻撃)
//===================================================
void CEnemySpin::Init(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 剣の軌跡のリセット
	pEnemy->DeleteOrbit();

	m_ProbAttack = rand() % 100;

	if (pMotion != nullptr)
	{
		pMotion->SetMotion(CEnemy::MOTIONTYPE_SPIN, true, 10);
	}
}

//===================================================
// 更新処理(回転攻撃)
//===================================================
void CEnemySpin::Update(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();
	CPlayer* pPlayer = CGame::GetPlayer();
	CMotion* pPlayerMotion = pPlayer->GetMotion();

	// 向きの取得
	float fAngle = pEnemy->GetRotation().y;

	// 向いている方向に移動する
	pEnemy->GetMovement()->MoveForWard(15.0f, fAngle + D3DX_PI);

	if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_SPIN))
	{
		// 軌跡の処理
		pEnemy->Orbit(16, D3DXCOLOR(1.0f, 1.0f, 0.5f, 0.5f));
	}

	m_nTime--;

	// 位置の取得
	D3DXVECTOR3 pos = pEnemy->GetPosition();

	if (pMotion->IsEventFrame(5, 5, CEnemy::MOTIONTYPE_SPIN))
	{
		// 音の取得
		CSound* pSound = CManager::GetSound();

		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_SWING);
		}
	}
	// 回転モーション
	if (pMotion->IsEventFrame(0, 999, CEnemy::MOTIONTYPE_SPIN))
	{
		// 状態の設定
		pEnemy->SetState(CCharacter3D::STATE_ACTION, 5);

		// 攻撃の結果を取得
		CEnemy::RESULT result = pEnemy->WeponAttackResult(pPlayer);

		// パリィされた
		if (result == CEnemy::RESULT_PARRY)
		{
			// 構えの設定
			pPlayer->SetStance(pos);

			// ヒットストップ
			pEnemy->SetHitStop(25);

			// ヒットストップ
			pPlayer->SetHitStop(25);

			// ヒット状態にする
			pEnemy->ChangeState(make_shared<CEnemyHit>());
		}
		// 回避された
		else if (result == CEnemy::RESULT_AVOID)
		{
			// スローモーションの取得
			CSlow* pSlow = CManager::GetSlow();

			// スローモーション
			pSlow->Start(60, 12);

			m_nTime = 120;
		}
		// 攻撃があたった
		else if (result == CEnemy::RESULT_HIT)
		{
			// プレイヤーのダメージモーションの設定
			pPlayer->SetDamageMotion(pos, 4);
		}
		// 絶対反撃
		else if (result == CEnemy::RESULT_SPREVENGE)
		{
			// プレイヤーの位置の取得
			D3DXVECTOR3 playerPos = pPlayer->GetPosition();

			// 角度を求める
			float fDirAngle = GetTargetAngle(playerPos, pos);

			// 角度を設定
			pPlayer->SetAngle(fDirAngle);

			// ヒット状態にする
			pEnemy->ChangeState(make_shared<CEnemySuperHit>());

			// 状態の変更
			pPlayer->ChangeState(make_shared<CPlayerRevengeAttack>());
		}
	}

	// 必ず回転する時間を計算
	int nTime = m_nMaxTime - ABSSPIN_TIME;

	// プレイヤーがダメージを受けて絶対回転する時間が終わったら
	if (pPlayerMotion->GetBlendType() == pPlayer->MOTIONTYPE_DAMAGE && nTime >= m_nTime)
	{
		// モーションが終わったら
		if (pMotion->FinishMotion())
		{
			// 状態マネージャーの取得
			auto pStateManager = pEnemy->GetStateManager();

			// プレイヤーの立ち位置でモーションを設定
			if (pStateManager != nullptr && pStateManager->SetMotionByPlayerPosition())
			{
				return;
			}

			// 状態の設定
			pEnemy->ChangeState(make_shared<CEnemyEndRush>(true));

			return;
		}
	}

	// プレイヤーの位置の取得
	D3DXVECTOR3 playerPos = pPlayer->GetPosition();

	// 距離を求める
	float fDistance = GetDistance(playerPos - pos);

	// 状態マネージャーの取得
	auto pStateManager = pEnemy->GetStateManager();

	// HPが半分を切ったら
	if (pStateManager != nullptr && pStateManager->CheckLowHp(2))
	{
		// 一定の距離まで近づいたら
		if (fDistance <= ATTACK_DISTANCE && m_ProbAttack <= PROB_ATTACK)
		{
			// 振り下ろしに派生する
			pEnemy->ChangeState(make_shared<CEnemyAttackSmash>());
			return;
		}
	}

	// モーションが終わったら
	if (m_nTime <= 0 && pMotion->FinishMotion())
	{
		// モーションの設定
		pMotion->SetMotion(CEnemy::MOTIONTYPE_NEUTRAL, true, 60);

		// 状態をIdleにする
		pEnemy->ChangeState(make_shared<CEnemyIdle>(60));

		return;
	}
}

//===================================================
// コンストラクタ(スイング攻撃)
//===================================================
CEnemySwing::CEnemySwing() : CEnemyState(ID_SWING)
{
	m_nProbAction = NULL;
	m_nNextAction = INIT_NEXT_ACTION;
}

//===================================================
// デストラクタ(スイング攻撃)
//===================================================
CEnemySwing::~CEnemySwing()
{
}

//===================================================
// 初期化処理(スイング攻撃)
//===================================================
void CEnemySwing::Init(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 軌跡のリセット
	pEnemy->DeleteOrbit();

	// 次の行動を抽選
	m_nNextAction = rand() % 100;

	// 攻撃に派生する確率
	m_nProbAction = rand() % 100;

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(CEnemy::MOTIONTYPE_SWING, true, 10);
	}
}

//===================================================
// 更新処理(スイング攻撃)
//===================================================
void CEnemySwing::Update(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// プレイヤーの取得
	CPlayer* pPlayer = CGame::GetPlayer();

	// 位置の取得
	D3DXVECTOR3 pos = pEnemy->GetPosition();

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// イベントフレームの判定
		if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_SWING) && pEnemy->IsDamageMotion() == false)
		{
			// 状態の設定
			pEnemy->SetState(CCharacter3D::STATE_ACTION, 5);

			// 軌跡の処理
			pEnemy->Orbit(16, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f));

			// 攻撃の結果を取得
			CEnemy::RESULT result = pEnemy->WeponAttackResult(pPlayer);

			// パリィされた
			if (result == CEnemy::RESULT_PARRY)
			{
				pPlayer->SetStance(pos);

				pEnemy->SetHitStop(25);

				pPlayer->SetHitStop(25);

				// ヒット状態にする
				pEnemy->ChangeState(make_shared<CEnemyHit>());
			}
			// 回避だったら
			else if (result == CEnemy::RESULT_AVOID)
			{
				CSlow* pSlow = CManager::GetSlow();

				pSlow->Start(60, 4);
			}
			// 範囲内で視界に入っていない、カウンターしていない
			else if (result == CEnemy::RESULT_HIT)
			{
				// プレイヤーのダメージモーションの設定
				pPlayer->SetDamageMotion(pos, 5);
			}
			// 絶対反撃
			else if (result == CEnemy::RESULT_SPREVENGE)
			{
				// プレイヤーの位置の取得
				D3DXVECTOR3 playerPos = pPlayer->GetPosition();

				// 角度を求める
				float fAngle = GetTargetAngle(playerPos, pos);

				// 角度を設定
				pPlayer->SetAngle(fAngle);

				// ヒット状態にする
				pEnemy->ChangeState(make_shared<CEnemySuperHit>());

				// 状態の変更
				pPlayer->ChangeState(make_shared<CPlayerRevengeAttack>());
			}
		}

		if (pMotion->IsEventFrame(0, 20, CEnemy::MOTIONTYPE_SWING))
		{
			// プレイヤーの方向を見る処理
			pEnemy->AngleToPlayer();
		}

		// モーションが終わったら
		if (pMotion->FinishMotion())
		{
			// 状態マネージャーの取得
			auto pStateManager = pEnemy->GetStateManager();

			// プレイヤーの立ち位置でモーションを設定
			if (pStateManager != nullptr && pStateManager->SetMotionByPlayerPosition())
			{
				return;
			}

			// 10%の確率
			if (m_nNextAction <= 10)
			{
				// バックステップする
				pEnemy->ChangeState(make_shared<CEnemyBackStep>());

				return;
			}
		}

		// モーションのブレンドが終わったら
		if (pMotion->IsFinishEndBlend())
		{
			// 剣の軌跡の消去
			pEnemy->DeleteOrbit();

			// 状態の変更
			pEnemy->ChangeState(make_shared<CEnemyIdle>(5));
		}

		// 状態マネージャーの取得
		auto pStateManager = pEnemy->GetStateManager();

		// 派生できるなら
		if (pStateManager != nullptr && pStateManager->CheckLowHp(2) && m_nProbAction <= PROB_ACTION)
		{
			if (pMotion->IsEventFrame(40,40, CEnemy::MOTIONTYPE_SWING))
			{
				// 横凪に派生
				pEnemy->ChangeState(make_shared<CEnemyAttackSmash>());
				return;
			}
		}
			
		// 敵の攻撃のカウンターの目安の表示
		if (pMotion->IsEventFrame(40, 40, CEnemy::MOTIONTYPE_SWING))
		{
			// 音の取得
			CSound* pSound = CManager::GetSound();

			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_ENEMY_ATTACK);
			}

			// モデルの位置の取得
			D3DXVECTOR3 chestPos = pEnemy->GetModelPos(CEnemy::MODEL_CHEST);

			// パーティクルの生成
			auto pParticle = CParticle3DNormal::Create(chestPos, 100.0f, D3DXCOLOR(1.0f, 0.3f, 0.3f, 1.0f));

			// パーティクルの設定
			pParticle->SetParticle(15.0f, 240, 50, 1, 314);
		}

		if (pMotion->IsEventFrame(60, 60, CEnemy::MOTIONTYPE_SWING))
		{
			// 音の取得
			CSound* pSound = CManager::GetSound();

			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_SWING);
			}
		}
	}
}

//===================================================
// コンストラクタ(ジャンプ攻撃)
//===================================================
CEnemyJumpAttack::CEnemyJumpAttack() : CEnemyState(ID_JUMPATTACK)
{
}

//===================================================
// デストラクタ(ジャンプ攻撃)
//===================================================
CEnemyJumpAttack::~CEnemyJumpAttack()
{
}

//===================================================
// 初期化処理(ジャンプ攻撃)
//===================================================
void CEnemyJumpAttack::Init(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 軌跡のリセット
	pEnemy->DeleteOrbit();

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(CEnemy::MOTIONTYPE_JUMPATTACK, true, 10);
	}
}

//===================================================
// 更新処理(ジャンプ攻撃)
//===================================================
void CEnemyJumpAttack::Update(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 位置の取得
	D3DXVECTOR3 pos = pEnemy->GetPosition();

	// プレイヤーの取得
	auto pPlayer = CGame::GetPlayer();

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// 構え中だったら
		if (pMotion->IsEventFrame(20, 20, CEnemy::MOTIONTYPE_JUMPATTACK))
		{
			// ウェーブの生成
			auto pWave = CMeshWave::Create(pos, 50.0f, 50.0f, D3DXCOLOR(1.0f, 0.4f, 0.4f, 1.0));

			// ウェーブの設定処理
			pWave->SetWave(30, 50.0f);
		}

		// 構え中だったら
		if (pMotion->IsEventFrame(1, 40, CEnemy::MOTIONTYPE_JUMPATTACK))
		{
			// プレイヤーの方向を見る
			pEnemy->AngleToPlayer();
		}

		// 40フレーム目になったら
		if (pMotion->IsEventFrame(40, 40, CEnemy::MOTIONTYPE_JUMPATTACK))
		{
			// 音の取得
			CSound* pSound = CManager::GetSound();

			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_ENEMY_ATTACK);

				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_ENEMY_JUMP);
			}

			// ジャンプする
			pEnemy->GetMovement()->Jump(24.0f);
		}

		// ジャンプ中だったら
		if (pMotion->IsEventFrame(40, 90, CEnemy::MOTIONTYPE_JUMPATTACK))
		{
			// 状態の設定
			pEnemy->SetState(CCharacter3D::STATE_ACTION, 5);

			// 軌跡の設定
			pEnemy->Orbit(16, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f));

			// プレイヤーまでの差分を求める
			D3DXVECTOR3 Diff = pPlayer->GetPosition() - pos;

			// 距離を求める
			float dir = GetDistance(Diff);

			// 向きの取得
			float fAngle = pEnemy->GetRotation().y;

			// ジャンプ攻撃中の移動
			pEnemy->GetMovement()->MoveForWard(dir / JUMPATTACK_MOVE_FRAME, fAngle + D3DX_PI);
		}

		// たたきつけになったら
		if (pMotion->IsEventFrame(90, 90, CEnemy::MOTIONTYPE_JUMPATTACK))
		{
			// 瓦礫の設定
			pEnemy->SetRubble();
		}

		if (pMotion->FinishMotion())
		{
			// 状態マネージャーの取得
			auto pStateManager = pEnemy->GetStateManager();

			// プレイヤーの立ち位置でモーションを設定
			if (pStateManager != nullptr && pStateManager->SetMotionByPlayerPosition())
			{
				return;
			}
		}
		// モーションのブレンドが終わったら
		else if (pMotion->IsFinishEndBlend())
		{
			pEnemy->ChangeState(make_shared<CEnemyIdle>(1));
			return;
		}
	}

	// プレイヤーとの当たり判定
	CollisionPlayer(pPlayer, pMotion);
}

//===================================================
// プレイヤーとの当たり判定
//===================================================
void CEnemyJumpAttack::CollisionPlayer(CPlayer* pPlayer, CMotion* pMotion)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// 位置の取得
	D3DXVECTOR3 pos = pEnemy->GetPosition();

	// 敵の攻撃のカウンターの目安の表示
	if (pMotion->IsEventFrame(70, 70, CEnemy::MOTIONTYPE_JUMPATTACK))
	{
		// モデルの位置の取得
		D3DXVECTOR3 chestPos = pEnemy->GetModelPos(CEnemy::MODEL_CHEST);

		// パーティクルの生成
		auto pParticle = CParticle3DNormal::Create(chestPos, 100.0f, D3DXCOLOR(1.0f, 0.3f, 0.3f, 1.0f));

		// パーティクルの設定
		pParticle->SetParticle(15.0f, 240, 50, 1, 314);
	}

	// イベントフレームの判定
	if (pMotion->IsEventFrame(80, 88, CEnemy::MOTIONTYPE_JUMPATTACK) && pEnemy->IsDamageMotion() == false)
	{
		// 状態の設定
		pEnemy->SetState(CCharacter3D::STATE_ACTION, 5);

		// 攻撃の結果を取得
		CEnemy::RESULT result = pEnemy->WeponAttackResult(pPlayer);

		// パリィされた
		if (result == CEnemy::RESULT_PARRY)
		{
			// 構えの設定処理
			pPlayer->SetStance(pos);

			// ヒットストップ
			pEnemy->SetHitStop(25);

			// ヒットストップ
			pPlayer->SetHitStop(25);

			// ヒット状態にする
			pEnemy->ChangeState(make_shared<CEnemyHit>());
		}
		// 回避だったら
		else if (result == CEnemy::RESULT_AVOID)
		{
			CSlow* pSlow = CManager::GetSlow();

			pSlow->Start(60, 4);
		}
		// 攻撃があたった
		else if (result == CEnemy::RESULT_HIT)
		{
			// プレイヤーのダメージモーションの設定
			pPlayer->SetDamageMotion(pos, 9);
		}
		// 絶対反撃
		else if (result == CEnemy::RESULT_SPREVENGE)
		{
			// プレイヤーの位置の取得
			D3DXVECTOR3 playerPos = pPlayer->GetPosition();

			// 角度を求める
			float fAngle = GetTargetAngle(playerPos, pos);

			// 角度を設定
			pPlayer->SetAngle(fAngle);

			// ヒット状態にする
			pEnemy->ChangeState(make_shared<CEnemySuperHit>());

			// 状態の変更
			pPlayer->ChangeState(make_shared<CPlayerRevengeAttack>());
		}
	}

	if (pMotion->IsEventFrame(80, 80, CEnemy::MOTIONTYPE_JUMPATTACK))
	{
		// 音の取得
		CSound* pSound = CManager::GetSound();

		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_SWING);
		}
	}
}

//===================================================
// コンストラクタ(突進攻撃)
//===================================================
CEnemyRush::CEnemyRush() : CEnemyState(ID_RUSH)
{
	m_nProbSmash = NULL;
	m_nEndTime = MAX_TIME;
}

//===================================================
// デストラクタ(突進攻撃)
//===================================================
CEnemyRush::~CEnemyRush()
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// もとに戻しておく
	pEnemy->GetMovement()->SetInertia(0.25f);
}

//===================================================
// 初期化処理(突進攻撃)
//===================================================
void CEnemyRush::Init(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 振り下ろしに派生するかどうかを求める
	m_nProbSmash = rand() % 100;

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(CEnemy::MOTIONTYPE_START_RUSH, true, 10);
	}
}

//===================================================
// 更新処理(突進攻撃)
//===================================================
void CEnemyRush::Update(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// プレイヤーの取得
	CPlayer* pPlayer = CGame::GetPlayer();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// プレイヤーが使われていないなら処理しない
	if (pPlayer == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// プレイヤーの方向を向く
	pEnemy->AngleToPlayer();

	if (pMotion != nullptr)
	{
		// 敵の攻撃のカウンターの目安の表示
		if (pMotion->IsEventFrame(68, 68, CEnemy::MOTIONTYPE_START_RUSH))
		{
			// 音の取得
			CSound* pSound = CManager::GetSound();

			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_ENEMY_ATTACK);
			}

			// モデルの位置の取得
			D3DXVECTOR3 chestPos = pEnemy->GetModelPos(CEnemy::MODEL_CHEST);

			// パーティクルの生成
			auto pParticle = CParticle3DNormal::Create(chestPos, 100.0f, D3DXCOLOR(1.0f, 0.3f, 0.3f, 1.0f));

			// パーティクルの設定
			pParticle->SetParticle(15.0f, 240, 50, 1, 314);
		}

		// 呼び動作中だったら処理しない
		if (pMotion->IsEventFrame(0, 70, CEnemy::MOTIONTYPE_START_RUSH))
		{
			return;
		}
		else
		{
			// モーションの再生
			pMotion->SetMotion(CEnemy::MOTIONTYPE_RUSH, true, 10);
		}
	}

	// プレイヤーの位置の取得
	D3DXVECTOR3 playerPos = pPlayer->GetPosition();

	// 位置の取得
	D3DXVECTOR3 pos = pEnemy->GetPosition();

	// 距離を求める
	float fDistance = GetDistance(playerPos - pos);

	// 状態マネージャーの取得
	auto pStateManager = pEnemy->GetStateManager();

	// HPが半分を切ったら
	if (pStateManager != nullptr && pStateManager->CheckLowHp(2))
	{
		// 一定の距離まで近づいたら
		if (fDistance <= SMASH_DISTANCE && m_nProbSmash <= PROB_SMASH)
		{
			// 振り下ろしに派生する
			pEnemy->ChangeState(make_shared<CEnemyAttackSmash>());
			return;
		}
	}

	// 状態の設定
	pEnemy->SetState(CCharacter3D::STATE_ACTION, 5);

	// プレイヤーの方向に向かう
	pEnemy->ChasePlayer(0.5f, 8.0f);

	m_nEndTime--;

	// 終了したら
	if (m_nEndTime <= 0)
	{
		// 状態変更
		pEnemy->ChangeState(make_shared<CEnemyEndRush>(0.05f));

		return;
	}

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_RUSH, 0) ||
			pMotion->IsEventFrame(CEnemy::MOTIONTYPE_RUSH, 2))
		{
			// サークルの設定処理
			pEnemy->RushEffect();
		}

		if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_RUSH))
		{
			// 音の取得
			CSound* pSound = CManager::GetSound();

			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_WARK003);
			}
		}

		// 攻撃の結果を取得
		CEnemy::RESULT result = pEnemy->AttackResult(pPlayer, CEnemy::MODEL_WAIST, 250.0f);

		// パリィされた
		if (result == CEnemy::RESULT_PARRY)
		{
			// 構えの設定処理
			pPlayer->SetStance(pos);

			// ヒットストップ
			pEnemy->SetHitStop(25);

			// ヒットストップ
			pPlayer->SetHitStop(25);

			// ヒット状態にする
			pEnemy->ChangeState(make_shared<CEnemyHit>());
		}
		// 回避だったら
		else if (result == CEnemy::RESULT_AVOID)
		{
			CSlow* pSlow = CManager::GetSlow();

			pSlow->Start(60, 4);

			// 状態変更
			pEnemy->ChangeState(make_shared<CEnemyEndRush>(0.009f));
		}
		// 攻撃があたった
		else if (result == CEnemy::RESULT_HIT)
		{
			// 状態変更
			pEnemy->ChangeState(make_shared<CEnemyEndRush>(0.05f));

			// プレイヤーのダメージモーションの設定
			pPlayer->SetDamageMotion(pos, 7);
		}
		// 絶対反撃
		else if (result == CEnemy::RESULT_SPREVENGE)
		{
			// 角度を求める
			float fAngle = GetTargetAngle(playerPos, pos);

			// 角度を設定
			pPlayer->SetAngle(fAngle);

			// ヒット状態にする
			pEnemy->ChangeState(make_shared<CEnemySuperHit>());

			// 状態の変更
			pPlayer->ChangeState(make_shared<CPlayerRevengeAttack>());
		}
	}
}

//===================================================
// コンストラクタ(突進攻撃終了)
//===================================================
CEnemyEndRush::CEnemyEndRush() : CEnemyState(ID_ENDRUSH)
{
	m_fInertia = NULL;
}

//===================================================
// コンストラクタ(突進攻撃終了)
//===================================================
CEnemyEndRush::CEnemyEndRush(const float fInertia) : CEnemyState(ID_ENDRUSH)
{
	m_fInertia = fInertia;
}

//===================================================
// デストラクタ(突進攻撃終了)
//===================================================
CEnemyEndRush::~CEnemyEndRush()
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// 慣性の設定をもとに戻す
	pEnemy->GetMovement()->SetInertia(0.25f);

	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pSound != nullptr)
	{
		// 音の停止
		pSound->StopSound(CSound::SOUND_LABEL_END_RUSH);
	}
}

//===================================================
// 初期化処理(突進攻撃終了)
//===================================================
void CEnemyEndRush::Init(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pSound != nullptr)
	{
		// 音の再生
		pSound->Play(CSound::SOUND_LABEL_END_RUSH, 0.5f);
	}

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(CEnemy::MOTIONTYPE_END_RUSH, true, 10);
	}
}

//===================================================
// 更新処理処理(突進攻撃終了)
//===================================================
void CEnemyEndRush::Update(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		pEnemy->SetState(CCharacter3D::STATE_ACTION, 5);

		// 慣性の設定
		pEnemy->GetMovement()->SetInertia(m_fInertia);

		// 右足の位置
		D3DXVECTOR3 footPosR = pEnemy->GetModelPos(CEnemy::MODEL_FOOTR);

		// 向きの選出
		float fAngle = static_cast<float>(rand() % 629 - 314) * 0.01f;

		// 瓦礫の生成
		CRubble::Create(footPosR, D3DXVECTOR3(fAngle, fAngle, fAngle), 60, CRubble::TYPE_THREE);

		if (pMotion->FinishMotion())
		{
			// 状態マネージャーの取得
			auto pStateManager = pEnemy->GetStateManager();

			// プレイヤーの立ち位置でモーションを設定
			if (pStateManager != nullptr && pStateManager->SetMotionByPlayerPosition())
			{
				return;
			}
		}
		// モーションが終わったら
		else if (pMotion->IsFinishEndBlend())
		{
			// 慣性の設定をもとに戻す
			pEnemy->GetMovement()->SetInertia(0.25f);

			// 状態の遷移
			pEnemy->ChangeState(make_shared<CEnemyIdle>(5));
		}
	}
}

//===================================================
// コンストラクタ(後ろ蹴り)
//===================================================
CEnemyBackKick::CEnemyBackKick() : CEnemyState(ID_BACKKICK)
{
	m_bHit = false;
}

//===================================================
// デストラクタ(後ろ蹴り)
//===================================================
CEnemyBackKick::~CEnemyBackKick()
{
}

//===================================================
// 初期化処理(後ろ蹴り)
//===================================================
void CEnemyBackKick::Init(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(CEnemy::MOTIONTYPE_BACKKICK, true, 10);
	}
}

//===================================================
// 更新処理(後ろ蹴り)
//===================================================
void CEnemyBackKick::Update(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// プレイヤーの取得
	CPlayer* pPlayer = CGame::GetPlayer();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// プレイヤーが使われていないなら処理しない
	if (pPlayer == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// モーションがあるなら
	if (pMotion == nullptr)
	{
		// 状態の遷移
		pEnemy->ChangeState(make_shared<CEnemyIdle>(5));
		return;
	}
	// 状態の設定
	pEnemy->SetState(CCharacter3D::STATE_ACTION, 5);

	// 攻撃の結果を取得
	CEnemy::RESULT result = pEnemy->AttackResult(pPlayer, CEnemy::MODEL_FOOTR, 150.0f);

	// 位置の取得
	D3DXVECTOR3 pos = pEnemy->GetPosition();

	// 敵の攻撃のカウンターの目安の表示
	if (pMotion->IsEventFrame(50, 50, CEnemy::MOTIONTYPE_BACKKICK))
	{
		// 音の取得
		CSound* pSound = CManager::GetSound();

		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_ENEMY_ATTACK);
		}

		// モデルの位置の取得
		D3DXVECTOR3 chestPos = pEnemy->GetModelPos(CEnemy::MODEL_CHEST);

		// パーティクルの生成
		auto pParticle = CParticle3DNormal::Create(chestPos, 100.0f, D3DXCOLOR(1.0f, 0.3f, 0.3f, 1.0f));

		// パーティクルの設定
		pParticle->SetParticle(15.0f, 240, 50, 1, 314);
	}

	if (pMotion->IsEventFrame(68, 75, CEnemy::MOTIONTYPE_BACKKICK))
	{
		// パリィされた
		if (result == CEnemy::RESULT_PARRY)
		{
			// 構えの設定処理
			pPlayer->SetStance(pos, CPlayer::PARRYMOTION_JUMP);

			// ヒットストップ
			pEnemy->SetHitStop(25);

			// ヒットストップ
			pPlayer->SetHitStop(25);

			// ヒット状態の生成
			auto pHitState = make_shared<CEnemyHit>();

			// 種類の設定
			pHitState->SetType(CEnemyHit::TYPE_BACK);

			// ヒット状態にする
			pEnemy->ChangeState(pHitState);
		}
		// 回避だったら
		else if (result == CEnemy::RESULT_AVOID)
		{
			CSlow* pSlow = CManager::GetSlow();

			pSlow->Start(60, 4);

			// 状態変更
			pEnemy->ChangeState(make_shared<CEnemyEndRush>(0.02f));
		}
		// 攻撃があたった
		else if (result == CEnemy::RESULT_HIT)
		{
			m_bHit = true;

			// プレイヤーのダメージモーションの設定
			pPlayer->SetDamageMotion(pos, 3);
		}
		// 絶対反撃
		else if (result == CEnemy::RESULT_SPREVENGE)
		{
			// プレイヤーの位置の取得
			D3DXVECTOR3 playerPos = pPlayer->GetPosition();

			// 角度を求める
			float fAngle = GetTargetAngle(playerPos, pos);

			// 角度を設定
			pPlayer->SetAngle(fAngle);

			// ヒット状態にする
			pEnemy->ChangeState(make_shared<CEnemySuperHit>());

			// 状態の変更
			pPlayer->ChangeState(make_shared<CPlayerRevengeAttack>());
		}

	}

	// モーションが終わったら
	if (pMotion->IsFinishEndBlend())
	{
		// 攻撃が当たってたら
		if (m_bHit)
		{
			// 突進に派生
			pEnemy->ChangeState(make_shared<CEnemyRushSwing>());
		}
		else
		{
			// 状態の遷移
			pEnemy->ChangeState(make_shared<CEnemyIdle>(5));
		}
	}
}

//===================================================
// コンストラクタ(薙ぎ払い右)
//===================================================
CEnemySweepRight::CEnemySweepRight() : CEnemyState(ID_SWEEP_RIGHT)
{
}

//===================================================
// デストラクタ(薙ぎ払い右)
//===================================================
CEnemySweepRight::~CEnemySweepRight()
{
}

//===================================================
// 初期化処理(薙ぎ払い右)
//===================================================
void CEnemySweepRight::Init(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 軌跡の破棄
	pEnemy->DeleteOrbit();

	// モデルの位置の取得
	D3DXVECTOR3 chestPos = pEnemy->GetModelPos(CEnemy::MODEL_CHEST);

	// パーティクルの生成
	auto pParticle = CParticle3DNormal::Create(chestPos, 100.0f, D3DXCOLOR(1.0f, 0.3f, 0.3f, 1.0f));

	// パーティクルの設定
	pParticle->SetParticle(15.0f, 240, 50, 1, 314);


	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(CEnemy::MOTIONTYPE_SWEEP_RIGHT, true, 40);
	}
}

//===================================================
// 更新処理(薙ぎ払い右)
//===================================================
void CEnemySweepRight::Update(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// プレイヤーの取得
	CPlayer* pPlayer = CGame::GetPlayer();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 位置の取得
	D3DXVECTOR3 pos = pEnemy->GetPosition();

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// 指定のフレーム内か判定
		if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_SWEEP_RIGHT))
		{
			// 状態の設定
			pEnemy->SetState(CCharacter3D::STATE_ACTION, 5);

			// 軌跡の処理
			pEnemy->Orbit(16, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f));

			// 攻撃の結果を取得
			CEnemy::RESULT result = pEnemy->WeponAttackResult(pPlayer);

			// パリィされた
			if (result == CEnemy::RESULT_PARRY)
			{
				pPlayer->SetStance(pos);

				pEnemy->SetHitStop(25);

				pPlayer->SetHitStop(25);

				// ヒット状態にする
				pEnemy->ChangeState(make_shared<CEnemyHit>());
			}
			// 回避だったら
			else if (result == CEnemy::RESULT_AVOID)
			{
				CSlow* pSlow = CManager::GetSlow();

				pSlow->Start(60, 4);
			}
			// 範囲内で視界に入っていない、カウンターしていない
			else if (result == CEnemy::RESULT_HIT)
			{
				// プレイヤーのダメージモーションの設定
				pPlayer->SetDamageMotion(pos, 5);
			}
			// 絶対反撃
			else if (result == CEnemy::RESULT_SPREVENGE)
			{
				// プレイヤーの位置の取得
				D3DXVECTOR3 playerPos = pPlayer->GetPosition();

				// 角度を求める
				float fAngle = GetTargetAngle(playerPos, pos);

				// 角度を設定
				pPlayer->SetAngle(fAngle);

				// ヒット状態にする
				pEnemy->ChangeState(make_shared<CEnemySuperHit>());

				// 状態の変更
				pPlayer->ChangeState(make_shared<CPlayerRevengeAttack>());
			}
		}

		// モーションが終わったら
		if (pMotion->IsFinishEndBlend())
		{
			// 状態マネージャーの取得
			auto pStateManager = pEnemy->GetStateManager();

			// プレイヤーの立ち位置でモーションを設定
			if (pStateManager != nullptr && pStateManager->SetLookBackMotion())
			{
				return;
			}

			// 状態の変更
			pEnemy->ChangeState(make_shared<CEnemyRushSwing>());

			return;
		}
		if (pMotion->IsEventFrame(44, 44, CEnemy::MOTIONTYPE_SWEEP_RIGHT))
		{
			// 音の取得
			CSound* pSound = CManager::GetSound();

			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_SWING);
			}
		}
	}
}

//===================================================
// コンストラクタ(薙ぎ払い左)
//===================================================
CEnemySweepLeft::CEnemySweepLeft() : CEnemyState(ID_SWEEP_LEFT)
{
}

//===================================================
// デストラクタ(薙ぎ払い左)
//===================================================
CEnemySweepLeft::~CEnemySweepLeft()
{
}

//===================================================
// 初期化処理(薙ぎ払い左)
//===================================================
void CEnemySweepLeft::Init(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(CEnemy::MOTIONTYPE_SWEEP_LEFT, true, 40);
	}

}

//===================================================
// 更新処理(薙ぎ払い左)
//===================================================
void CEnemySweepLeft::Update(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// プレイヤーの取得
	CPlayer* pPlayer = CGame::GetPlayer();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 位置の取得
	D3DXVECTOR3 pos = pEnemy->GetPosition();

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// 指定のフレーム内か判定
		if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_SWEEP_LEFT))
		{
			// 状態の設定
			pEnemy->SetState(CCharacter3D::STATE_ACTION, 5);

			// 攻撃の結果を取得
			CEnemy::RESULT result = pEnemy->AttackResult(pPlayer, CEnemy::MODEL_HANDL, 150.0f);

			// パリィされた
			if (result == CEnemy::RESULT_PARRY)
			{
				pPlayer->SetStance(pos);

				pEnemy->SetHitStop(25);

				pPlayer->SetHitStop(25);

				// ヒット状態にする
				pEnemy->ChangeState(make_shared<CEnemyHit>());
			}
			// 回避だったら
			else if (result == CEnemy::RESULT_AVOID)
			{
				CSlow* pSlow = CManager::GetSlow();

				pSlow->Start(60, 4);
			}
			// 範囲内で視界に入っていない、カウンターしていない
			else if (result == CEnemy::RESULT_HIT)
			{
				// プレイヤーのダメージモーションの設定
				pPlayer->SetDamageMotion(pos, 4);
			}
			// 絶対反撃
			else if (result == CEnemy::RESULT_SPREVENGE)
			{
				// プレイヤーの位置の取得
				D3DXVECTOR3 playerPos = pPlayer->GetPosition();

				// 角度を求める
				float fAngle = GetTargetAngle(playerPos, pos);

				// 角度を設定
				pPlayer->SetAngle(fAngle);

				// ヒット状態にする
				pEnemy->ChangeState(make_shared<CEnemySuperHit>());

				// 状態の変更
				pPlayer->ChangeState(make_shared<CPlayerRevengeAttack>());
			}
		}

		// モーションが終わったら
		if (pMotion->IsFinishEndBlend())
		{
			// 状態マネージャーの取得
			auto pStateManager = pEnemy->GetStateManager();

			// プレイヤーの立ち位置でモーションを設定
			if (pStateManager != nullptr && pStateManager->SetLookBackMotion())
			{
				return;
			}

			// 状態の変更
			pEnemy->ChangeState(make_shared<CEnemyRushSwing>());

			return;
		}

		// 敵の攻撃のカウンターの目安の表示
		if (pMotion->IsEventFrame(50, 50, CEnemy::MOTIONTYPE_SWEEP_LEFT))
		{
			// 音の取得
			CSound* pSound = CManager::GetSound();

			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_ENEMY_ATTACK);
			}

			// モデルの位置の取得
			D3DXVECTOR3 chestPos = pEnemy->GetModelPos(CEnemy::MODEL_CHEST);

			// パーティクルの生成
			auto pParticle = CParticle3DNormal::Create(chestPos, 100.0f, D3DXCOLOR(1.0f, 0.3f, 0.3f, 1.0f));

			// パーティクルの設定
			pParticle->SetParticle(15.0f, 240, 50, 1, 314);
		}

		if (pMotion->IsEventFrame(64, 64, CEnemy::MOTIONTYPE_SWEEP_LEFT))
		{
			// 音の取得
			CSound* pSound = CManager::GetSound();

			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_SWING);
			}
		}
	}
}

//===================================================
// コンストラクタ(突進なぎ)
//===================================================
CEnemyRushSwing::CEnemyRushSwing() : CEnemyState(ID_RUSH_SWING)
{
}

//===================================================
// デストラクタ(突進なぎ)
//===================================================
CEnemyRushSwing::~CEnemyRushSwing()
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// 慣性をもとに戻す
	pEnemy->GetMovement()->SetInertia(0.25f);
}

//===================================================
// 初期化処理
//===================================================
void CEnemyRushSwing::Init(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// 剣のリセット
	pEnemy->DeleteOrbit();

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(CEnemy::MOTIONTYPE_RUSH_SWING, true, 10);
	}
}

//===================================================
// 更新処理
//===================================================
void CEnemyRushSwing::Update(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// プレイヤーの取得
	CPlayer* pPlayer = CGame::GetPlayer();

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 音の取得
	CSound* pSound = CManager::GetSound();

	// 取得できなかったら処理しない
	if (pMotion == nullptr)
	{
		pEnemy->ChangeState(make_shared<CEnemyIdle>(1));
		return;
	}

	if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_RUSH_SWING, 2))
	{
		// プレイヤーの取得
		D3DXVECTOR3 playerPos = pPlayer->GetPosition();
		D3DXVECTOR3 pos = pEnemy->GetPosition();

		// プレイヤーの方向を見る
		pEnemy->AngleToPlayer();

		// 距離を求める
		float fDistance = GetDistance(playerPos - pos);

		// 向きの取得
		float fAngle = pEnemy->GetRotation().y;

		// 移動量を設定
		pEnemy->GetMovement()->MoveForWard(fDistance / MOVE_FRAME, fAngle + D3DX_PI);
	}
	else if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_RUSH_SWING, 3))
	{
		// 慣性を設定
		pEnemy->GetMovement()->SetInertia(0.02f);
	}
	if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_RUSH_SWING, 4) ||
		pMotion->IsEventFrame(CEnemy::MOTIONTYPE_RUSH_SWING, 5) ||
		pMotion->IsEventFrame(CEnemy::MOTIONTYPE_RUSH_SWING, 6))
	{
		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_WARK003);
		}
	}

	if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_RUSH_SWING, 7))
	{
		// プレイヤーの方向を見る
		pEnemy->AngleToPlayer();
	}

	// 状態マネージャーの取得
	auto pStateManager = pEnemy->GetStateManager();

	if (pMotion->IsEventFrame(110, 110, CEnemy::MOTIONTYPE_RUSH_SWING))
	{
		// 確率
		int nFeint = rand() % 100;

		// Hpが半分以下だったら
		if (pStateManager != nullptr && pStateManager->CheckLowHp(2) && nFeint <= PROB_ACTION)
		{
			// 攻撃モーションを選出
			int nAction = rand() % ACTION_MAX;

			// 種類の遷移
			switch (nAction)
			{
			case ACTION_SMASH:
				// 振り下ろしに派生する
				pEnemy->ChangeState(make_shared<CEnemyAttackSmash>());
				return;
			case ACTION_SWING:
				// 振りに派生する
				pEnemy->ChangeState(make_shared<CEnemySwing>());
				return;
			default:
				break;
			}
		}
	}

	// プレイヤーとの当たり判定
	CollisionPlayer(pEnemy, pMotion);

	if (pMotion->FinishMotion())
	{
		// プレイヤーの立ち位置でモーションを設定
		if (pStateManager != nullptr && pStateManager->SetMotionByPlayerPosition())
		{
			return;
		}
	}
	// モーションが終わったら
	if (pMotion->IsFinishEndBlend())
	{
		// 状態の変更
		pEnemy->ChangeState(make_shared<CEnemyIdle>(20));

		return;
	}
}

//===================================================
// プレイヤーとの当たり判定
//===================================================
void CEnemyRushSwing::CollisionPlayer(CEnemy* pEnemy, CMotion* pMotion)
{
	// 位置の取得
	D3DXVECTOR3 pos = pEnemy->GetPosition();

	// プレイヤーの取得
	CPlayer* pPlayer = CGame::GetPlayer();

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// 敵の攻撃のカウンターの目安の表示
	if (pMotion->IsEventFrame(110, 110, CEnemy::MOTIONTYPE_RUSH_SWING))
	{
		// 音の取得
		CSound* pSound = CManager::GetSound();

		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_ENEMY_ATTACK);
		}

		// モデルの位置の取得
		D3DXVECTOR3 chestPos = pEnemy->GetModelPos(CEnemy::MODEL_CHEST);

		// パーティクルの生成
		auto pParticle = CParticle3DNormal::Create(chestPos, 100.0f, D3DXCOLOR(1.0f, 0.3f, 0.3f, 1.0f));

		// パーティクルの設定
		pParticle->SetParticle(15.0f, 240, 50, 1, 314);
	}

	if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_RUSH_SWING, 0))
	{
		// 音の取得
		CSound* pSound = CManager::GetSound();

		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_SWING);
		}
	}
	// イベントフレームの判定
	if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_RUSH_SWING, 1))
	{
		// 剣の軌跡
		pEnemy->Orbit(16, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f));

		// 状態の設定
		pEnemy->SetState(CCharacter3D::STATE_ACTION, 5);

		// 攻撃の結果を取得
		CEnemy::RESULT result = pEnemy->WeponAttackResult(pPlayer);

		// パリィされた
		if (result == CEnemy::RESULT_PARRY)
		{
			// 構えの設定処理
			pPlayer->SetStance(pos);

			// ヒットストップ
			pEnemy->SetHitStop(25);

			// ヒットストップ
			pPlayer->SetHitStop(25);

			// ヒット状態にする
			pEnemy->ChangeState(make_shared<CEnemyHit>());
		}
		// 回避だったら
		else if (result == CEnemy::RESULT_AVOID)
		{
			CSlow* pSlow = CManager::GetSlow();

			pSlow->Start(60, 4);
		}
		// 攻撃があたった
		else if (result == CEnemy::RESULT_HIT)
		{
			// プレイヤーのダメージモーションの設定
			pPlayer->SetDamageMotion(pos, 6);
		}
		// 絶対反撃
		else if (result == CEnemy::RESULT_SPREVENGE)
		{
			// プレイヤーの位置の取得
			D3DXVECTOR3 playerPos = pPlayer->GetPosition();

			// 角度を求める
			float fAngle = GetTargetAngle(playerPos, pos);

			// 角度を設定
			pPlayer->SetAngle(fAngle);

			// ヒット状態にする
			pEnemy->ChangeState(make_shared<CEnemySuperHit>());

			// 状態の変更
			pPlayer->ChangeState(make_shared<CPlayerRevengeAttack>());
		}
	}
}

//===================================================
// コンストラクタ(カウンター)
//===================================================
CEnemyCounter::CEnemyCounter() : CEnemyState(ID_COUNTER)
{
}

//===================================================
// デストラクタ(カウンター)
//===================================================
CEnemyCounter::~CEnemyCounter()
{
}

//===================================================
// 初期化処理(カウンター)
//===================================================
void CEnemyCounter::Init(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 剣の軌跡のリセット
	pEnemy->DeleteOrbit();

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(CEnemy::MOTIONTYPE_COUNTER, true, 25);
	}
}

//===================================================
// 更新処理(カウンター)
//===================================================
void CEnemyCounter::Update(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 位置の取得
	D3DXVECTOR3 pos = pEnemy->GetPosition();

	// プレイヤーの取得
	CPlayer* pPlayer = CGame::GetPlayer();

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// 敵の攻撃のカウンターの目安の表示
	if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_COUNTER, 0))
	{
		// 音の取得
		CSound* pSound = CManager::GetSound();

		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_ENEMY_ATTACK);
		}

		// モデルの位置の取得
		D3DXVECTOR3 chestPos = pEnemy->GetModelPos(CEnemy::MODEL_CHEST);

		// パーティクルの生成
		auto pParticle = CParticle3DNormal::Create(chestPos, 100.0f, D3DXCOLOR(1.0f, 0.3f, 0.3f, 1.0f));

		// パーティクルの設定
		pParticle->SetParticle(15.0f, 240, 50, 1, 314);
	}

	if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_COUNTER, 2))
	{
		// 音の取得
		CSound* pSound = CManager::GetSound();

		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_SWING);
		}
	}
	// イベントフレームの判定
	if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_COUNTER, 1))
	{
		// 剣の軌跡
		pEnemy->Orbit(16, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f));

		// 状態の設定
		pEnemy->SetState(CCharacter3D::STATE_ACTION, 5);

		// 攻撃の結果を取得
		CEnemy::RESULT result = pEnemy->WeponAttackResult(pPlayer);

		// パリィされた
		if (result == CEnemy::RESULT_PARRY)
		{
			// 構えの設定処理
			pPlayer->SetStance(pos);

			// ヒットストップ
			pEnemy->SetHitStop(25);

			// ヒットストップ
			pPlayer->SetHitStop(25);

			// ヒット状態にする
			pEnemy->ChangeState(make_shared<CEnemyHit>());
		}
		// 回避だったら
		else if (result == CEnemy::RESULT_AVOID)
		{
			CSlow* pSlow = CManager::GetSlow();

			pSlow->Start(60, 4);
		}
		// 攻撃があたった
		else if (result == CEnemy::RESULT_HIT)
		{
			// プレイヤーのダメージモーションの設定
			pPlayer->SetDamageMotion(pos, 6,200.0f,100.0f);
		}
		// 絶対反撃
		else if (result == CEnemy::RESULT_SPREVENGE)
		{
			// プレイヤーの位置の取得
			D3DXVECTOR3 playerPos = pPlayer->GetPosition();

			// 角度を求める
			float fAngle = GetTargetAngle(playerPos, pos);

			// 角度を設定
			pPlayer->SetAngle(fAngle);

			// ヒット状態にする
			pEnemy->ChangeState(make_shared<CEnemySuperHit>());

			// 状態の変更
			pPlayer->ChangeState(make_shared<CPlayerRevengeAttack>());
		}
	}

	// モーションが終わったら
	if (pMotion->IsFinishEndBlend())
	{
		// 状態の変更
		pEnemy->ChangeState(make_shared<CEnemyIdle>(1));
		return;
	}
}

//===================================================
// コンストラクタ(衝撃波の跳ね返し)
//===================================================
CEnemyRevengeImpact::CEnemyRevengeImpact()
{
}

//===================================================
// デストラクタ(衝撃波の跳ね返し)
//===================================================
CEnemyRevengeImpact::~CEnemyRevengeImpact()
{
}

//===================================================
// 初期化処理(衝撃波の跳ね返し)
//===================================================
void CEnemyRevengeImpact::Init(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pSound != nullptr)
	{
		// 音の再生
		pSound->Play(CSound::SOUND_LABEL_IMPACT001);
	}

	// 剣の軌跡のリセット
	pEnemy->DeleteOrbit();

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(CEnemy::MOTIONTYPE_REVENGE_IMPACT, true, 5);
	}
}

//===================================================
// 更新処理処理(衝撃波の跳ね返し)
//===================================================
void CEnemyRevengeImpact::Update(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 剣の軌跡
	pEnemy->Orbit(16, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f));

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションが終わったら
		if (pMotion->IsFinishEndBlend())
		{
			// 状態の遷移
			pEnemy->ChangeState(make_shared<CEnemyIdle>(1));
		}
	}
}