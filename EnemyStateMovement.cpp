//
// 敵の移動状態の処理 [EnemyStateMovemet.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "EnemyStateMovement.h"
#include "math.h"
#include "enemy.h"
#include "EnemyStateAttack.h"
#include "motion.h"
#include "sound.h"
#include "manager.h"
#include "EnemyMovement.h"
#include "enemy.h"
#include "transform.h"

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
	constexpr float AWAY_TIME = 24.0f;				// ジャンプする時間
	constexpr int MAX_AWAYPOS_X = 500;				// 最大の離れる位置X
	constexpr int MAX_AWAYPOS_Z = 500;				// 最大の離れる位置Z
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
void CEnemyMove::ElectinMove(CEnemy* pEnemy)
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

	// 向きの取得
	float fAngle = pEnemy->GetRotation().y;

	pEnemy->GetMovement()->SetMoveDir(0.0f, 10.0f, fAngle);

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	if (pMotion != nullptr)
	{
		// モーションがおわったら
		if (pMotion->IsFinishEndBlend())
		{
			// 状態の遷移
			pEnemy->ChangeState(make_shared<CEnemyIdle>(5));
		}
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

	if (pMotion != nullptr)
	{
		// 攻撃状態にする
		pEnemy->SetState(CCharacter3D::STATE_ACTION, 5);

		// ダッシュ
		pMotion->SetMotion(CEnemy::MOTIONTYPE_DASH, true, 10);

		if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_DASH))
		{
			// 音の取得
			CSound* pSound = CManager::GetSound();

			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_WARK003);
			}

		}
	}
	// プレイヤーを追いかける
	pEnemy->ChasePlayer(0.1f, 7.0f);

	// 一定の距離に入ったら
	if (pEnemy->CheckDistane(250.0f))
	{
		// 回転攻撃
		pEnemy->ChangeState(make_shared<CEnemySpin>(SPIN_TIME));

		return;
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

	// 向きの取得
	float fAngle = pEnemy->GetRotation().y;

	pEnemy->GetMovement()->MoveForWard(150.0f, fAngle + D3DX_PI);

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

	//// 距離を求める
	//float fDistance = GetDistance(m_pos - pos);

	//// 目標地点までの距離が1000以下だったら
	//if (fDistance <= 1000.0f)
	//{
	//	// ジャンプ攻撃に派生
	//	pEnemy->ChangeState(make_shared<CEnemyJumpAttack>());
	//	return;
	//}

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

	// 向きの取得
	float fAngle = pEnemy->GetRotation().y;

	// 移動量を設定する
	pEnemy->GetMovement()->MoveForWard(dir, fAngle + D3DX_PI);
}

//===================================================
// コンストラクタ(横移動)
//===================================================
CEnemySideMove::CEnemySideMove()
{
	m_fMoveAngle = NULL;
	m_fMoveValue = NULL;
	m_nTime = NULL;
}

//===================================================
// コンストラクタ(横移動)
//===================================================
CEnemySideMove::CEnemySideMove(ID id) : CEnemyState(id)
{
	m_fMoveAngle = NULL;
	m_fMoveValue = NULL;
	m_nTime = NULL;
}

//===================================================
// デストラクタ(横移動)
//===================================================
CEnemySideMove::~CEnemySideMove()
{
}

//===================================================
// 横歩きの初期化処理
//===================================================
void CEnemySideMove::Init(void)
{
	// 移動時間の設定
	m_nTime = rand() % 120 + MOVE_TIME;
}

//===================================================
// 更新処理処理(横移動)
//===================================================
void CEnemySideMove::Update(void)
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

	// 向きの取得
	float fAngle = pEnemy->GetRotation().y;

	// 右に移動する
	pEnemy->GetMovement()->SetMoveDir(m_fMoveAngle, m_fMoveValue, fAngle);

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
			return;
		}

		// 次の行動の選出
		int nAction = rand() % 3;

		// 次の行動の遷移
		switch (nAction)
		{
		case 0:
			// 状態の変更
			pEnemy->ChangeState(make_shared<CEnemyRushSwing>());
			break;
		case 1:
			// 状態の変更
			pEnemy->ChangeState(make_shared<CEnemyRoar>());
			break;
		case 2:
			// 状態の変更
			pEnemy->ChangeState(make_shared<CEnemyJumpAttack>());
			break;
		default:
			break;
		}
	}
}

//===================================================
// パラメータの設定処理
//===================================================
void CEnemySideMove::SetParam(const float fMoveAngle, const float fMoveValue)
{
	m_fMoveAngle = fMoveAngle;
	m_fMoveValue = fMoveValue;
}

//===================================================
// コンストラクタ(右横歩き)
//===================================================
CEnemyRightMove::CEnemyRightMove() : CEnemySideMove(ID_RIGHT_MOVE)
{
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
	// 初期化処理
	CEnemySideMove::Init();

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

	// パラメータの設定処理
	CEnemySideMove::SetParam(-D3DX_PI * 0.5f, 5.0f);
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

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

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

	// 状態の更新処理
	CEnemySideMove::Update();
}

//===================================================
// コンストラクタ(左横歩き)
//===================================================
CEnemyLeftMove::CEnemyLeftMove() : CEnemySideMove(ID_LEFT_MOVE)
{
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
	// 初期化処理
	CEnemySideMove::Init();

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

	// パラメータの設定処理
	CEnemySideMove::SetParam(D3DX_PI * 0.5f, 5.0f);
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

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

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

	// 状態の更新処理
	CEnemySideMove::Update();
}

//===================================================
// コンストラクタ(早歩き)
//===================================================
CEnemyBriskMove::CEnemyBriskMove() : CEnemyState(ID_BRISK_MOVE)
{
	m_destPos = VEC3_NULL;
}

//===================================================
// デストラクタ(早歩き)
//===================================================
CEnemyBriskMove::~CEnemyBriskMove()
{
}

//===================================================
// 初期化処理(早歩き)
//===================================================
void CEnemyBriskMove::Init(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 位置の取得
	D3DXVECTOR3 pos = pEnemy->GetPosition();

	m_destPos = VEC3_NULL;

	// 移動方向を求める
	float fAngle = GetTargetAngle(m_destPos, pos) + D3DX_PI;

	// 目的の方向を見る
	pEnemy->SetAngle(fAngle);

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(CEnemy::MOTIONTYPE_BRISK_MOVE, true, 10);
	}
}

//===================================================
// 更新処理(早歩き)
//===================================================
void CEnemyBriskMove::Update(void)
{
	// 敵の取得
	CEnemy* pEnemy = CEnemyState::GetEnemy();

	// 敵が使われていないなら処理しない
	if (pEnemy == nullptr) return;

	// モーションクラスの取得
	CMotion* pMotion = pEnemy->GetMotion();

	// 向きの取得
	float fAngle = pEnemy->GetRotation().y;

	pEnemy->SetState(CCharacter3D::STATE_ACTION,5);

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// モーションの再生
		if (pMotion->IsEventFrame(CEnemy::MOTIONTYPE_BRISK_MOVE))
		{
			// 音の取得
			CSound* pSound = CManager::GetSound();

			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_WARK003);
			}
		}

		// 移動する
		pEnemy->GetMovement()->MoveForWard(10.0f, fAngle + D3DX_PI);

		// 位置の取得
		D3DXVECTOR3 pos = pEnemy->GetPosition();

		// 目的の位置までの距離を求める
		float fDistance = GetDistance(m_destPos - pos);

		// 目的の場所に到達したら
		if (fDistance <= 100.0f)
		{
			// 状態の遷移
			pEnemy->ChangeState(make_shared<CEnemyIdle>(2));
		}
	}
}