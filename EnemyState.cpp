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
#include "sound.h"
#include "renderer.h"
#include "EnemyStateAttack.h"
#include "EnemyStateMovement.h"
#include "EnemyMovement.h"
#include "meshfield.h"

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
	constexpr int INIT_NEXT_ACTION = 999;			// 次のアクションに移行する数値(初期化)
};

//===================================================
// HPがLowのときの演出処理
//===================================================
void CEnemyStateManager::SetLowEvent(void)
{
	// 敵が使われていないなら処理しない
	if (m_pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = m_pEnemy->GetMotion();

	// 取得できなかったら処理しない
	if (pMotion == nullptr) return;

	// モーションの種類の取得
	int nMotionType = pMotion->GetType();

	// HPが半分になったら
	if (m_nLife <= m_nMaxLife / 2 && m_nLife > 0 && nMotionType == CEnemy::MOTIONTYPE_NEUTRAL)
	{
		if (!m_bLowEventFinish)
		{
			m_bEvent = true;

			// 状態の変更
			m_pEnemy->ChangeState(make_shared<CEnemyAway>());
		}
	}
}

//===================================================
// HPが一定以下かどうか
//===================================================
bool CEnemyStateManager::CheckLowHp(const int nRate)
{
	// 0割り対策
	if (nRate == 0) return false;

	// 演出が終わっていなかったら
	if (!m_bLowEventFinish) return false;

	// 一定の割合を下回っていたら
	if (m_nLife <= m_nMaxLife / nRate)
	{
		return true;
	}
	return false;
}

//===================================================
// イベントの終了
//===================================================
void CEnemyStateManager::EndEvent(void)
{
	m_bEvent = false;
	m_bLowEventFinish = true;
}

//===================================================
// コンストラクタ
//===================================================
CEnemyStateManager::CEnemyStateManager()
{
	m_bEvent = false;
	m_bLowEventFinish = false;
	m_nLife = NULL;
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
		// 左後ろにいたら
		else if (m_pEnemy->CollisionFOV(playerPos, -D3DX_PI * 0.5f, D3DX_PI))
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

	// 状態マネージャーの取得
	auto pStateManager = pEnemy->GetStateManager();

	// 次の行動に移るまでの時間が0だったら
	if (m_nNextStateCount <= 0)
	{
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

	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pSound != nullptr)
	{
		// 音の再生
		pSound->Play(CSound::SOUND_LABEL_ENEMY_DAMAGE);
	}
	if (pMotion != nullptr)
	{
		// 大ダメージモーションの設定
		pMotion->SetMotion(CEnemy::MOTIONTYPE_DAMAGEL, true, 2);
	}

	// 移動制御処理の取得
	CEnemyMovement *pMovement = pEnemy->GetMovement();

	if (pMovement != nullptr && pPlayer != nullptr)
	{
		// 位置の取得
		D3DXVECTOR3 pos = pEnemy->GetPosition();

		// 吹き飛び処理
		float fAngle = pMovement->BlowOff(pos - PlayerPos, 250.0f, 5.0f);
		pEnemy->SetAngle(fAngle);
	}

	// 成功度の取得
	int nSuccess = pPlayer->SuccessParry();

	// 成功度の設定
	pPlayer->SetParryResult(nSuccess);
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

	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pMotion->IsEventFrame(33, 33, CEnemy::MOTIONTYPE_DAMAGEL))
	{
		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_BLOW);
		}
	}

	if (pMotion->IsEventFrame(1, 10, CEnemy::MOTIONTYPE_DAMAGEL))
	{
		// プレイヤーの取得
		CPlayer* pPlayer = CGame::GetPlayer();

		// プレイヤーの位置の取得
		D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();

		// 位置の取得
		D3DXVECTOR3 pos = pEnemy->GetPosition();

		// 吹き飛び処理
		float fAngle = pEnemy->GetMovement()->BlowOff(pos - PlayerPos, 50.0f, 5.0f);
		pEnemy->SetAngle(fAngle);
	}

	// モーションを最後まで行ったら
	if (pMotion->IsFinishEndBlend())
	{
		// バックステップするなら
		if (m_bBackStap)
		{
			// 状態の変更
			pEnemy->ChangeState(make_shared<CEnemyBackStep>());
			return;
		}
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

	// 状態マネージャーの取得
	auto pStateManager = pEnemy->GetStateManager();

	if (pMotion != nullptr)
	{
		// イベント状態だったら
		if (pStateManager->GetEvent())
		{
			// イベント
			pEnemy->ChangeState(make_shared<CEnemyEventRoar>());
			return;
		}

		// モーションが終わった後40%の確率で攻撃に移る
		if (pMotion->FinishMotion() && m_nNextAction <= 70)
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
		// 種類の遷移
		switch (m_type)
		{
		case TYPE_NORMAL:

			// プレイヤーの方向を見る
			pEnemy->AngleToPlayer();

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

	//// HPが半分以下だったら
	//if (pStateManager != nullptr && pStateManager->CheckLowHp(2))
	//{
	//	// 確率を求める
	//	int nAction = rand() % 100;

	//	// 攻撃をだせるなら
	//	if (nAction <= m_Const.PROB_ACTION)
	//	{
	//		// カウンター
	//		pEnemy->ChangeState(make_shared<CEnemyCounter>());
	//		return;
	//	}
	//}
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

	// 向きの取得
	float fAngle = pEnemy->GetRotation().y;

	if (pMotion != nullptr)
	{
		// モーションの種類の取得
		int motiontype = pMotion->GetBlendType();

		if (pMotion->IsEventFrame(1,50,CEnemy::MOTIONTYPE_HIT))
		{
			// 後ろに進む
			pEnemy->GetMovement()->SetMoveDir(m_Const.BACK_MOVE_VALUE.x, m_Const.BACK_MOVE_VALUE.y, fAngle);
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
			pEnemy->GetMovement()->MoveForWard(m_Const.FORWARD_MOVE_VALUE, fAngle + D3DX_PI);
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

	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pSound != nullptr)
	{
		// 音の再生
		pSound->Play(CSound::SOUND_LABEL_NORMAL);

		// 音の再生
		pSound->Play(CSound::SOUND_LABEL_ENEMY_DAMAGE);
	}

	// 位置の取得
	D3DXVECTOR3 pos = pEnemy->GetPosition();

	float fAngle = 0.0f;

	if (pMotion != nullptr)
	{
		switch (m_type)
		{
		case TYPE_NORMAL:
			if (pMovement != nullptr && pPlayer != nullptr)
			{
				// 吹き飛び処理
				fAngle = pMovement->BlowOff(pos - PlayerPos, 100.0f, 5.0f);
				pEnemy->SetAngle(fAngle);
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

	// 成功度の取得
	int nSuccess = pPlayer->SuccessParry();

	// 成功度の設定
	pPlayer->SetParryResult(nSuccess);
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

	// 状態マネージャーの取得
	auto pStateManager = pEnemy->GetStateManager();

	// モーションが終わったら
	if (pMotion != nullptr)
	{
		// モーションの種類の取得
		int nMotionType = pMotion->GetType();

		// HPが半分以下だったら
		if (pMotion->IsEventFrame(80, 80, CEnemy::MOTIONTYPE_DAMAGES) && pStateManager != nullptr && pStateManager->CheckLowHp(2))
		{
			// 確率
			int nAction = rand() % 100;

			if (nAction <= PROB_ACTION)
			{
				// カウンター
				pEnemy->ChangeState(make_shared<CEnemyCounter>());

				return;
			}
		}
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
}

//===================================================
// コンストラクタ(ガード)
//===================================================
CEnemyGuard::CEnemyGuard(const D3DXVECTOR3 ImpactPos, const int nDamage) : CEnemyState(ID_GUARD)
{
	m_nDamage = nDamage;
	m_ImpactPos = ImpactPos;
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
	auto pCircle = CMeshCircle::Create(D3DXCOLOR(1.0f, 0.4f, 0.4f, 0.8f), m_ImpactPos, 0.0f, 50.0f);

	// サークルの設定処理
	pCircle->SetCircle(-35.0f, 8.0f, 60, false, D3DXVECTOR3(D3DX_PI * 0.5f, fAngle, 0.0f));

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(CEnemy::MOTIONTYPE_GUARD, true, 2);
	}

	// 成功度の取得
	int nSuccess = pPlayer->SuccessParry();

	// 成功度の設定
	pPlayer->SetParryResult(nSuccess);
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

	// モーションが終わったら
	if (pMotion->FinishMotion())
	{
		// 攻撃モーションの選択
		int nAction = rand() % ACTION_MAX;

		// 距離をとる確率
		int nAway = rand() % 100;

		if (nAway <= PROB_AWAY)
		{
			// 距離を取る
			pEnemy->ChangeState(make_shared<CEnemyAway>());
			return;
		}

		// 種類の遷移
		switch (nAction)
		{
		case ACTION_SMASH:
			// 振り下ろしに派生する
			pEnemy->ChangeState(make_shared<CEnemyAttackSmash>());
			break;
		case ACTION_SWING:
			// 振りに派生する
			pEnemy->ChangeState(make_shared<CEnemySwing>());
			break;
		case ACTION_RUSH_SWING:
			// 振りに派生する
			pEnemy->ChangeState(make_shared<CEnemyRushSwing>());
			break;
		default:
			break;
		}
	}
}

//===================================================
// コンストラクタ(死亡)
//===================================================
CEnemyDeath::CEnemyDeath() : CEnemyState(ID_DEATH)
{
	m_type = TYPE_NORMAL;
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

	// 死亡状態にする
	pEnemy->SetState(CCharacter3D::STATE_DEATH, 99999);

	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pSound != nullptr)
	{
		// 音の再生
		pSound->Play(CSound::SOUND_LABEL_ENEMY_DEATH);
	}

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// 種類の遷移
		switch (m_type)
		{
		case TYPE_NORMAL:
			// モーションの再生
			pMotion->SetMotion(CEnemy::MOTIONTYPE_DEATH, true, 1);
			break;
		case TYPE_STANP:
			// モーションの再生
			pMotion->SetMotion(CEnemy::MOTIONTYPE_STANP_DEATH, true, 1);
			break;
		default:
			break;
		}
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

	// 向きの取得
	float fAngle = pEnemy->GetRotation().y;

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// 吹き飛び中だったら
		if (pMotion->IsEventFrame(1, 90, CEnemy::MOTIONTYPE_DEATH))
		{
			// 移動方向を設定
			pEnemy->GetMovement()->SetMoveDir(0.0f, 20.0f, fAngle);
		}
		// 音の取得
		CSound* pSound = CManager::GetSound();

		if (pMotion->IsEventFrame(82, 82, CEnemy::MOTIONTYPE_DEATH))
		{
			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_BLOW);
			}
		}

		if (pMotion->IsEventFrame(110, 110, CEnemy::MOTIONTYPE_DEATH))
		{
			pEnemy->ChangeState(make_shared<CEnemyDown>());
		}

		if (pMotion->IsEventFrame(210, 210, CEnemy::MOTIONTYPE_STANP_DEATH))
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
		// モーションの種類の取得
		int nMotiontype = pMotion->GetBlendType();

		if (nMotiontype == CEnemy::MOTIONTYPE_STANP_DEATH)
		{
			// モーションの再生
			pMotion->SetMotion(CEnemy::MOTIONTYPE_STANP_DOWN, true, 4);
		}
		else
		{
			// モーションの再生
			pMotion->SetMotion(CEnemy::MOTIONTYPE_DOWN, true, 4);
		}
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

	// プレイヤーの方向を見る
	pEnemy->AngleToPlayer();

	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pSound != nullptr)
	{
		// 音の再生
		pSound->Play(CSound::SOUND_LABEL_ENEMY_DAMAGE);
	}

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

	// 向きの取得
	float fAngle = pEnemy->GetRotation().y;

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		if (pMotion->IsEventFrame(1, 44, CEnemy::MOTIONTYPE_SUPER_HIT))
		{
			// 後ろ方向
			pEnemy->GetMovement()->SetMoveDir(0.0f, 2.0f, fAngle);
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
		pEffect->SetParam(CEffect3D::TYPE_NORAML, 0, 0.1f);

		// メッシュサークルの生成
		auto pMeshCircle = CMeshCircle::Create(D3DXCOLOR(1.0f, 1.0f, 0.4f, 0.5f), NewImpactPos, 10.0f, 50.0f);
		pMeshCircle->SetCircle(-50.0f, 8.0f, 30, false, D3DXVECTOR3(D3DX_PI * 0.5f, angle.y, 0.0f));

		// ヒット時の設定
		pEnemy->Hit(1);
	}
	if (pMotion->IsEventFrame(159, 159, CEnemy::MOTIONTYPE_COMBODAMAGE))
	{
		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_PERFECT);

			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_ENEMY_DAMAGE);
		}

		// メッシュサークルの生成
		auto pMeshCircle = CMeshCircle::Create(D3DXCOLOR(1.0f, 1.0f, 0.4f, 0.9f), NewImpactPos, 10.0f, 50.0f);
		pMeshCircle->SetCircle(-50.0f, 8.0f, 30, false, D3DXVECTOR3(D3DX_PI * 0.5f, angle.y, 0.0f));

		// エフェクトの生成
		auto pEffect = CParticle3DNormal::Create(ImpactPos, 20.0f, D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f));
		pEffect->SetParticle(35.0f, 360, 50, 1, 314);
		pEffect->SetParam(CEffect3D::TYPE_NORAML,0, 0.1f);

		// ヒット時の設定
		pEnemy->Hit(2);

		// 状態の変更
		pEnemy->ChangeState(make_shared<CEnemyDamageL>(0,false));
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
	m_fAngle = pEnemy->GetRotDest();

	// 今の角度から目的の角度までの距離を求める
	m_fDiffAngle = fAngle - m_fAngle;

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

		// 音の取得
		CSound* pSound = CManager::GetSound();

		if(pMotion->IsEventFrame(154,154, CEnemy::MOTIONTYPE_LOOK_BACKL) || pMotion->IsEventFrame(194, 194, CEnemy::MOTIONTYPE_LOOK_BACKL))
		{
			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_WARK003);
			}
		}
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
	m_fAngle = pEnemy->GetRotDest();

	// 今の角度から目的の角度までの距離を求める
	m_fDiffAngle = fAngle - m_fAngle;

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

		// 後ろを見るモーションじゃないなら飛ばす
		if (pMotion->GetBlendType() != CEnemy::MOTIONTYPE_LOOK_BACKR || pMotion->GetType() != CEnemy::MOTIONTYPE_LOOK_BACKR) return;

		// 音の取得
		CSound* pSound = CManager::GetSound();

		if (pMotion->IsEventFrame(155, 155, CEnemy::MOTIONTYPE_LOOK_BACKR) || pMotion->IsEventFrame(200, 200, CEnemy::MOTIONTYPE_LOOK_BACKR))
		{
			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_WARK003);
			}
		}

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
// コンストラクタ(HP半分時のイベント)
//===================================================
CEnemyEventRoar::CEnemyEventRoar() : CEnemyState(ID_EVENT_ROAR)
{
}

//===================================================
// デストラクタ(HP半分時のイベント)
//===================================================
CEnemyEventRoar::~CEnemyEventRoar()
{
}

//===================================================
// 初期化処理(HP半分時のイベント)
//===================================================
void CEnemyEventRoar::Init(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(CEnemy::MOTIONTYPE_ROAR, true, 10);
	}
}

//===================================================
// 更新処理(HP半分時のイベント)
//===================================================
void CEnemyEventRoar::Update(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 状態マネージャーの取得
	auto pStateManager = pEnemy->GetStateManager();

	// レンダラーの取得
	CRenderer* pRenderer = CManager::GetRenderer();

	// メッシュフィールドの取得
	CMeshField* pMeshField = CGame::GetField();

	if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_ROAR))
	{
		// 当たり判定
		CollisionPlayer(pEnemy);

		// 胸の位置の取得
		D3DXVECTOR3 chestPos = pEnemy->GetModelPos(CEnemy::MODEL_CHEST);

		// パーティクルの生成
		CParticle3DNormal *pParticle = CParticle3DNormal::Create(chestPos, 150.0f, D3DXCOLOR(1.0f, 0.4f, 1.0f, 1.0f));
		pParticle->SetParticle(100.0f, 300, 100, 50, 314);
		pParticle->SetParam(CEffect3D::TYPE_NORAML, 10, 0.01f);

		// 位置の取得
		D3DXVECTOR3 pos = pEnemy->GetPosition();

		// メッシュサークルの生成
		auto pCircle = CMeshCircle::Create(D3DXCOLOR(1.0f, 0.5f, 0.5f, 1.0f), pos, 0.0f, 35.0f);

		// サークルの設定処理
		pCircle->SetCircle(0.0f, 50.0f, 60, true);

		if (pMeshField != nullptr)
		{
			// フィールドの波の設定
			CMeshFieldWave::Config config = { pos,250.0f,380.0f,280.0f,12.0f,0.01f,120 };

			// フィールドの波
			pMeshField->SetWave(config);
		}
		// 音の取得
		CSound* pSound = CManager::GetSound();

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

	// 取得できなかったら処理しない
	if (pMotion == nullptr) return;
	
	// モーションが終わったら
	if (pMotion->FinishMotion())
	{
		if (pRenderer != nullptr)
		{
			// ブラーをオン
			pRenderer->offEffect();
		}
	}
	// モーションが終わったら(ブレンドまで)
	else if (pMotion->IsFinishEndBlend() && pStateManager != nullptr)
	{
		// 攻撃を選出
		int nAction = rand() % 3;

		// 種類の遷移
		switch (nAction)
		{
		case 0:
			// 衝撃波攻撃
			pEnemy->ChangeState(make_shared<CEnemyAttackImpact>());
			break;
		case 1:
			// 衝撃波攻撃
			pEnemy->ChangeState(make_shared<CEnemyRush>());
			break;
		case 2:
			// 衝撃波攻撃
			pEnemy->ChangeState(make_shared<CEnemyJumpAttack>());
			break;
		default:
			break;
		}

		// イベント終了
		pStateManager->EndEvent();
	}
}

//===================================================
// プレイヤーとの当たり判定
//===================================================
void CEnemyEventRoar::CollisionPlayer(CEnemy* pEnemy)
{
	// 位置の取得
	D3DXVECTOR3 pos = pEnemy->GetPosition();

	// プレイヤーの取得
	CPlayer* pPlayer = CGame::GetPlayer();

	// 取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// 位置の取得
	D3DXVECTOR3 playerPos = pPlayer->GetPosition();

	// 距離の取得
	float fDistance = GetDistance(playerPos - pos);

	if (fDistance <= COLLISION_DISTANCE)
	{
		// ダメージモーションの設定
		pPlayer->SetDamageMotion(pos, 0,200.0f,200.0f);
	}
}
