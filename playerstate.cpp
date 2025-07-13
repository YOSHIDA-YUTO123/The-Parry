//===================================================
//
// プレイヤーの状態 [playerstate.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "playerstate.h"
#include"player.h"
#include"motion.h"

using MOTION = CPlayer::TYPE; // プレイヤーの列挙型の使用
using namespace std;		  // 名前空間stdの使用

//===================================================
// コンストラクタ
//===================================================
CPlayerState::CPlayerState()
{
	m_pPlayer = nullptr;
}

//===================================================
// デストラクタ
//===================================================
CPlayerState::~CPlayerState()
{
}

//===================================================
// コンストラクタ(Normla)
//===================================================
CPlayerNormal::CPlayerNormal()
{
}

//===================================================
// デストラクタ(Normla)
//===================================================
CPlayerNormal::~CPlayerNormal()
{
}

//===================================================
// 更新処理(Normla)
//===================================================
void CPlayerNormal::Update(void)
{
}

//===================================================
// コンストラクタ(Move)
//===================================================
CPlayerMove::CPlayerMove()
{
}

//===================================================
// デストラクタ(Move)
//===================================================
CPlayerMove::~CPlayerMove()
{
}

//===================================================
// 更新処理(Move)
//===================================================
void CPlayerMove::Update(void)
{

}

//===================================================
// コンストラクタ(Damage)
//===================================================
CPlayerDamage::CPlayerDamage()
{
}

//===================================================
// デストラクタ(Damage)
//===================================================
CPlayerDamage::~CPlayerDamage()
{
}

//===================================================
// 初期化処理(Damage)
//===================================================
void CPlayerDamage::Init(void)
{
	// モーションの取得
	CMotion* pMotion = m_pPlayer->GetMotion();

	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(MOTION::TYPE_DAMAGE, true, 5);
	}
}

//===================================================
// 更新処理(Damage)
//==================================================
void CPlayerDamage::Update(void)
{
	// モーションの取得
	CMotion* pMotion = m_pPlayer->GetMotion();

	if (pMotion->GetType() == MOTION::TYPE_DAMAGE)
	{
		// モーションが終わったら
		if (pMotion != nullptr && pMotion->FinishMotion())
		{
			m_pPlayer->ChangeState(make_shared<CPlayerDownNeutral>());
		}
	}
}

//===================================================
// コンストラクタ(DownNeutral)
//===================================================
CPlayerDownNeutral::CPlayerDownNeutral()
{
}

//===================================================
// デストラクタ(DownNeutral)
//===================================================
CPlayerDownNeutral::~CPlayerDownNeutral()
{
}

//===================================================
// 初期化処理(DownNeutral)
//===================================================
void CPlayerDownNeutral::Init(void)
{
	// モーションの取得
	CMotion* pMotion = m_pPlayer->GetMotion();

	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(MOTION::TYPE_DOWN_NEUTRAL, false, 5);
	}
}

//===================================================
// 更新処理(DownNeutral)
//===================================================
void CPlayerDownNeutral::Update(void)
{

}

//===================================================
// コンストラクタ(回避)
//===================================================
CPlayerAvoid::CPlayerAvoid(const float fSpeed)
{
	m_fSpeed = fSpeed;
}

//===================================================
// デストラクタ(回避)
//===================================================
CPlayerAvoid::~CPlayerAvoid()
{
}

//===================================================
// 初期化処理(回避)
//===================================================
void CPlayerAvoid::Init(void)
{
	// モーションの取得
	CMotion* pMotion = m_pPlayer->GetMotion();

	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(MOTION::TYPE_AVOID, false, 5);
	}
}

//===================================================
// 更新処理(回避)
//===================================================
void CPlayerAvoid::Update(void)
{
	// モーションの取得
	CMotion* pMotion = m_pPlayer->GetMotion();

	m_pPlayer->MoveForward(m_fSpeed);

	if (pMotion->FinishMotion())
	{
		m_pPlayer->ChangeState(make_shared<CPlayerNormal>());
		pMotion->SetMotion(m_pPlayer->TYPE_NEUTRAL, true, 5);

		return;
	}
}
