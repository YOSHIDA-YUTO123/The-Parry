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
#include "enemy.h"
#include "motion.h"
#include "math.h"
#include "player.h"
#include "manager.h"
#include "playerstate.h"
#include "slow.h"
#include "game.h"
#include "particle.h"
#include "impact.h"
#include "Wave.h"
#include "GameCamera.h"
#include "MoveSmoke.h"
#include "result.h"
#include "fade.h"
#include "debugproc.h"
#include "transform.h"
#include "dust.h"
#include "Collider.h"
#include"sound.h"
#include "renderer.h"

//***************************************************
// 名前空間
//***************************************************
using namespace std;  // 名前空間stdを使用
using namespace math; // 名前空間mathを使用
using namespace Const; // 名前空間Constを使用

//***************************************************
// 定数宣言
//***************************************************
namespace
{
	constexpr int NEXT_STAE_TIME = 120;				// 次の行動に移るまでの時間
	constexpr int START_ACTION = 55;				// アクションの開始確率
	constexpr int SPIN_TIME = 60;					// 回転モーションの時間
	constexpr int ABSSPIN_TIME = 30;				// 絶対回転する時間
	constexpr int INIT_NEXT_ACTION = 999;			// 次のアクションに移行する数値(初期化)
	constexpr int MAX_AWAYPOS_X = 1300;				// 最大の離れる位置X
	constexpr int MAX_AWAYPOS_Z = 1300;				// 最大の離れる位置Z
	constexpr float JUMPATTACK_MOVE_FRAME = 20.0f;	// ジャンプ攻撃の移動フレーム
	constexpr float AWAY_TIME = 24.0f;				// ジャンプする時間
};

//===================================================
// コンストラクタ
//===================================================
CEnemyStateManager::CEnemyStateManager()
{
	m_pEnemy = nullptr;
}

//===================================================
// デストラクタ
//===================================================
CEnemyStateManager::~CEnemyStateManager()
{
}

//===================================================
// 生成処理
//===================================================
CEnemyStateManager* CEnemyStateManager::Create(void)
{
	// 自分を生成
	return new CEnemyStateManager;
}

//===================================================
// 振り向きモーションの設定
//===================================================
bool CEnemyStateManager::SetMotionByPlayerPosition(void)
{
	// 取得できなかったら処理しない
	if (m_pEnemy == nullptr) return false;

	// プレイヤーの取得
	CPlayer* pPlayer = CGame::GetPlayer();

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return false;

	// プレイヤーの位置の取得
	D3DXVECTOR3 playerPos = pPlayer->GetPosition();

	// 近くにいたら
	if (m_pEnemy->CheckDistane(400.0f))
	{
		// 後ろにいたら
		if (m_pEnemy->CollisionFOV(playerPos, -D3DX_PI * 0.15f, D3DX_PI * 0.15f))
		{
			m_pEnemy->ChangeState(make_shared<CEnemyBackKick>());
			return true;
		}
		// 右側にいたら
		else if (m_pEnemy->CollisionFOV(playerPos, D3DX_PI * 0.75f, D3DX_PI * 0.15f))
		{
			m_pEnemy->ChangeState(make_shared<CEnemySweepRight>());
			return true;
		}
		// 左側にいたら
		else if (m_pEnemy->CollisionFOV(playerPos, -D3DX_PI * 0.15f, -D3DX_PI * 0.75f))
		{
			m_pEnemy->ChangeState(make_shared<CEnemySweepLeft>());
			return true;
		}
	}
	else
	{
		// 左後ろにいたら
		if (m_pEnemy->CollisionFOV(playerPos, -D3DX_PI * 0.5f, D3DX_PI))
		{
			// 振り返り
			m_pEnemy->ChangeState(make_shared<CEnemyLookBackL>());
			return true;
		}
		// 右後ろにいたら
		else if (m_pEnemy->CollisionFOV(playerPos, D3DX_PI, D3DX_PI * 0.5f))
		{
			// 振り返り
			m_pEnemy->ChangeState(make_shared<CEnemyLookBackR>());
			return true;
		}
	}

	return false;
}

//===================================================
// 後ろを見るモーションの設定
//===================================================
bool CEnemyStateManager::SetLookBackMotion(void)
{
	// 取得できなかったら処理しない
	if (m_pEnemy == nullptr) return false;

	// プレイヤーの取得
	CPlayer* pPlayer = CGame::GetPlayer();

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return false;

	// プレイヤーの位置の取得
	D3DXVECTOR3 playerPos = pPlayer->GetPosition();

	// 近くにいたら
	if (m_pEnemy->CheckDistane(400.0f))
	{
		// 後ろにいたら
		if (m_pEnemy->CollisionFOV(playerPos, -D3DX_PI * 0.15f, D3DX_PI * 0.15f))
		{
			m_pEnemy->ChangeState(make_shared<CEnemyBackKick>());
			return true;
		}
	}
	else
	{
		// 左後ろにいたら
		if (m_pEnemy->CollisionFOV(playerPos, -D3DX_PI * 0.5f, D3DX_PI))
		{
			// 振り返り
			m_pEnemy->ChangeState(make_shared<CEnemyLookBackL>());
			return true;
		}
		// 右後ろにいたら
		else if (m_pEnemy->CollisionFOV(playerPos, D3DX_PI, D3DX_PI * 0.5f))
		{
			// 振り返り
			m_pEnemy->ChangeState(make_shared<CEnemyLookBackR>());
			return true;
		}
	}

	return false;
}

//===================================================
// コンストラクタ
//===================================================
CEnemyState::CEnemyState()
{
	m_ID = ID_BASE;
	m_pEnemy = nullptr;
}

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
CEnemyIdle::CEnemyIdle() : CEnemyState(ID_IDLE)
{
	m_nNextStateCount = NULL;
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
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	if (pMotion != nullptr)
	{
		// モーションの設定処理
		pMotion->SetMotion(CEnemy::MOTIONTYPE_NEUTRAL, true, 10);
	}
}

//===================================================
// 更新処理
//===================================================
void CEnemyIdle::Update(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// 軌跡の破棄
	pEnemy->DeleteOrbit();

	// プレイヤーの取得
	CPlayer* pPlayer = CGame::GetPlayer();

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// 位置の取得
	D3DXVECTOR3 playerPos = pPlayer->GetPosition();

	// 次の行動に移るまでの時間が0だったら
	if (m_nNextStateCount <= 0)
	{
		// 状態マネージャーの取得
		auto pStateManager = pEnemy->GetStateManager();

		// プレイヤーの立ち位置でモーションを設定
		if (pStateManager != nullptr && pStateManager->SetMotionByPlayerPosition())
		{
			return;
		}

		// 次の行動を選択
		if (pEnemy->CheckDistane(350.0f))
		{
			// ランダムな値の選出
			int random = rand() % 2;

			switch (random)
			{
			case 0:
				pEnemy->ChangeState(make_shared<CEnemySwing>());
				break;
			case 1:
				pEnemy->ChangeState(make_shared<CEnemyAttackSmash>());
				break;
			default:
				break;
			}

			return;
		}
		else
		{

			// 状態の設定
			pEnemy->ChangeState(make_shared<CEnemyMove>());
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
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 移動時の煙
	pEnemy->MoveSmoke();

	// 距離が近かったら
	if (pEnemy->CheckDistane(ACTION_DISTANCE))
	{
		// ランダムな値の選出
		int random = rand() % 2;

		switch (random)
		{
		case 0:
			pEnemy->ChangeState(make_shared<CEnemySwing>());
			break;
		case 1:
			pEnemy->ChangeState(make_shared<CEnemyAttackSmash>());
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

		if (random <= START_ACTION)
		{
			// 設定した距離以内かを判定
			if (pEnemy->CheckDistane(SIDE_MOVE_DISTANCE))
			{
				// 次の行動の選出
				ElectinMove(pEnemy);
			}
			else
			{
				bAction = true;
			}
		}
		//// プレイヤーから遠いいなら
		//else if (random <= 90 && !pEnemy->CheckDistane(550.0f))
		//{
		//	pEnemy->ChangeState(make_shared<CEnemyStep>());
		//	return;
		//}
	}

	// アクションを出せる
	if (bAction)
	{
		// 出す攻撃の設定
		int action = rand() % 3;

		switch (action)
		{
		case 0:
			pEnemy->ChangeState(make_shared<CEnemyAttackImpact>());
			break;
		case 1:
			pEnemy->ChangeState(make_shared<CEnemyRoar>());
			break;
		case 2:
			pEnemy->ChangeState(make_shared<CEnemyJumpAttack>());
			break;
		default:
			break;
		}
		return;
	}

	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_MOVE))
	{
		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_WARK003, 0.5f);
		}
	}

	// プレイヤーを追いかける処理
	pEnemy->ChasePlayer(0.9f);

	// モーションの設定処理
	pMotion->SetMotion(CEnemy::MOTIONTYPE_MOVE, true, 10);
}

//===================================================
// 移動のランダム選出
//===================================================
void CEnemyMove::ElectinMove(CEnemy *pEnemy)
{
	// 次の行動を選出
	int nAction = rand() % 2;

	// 行動の遷移
	switch (nAction)
	{
	case 0:
		pEnemy->ChangeState(make_shared<CEnemyRightMove>());
		break;
	case 1:
		pEnemy->ChangeState(make_shared<CEnemyLeftMove>());
		break;
	default:
		break;
	}
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
		pMotion->SetMotion(CEnemy::MOTIONTYPE_SMASH, true, 20);
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
	
	if (pMotion->IsEventFrame(0, 30, CEnemy::MOTIONTYPE_SMASH))
	{
		// プレイヤーの方向を見る処理
		pEnemy->AngleToPlayer();
	}

	if (pMotion->IsEventFrame(64, 72, CEnemy::MOTIONTYPE_SMASH))
	{
		// 軌跡の処理
		pEnemy->Orbit(16, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f));
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
		pEnemy->ChangeState(make_shared<CEnemyIdle>(30));

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
// コンストラクタ(大ダメージ)
//===================================================
CEnemyDamageL::CEnemyDamageL() : CEnemyState(ID_DAMAGEL)
{
	m_bFinish = false;
	m_nIdleTime = IDLE_TIME;
	m_nDamage = NULL;
	m_bBackStap = false;
}

//===================================================
// コンストラクタ(大ダメージ)
//===================================================
CEnemyDamageL::CEnemyDamageL(const int nDamage,const bool bBackStatp) : CEnemyState(ID_DAMAGEL)
{
	m_bFinish = false;
	m_nIdleTime = IDLE_TIME;
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
	CPlayer* pPlayer = CGame::GetPlayer();

	// プレイヤーの位置の取得
	D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();

	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// ダメージの設定
	pEnemy->Hit(m_nDamage);

	if (pMotion != nullptr)
	{
		// 大ダメージモーションの設定
		pMotion->SetMotion(CEnemy::MOTIONTYPE_DAMAGEL, true, 2);
	}

	// 移動制御処理の取得
	CEnemyMovement *pMovement = pEnemy->GetMovement();

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
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	if (pMotion->IsEventFrame(1, 10, CEnemy::MOTIONTYPE_DAMAGEL))
	{
		// プレイヤーの取得
		CPlayer* pPlayer = CGame::GetPlayer();

		D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();

		// 吹き飛び処理
		pEnemy->GetMovement()->BlowOff(PlayerPos, 50.0f, 5.0f);
	}

	// モーションを最後まで行ったら
	if (pMotion->IsFinishEndBlend())
	{
		m_bFinish = true;
	}

	if (m_bFinish)
	{
		m_nIdleTime--;
	}

	// 何もしない時間が終わったら
	if (m_nIdleTime <= 0)
	{
		// 状態マネージャーの取得
		auto pStateManager = pEnemy->GetStateManager();

		// プレイヤーの立ち位置でモーションを設定
		if (pStateManager != nullptr && pStateManager->SetMotionByPlayerPosition())
		{
			return;
		}

		// 次の行動を選出
		int nAction = rand() % 2;

		// 行動の遷移
		switch (nAction)
		{
		case 0:
			pEnemy->ChangeState(make_shared<CEnemyRightMove>());
			break;
		case 1:
			pEnemy->ChangeState(make_shared<CEnemyLeftMove>());
			break;
		default:
			break;
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

	if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_IMPACT,0))
	{
		// プレイヤーの方向を見る処理
		pEnemy->AngleToPlayer();
	}

	if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_IMPACT, 1))
	{
		// 軌跡の処理
		pEnemy->Orbit(16, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f));
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
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 取得できなかったら処理しない
	if (pMotion == nullptr) return;

	// 叫びモーションの設定
	pMotion->SetMotion(CEnemy::MOTIONTYPE_ROAR, true, 10);

	// レンダラーの取得
	CRenderer* pRenderer = CManager::GetRenderer();

	if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_ROAR))
	{
		// 音の取得
		CSound *pSound = CManager::GetSound();

		if (pRenderer != nullptr)
		{
			// ブラーをオン
			pRenderer->onEffect(0.8f);
		}

		if (pSound != nullptr)
		{
			// 咆哮
			pSound->Play(CSound::SOUND_LABEL_ROAR);
		}
	}

	// 攻撃モーションが終わったら
	if (pMotion->FinishMotion())
	{
		if (pRenderer != nullptr)
		{
			// ブラーをオン
			pRenderer->offEffect();
		}

		// 次の行動
		int nAction = rand() % 2;

		switch (nAction)
		{
		case 0:
			// Dashにする
			pEnemy->ChangeState(make_shared<CEnemyDash>());
			break;
		case 1:
			// Rushにする
			pEnemy->ChangeState(make_shared<CEnemyRush>());
			break;
		default:
			break;
		}

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
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// ダッシュ
	pMotion->SetMotion(CEnemy::MOTIONTYPE_DASH, true, 10);

	// プレイヤーを追いかける
	pEnemy->ChasePlayer(0.1f,7.0f);

	// 一定の距離に入ったら
	if (pEnemy->CheckDistane(250.0f))
	{
		// 回転攻撃
		pEnemy->ChangeState(make_shared<CEnemySpin>(SPIN_TIME));

		return;
	}
}

//===================================================
// コンストラクタ(回転攻撃)
//===================================================
CEnemySpin::CEnemySpin() : CEnemyState(ID_SPIN)
{
	m_nTime = NULL;
	m_nMaxTime = NULL;
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

	// 向いている方向に移動する
	pEnemy->GetMovement()->MoveForWard(15.0f);

	if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_SPIN))
	{
		// 軌跡の処理
		pEnemy->Orbit(16, D3DXCOLOR(1.0f, 1.0f, 0.5f, 0.5f));
	}

	m_nTime--;

	// 位置の取得
	D3DXVECTOR3 pos = pEnemy->GetPosition();

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
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	pEnemy->GetMovement()->Jump(24.0f);

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// モーションの設定
	pMotion->SetMotion(CEnemy::MOTIONTYPE_JUMP, true, 10);
}

//===================================================
// 更新処理(バックステップ)
//===================================================
void CEnemyBackStep::Update(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	pEnemy->GetMovement()->SetMoveDir(0.0f,20.0f);
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
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// モーションの設定
	pMotion->SetMotion(CEnemy::MOTIONTYPE_LANDING, true, 10);

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
				pEnemy->ChangeState(make_shared<CEnemyJumpAttack>());
				break;
			case 1:
				pEnemy->ChangeState(make_shared<CEnemyAttackImpact>());
				break;
			case 2:
				pEnemy->ChangeState(make_shared<CEnemyRoar>());
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
			pEnemy->ChangeState(make_shared<CEnemyIdle>(1));
		}
	}
}

//===================================================
// コンストラクタ(ヒット)
//===================================================
CEnemyHit::CEnemyHit() : CEnemyState(ID_HIT)
{
	m_type = TYPE_NORMAL;
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
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	if (pMotion != nullptr)
	{
		switch (m_type)
		{
		case TYPE_NORMAL:
			// モーションの設定
			pMotion->SetMotion(CEnemy::MOTIONTYPE_HIT, true, 5);
			break;
		case TYPE_BACK:
			// モーションの設定
			pMotion->SetMotion(CEnemy::MOTIONTYPE_BACK_HIT, true, 5);
			break;
		default:
			break;
		}
	}
}

//===================================================
// 更新処理(ヒット)
//===================================================
void CEnemyHit::Update(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	if (pMotion != nullptr)
	{
		// モーションの種類の取得
		int motiontype = pMotion->GetBlendType();

		if (pMotion->IsEventFrame(1,50,CEnemy::MOTIONTYPE_HIT))
		{
			// 後ろに進む
			pEnemy->GetMovement()->SetMoveDir(m_Const.BACK_MOVE_VALUE.x, m_Const.BACK_MOVE_VALUE.y);
		}

		// モーションが終わったら
		if (pMotion->FinishMotion() && motiontype == CEnemy::MOTIONTYPE_HIT)
		{
			// 状態の変更
			pEnemy->ChangeState(make_shared<CEnemyBackStep>());
		}

		if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_BACK_HIT))
		{
			// 前に進む
			pEnemy->GetMovement()->MoveForWard(m_Const.FORWARD_MOVE_VALUE);
		}

		// モーションが終わったら
		if (pMotion->IsFinishEndBlend())
		{
			// 状態の変更
			pEnemy->ChangeState(make_shared<CEnemyIdle>(1));
		}
	}
}

//===================================================
// コンストラクタ(ダメージ小)
//===================================================
CEnemyDamageS::CEnemyDamageS()
{
	m_type = TYPE_NORMAL;
	m_bFinish = false;
	m_nIdleTime = IDLE_TIME;
	m_nNextAction = INIT_NEXT_ACTION;
	m_nDamage = NULL;
}

//===================================================
// コンストラクタ(ダメージ小)
//===================================================
CEnemyDamageS::CEnemyDamageS(const int nDamage) : CEnemyState(ID_DAMAGES)
{
	m_type = TYPE_NORMAL;
	m_bFinish = false;
	m_nIdleTime = IDLE_TIME;
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
	CPlayer* pPlayer = CGame::GetPlayer();

	// プレイヤーの位置の取得
	D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();

	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 移動制御処理の取得
	CEnemyMovement* pMovement = pEnemy->GetMovement();

	// 次の行動を抽選
	m_nNextAction = rand() % 100;

	// ダメージの設定
	pEnemy->Hit(m_nDamage);

	if (pMotion != nullptr)
	{
		switch (m_type)
		{
		case TYPE_NORMAL:
			if (pMovement != nullptr && pPlayer != nullptr)
			{
				// 吹き飛び処理
				pMovement->BlowOff(PlayerPos, 100.0f, 5.0f);
			}
			// ダメージモーションにする
			pMotion->SetMotion(CEnemy::MOTIONTYPE_DAMAGES, true, 2);
			break;
		case TYPE_STANP:
			// ダメージモーションにする
			pMotion->SetMotion(CEnemy::MOTIONTYPE_STANP_DAMAGE, true, 2);
			break;
		default:
			break;
		}
	}
}

//===================================================
// 更新処理(ダメージ小)
//===================================================
void CEnemyDamageS::Update(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// モーションが終わったら
	if (pMotion != nullptr)
	{
		// モーションの種類の取得
		int nMotionType = pMotion->GetType();

		// 10%の確率
		if (pMotion->FinishMotion() && m_nNextAction <= 10 && nMotionType == CEnemy::MOTIONTYPE_DAMAGES)
		{
			// 距離を取る
			pEnemy->ChangeState(make_shared<CEnemyAway>());
			return;
		}
		else if (pMotion->IsFinishEndBlend())
		{
			m_bFinish = true;
		}
	}

	// モーションが終わったら
	if (m_bFinish)
	{
		m_nIdleTime--;
	}

	// 何もしない時間が終わったら
	if (m_nIdleTime <= 0)
	{
		// 状態マネージャーの取得
		auto pStateManager = pEnemy->GetStateManager();

		// プレイヤーの立ち位置でモーションを設定
		if (pStateManager != nullptr && pStateManager->SetMotionByPlayerPosition())
		{
			return;
		}

		// 次の行動を選出
		int nAction = rand() % 2;

		// 行動の遷移
		switch (nAction)
		{
		case 0:
			pEnemy->ChangeState(make_shared<CEnemyRightMove>());
			break;
		case 1:
			pEnemy->ChangeState(make_shared<CEnemyLeftMove>());
			break;
		default:
			break;
		}
	}
}

//===================================================
// コンストラクタ(ガード)
//===================================================
CEnemyGuard::CEnemyGuard()
{
	m_nDamage = NULL;
	m_ImpactPos = VEC3_NULL;
	m_nNextAction = INIT_NEXT_ACTION;
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
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// ダメージの設定
	pEnemy->Hit(m_nDamage);

	// プレイヤーの取得
	CPlayer* pPlayer = CGame::GetPlayer();

	// 位置の取得
	D3DXVECTOR3 pos = pEnemy->GetPosition();

	// プレイヤーの位置の取得
	D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();

	// パーティクルの生成
	auto pParticle = CParticle3DNormal::Create(m_ImpactPos, 10.0f, D3DXCOLOR(1.0f, 0.4f, 0.4f, 1.0f));

	// パーティクルの設定処理
	pParticle->SetParticle(15.0f, 240, 50, 5,314);

	// ボスまでの角度を取得
	float fAngle = GetTargetAngle(pos, PlayerPos);

	// 向きの設定
	pPlayer->SetAngle(fAngle + D3DX_PI);

	// インパクトを生成
	auto pCircle = CMeshCircle::Create(D3DXCOLOR(1.0f, 0.4f, 0.4f, 0.8f), m_ImpactPos, 0.0f, 120.0f);

	// サークルの設定処理
	pCircle->SetCircle(35.0f, 15.0f, 120, false, D3DXVECTOR3(D3DX_PI * 0.5f, fAngle, 0.0f));

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(CEnemy::MOTIONTYPE_GUARD, true, 2);
	}

	// 次の行動を抽選
	m_nNextAction = rand() % 100;
}

//===================================================
// 更新処理(ガード)
//===================================================
void CEnemyGuard::Update(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	if (pMotion->FinishMotion())
	{
		if (m_nNextAction <= 50)
		{
			// バックステップする
			pEnemy->ChangeState(make_shared<CEnemyBackStep>());
		}
		else
		{
			// 振り下ろし攻撃に派生
			pEnemy->ChangeState(make_shared<CEnemyAttackSmash>());
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
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	pEnemy->GetMovement()->MoveForWard(150.0f);

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(CEnemy::MOTIONTYPE_STEP, true, 4);
	}
}

//===================================================
// 更新処理(ステップ)
//===================================================
void CEnemyStep::Update(void)
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
		// モーションが終わったら
		if (pMotion->IsEndMotion())
		{
			// 通常状態に戻す
			pEnemy->ChangeState(make_shared<CEnemyMove>());
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
				CSlow *pSlow = CManager::GetSlow();

				pSlow->Start(60,4);
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
			auto pWave = CMeshWave::Create(pos, 50.0f, 50.0f, D3DXCOLOR(1.0f,0.4f,0.4f,1.0));

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

			// ジャンプ攻撃中の移動
			pEnemy->GetMovement()->MoveForWard(dir / JUMPATTACK_MOVE_FRAME);
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
			CSlow *pSlow = CManager::GetSlow();

			pSlow->Start(60,4);
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
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	CSlow* pSlow = CManager::GetSlow();

	pSlow->Start(60, 8);

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(CEnemy::MOTIONTYPE_DEATH, true, 1);
	}
}

//===================================================
// 更新処理(死亡)
//===================================================
void CEnemyDeath::Update(void)
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
		// 吹き飛び中だったら
		if (pMotion->IsEventFrame(1, 90, CEnemy::MOTIONTYPE_DEATH))
		{
			// 移動方向を設定
			pEnemy->GetMovement()->SetMoveDir(0.0f, 20.0f);
		}

		if (pMotion->IsEventFrame(110, 110, CEnemy::MOTIONTYPE_DEATH))
		{
			pEnemy->ChangeState(make_shared<CEnemyDown>());
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
		pMotion->SetMotion(CEnemy::MOTIONTYPE_DOWN, true, 4);
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
		CGame::SetState(CGame::STATE_END);
		CGame::SetResult(CGame::RESULTTYPE_WIN);

		// カメラの取得
		CGameCamera* pCamera = CGame::GetCamera();

		// プレイヤーに戻す
		pCamera->SetTracking(CGameCamera::TRACKOBJ_PLAYER);
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
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	int nPosXMax = MAX_AWAYPOS_X * 2;
	float fPosXMin = static_cast<float>(MAX_AWAYPOS_X);

	int nPosZMax = MAX_AWAYPOS_Z * 2;
	float fPosZMin = static_cast<float>(MAX_AWAYPOS_Z);

	m_pos.x = static_cast<float>(rand() % nPosXMax - fPosXMin);
	m_pos.z = static_cast<float>(rand() % nPosZMax - fPosZMin);

	// 位置の取得
	D3DXVECTOR3 pos = pEnemy->GetPosition();

	// 距離を求める
	float fDistance = GetDistance(m_pos - pos);

	// 目標地点までの距離が1000以下だったら
	if (fDistance <= 1000.0f)
	{
		// ジャンプ攻撃に派生
		pEnemy->ChangeState(make_shared<CEnemyJumpAttack>());
		return;
	}

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(CEnemy::MOTIONTYPE_JUMP, true, 4);
	}

	// 向きを求める
	float fAngle = GetTargetAngle(m_pos, pos);

	// 向きの設定
	pEnemy->SetAngle(fAngle + D3DX_PI);

	// ジャンプする
	pEnemy->GetMovement()->Jump(25.0f);
}

//===================================================
// 更新処理(距離を取る状態)
//===================================================
void CEnemyAway::Update(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// 位置の取得
	D3DXVECTOR3 pos = pEnemy->GetPosition();

	// 距離を求める
	float fDistance = GetDistance(m_pos - pos);

	// 距離を到達時間で割る
	float dir = fDistance / AWAY_TIME;

	// 移動量を設定する
	pEnemy->GetMovement()->MoveForWard(dir);
}

//===================================================
// コンストラクタ(特大ヒット)
//===================================================
CEnemySuperHit::CEnemySuperHit() : CEnemyState(ID_SUPER_HIT)
{
	
}

//===================================================
// デストラクタ(特大ヒット)
//===================================================
CEnemySuperHit::~CEnemySuperHit()
{
}

//===================================================
// 初期化処理
//===================================================
void CEnemySuperHit::Init(void)
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
		pMotion->SetMotion(CEnemy::MOTIONTYPE_SUPER_HIT, true, 4);
	}
}

//===================================================
// 更新処理
//===================================================
void CEnemySuperHit::Update(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// プレイヤーの取得
	CPlayer* pPlayer = CGame::GetPlayer();

	// プレイヤーを取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		if (pMotion->IsEventFrame(1, 44, CEnemy::MOTIONTYPE_SUPER_HIT))
		{
			// 後ろ方向
			pEnemy->GetMovement()->SetMoveDir(0.0f, 2.0f);
		}
		
		// プレイヤーのモーションの取得
		CMotion* pPlayerMotion = pPlayer->GetMotion();

		// プレイヤーのモーションが必殺攻撃の120フレーム目か判定
		if (pPlayerMotion != nullptr && pPlayerMotion->IsEventFrame(120, 120, CPlayer::MOTIONTYPE_REVENGEATTACK))
		{
			// 状態の変更
			pEnemy->ChangeState(make_shared<CEnemyComboDamage>());
		}
		if (pMotion->IsFinishEndBlend())
		{
			// 状態の変更
			pEnemy->ChangeState(make_shared<CEnemyIdle>(5));
		}
	}
}

//===================================================
// コンストラクタ(連続ダメージ)
//===================================================
CEnemyComboDamage::CEnemyComboDamage() : CEnemyState(ID_COMBO_DAMAGE)
{
}

//===================================================
// デストラクタ(連続ダメージ)
//===================================================
CEnemyComboDamage::~CEnemyComboDamage()
{
}

//===================================================
// 初期化処理(連続ダメージ)
//===================================================
void CEnemyComboDamage::Init(void)
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
		pMotion->SetMotion(CEnemy::MOTIONTYPE_COMBODAMAGE, true, 2);
	}
}

//===================================================
// 更新処理(連続ダメージ)
//===================================================
void CEnemyComboDamage::Update(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// プレイヤーの取得
	CPlayer* pPlayer = CGame::GetPlayer();

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 取得できなかったら処理しない
	if (pMotion == nullptr) return;
	
	// 音の取得
	CSound* pSound = CManager::GetSound();

	// エフェクトの生成
	D3DXVECTOR3 HandL = pPlayer->GetModelPos(CEnemy::MODEL_HANDL);
	D3DXVECTOR3 HandR = pPlayer->GetModelPos(CEnemy::MODEL_HANDR);

	// プレイヤーの位置
	D3DXVECTOR3 playerPos = pPlayer->GetPosition();

	// プレイヤー向きの取得
	D3DXVECTOR3 angle = pPlayer->GetRotaition()->Get();

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

	if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_COMBODAMAGE))
	{
		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_PERFECT);
		}

		// エフェクトの生成
		auto pEffect = CParticle3DNormal::Create(ImpactPos,20.0f, D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f));
		pEffect->SetParticle(35.0f, 360, 30, 1, 314);
		pEffect->SetParam(CEffect3D::TYPE_NORAML,0.1f);

		// メッシュサークルの生成
		auto pMeshCircle = CMeshCircle::Create(D3DXCOLOR(1.0f, 1.0f, 0.4f, 0.5f), NewImpactPos, 10.0f, 50.0f);
		pMeshCircle->SetCircle(-50.0f, 8.0f, 30, false, D3DXVECTOR3(D3DX_PI * 0.5f, angle.y, 0.0f));

		// ヒット時の設定
		pEnemy->Hit(2);
	}
	if (pMotion->IsEventFrame(159, 159, CEnemy::MOTIONTYPE_COMBODAMAGE))
	{
		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_PERFECT);
		}

		// メッシュサークルの生成
		auto pMeshCircle = CMeshCircle::Create(D3DXCOLOR(1.0f, 1.0f, 0.4f, 0.9f), NewImpactPos, 10.0f, 50.0f);
		pMeshCircle->SetCircle(-50.0f, 8.0f, 30, false, D3DXVECTOR3(D3DX_PI * 0.5f, angle.y, 0.0f));

		// エフェクトの生成
		auto pEffect = CParticle3DNormal::Create(ImpactPos, 20.0f, D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f));
		pEffect->SetParticle(35.0f, 360, 50, 1, 314);
		pEffect->SetParam(CEffect3D::TYPE_NORAML, 0.1f);

		// ヒット時の設定
		pEnemy->Hit(3);

		// 状態の変更
		pEnemy->ChangeState(make_shared<CEnemyDamageL>(0,false));
	}
}

//===================================================
// コンストラクタ(右横歩き)
//===================================================
CEnemyRightMove::CEnemyRightMove() : CEnemyState(ID_RIGHT_MOVE)
{
	m_nTime = rand() % 180 + MOVE_TIME;
}

//===================================================
// デストラクタ(右横歩き)
//===================================================
CEnemyRightMove::~CEnemyRightMove()
{
}

//===================================================
// 初期化処理(右横歩き)
//===================================================
void CEnemyRightMove::Init(void)
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
		pMotion->SetMotion(CEnemy::MOTIONTYPE_RIGHTMOVE, true, 10);
	}
}

//===================================================
// 更新処理(右横歩き)
//===================================================
void CEnemyRightMove::Update(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 取得できなかったら処理しない
	if (pMotion == nullptr) return;

	// プレイヤーの方向を見る
	pEnemy->AngleToPlayer();

	// 右に移動する
	pEnemy->GetMovement()->SetMoveDir(-D3DX_PI * 0.5f, 3.0f);

	m_nTime--;

	// 障害物の近くだったら
	if (pEnemy->CheckObstacleDistance(OBSTACLE_DISTANCE))
	{
		// 状態の遷移
		pEnemy->ChangeState(make_shared<CEnemyMove>());
	}
	// 時間が終わったら
	if (m_nTime <= 0)
	{
		// 距離以内か判定
		if (!pEnemy->CheckDistane(ACTION_DISTANCE))
		{
			// 次の行動の選出
			int nAction = rand() % 4;

			// 次の行動の遷移
			switch (nAction)
			{
			case 0:
				pEnemy->ChangeState(make_shared<CEnemyJumpAttack>());
				break;
			case 1:
				pEnemy->ChangeState(make_shared<CEnemyRush>());
				break;
			case 2:
				pEnemy->ChangeState(make_shared<CEnemyAttackImpact>());
				break;
			case 3:
				pEnemy->ChangeState(make_shared<CEnemyRoar>());
				break;
			default:
				break;
			}
		}
		else
		{
			// 状態の変更
			pEnemy->ChangeState(make_shared<CEnemyRushSwing>());
		}
	}

	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_RIGHTMOVE))
	{
		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_WARK003, 0.5f);
		}
	}
}

//===================================================
// コンストラクタ(左横歩き)
//===================================================
CEnemyLeftMove::CEnemyLeftMove() : CEnemyState(ID_LEFT_MOVE)
{
	m_nTime = rand() % 180 + MOVE_TIME;
}

//===================================================
// デストラクタ(左横歩き)
//===================================================
CEnemyLeftMove::~CEnemyLeftMove()
{
}

//===================================================
// 初期化処理(左横歩き)
//===================================================
void CEnemyLeftMove::Init(void)
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
		pMotion->SetMotion(CEnemy::MOTIONTYPE_LEFTMOVE, true, 10);
	}
}

//===================================================
// 更新処理(左横歩き)
//===================================================
void CEnemyLeftMove::Update(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 取得できなかったら処理しない
	if (pMotion == nullptr) return;

	// プレイヤーの方向を見る
	pEnemy->AngleToPlayer();

	// 右に移動する
	pEnemy->GetMovement()->SetMoveDir(D3DX_PI * 0.5f, 3.0f);

	// 障害物の近くだったら
	if (pEnemy->CheckObstacleDistance(OBSTACLE_DISTANCE))
	{
		// 状態の遷移
		pEnemy->ChangeState(make_shared<CEnemyMove>());
	}

	m_nTime--;

	// 時間が終わったら
	if (m_nTime <= 0)
	{
		// 距離以内か判定
		if (!pEnemy->CheckDistane(ACTION_DISTANCE))
		{
			// 次の行動の選出
			int nAction = rand() % 4;

			// 次の行動の遷移
			switch (nAction)
			{
			case 0:
				pEnemy->ChangeState(make_shared<CEnemyJumpAttack>());
				break;
			case 1:
				pEnemy->ChangeState(make_shared<CEnemyRush>());
				break;
			case 2:
				pEnemy->ChangeState(make_shared<CEnemyAttackImpact>());
				break;
			case 3:
				pEnemy->ChangeState(make_shared<CEnemyRoar>());
				break;
			default:
				break;
			}
		}
		else
		{
			// 状態の変更
			pEnemy->ChangeState(make_shared<CEnemyRushSwing>());
		}
	}

	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_LEFTMOVE))
	{
		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_WARK003, 0.5f);
		}
	}
}

//===================================================
// コンストラクタ(突進攻撃)
//===================================================
CEnemyRush::CEnemyRush() : CEnemyState(ID_RUSH)
{
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
	pEnemy->SetInertia(0.25f);
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
		if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_RUSH,0) ||
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
		CEnemy::RESULT result = pEnemy->AttackResult(pPlayer,CEnemy::MODEL_WAIST,250.0f);

		// 位置の取得
		D3DXVECTOR3 pos = pEnemy->GetPosition();

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
	pEnemy->SetInertia(0.25f);

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
		pSound->Play(CSound::SOUND_LABEL_END_RUSH,0.5f);
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
		pEnemy->SetInertia(m_fInertia);

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
			pEnemy->SetInertia(0.25f);

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
	if (pMotion != nullptr)
	{
		// 状態の設定
		pEnemy->SetState(CCharacter3D::STATE_ACTION, 5);

		// 攻撃の結果を取得
		CEnemy::RESULT result = pEnemy->AttackResult(pPlayer, CEnemy::MODEL_FOOTR, 150.0f);

		// 位置の取得
		D3DXVECTOR3 pos = pEnemy->GetPosition();

		if (pMotion->IsEventFrame(62, 75, CEnemy::MOTIONTYPE_BACKKICK))
		{
			// パリィされた
			if (result == CEnemy::RESULT_PARRY)
			{
				// 構えの設定処理
				pPlayer->SetStance(pos,CPlayer::PARRYMOTION_JUMP);

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
			// 状態の遷移
			pEnemy->ChangeState(make_shared<CEnemyIdle>(5));
		}
	}
}

//===================================================
// コンストラクタ(左後ろを見る)
//===================================================
CEnemyLookBackL::CEnemyLookBackL() : CEnemyState(ID_LOOK_BACKL)
{
	m_fAngle = NULL;
	m_nCounter = NULL;
	m_nFrame = 60;
	m_fDiffAngle = NULL;
}

//===================================================
// デストラクタ(左後ろを見る)
//===================================================
CEnemyLookBackL::~CEnemyLookBackL()
{
}

//===================================================
// 初期化処理(左後ろを見る)
//===================================================
void CEnemyLookBackL::Init(void)
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

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(CEnemy::MOTIONTYPE_LOOK_BACKL, true, 40);
	}

	// 敵の位置の取得
	D3DXVECTOR3 pos = pEnemy->GetPosition();

	// プレイヤーの位置の取得
	D3DXVECTOR3 playerPos = pPlayer->GetPosition();

	// プレイヤーまでの角度を求める
	float fAngle = GetTargetAngle(pos, playerPos);

	// 向きの取得
	D3DXVECTOR3 rot = pEnemy->GetRotaition()->GetDest();

	// 今の角度から目的の角度までの距離を求める
	m_fAngle = rot.y;
	m_fDiffAngle = fAngle - rot.y;

	// 角度の差分を求める
	if (m_fDiffAngle < -D3DX_PI)
	{
		m_fDiffAngle += D3DX_PI * 2.0f;
	}
	else if (m_fDiffAngle > D3DX_PI)
	{
		m_fDiffAngle += -D3DX_PI * 2.0f;
	}
}

//===================================================
// 更新処理(左後ろを見る)
//===================================================
void CEnemyLookBackL::Update(void)
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
		// モーションが終わったら
		if (pMotion->IsFinishEndBlend())
		{
			// 状態の遷移
			pEnemy->ChangeState(make_shared<CEnemyIdle>(5));
			return;
		}

		if (pMotion->GetBlendType() != CEnemy::MOTIONTYPE_LOOK_BACKL || pMotion->GetType() != CEnemy::MOTIONTYPE_LOOK_BACKL) return;

		if (!pMotion->IsEventFrame(0, 120, CEnemy::MOTIONTYPE_LOOK_BACKL))
		{
			// フレームを超えたら
			if (m_nFrame <= m_nCounter)
			{
				// 制限する
				m_nCounter = m_nFrame;
			}

			// 相対値
			float fRate = m_nCounter / static_cast<float>(m_nFrame);

			// 目的の向きまで近づける
			float fAngle = m_fAngle + m_fDiffAngle * fRate;

			pEnemy->SetAngle(fAngle);

			// カウンターを進める
			m_nCounter++;
		}
	}
}

//===================================================
// コンストラクタ(右後ろを見る)
//===================================================
CEnemyLookBackR::CEnemyLookBackR() : CEnemyState(ID_LOOK_BACKR)
{
	m_fAngle = NULL;
	m_nCounter = NULL;
	m_nFrame = 60;
	m_fDiffAngle = NULL;
}

//===================================================
// デストラクタ(右後ろを見る)
//===================================================
CEnemyLookBackR::~CEnemyLookBackR()
{
}

//===================================================
// 初期化処理(右後ろを見る)
//===================================================
void CEnemyLookBackR::Init(void)
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

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(CEnemy::MOTIONTYPE_LOOK_BACKR, true, 40);
	}

	// 敵の位置の取得
	D3DXVECTOR3 pos = pEnemy->GetPosition();

	// プレイヤーの位置の取得
	D3DXVECTOR3 playerPos = pPlayer->GetPosition();

	// プレイヤーまでの角度を求める
	float fAngle = GetTargetAngle(pos, playerPos);

	// 向きの取得
	D3DXVECTOR3 rot = pEnemy->GetRotaition()->GetDest();

	// 今の角度から目的の角度までの距離を求める
	m_fAngle = rot.y;
	m_fDiffAngle = fAngle - rot.y;

	// 角度の差分を求める
	if (m_fDiffAngle < -D3DX_PI)
	{
		m_fDiffAngle += D3DX_PI * 2.0f;
	}
	else if (m_fDiffAngle > D3DX_PI)
	{
		m_fDiffAngle += -D3DX_PI * 2.0f;
	}
}

//===================================================
// 更新処理(右後ろを見る)
//===================================================
void CEnemyLookBackR::Update(void)
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
		// モーションが終わったら
		if (pMotion->IsFinishEndBlend())
		{
			// 状態の遷移
			pEnemy->ChangeState(make_shared<CEnemyIdle>(5));
			return;
		}

		if (pMotion->GetBlendType() != CEnemy::MOTIONTYPE_LOOK_BACKR || pMotion->GetType() != CEnemy::MOTIONTYPE_LOOK_BACKR) return;

		if (!pMotion->IsEventFrame(0, 120, CEnemy::MOTIONTYPE_LOOK_BACKR))
		{
			// フレームを超えたら
			if (m_nFrame <= m_nCounter)
			{
				// 制限する
				m_nCounter = m_nFrame;
			}

			// 相対値
			float fRate = m_nCounter / static_cast<float>(m_nFrame);

			// 目的の向きまで近づける
			float fAngle = m_fAngle + m_fDiffAngle * fRate;

			pEnemy->SetAngle(fAngle);

			// カウンターを進める
			m_nCounter++;
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
			CEnemy::RESULT result = pEnemy->AttackResult(pPlayer,CEnemy::MODEL_HANDL,150.0f);

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
	pEnemy->SetInertia(0.25f);
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

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_RUSH_SWING, 2))
		{
			// プレイヤーの取得
			D3DXVECTOR3 playerPos = pPlayer->GetPosition();
			D3DXVECTOR3 pos = pEnemy->GetPosition();

			// プレイヤーの方向を見る
			pEnemy->AngleToPlayer();

			// 距離を求める
			float fDistance = GetDistance(playerPos - pos);

			// 移動量を設定
			pEnemy->GetMovement()->MoveForWard(fDistance / MOVE_FRAME);
		}
		else if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_RUSH_SWING, 3))
		{
			// 慣性を設定
			pEnemy->SetInertia(0.02f);
		}
		if(pMotion->IsEventFrame(CEnemy::MOTIONTYPE_RUSH_SWING, 4) || 
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

		// プレイヤーとの当たり判定
		CollisionPlayer(pEnemy, pMotion);

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
		if (pMotion->IsFinishEndBlend())
		{
			// 状態の変更
			pEnemy->ChangeState(make_shared<CEnemyIdle>(20));

			return;
		}
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

	if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_RUSH_SWING,0))
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
	if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_RUSH_SWING,1))
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
