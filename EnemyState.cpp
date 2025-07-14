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
#include "math.h"
#include"player.h"
#include "manager.h"
#include "playerstate.h"

using namespace std;  // 名前空間stdを使用
using namespace math; // 名前空間sを使用
using MOTION = CEnemy::MOTION;

constexpr int NEXT_STAE_TIME = 180; // 次の行動に移るまでの時間
constexpr int START_IMPACT = 55;	// インパクト攻撃の開始確率
constexpr int SPIN_TIME = 60;		// 回転モーションの時間
constexpr int ABSSPIN_TIME = 30;	// 絶対回転する時間

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

	// 軌跡の破棄
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
			pMotion->SetMotion(MOTION::MOTION_NEUTRAL, true, 10);
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
	pMotion->SetMotion(MOTION::MOTION_MOVE, true, 10);
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
	pMotion->SetMotion(MOTION::MOTION_SMASH, true, 20);

	// プレイヤーの取得
	CPlayer* pPlayer = CManager::GetPlayer();

	// プレイヤーが使われていないなら
	if (pPlayer == nullptr)
	{
		return;
	}

	// プレイヤーの取得
	CMotion* pPlayerMotion = pPlayer->GetMotion();

	// 位置の取得
	D3DXVECTOR3 pos = m_pEnemy->GetPosition();

	// イベントフレームの判定
	if (pMotion->IsEventFrame(64, 71, MOTION::MOTION_SMASH) && pMotion->GetBlendType() != MOTION::MOTION_DAMAGE)
	{
		// プレイヤーの視界の中にいる
		const bool bParry = pPlayer->IsParry(pos);

		// 当たったら
		if (m_pEnemy->CollisionWepon() && bParry)
		{
			// パリィモーションの再生
			pPlayerMotion->SetMotion(pPlayer->TYPE_PARRY, true, 2);

			// ヒット状態にする
			m_pEnemy->ChangeState(make_shared<CEnemyHit>());
		}
		//// 回避だったら
		//else if (m_pEnemy->CollisionWepon() && pPlayerMotion->GetBlendType() == pPlayer->TYPE_AVOID)
		//{
		//	CSlow *pSlow = CManager::GetSlow();

		//	pSlow->Start(60,4);
		//}
		// 範囲内で視界に入っていない、カウンターしていない
		else if (m_pEnemy->CollisionWepon() && bParry == false)
		{
			// 吹き飛び処理
			pPlayer->BlowOff(pos, 10.0f, 10.0f);

			// プレイヤー状態の変更
			pPlayer->ChangeState(make_shared<CPlayerDamage>());
		}
	}

	if (pMotion->IsEventFrame(1, 64, MOTION::MOTION_SMASH))
	{
		// プレイヤーの方向を見る処理
		m_pEnemy->AngleToPlayer();
	}

	if (pMotion->IsEventFrame(64, 72, MOTION::MOTION_SMASH))
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
CEnemyDamage::CEnemyDamage(const bool bBackStatp)
{
	m_bBackStap = bBackStatp;
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
	pMotion->SetMotion(MOTION::MOTION_DAMAGE, true, 2);

	if (pMotion->IsEventFrame(1, 10, MOTION::MOTION_DAMAGE))
	{
		// プレイヤーの取得
		CPlayer* pPlayer = CManager::GetPlayer();

		D3DXVECTOR3 PlayerPos = pPlayer->GetPos();

		// 吹き飛び処理
		m_pEnemy->GetMovement()->BlowOff(PlayerPos, 50.0f, 5.0f);
	}
	// モーションを最後まで行ったら
	if (pMotion->IsFinishEndBlend())
	{
		if (m_bBackStap)
		{
			// バックステップ
			m_pEnemy->ChangeState(make_shared<CEnemyBackStep>());
		}
		else
		{
			m_pEnemy->ChangeState(make_shared<CEnemyIdle>(10));
		}
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
	pMotion->SetMotion(MOTION::MOTION_IMPACT, true, 10);

	if (pMotion->IsEventFrame(1, 93, MOTION::MOTION_IMPACT))
	{
		// プレイヤーの方向を見る処理
		m_pEnemy->AngleToPlayer();
	}

	if (pMotion->IsEventFrame(93, 116, MOTION::MOTION_IMPACT))
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
	pMotion->SetMotion(MOTION::MOTION_ROAR, true, 10);

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
	pMotion->SetMotion(MOTION::MOTION_DASH, true, 10);

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
	m_nMaxTime = nTime;
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
	CPlayer* pPlayer = CManager::GetPlayer();
	CMotion* pPlayerMotion = pPlayer->GetMotion();

	// 向いている方向に移動する
	m_pEnemy->GetMovement()->MoveForWard(15.0f);

	pMotion->SetMotion(MOTION::MOTION_SPIN, true, 10);

	if (pMotion->IsEventFrame(1, 116, MOTION::MOTION_SPIN))
	{
		// 軌跡の処理
		m_pEnemy->Orbit(16, D3DXCOLOR(1.0f, 1.0f, 0.5f, 0.5f));
	}

	m_nTime--;

	// 位置の取得
	D3DXVECTOR3 pos = m_pEnemy->GetPosition();

	// 回転モーション
	if (pMotion->IsEventFrame(0, 999, MOTION::MOTION_SPIN))
	{
		// プレイヤーの視界の中にいる
		const bool bParry = pPlayer->IsParry(pos);

		// 当たったら
		if (m_pEnemy->CollisionWepon() && bParry)
		{
			// パリィモーションの再生
			pPlayerMotion->SetMotion(pPlayer->TYPE_PARRY, true, 2);
		}
		else if (m_pEnemy->CollisionWepon() && pPlayerMotion->GetBlendType() == pPlayer->TYPE_AVOID)
		{
			// スローモーションの取得
			CSlow* pSlow = CManager::GetSlow();

			// スローモーション
			pSlow->Start(60, 12);

			m_nTime = 120;
		}
		// 武器との当たり判定
		else if (m_pEnemy->CollisionWepon() == true && pPlayerMotion->GetBlendType() != pPlayer->TYPE_PARRY)
		{
			// 吹き飛び処理
			pPlayer->BlowOff(pos, 50.0f, 10.0f);

			// プレイヤー状態の変更
			pPlayer->ChangeState(make_shared<CPlayerDamage>());
		}
	}

	// 必ず回転する時間を計算
	int nTime = m_nMaxTime - ABSSPIN_TIME;

	// プレイヤーがダメージを受けて絶対回転する時間が終わったら
	if (pPlayerMotion->GetBlendType() == pPlayer->TYPE_DAMAGE && nTime >= m_nTime)
	{
		if (pMotion->FinishMotion())
		{
			// 状態の設定
			m_pEnemy->ChangeState(make_shared<CEnemyIdle>(60));

			pMotion->SetMotion(MOTION::MOTION_NEUTRAL, true, 20);

			return;
		}
	}

	// モーションが終わったら
	if (m_nTime <= 0 && pMotion->FinishMotion())
	{
		// モーションの設定
		pMotion->SetMotion(MOTION::MOTION_NEUTRAL, true, 60);

		// 状態をIdleにする
		m_pEnemy->ChangeState(make_shared<CEnemyIdle>(60));

		return;
	}
}

//===================================================
// コンストラクタ(バックステップ)
//===================================================
CEnemyBackStep::CEnemyBackStep()
{
}

//===================================================
// デストラクタ(バックステップ)
//===================================================
CEnemyBackStep::~CEnemyBackStep()
{
}

//===================================================
// 初期化処理
//===================================================
void CEnemyBackStep::Init(void)
{
	m_pEnemy->GetMovement()->Jump(24.0f);

	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	// モーションの設定
	pMotion->SetMotion(MOTION::MOTION_JUMP, true, 10);
}

//===================================================
// 更新処理(バックステップ)
//===================================================
void CEnemyBackStep::Update(void)
{

	m_pEnemy->GetMovement()->SetMoveDir(0.0f,20.0f);
}

//===================================================
// コンストラクタ(着地)
//===================================================
CEnemyLanding::CEnemyLanding()
{
}

//===================================================
// デストラクタ(着地)
//===================================================
CEnemyLanding::~CEnemyLanding()
{
}

//===================================================
// 更新処理(着地)
//===================================================
void CEnemyLanding::Update(void)
{
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	// モーションの設定
	pMotion->SetMotion(MOTION::MOTION_LANDING, true, 10);

	if (pMotion->IsFinishEndBlend())
	{
		m_pEnemy->ChangeState(make_shared<CEnemyIdle>(10));
	}
}

//===================================================
// コンストラクタ(ヒット)
//===================================================
CEnemyHit::CEnemyHit()
{
}

//===================================================
// デストラクタ(ヒット)
//===================================================
CEnemyHit::~CEnemyHit()
{
}

//===================================================
// 初期化(ヒット)
//===================================================
void CEnemyHit::Init(void)
{
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	// モーションの設定
	pMotion->SetMotion(m_pEnemy->MOTION_HIT, true, 10);
}

//===================================================
// 更新処理(ヒット)
//===================================================
void CEnemyHit::Update(void)
{
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	if (pMotion->FinishMotion())
	{
		m_pEnemy->ChangeState(make_shared<CEnemyBackStep>());
	}
}
