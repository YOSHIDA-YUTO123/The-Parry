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
#include"motion.h"

constexpr int NEXT_STAE_TIME = 180; // 次の行動に移るまでの時間
constexpr int START_IMPACT = 55;	// インパクト攻撃の開始確率
constexpr int SPIN_TIME = 100;		// 回転モーションの時間

using namespace std; // 名前空間stdを使用
using MOTION = CEnemy::MOTIONTYPE;

//===================================================
// コンストラクタ
//===================================================
CEnemyState::CEnemyState()
{
	m_pEnemy = nullptr;
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
CEnemyIdle::CEnemyIdle(int nNextCount)
{
	// 次の行動に移行する時間の設定
	m_nNextStateCount = nNextCount;
}

//===================================================
// デストラクタ
//===================================================
CEnemyIdle::~CEnemyIdle()
{
}

//===================================================
// 更新処理
//===================================================
void CEnemyIdle::Update(void)
{
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	m_pEnemy->DeleteOrbit();

	// 次の行動に移るまでの時間が0だったら
	if (m_nNextStateCount <= 0)
	{
		// 次の行動を選択
		if (m_pEnemy->CheckDistane(250.0f))
		{
			m_pEnemy->ChangeState(make_shared<CEnemyAttackSmash>());

			return;
		}
		else
		{
			// 状態の設定
			m_pEnemy->ChangeState(make_shared<CEnemyMove>());

			// モーションの設定処理
			pMotion->SetMotion(MOTION::MOTIONTYPE_NEUTRAL, true, 10);
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
CEnemyMove::CEnemyMove()
{
	m_nNextStateCount = NULL;
}

//===================================================
// デストラクタ(移動)
//===================================================
CEnemyMove::~CEnemyMove()
{
}

//===================================================
// 更新処理(移動)
//===================================================
void CEnemyMove::Update(void)
{
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	// 距離が近かったら
	if (m_pEnemy->CheckDistane(250.0f))
	{
		// 攻撃する
		m_pEnemy->ChangeState(make_shared<CEnemyAttackSmash>());

		return;
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
			m_pEnemy->ChangeState(make_shared<CEnemyAttackImpact>());
			break;
		case 1:
			m_pEnemy->ChangeState(make_shared<CEnemyRoar>());
			break;
		default:
			break;
		}
		return;
	}

	// プレイヤーを追いかける処理
	m_pEnemy->ChasePlayer(0.9f);

	// モーションの設定処理
	pMotion->SetMotion(MOTION::MOTIONTYPE_MOVE, true, 10);
}

//===================================================
// コンストラクタ
//===================================================
CEnemyAttackSmash::CEnemyAttackSmash()
{
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
void CEnemyAttackSmash::Update(void)
{
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	// 攻撃モーションの設定
	pMotion->SetMotion(MOTION::MOTIONTYPE_SMASH, true, 20);

	if (pMotion->IsEventFrame(1, 64, MOTION::MOTIONTYPE_SMASH))
	{
		// プレイヤーの方向を見る処理
		m_pEnemy->AngleToPlayer();
	}

	if (pMotion->IsEventFrame(64, 72, MOTION::MOTIONTYPE_SMASH))
	{
		// 軌跡の処理
		m_pEnemy->Orbit(16, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f));
	}
	
	// 攻撃モーションが終わったら
	if (pMotion->IsFinishEndBlend())
	{
		// IDLEにする
		m_pEnemy->ChangeState(make_shared<CEnemyIdle>(50));

		return;
	}
}

//===================================================
// コンストラクタ(ダメージ)
//===================================================
CEnemyDamage::CEnemyDamage()
{
}

//===================================================
// デストラクタ(ダメージ)
//===================================================
CEnemyDamage::~CEnemyDamage()
{
}

//===================================================
// 更新処理(ダメージ)
//===================================================
void CEnemyDamage::Update(void)
{
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	// ダメージだったら
	pMotion->SetMotion(MOTION::MOTIONTYPE_DAMAGE, true, 2);

	// モーションを最後まで行ったら
	if (pMotion->IsFinishEndBlend())
	{
		m_pEnemy->ChangeState(make_shared<CEnemyIdle>(10));
	}
}

//===================================================
// コンストラクタ(衝撃波攻撃)
//===================================================
CEnemyAttackImpact::CEnemyAttackImpact()
{

}

//===================================================
// デストラクタ(衝撃波攻撃)
//===================================================
CEnemyAttackImpact::~CEnemyAttackImpact()
{
}

//===================================================
// 更新処理(衝撃波攻撃)
//===================================================
void CEnemyAttackImpact::Update(void)
{
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	// 攻撃モーションの設定
	pMotion->SetMotion(MOTION::MOTIONTYPE_IMPACT, true, 10);

	if (pMotion->IsEventFrame(1, 93, MOTION::MOTIONTYPE_IMPACT))
	{
		// プレイヤーの方向を見る処理
		m_pEnemy->AngleToPlayer();
	}

	if (pMotion->IsEventFrame(93, 116, MOTION::MOTIONTYPE_IMPACT))
	{
		// 軌跡の処理
		m_pEnemy->Orbit(16, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f));
	}

	// 攻撃モーションが終わったら
	if (pMotion->IsFinishEndBlend())
	{
		// IDLEにする
		m_pEnemy->ChangeState(make_shared<CEnemyIdle>(50));

		return;
	}
}

//===================================================
// コンストラクタ(咆哮)
//===================================================
CEnemyRoar::CEnemyRoar()
{

}

//===================================================
// デストラクタ(咆哮)
//===================================================
CEnemyRoar::~CEnemyRoar()
{
}

//===================================================
// 更新処理(咆哮)
//===================================================
void CEnemyRoar::Update(void)
{
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	// 攻撃モーションの設定
	pMotion->SetMotion(MOTION::MOTIONTYPE_ROAR, true, 10);

	// 攻撃モーションが終わったら
	if (pMotion->FinishMotion())
	{
		// Dashにする
		m_pEnemy->ChangeState(make_shared<CEnemyDash>());

		return;
	}

}

//===================================================
// コンストラクタ(ダッシュ)
//===================================================
CEnemyDash::CEnemyDash()
{
}

//===================================================
// デストラクタ(ダッシュ)
//===================================================
CEnemyDash::~CEnemyDash()
{
}

//===================================================
// 更新処理(ダッシュ)
//===================================================
void CEnemyDash::Update(void)
{
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	// ダッシュ
	pMotion->SetMotion(MOTION::MOTIONTYPE_DASH, true, 10);

	// プレイヤーを追いかける
	m_pEnemy->ChasePlayer(0.1f,5.0f);

	// 一定の距離に入ったら
	if (m_pEnemy->CheckDistane(250.0f))
	{
		// 回転攻撃
		m_pEnemy->ChangeState(make_shared<CEnemySpin>(SPIN_TIME));

		return;
	}
}

//===================================================
// コンストラクタ(回転攻撃)
//===================================================
CEnemySpin::CEnemySpin(const int nTime) 
{
	m_nTime = nTime;
}

//===================================================
// デストラクタ(回転攻撃)
//===================================================
CEnemySpin::~CEnemySpin()
{
}

//===================================================
// 更新処理(回転攻撃)
//===================================================
void CEnemySpin::Update(void)
{
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	// 向いている方向に移動する
	m_pEnemy->MoveForWard(15.0f);

	pMotion->SetMotion(MOTION::MOTIONTYPE_SPIN, true, 10);

	if (pMotion->IsEventFrame(1, 116, MOTION::MOTIONTYPE_SPIN))
	{
		// 軌跡の処理
		m_pEnemy->Orbit(16, D3DXCOLOR(1.0f, 1.0f, 0.5f, 0.5f));
	}

	m_nTime--;

	// モーションが終わったら
	if (m_nTime <= 0 && pMotion->FinishMotion())
	{
		// モーションの設定
		pMotion->SetMotion(MOTION::MOTIONTYPE_NEUTRAL, true, 60);

		// 状態をIdleにする
		m_pEnemy->ChangeState(make_shared<CEnemyIdle>(60));

		return;
	}
}
