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

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// 取得できなかったら処理しない
	if (pMotion == nullptr) return;

	// 範囲外にいたらかつモーションが終わったら
	if (!pEnemy->CheckDistance(ACTION_RANGE) && pMotion->IsEndLoopMotion())
	{
		// 攻撃状態
		pEnemy->ChangeState(make_shared<CTrainingEnemyIdle>());
	}
}
