//===================================================
//
// 敵の状態 [enemyState.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "EnemyState.h"
#include"enemy.h"

constexpr int NEXT_STAE_TIME = 180; // 次の行動に移るまでの時間
constexpr int START_IMPACT = 55;	// インパクト攻撃の開始確率
constexpr int SPIN_TIME = 100;		// 回転モーションの時間

using namespace std; // 名前空間stdを使用

//===================================================
// コンストラクタ
//===================================================
CEnemyState::CEnemyState(TYPE id) : m_ID(id)
{
}

//===================================================
// デストラクタ
//===================================================
CEnemyState::~CEnemyState()
{

}

//===================================================
// コンストラクタ
//===================================================
CStateIdle::CStateIdle(int nNextCount) : CEnemyState(TYPE::TYPE_IDLE)
{
	// 次の行動に移行する時間の設定
	m_nNextStateCount = nNextCount;
}

//===================================================
// デストラクタ
//===================================================
CStateIdle::~CStateIdle()
{
}

//===================================================
// 更新処理
//===================================================
void CStateIdle::Update(CEnemy* pEnemy)
{
	// モーションの制御クラスの取得
	CEnemyMotionController* pMotion = pEnemy->GetMotionController();

	// 次の行動に移るまでの時間が0だったら
	if (m_nNextStateCount <= 0)
	{
		// 次の行動を選択
		if (pEnemy->CheckDistane(250.0f))
		{
			pEnemy->SetState(make_unique<CStateAttackSmash>());
		}
		else
		{
			// 状態の設定
			pEnemy->SetState(make_unique<CStateMove>());

			// モーションの設定処理
			pMotion->SetMotion(pMotion->TYPE_MOVE, true, 10);
		}
	}
	else
	{
		// デクリメント
		m_nNextStateCount--;
	}
}

//===================================================
// コンストラクタ(移動)
//===================================================
CStateMove::CStateMove() : CEnemyState(TYPE::TYPE_MOVE)
{
	m_nNextStateCount = NULL;
}

//===================================================
// デストラクタ(移動)
//===================================================
CStateMove::~CStateMove()
{
}

//===================================================
// 更新処理(移動)
//===================================================
void CStateMove::Update(CEnemy* pEnemy)
{
	// モーションの制御クラスの取得
	CEnemyMotionController* pMotion = pEnemy->GetMotionController();

	// 距離が近かったら
	if (pEnemy->CheckDistane(250.0f))
	{
		// 攻撃する
		pEnemy->SetState(make_unique<CStateAttackSmash>());
	}

	m_nNextStateCount++;

	bool bAction = false;

	if (m_nNextStateCount >= NEXT_STAE_TIME)
	{
		// ランダムな数値の生成
		int random = rand() % 100 + 1;
		
		if (random <= START_IMPACT)
		{
			bAction = true;
		}

		m_nNextStateCount = 0;
	}

	// アクションを出せる
	if (bAction)
	{
		// 出す攻撃の設定
		int action = rand() % 3;

		switch (action)
		{
		case 0:
			pEnemy->SetState(make_unique<CStateAttackImpact>());
			break;
		case 1:
			pEnemy->SetState(make_unique<CStateRoar>());
			break;
		default:
			break;
		}
		return;
	}

	// プレイヤーを追いかける処理
	pEnemy->ChasePlayer(0.9f);

	// モーションの設定処理
	pMotion->SetMotion(pMotion->TYPE_MOVE, true, 10);
}

//===================================================
// コンストラクタ
//===================================================
CStateAttackSmash::CStateAttackSmash() : CEnemyState(TYPE::TYPE_SMASH)
{

}

//===================================================
// デストラクタ
//===================================================
CStateAttackSmash::~CStateAttackSmash()
{
}

//===================================================
// 更新処理(攻撃)
//===================================================
void CStateAttackSmash::Update(CEnemy* pEnemy)
{
	// モーションの制御クラスの取得
	CEnemyMotionController* pMotion = pEnemy->GetMotionController();

	// 攻撃モーションの設定
	pMotion->SetMotion(pMotion->TYPE_SMASH, true, 20);

	if (pMotion->IsEventFrame(64, 72, pMotion->TYPE_SMASH))
	{
		// 軌跡の処理
		pEnemy->Orbit(16, D3DXCOLOR(1.0f, 1.0f, 0.5f, 0.8f), 120);
	}
	
	// 攻撃モーションが終わったら
	if (pMotion->IsFinishEndBlend())
	{
		// IDLEにする
		pEnemy->SetState(make_unique<CStateIdle>(50));
	}
}

//===================================================
// コンストラクタ(ダメージ)
//===================================================
CStateDamage::CStateDamage() : CEnemyState(TYPE::TYPE_DAMAGE)
{

}

//===================================================
// デストラクタ(ダメージ)
//===================================================
CStateDamage::~CStateDamage()
{
}

//===================================================
// 更新処理(ダメージ)
//===================================================
void CStateDamage::Update(CEnemy* pEnemy)
{
	// モーションの制御クラスの取得
	CEnemyMotionController* pMotion = pEnemy->GetMotionController();

	// モーションを最後まで行ったら
	if (pMotion->IsFinishEndBlend())
	{
		pEnemy->SetState(make_unique<CStateIdle>(10));
	}
}

//===================================================
// コンストラクタ(衝撃波攻撃)
//===================================================
CStateAttackImpact::CStateAttackImpact() : CEnemyState(TYPE::TYPE_IMPACT)
{

}

//===================================================
// デストラクタ(衝撃波攻撃)
//===================================================
CStateAttackImpact::~CStateAttackImpact()
{
}

//===================================================
// 更新処理(衝撃波攻撃)
//===================================================
void CStateAttackImpact::Update(CEnemy* pEnemy)
{
	// モーションの制御クラスの取得
	CEnemyMotionController* pMotion = pEnemy->GetMotionController();

	// 攻撃モーションの設定
	pMotion->SetMotion(pMotion->TYPE_IMPACT, true, 10);

	if (pMotion->IsEventFrame(93, 116, pMotion->TYPE_IMPACT))
	{
		// 軌跡の処理
		pEnemy->Orbit(16, D3DXCOLOR(1.0f, 1.0f, 0.5f, 0.5f), 120);
	}

	// 攻撃モーションが終わったら
	if (pMotion->IsFinishEndBlend())
	{
		// IDLEにする
		pEnemy->SetState(make_unique<CStateIdle>(50));
	}
}

//===================================================
// コンストラクタ(咆哮)
//===================================================
CStateRoar::CStateRoar() : CEnemyState(TYPE::TYPE_ROAR)
{

}

//===================================================
// デストラクタ(咆哮)
//===================================================
CStateRoar::~CStateRoar()
{
}

//===================================================
// 更新処理(咆哮)
//===================================================
void CStateRoar::Update(CEnemy* pEnemy)
{
	// モーションの制御クラスの取得
	CEnemyMotionController* pMotion = pEnemy->GetMotionController();

	// 攻撃モーションの設定
	pMotion->SetMotion(pMotion->TYPE_ROAR, true, 10);

	// 攻撃モーションが終わったら
	if (pMotion->IsFinishMotion())
	{
		// Dashにする
		pEnemy->SetState(make_unique<CStateDash>());
	}

}

//===================================================
// コンストラクタ(ダッシュ)
//===================================================
CStateDash::CStateDash() : CEnemyState(TYPE::TYPE_DASH)
{
}

//===================================================
// デストラクタ(ダッシュ)
//===================================================
CStateDash::~CStateDash()
{
}

//===================================================
// 更新処理(ダッシュ)
//===================================================
void CStateDash::Update(CEnemy* pEnemy)
{
	// モーションの制御クラスの取得
	CEnemyMotionController* pMotion = pEnemy->GetMotionController();

	// ダッシュ
	pMotion->SetMotion(pMotion->TYPE_DASH, true, 10);

	// プレイヤーを追いかける
	pEnemy->ChasePlayer(0.1f,5.0f);

	// 一定の距離に入ったら
	if (pEnemy->CheckDistane(250.0f))
	{
		// 回転攻撃
		pEnemy->SetState(make_unique<CStateSpin>(SPIN_TIME));
	}
}

//===================================================
// コンストラクタ(回転攻撃)
//===================================================
CStateSpin::CStateSpin(const int nTime) : CEnemyState(TYPE::TYPE_SPIN)
{
	m_nTime = nTime;
}

//===================================================
// デストラクタ(回転攻撃)
//===================================================
CStateSpin::~CStateSpin()
{
}

//===================================================
// 更新処理(回転攻撃)
//===================================================
void CStateSpin::Update(CEnemy* pEnemy)
{
	// モーションの制御クラスの取得
	CEnemyMotionController* pMotion = pEnemy->GetMotionController();

	// 向いている方向に移動する
	pEnemy->MoveForWard(15.0f);

	pMotion->SetMotion(pMotion->TYPE_SPIN, true, 10);

	if (pMotion->IsEventFrame(1, 116, pMotion->TYPE_SPIN))
	{
		// 軌跡の処理
		pEnemy->Orbit(16, D3DXCOLOR(1.0f, 1.0f, 0.5f, 0.5f), 120);
	}

	m_nTime--;

	// モーションが終わったら
	if (m_nTime <= 0 && pMotion->IsFinishMotion())
	{
		// 状態をIdleにする
		pEnemy->SetState(make_unique<CStateIdle>(60));

		// モーションの設定
		pMotion->SetMotion(pMotion->TYPE_NEUTRAL, true, 60);
	}
}
