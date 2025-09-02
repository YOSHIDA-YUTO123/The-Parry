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

using namespace std; // 名前空間stdの使用

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

	// 範囲外にいたらかつモーションが終わったら
	if (!pEnemy->CheckDistance(ACTION_RANGE) && pMotion->IsEndLoopMotion())
	{
		// 攻撃状態
		pEnemy->ChangeState(make_shared<CTrainingEnemyIdle>());
	}

	// 位置の取得
	D3DXVECTOR3 pos = pEnemy->GetPosition();

	// イベントフレームの判定
	if (pMotion->IsEventFrame(65, 88, CTrainingEnemy::MOTIONTYPE_ACTION))
	{
		// 攻撃の結果の取得
		auto result = pEnemy->GetAttackResult();

		// パリィされた
		if (result == CTrainingEnemy::RESULT_PARRY)
		{
			// 構えの設定処理
			pPlayer->SetStance(pos);

			// 成功度の取得
			int nSuccess = pPlayer->SuccessParry();

			// ヒットストップ
			pEnemy->SetHitStop(25);

			// ヒットストップ
			pPlayer->SetHitStop(25);

			//// 成功度の設定
			//pEnemy->SetSuccess(nSuccess);

			// ヒット状態にする
			pEnemy->ChangeState(make_shared<CTrainingEnemyHit>());
		}
		// 回避だったら
		else if (result == CTrainingEnemy::RESULT_AVOID)
		{
			//CSlow* pSlow = CManager::GetSlow();

			//pSlow->Start(60, 4);
		}
		// 攻撃があたった
		else if (result == CTrainingEnemy::RESULT_HIT)
		{
			// 吹き飛び処理
			pPlayer->BlowOff(pos, 100.0f, 10.0f);

			// プレイヤー状態の変更
			pPlayer->ChangeState(make_shared<CPlayerDamage>(0));
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

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(CTrainingEnemy::MOTIONTYPE_HIT, true, 10);
	}
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
// コンストラクタ(ダメージ状態)
//================================================
CTrainingEnemyDamage::CTrainingEnemyDamage() : CTrainingEnemyState(ID_DAMAGE)
{
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

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(CTrainingEnemy::MOTIONTYPE_DAMAGE, true, 10);
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
