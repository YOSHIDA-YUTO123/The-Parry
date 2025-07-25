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
#include"slow.h"
#include "game.h"
#include"particle.h"
#include"impact.h"
#include "Wave.h"
#include"camera.h"
#include "MoveSmoke.h"

//***************************************************
// 名前空間
//***************************************************
using namespace std;  // 名前空間stdを使用
using namespace math; // 名前空間mathを使用
using namespace Const; // 名前空間Constを使用
using MOTION = CEnemy::MOTION;

//***************************************************
// 定数宣言
//***************************************************
constexpr int NEXT_STAE_TIME = 120;				// 次の行動に移るまでの時間
constexpr int START_IMPACT = 55;				// インパクト攻撃の開始確率
constexpr int SPIN_TIME = 60;					// 回転モーションの時間
constexpr int ABSSPIN_TIME = 30;				// 絶対回転する時間
constexpr float JUMPATTACK_MOVE_FRAME = 25.0f;	// ジャンプ攻撃の移動フレーム
constexpr int INIT_NEXT_ACTION = 999;			// 絶対に被らない数値
constexpr int MAX_AWAYPOS_X = 1300;				// 最大の離れる位置X
constexpr int MAX_AWAYPOS_Z = 1300;				// 最大の離れる位置Z
constexpr float AWAY_TIME = 24.0f;				// ジャンプする時間

//===================================================
// コンストラクタ
//===================================================
CEnemyState::CEnemyState(ID Id)
{
	m_ID = Id;
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
CEnemyIdle::CEnemyIdle(int nNextCount) : CEnemyState(ID_IDLE)
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
// 初期化処理
//===================================================
void CEnemyIdle::Init(void)
{	
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	if (pMotion != nullptr)
	{
		// モーションの設定処理
		pMotion->SetMotion(MOTION::MOTION_NEUTRAL, true, 10);
	}
}

//===================================================
// 更新処理
//===================================================
void CEnemyIdle::Update(void)
{
	// 軌跡の破棄
	m_pEnemy->DeleteOrbit();

	// 次の行動に移るまでの時間が0だったら
	if (m_nNextStateCount <= 0)
	{
		// 次の行動を選択
		if (m_pEnemy->CheckDistane(250.0f))
		{
			// ランダムな値の選出
			int random = rand() % 2;

			switch (random)
			{
			case 0:
				m_pEnemy->ChangeState(make_shared<CEnemySwing>());
				break;
			case 1:
				m_pEnemy->ChangeState(make_shared<CEnemyAttackSmash>());
				break;
			default:
				break;
			}

			return;
		}
		else
		{
			// 状態の設定
			m_pEnemy->ChangeState(make_shared<CEnemyMove>());
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
CEnemyMove::CEnemyMove() : CEnemyState(ID_MOVE)
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

	// 移動時の煙
	m_pEnemy->MoveSmoke();

	// 距離が近かったら
	if (m_pEnemy->CheckDistane(250.0f))
	{
		// ランダムな値の選出
		int random = rand() % 2;

		switch (random)
		{
		case 0:
			m_pEnemy->ChangeState(make_shared<CEnemySwing>());
			break;
		case 1:
			m_pEnemy->ChangeState(make_shared<CEnemyAttackSmash>());
			break;
		default:
			break;
		}

		return;
	}

	// 次の行動までのカウンターを加算
	m_nNextStateCount++;

	bool bAction = false;

	// 次の行動に移れるなら
	if (m_nNextStateCount >= NEXT_STAE_TIME)
	{		
		// もう一度抽選
		int random = rand() % 100 + 1;

		m_nNextStateCount = 0;

		if (random <= START_IMPACT)
		{
			bAction = true;
		}
		// プレイヤーから遠いいなら
		else if (random <= 90 && !m_pEnemy->CheckDistane(550.0f))
		{
			m_pEnemy->ChangeState(make_shared<CEnemyStep>());
			return;
		}
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
		case 2:
			m_pEnemy->ChangeState(make_shared<CEnemyJumpAttack>());
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
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	// 剣の軌跡のリセット
	m_pEnemy->DeleteOrbit();

	// 次の行動を選出
	m_nNextAction = rand() % 100;

	if (pMotion != nullptr)
	{
		// 攻撃モーションの設定
		pMotion->SetMotion(MOTION::MOTION_SMASH, true, 20);
	}
}

//===================================================
// 更新処理(攻撃)
//===================================================
void CEnemyAttackSmash::Update(void)
{
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	// プレイヤーの取得
	CPlayerGame* pPlayer = CGame::GetPlayer();

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
	if (pMotion->IsEventFrame(64, 71, MOTION::MOTION_SMASH) && m_pEnemy->IsDamageMotion() == false)
	{
		// 攻撃の結果を取得
		CEnemy::RESULT result = m_pEnemy->AttackResult(pPlayer);

		// パリィされた
		if (result == CEnemy::RESULT_PARRY)
		{
			// 構えの設定
			pPlayer->SetStance();

			// プレイヤーの位置の取得
			D3DXVECTOR3 playerPos = pPlayer->GetPos();

			// 角度を求める
			float fAngle = GetTargetAngle(playerPos, pos);

			// 角度を設定
			pPlayer->SetAngle(fAngle);

			// 成功度
			int nSuccess = pPlayer->SuccessParry();

			// 右手の位置
			D3DXVECTOR3 playerHandR = pPlayer->GetModelPos(8);

			// パーティクルの生成
			auto pParticle = CParticle3DNormal::Create(playerHandR, 25.0f, D3DXCOLOR(1.0f,0.4f,0.4f, 1.0f));

			// パーティクルの設定処理
			pParticle->SetParticle(15.0f, 120, 150, 1);
			pParticle->SetParticle(CEffect3D::TYPE_HIT);

			// ヒットストップ
			m_pEnemy->SetHitStop(25);

			// ヒットストップ
			pPlayer->SetHitStop(25);

			// 成功度の設定
			m_pEnemy->SetSuccess(nSuccess);

			// ヒット状態にする
			m_pEnemy->ChangeState(make_shared<CEnemyHit>());
		}
		//// 回避だったら
		//else if (m_pEnemy->CollisionWepon() && pPlayerMotion->GetBlendType() == pPlayer->TYPE_AVOID)
		//{
		//	CSlow *pSlow = CManager::GetSlow();

		//	pSlow->Start(60,4);
		//}
		// 攻撃があたった
		else if (result == CEnemy::RESULT_HIT)
		{
			// 吹き飛び処理
			pPlayer->BlowOff(pos, 10.0f, 10.0f);

			// プレイヤーのモーションの設定
			pPlayerMotion->SetMotion(pPlayer->TYPE_DAMAGE, true, 2);

			// プレイヤー状態の変更
			pPlayer->ChangeState(make_shared<CPlayerDamage>(2));
		}
	}
	
	if (pMotion->IsEventFrame(1, 54, MOTION::MOTION_SMASH))
	{
		// プレイヤーの方向を見る処理
		m_pEnemy->AngleToPlayer();
	}

	if (pMotion->IsEventFrame(64, 72, MOTION::MOTION_SMASH))
	{
		// 軌跡の処理
		m_pEnemy->Orbit(16, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f));
	}

	// モーションが終わったら
	if (pMotion->FinishMotion())
	{
		// 20%の確率
		if (m_nNextAction <= 20)
		{
			// バックステップする
			m_pEnemy->ChangeState(make_shared<CEnemyBackStep>());
		}
	}
	// 攻撃モーションが終わったら
	if (pMotion->IsFinishEndBlend())
	{
		// IDLEにする
		m_pEnemy->ChangeState(make_shared<CEnemyIdle>(10));

		return;
	}
}

//===================================================
// コンストラクタ(大ダメージ)
//===================================================
CEnemyDamageL::CEnemyDamageL(const int nDamage,const bool bBackStatp) : CEnemyState(ID_DAMAGEL)
{
	m_nDamage = nDamage;
	m_bBackStap = bBackStatp;
}

//===================================================
// デストラクタ(大ダメージ)
//===================================================
CEnemyDamageL::~CEnemyDamageL()
{
}

//===================================================
// 初期化処理(大ダメージ)
//===================================================
void CEnemyDamageL::Init(void)
{
	// プレイヤーの取得
	CPlayerGame* pPlayer = CGame::GetPlayer();

	// プレイヤーの位置の取得
	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();

	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	// ダメージの設定
	m_pEnemy->Hit(m_nDamage);

	if (pMotion != nullptr)
	{
		// 大ダメージモーションの設定
		pMotion->SetMotion(MOTION::MOTION_DAMAGEL, true, 2);
	}

	// 移動制御処理の取得
	CEnemyMovement *pMovement = m_pEnemy->GetMovement();

	if (pMovement != nullptr && pPlayer != nullptr)
	{
		// 吹き飛び処理
		pMovement->BlowOff(PlayerPos, 250.0f, 5.0f);
	}
}

//===================================================
// 更新処理(ダメージ大)
//===================================================
void CEnemyDamageL::Update(void)
{
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	if (pMotion->IsEventFrame(1, 10, MOTION::MOTION_DAMAGEL))
	{
		// プレイヤーの取得
		CPlayer* pPlayer = CGame::GetPlayer();

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
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	// 剣の軌跡のリセット
	m_pEnemy->DeleteOrbit();

	if (pMotion != nullptr)
	{
		// 攻撃モーションの設定
		pMotion->SetMotion(MOTION::MOTION_IMPACT, true, 10);
	}
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
		m_pEnemy->ChangeState(make_shared<CEnemyIdle>(10));

		return;
	}
}

//===================================================
// コンストラクタ(咆哮)
//===================================================
CEnemyRoar::CEnemyRoar() : CEnemyState(ID_ROAR)
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
CEnemyDash::CEnemyDash() : CEnemyState(ID_DASH)
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
	m_pEnemy->ChasePlayer(0.1f,3.0f);

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
CEnemySpin::CEnemySpin(const int nTime) : CEnemyState(ID_SPIN)
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
// 初期化処理(回転攻撃)
//===================================================
void CEnemySpin::Init(void)
{	
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	// 剣の軌跡のリセット
	m_pEnemy->DeleteOrbit();

	if (pMotion != nullptr)
	{
		pMotion->SetMotion(MOTION::MOTION_SPIN, true, 10);
	}
}

//===================================================
// 更新処理(回転攻撃)
//===================================================
void CEnemySpin::Update(void)
{
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();
	CPlayerGame* pPlayer = CGame::GetPlayer();
	CMotion* pPlayerMotion = pPlayer->GetMotion();

	// 向いている方向に移動する
	m_pEnemy->GetMovement()->MoveForWard(15.0f);


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
		// 攻撃の結果を取得
		CEnemy::RESULT result = m_pEnemy->AttackResult(pPlayer);

		// パリィされた
		if (result == CEnemy::RESULT_PARRY)
		{
			// 構えの設定
			pPlayer->SetStance();

			// プレイヤーの位置の取得
			D3DXVECTOR3 playerPos = pPlayer->GetPos();

			// 角度を求める
			float fAngle = GetTargetAngle(playerPos, pos);

			// 角度を設定
			pPlayer->SetAngle(fAngle);

			int nSuccess = pPlayer->SuccessParry();

			// 右手の位置
			D3DXVECTOR3 playerHandR = pPlayer->GetModelPos(8);

			// パーティクルの生成
			auto pParticle = CParticle3DNormal::Create(playerHandR, 25.0f, D3DXCOLOR(1.0f, 0.4f, 0.4f, 1.0f));

			// パーティクルの設定処理
			pParticle->SetParticle(15.0f, 120, 150, 1);
			pParticle->SetParticle(CEffect3D::TYPE_HIT);

			// ヒットストップ
			m_pEnemy->SetHitStop(25);

			// ヒットストップ
			pPlayer->SetHitStop(25);

			// 成功度の設定
			m_pEnemy->SetSuccess(nSuccess);

			// ヒット状態にする
			m_pEnemy->ChangeState(make_shared<CEnemyHit>());
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
			// 吹き飛び処理
			pPlayer->BlowOff(pos, 50.0f, 10.0f);

			// プレイヤー状態の変更
			pPlayer->ChangeState(make_shared<CPlayerDamage>(1));
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
CEnemyBackStep::CEnemyBackStep() : CEnemyState(ID_BACKSTEP)
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
CEnemyLanding::CEnemyLanding() : CEnemyState(ID_LANDING)
{
	m_nNextAction = INIT_NEXT_ACTION;
}

//===================================================
// デストラクタ(着地)
//===================================================
CEnemyLanding::~CEnemyLanding()
{
}

//===================================================
// 初期化処理(着地)
//===================================================
void CEnemyLanding::Init(void)
{
	// ランダムな値
	m_nNextAction = rand() % 100;
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

	if (pMotion != nullptr)
	{
		// モーションが終わった後40%の確率で攻撃に移る
		if (pMotion->FinishMotion() && m_nNextAction <= 50)
		{
			// 次の行動を選出
			int random = rand() % 3;

			// 次の行動の遷移
			switch (random)
			{
			case 0:
				m_pEnemy->ChangeState(make_shared<CEnemyJumpAttack>());
				break;
			case 1:
				m_pEnemy->ChangeState(make_shared<CEnemyAttackImpact>());
				break;
			case 2:
				m_pEnemy->ChangeState(make_shared<CEnemyRoar>());
				break;
			default:
				break;
			}
			return;
		}

		// モーションの終わりのブレンドが終わったら
		if (pMotion->IsFinishEndBlend())
		{
			// 状態をIdleに戻す
			m_pEnemy->ChangeState(make_shared<CEnemyIdle>(1));
		}
	}
}

//===================================================
// コンストラクタ(ヒット)
//===================================================
CEnemyHit::CEnemyHit() : CEnemyState(ID_HIT)
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

	if (pMotion != nullptr)
	{
		// モーションの設定
		pMotion->SetMotion(m_pEnemy->MOTION_HIT, true, 10);
	}
}

//===================================================
// 更新処理(ヒット)
//===================================================
void CEnemyHit::Update(void)
{
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	if (pMotion != nullptr && pMotion->FinishMotion())
	{
		m_pEnemy->ChangeState(make_shared<CEnemyBackStep>());
	}
}

//===================================================
// コンストラクタ(ダメージ小)
//===================================================
CEnemyDamageS::CEnemyDamageS(const int nDamage) : CEnemyState(ID_DAMAGES)
{
	m_nNextAction = INIT_NEXT_ACTION;
	m_nDamage = nDamage;
}

//===================================================
// デストラクタ(ダメージ小)
//===================================================
CEnemyDamageS::~CEnemyDamageS()
{
}

//===================================================
// 初期化処理(ダメージ小)
//===================================================
void CEnemyDamageS::Init(void)
{
	// プレイヤーの取得
	CPlayerGame* pPlayer = CGame::GetPlayer();

	// プレイヤーの位置の取得
	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();

	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	// 移動制御処理の取得
	CEnemyMovement* pMovement = m_pEnemy->GetMovement();

	// 次の行動を抽選
	m_nNextAction = rand() % 100;

	// ダメージの設定
	m_pEnemy->Hit(m_nDamage);

	if (pMovement != nullptr && pPlayer != nullptr)
	{
		// 吹き飛び処理
		pMovement->BlowOff(PlayerPos, 100.0f, 5.0f);
	}

	if (pMotion != nullptr)
	{
		// ダメージモーションにする
		pMotion->SetMotion(MOTION::MOTION_DAMAGES, true, 2);
	}
}

//===================================================
// 更新処理(ダメージ小)
//===================================================
void CEnemyDamageS::Update(void)
{
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	// モーションが終わったら
	if (pMotion != nullptr && pMotion->FinishMotion())
	{
		// 30%の確率
		if (m_nNextAction <= 30)
		{
			// 距離を取る
			m_pEnemy->ChangeState(make_shared<CEnemyAway>());
		}
		else
		{
			// 通常状態に戻す
			m_pEnemy->ChangeState(make_shared<CEnemyIdle>(5));
		}

	}
}

//===================================================
// コンストラクタ(ガード)
//===================================================
CEnemyGuard::CEnemyGuard(const D3DXVECTOR3 ImpactPos, const int nDamage) : CEnemyState(ID_GUARD)
{
	m_nDamage = nDamage;
	m_ImpactPos = ImpactPos;
	m_nNextAction = INIT_NEXT_ACTION;
}

//===================================================
// デストラクタ(ガード)
//===================================================
CEnemyGuard::~CEnemyGuard()
{
}

//===================================================
// 初期化処理(ガード)
//===================================================
void CEnemyGuard::Init(void)
{
	// ダメージの設定
	m_pEnemy->Hit(m_nDamage);

	// プレイヤーの取得
	CPlayerGame* pPlayer = CGame::GetPlayer();

	// 位置の取得
	D3DXVECTOR3 pos = m_pEnemy->GetPosition();

	// プレイヤーの位置の取得
	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();

	// パーティクルの生成
	auto pParticle = CParticle3DNormal::Create(m_ImpactPos, 10.0f, D3DXCOLOR(1.0f, 0.4f, 0.4f, 1.0f));

	// パーティクルの設定処理
	pParticle->SetParticle(15.0f, 240, 50, 5);

	// ボスまでの角度を取得
	float fAngle = GetTargetAngle(pos, PlayerPos);

	// 吹き飛ばす
	m_pEnemy->GetMovement()->BlowOff(PlayerPos,100.0f, 0.0f);

	// 向きの設定
	pPlayer->SetAngle(fAngle + D3DX_PI);

	// インパクトを生成
	auto pCircle = CMeshCircle::Create(D3DXCOLOR(1.0f, 0.4f, 0.4f, 0.8f), m_ImpactPos, 0.0f, 120.0f);

	// サークルの設定処理
	pCircle->SetCircle(35.0f, 15.0f, 120, false, D3DXVECTOR3(D3DX_PI * 0.5f, fAngle, 0.0f));

	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(MOTION::MOTION_GUARD, true, 2);
	}

	// 次の行動を抽選
	m_nNextAction = rand() % 100;
}

//===================================================
// 更新処理(ガード)
//===================================================
void CEnemyGuard::Update(void)
{
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	if (pMotion->FinishMotion())
	{
		if (m_nNextAction <= 50)
		{
			// バックステップする
			m_pEnemy->ChangeState(make_shared<CEnemyBackStep>());
		}
		else
		{
			// 振り下ろし攻撃に派生
			m_pEnemy->ChangeState(make_shared<CEnemyAttackSmash>());
		}
	}
}

//===================================================
// コンストラクタ(ステップ)
//===================================================
CEnemyStep::CEnemyStep() : CEnemyState(ID_STEP)
{
}

//===================================================
// デストラクタ(ステップ)
//===================================================
CEnemyStep::~CEnemyStep()
{
}

//===================================================
// 初期化処理(ステップ)
//===================================================
void CEnemyStep::Init(void)
{
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	m_pEnemy->GetMovement()->MoveForWard(150.0f);

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(MOTION::MOTION_STEP, true, 4);
	}
}

//===================================================
// 更新処理(ステップ)
//===================================================
void CEnemyStep::Update(void)
{
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションが終わったら
		if (pMotion->IsEndMotion())
		{
			// 通常状態に戻す
			m_pEnemy->ChangeState(make_shared<CEnemyMove>());
		}
	}

}

//===================================================
// コンストラクタ(スイング攻撃)
//===================================================
CEnemySwing::CEnemySwing() : CEnemyState(ID_SWING)
{
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
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	// 軌跡のリセット
	m_pEnemy->DeleteOrbit();

	// 次の行動を抽選
	m_nNextAction = rand() % 100;

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(MOTION::MOTION_SWING, true, 10);
	}
}

//===================================================
// 更新処理(スイング攻撃)
//===================================================
void CEnemySwing::Update(void)
{
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	// プレイヤーの取得
	CPlayerGame* pPlayer = CGame::GetPlayer();

	// プレイヤーの取得
	CMotion* pPlayerMotion = pPlayer->GetMotion();

	// 位置の取得
	D3DXVECTOR3 pos = m_pEnemy->GetPosition();
	
	if (pMotion->IsEventFrame(0, 60, MOTION::MOTION_SWING))
	{
		// 軌跡の処理
		m_pEnemy->Orbit(16, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f));
	}

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// イベントフレームの判定
		if (pMotion->IsEventFrame(40, 60, MOTION::MOTION_SWING) && m_pEnemy->IsDamageMotion() == false)
		{
			// 攻撃の結果を取得
			CEnemy::RESULT result = m_pEnemy->AttackResult(pPlayer);

			// パリィされた
			if (result == CEnemy::RESULT_PARRY)
			{
				pPlayer->SetStance();

				// プレイヤーの位置の取得
				D3DXVECTOR3 playerPos = pPlayer->GetPos();

				// 角度を求める
				float fAngle = GetTargetAngle(playerPos, pos);

				// 角度を設定
				pPlayer->SetAngle(fAngle);

				int nSuccess = pPlayer->SuccessParry();

				// 右手の位置
				D3DXVECTOR3 playerHandR = pPlayer->GetModelPos(8);

				// パーティクルの生成
				auto pParticle = CParticle3DNormal::Create(playerHandR, 25.0f, D3DXCOLOR(1.0f, 0.4f, 0.4f, 1.0f));

				// パーティクルの設定処理
				pParticle->SetParticle(15.0f, 120, 150, 1);
				pParticle->SetParticle(CEffect3D::TYPE_HIT);

				m_pEnemy->SetHitStop(25);

				pPlayer->SetHitStop(25);

				// 成功度の設定
				m_pEnemy->SetSuccess(nSuccess);

				// ヒット状態にする
				m_pEnemy->ChangeState(make_shared<CEnemyHit>());
			}
			// 回避だったら
			else if (result == CEnemy::RESULT_AVOID)
			{
				CSlow *pSlow = CManager::GetSlow();

				pSlow->Start(60,4);
			}
			// 範囲内で視界に入っていない、カウンターしていない
			else if (result == CEnemy::RESULT_HIT)
			{
				// 吹き飛び処理
				pPlayer->BlowOff(pos, 10.0f, 10.0f);

				// プレイヤーのモーションの設定
				pPlayerMotion->SetMotion(pPlayer->TYPE_DAMAGE, true, 2);

				// プレイヤー状態の変更
				pPlayer->ChangeState(make_shared<CPlayerDamage>(2));
			}
		}

		if (pMotion->IsEventFrame(0, 20, MOTION::MOTION_SWING))
		{
			// プレイヤーの方向を見る処理
			m_pEnemy->AngleToPlayer();
		}

		// モーションが終わったら
		if (pMotion->FinishMotion())
		{
			// 40%の確率
			if (m_nNextAction <= 20)
			{
				// バックステップする
				m_pEnemy->ChangeState(make_shared<CEnemyBackStep>());

				return;
			}
		}

		// モーションのブレンドが終わったら
		if (pMotion->IsFinishEndBlend())
		{
			// 剣の軌跡の消去
			m_pEnemy->DeleteOrbit();

			// 状態の変更
			m_pEnemy->ChangeState(make_shared<CEnemyIdle>(5));
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
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	// 軌跡のリセット
	m_pEnemy->DeleteOrbit();

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(MOTION::MOTION_JUMPATTACK, true, 10);
	}
}

//===================================================
// 更新処理(ジャンプ攻撃)
//===================================================
void CEnemyJumpAttack::Update(void)
{
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	// 位置の取得
	D3DXVECTOR3 pos = m_pEnemy->GetPosition();

	// プレイヤーの取得
	auto pPlayer = CGame::GetPlayer();

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// 構え中だったら
		if (pMotion->IsEventFrame(20, 20, MOTION::MOTION_JUMPATTACK))
		{
			// ウェーブの生成
			auto pWave = CMeshWave::Create(pos, 50.0f, 50.0f, D3DXCOLOR(1.0f,0.4f,0.4f,1.0));

			// ウェーブの設定処理
			pWave->SetWave(30, 50.0f);
		}

		// 構え中だったら
		if (pMotion->IsEventFrame(1, 30, MOTION::MOTION_JUMPATTACK))
		{
			// プレイヤーの方向を見る
			m_pEnemy->AngleToPlayer();
		}

		// 40フレーム目になったら
		if (pMotion->IsEventFrame(40, 40, MOTION::MOTION_JUMPATTACK))
		{
			// ジャンプする
			m_pEnemy->GetMovement()->Jump(24.0f);
		}

		// ジャンプ中だったら
		if (pMotion->IsEventFrame(40, 90, MOTION::MOTION_JUMPATTACK))
		{
			// 軌跡の設定
			m_pEnemy->Orbit(16, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f));

			// プレイヤーまでの差分を求める
			D3DXVECTOR3 Diff = pPlayer->GetPos() - pos;

			// 距離を求める
			float dir = GetDistance(Diff);

			// ジャンプ攻撃中の移動
			m_pEnemy->GetMovement()->MoveForWard(dir / JUMPATTACK_MOVE_FRAME);
		}

		// たたきつけになったら
		if (pMotion->IsEventFrame(90, 90, MOTION::MOTION_JUMPATTACK))
		{
			// 瓦礫の設定
			m_pEnemy->SetRubble();
		}

		// モーションのブレンドが終わったら
		if (pMotion->IsFinishEndBlend())
		{
			m_pEnemy->ChangeState(make_shared<CEnemyIdle>(1));
			return;
		}
	}

	// プレイヤーとの当たり判定
	CollisionPlayer(pPlayer, pMotion);
}
	
//===================================================
// プレイヤーとの当たり判定
//===================================================
void CEnemyJumpAttack::CollisionPlayer(CPlayerGame* pPlayer, CMotion* pMotion)
{
	// 位置の取得
	D3DXVECTOR3 pos = m_pEnemy->GetPosition();

	// イベントフレームの判定
	if (pMotion->IsEventFrame(80, 88, MOTION::MOTION_JUMPATTACK) && m_pEnemy->IsDamageMotion() == false)
	{
		// 攻撃の結果を取得
		CEnemy::RESULT result = m_pEnemy->AttackResult(pPlayer);

		// パリィされた
		if (result == CEnemy::RESULT_PARRY)
		{
			// 構えの設定処理
			pPlayer->SetStance();

			// プレイヤーの位置の取得
			D3DXVECTOR3 playerPos = pPlayer->GetPos();

			// 角度を求める
			float fAngle = GetTargetAngle(playerPos, pos);

			// 角度を設定
			pPlayer->SetAngle(fAngle);

			// 成功度の取得
			int nSuccess = pPlayer->SuccessParry();

			// 右手の位置
			D3DXVECTOR3 playerHandR = pPlayer->GetModelPos(8);

			// パーティクルの生成
			auto pParticle = CParticle3DNormal::Create(playerHandR, 25.0f, D3DXCOLOR(1.0f, 0.4f, 0.4f, 1.0f));

			// パーティクルの設定処理
			pParticle->SetParticle(15.0f, 120, 150, 1);
			pParticle->SetParticle(CEffect3D::TYPE_HIT);

			// ヒットストップ
			m_pEnemy->SetHitStop(25);

			// ヒットストップ
			pPlayer->SetHitStop(25);

			// 成功度の設定
			m_pEnemy->SetSuccess(nSuccess);

			// ヒット状態にする
			m_pEnemy->ChangeState(make_shared<CEnemyHit>());
		}
		// 回避だったら
		else if (result == CEnemy::RESULT_AVOID)
		{
			CSlow *pSlow = CManager::GetSlow();

			pSlow->Start(60,4);
		}
		// 攻撃があたった
		else if (result == CEnemy::RESULT_HIT)
		{
			// 吹き飛び処理
			pPlayer->BlowOff(pos, 100.0f, 10.0f);

			// プレイヤー状態の変更
			pPlayer->ChangeState(make_shared<CPlayerDamage>(2));
		}
	}
}

//===================================================
// コンストラクタ(死亡)
//===================================================
CEnemyDeath::CEnemyDeath() : CEnemyState(ID_DEATH)
{
}

//===================================================
// デストラクタ(死亡)
//===================================================
CEnemyDeath::~CEnemyDeath()
{
}

//===================================================
// 初期化処理(死亡)
//===================================================
void CEnemyDeath::Init(void)
{
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	CSlow* pSlow = CManager::GetSlow();

	pSlow->Start(60, 8);

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(MOTION::MOTION_DEATH, true, 1);
	}
}

//===================================================
// 更新処理(死亡)
//===================================================
void CEnemyDeath::Update(void)
{
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// 吹き飛び中だったら
		if (pMotion->IsEventFrame(1, 90, MOTION::MOTION_DEATH))
		{
			// 移動方向を設定
			m_pEnemy->GetMovement()->SetMoveDir(0.0f, 20.0f);
		}

		if (pMotion->IsEventFrame(110, 110, MOTION::MOTION_DEATH))
		{
			m_pEnemy->ChangeState(make_shared<CEnemyDown>());
		}
	}
}

//===================================================
// コンストラクタ(ダウン)
//===================================================
CEnemyDown::CEnemyDown() : CEnemyState(ID_DOWN)
{
	m_nTrackingTime = 120;
}

//===================================================
// デストラクタ(ダウン)
//===================================================
CEnemyDown::~CEnemyDown()
{
}

//===================================================
// 初期化処理(ダウン)
//===================================================
void CEnemyDown::Init(void)
{
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(MOTION::MOTION_DOWN, true, 4);
	}
}

//===================================================
// 更新処理(ダウン)
//===================================================
void CEnemyDown::Update(void)
{
	// 追従時間
	m_nTrackingTime--;

	// 追従時間が終わったら
	if (m_nTrackingTime <= 0)
	{
		// カメラの取得
		CCamera* pCamera = CManager::GetCamera();

		// プレイヤーに戻す
		pCamera->SetTracking(CCamera::TRACKOBJ_PLAYER);
	}
}

//===================================================
// コンストラクタ(距離を取る状態)
//===================================================
CEnemyAway::CEnemyAway() : CEnemyState(ID_AWAY)
{
	m_pos = VEC3_NULL;
}

//===================================================
// デストラクタ(距離を取る状態)
//===================================================
CEnemyAway::~CEnemyAway()
{
}

//===================================================
// 初期化処理(距離を取る状態)
//===================================================
void CEnemyAway::Init(void)
{
	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	int nPosXMax = MAX_AWAYPOS_X * 2;
	float fPosXMin = static_cast<float>(MAX_AWAYPOS_X);

	int nPosZMax = MAX_AWAYPOS_Z * 2;
	float fPosZMin = static_cast<float>(MAX_AWAYPOS_Z);

	m_pos.x = static_cast<float>(rand() % nPosXMax - fPosXMin);
	m_pos.z = static_cast<float>(rand() % nPosZMax - fPosZMin);

	// 位置の取得
	D3DXVECTOR3 pos = m_pEnemy->GetPosition();

	// 距離を求める
	float fDistance = GetDistance(m_pos - pos);

	// 目標地点までの距離が1000以下だったら
	if (fDistance <= 1000.0f)
	{
		// ジャンプ攻撃に派生
		m_pEnemy->ChangeState(make_shared<CEnemyJumpAttack>());
		return;
	}

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(MOTION::MOTION_JUMP, true, 4);
	}

	// 向きを求める
	float fAngle = GetTargetAngle(m_pos, pos);

	// 向きの設定
	m_pEnemy->SetAngle(fAngle + D3DX_PI);

	// ジャンプする
	m_pEnemy->GetMovement()->Jump(25.0f);
}

//===================================================
// 更新処理(距離を取る状態)
//===================================================
void CEnemyAway::Update(void)
{
	// 位置の取得
	D3DXVECTOR3 pos = m_pEnemy->GetPosition();

	// 距離を求める
	float fDistance = GetDistance(m_pos - pos);

	// 距離を到達時間で割る
	float dir = fDistance / AWAY_TIME;

	// 移動量を設定する
	m_pEnemy->GetMovement()->MoveForWard(dir);
}
