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
#include "debugproc.h"
#include "impact.h"
#include "game.h"
#include "meshfield.h"

using MOTION = CPlayer::TYPE; // プレイヤーの列挙型の使用
using namespace std;		  // 名前空間stdの使用

//===================================================
// コンストラクタ
//===================================================
CPlayerState::CPlayerState(ID Id)
{
	m_ID = Id;
	m_pPlayer = nullptr;
	m_pCharacter = nullptr;
}

//===================================================
// デストラクタ
//===================================================
CPlayerState::~CPlayerState()
{
}

//===================================================
// オーナーの設定
//===================================================
void CPlayerState::SetOwner(CPlayer* pPlayer,CCharacter3D *pCaracter)
{
	m_pPlayer = pPlayer;
	m_pCharacter = pCaracter;
}

//===================================================
// コンストラクタ(Normla)
//===================================================
CPlayerNormal::CPlayerNormal() : CPlayerState(ID_NORMAL)
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
CPlayerMove::CPlayerMove() : CPlayerState(ID_MOVE)
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
CPlayerDamage::CPlayerDamage(int nDamage) : CPlayerState(ID_DAMAGE)
{
	m_nDamage = nDamage;
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

	m_pCharacter->Hit(m_nDamage);

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
CPlayerDownNeutral::CPlayerDownNeutral() : CPlayerState(ID_DOWNNEUTRAL)
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
CPlayerAvoid::CPlayerAvoid(const float fSpeed) : CPlayerState(ID_AVOID)
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

	// 移動制御の取得
	auto pMoveMent = m_pPlayer->GetMovement();

	// 移動クラスの取得
	if (pMoveMent != nullptr)
	{
		// 向いている方向に進む処理
		pMoveMent->MoveForward(m_fSpeed);
	}

	// モーションが終わったら
	if (pMotion->FinishMotion())
	{
		m_pPlayer->ChangeState(make_shared<CPlayerNormal>());

		pMotion->SetMotion(m_pPlayer->TYPE_NEUTRAL, true, 5);

		return;
	}
}

//===================================================
// コンストラクタ(回し蹴り)
//===================================================
CPlayerRoundKick::CPlayerRoundKick() : CPlayerState(ID_ROUNDKICK)
{

}

//===================================================
// デストラクタ(回し蹴り)
//===================================================
CPlayerRoundKick::~CPlayerRoundKick()
{
}

//===================================================
// 初期化処理(回し蹴り)
//===================================================
void CPlayerRoundKick::Init(void)
{
	// モーションの取得
	CMotion* pMotion = m_pPlayer->GetMotion();

	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(MOTION::TYPE_ROUNDKICK,true, 2);
	}
}

//===================================================
// 更新処理(回し蹴り)
//===================================================
void CPlayerRoundKick::Update(void)
{
	// モーションの取得
	CMotion* pMotion = m_pPlayer->GetMotion();

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		if (pMotion->IsEventFrame(15, 15, m_pPlayer->TYPE_ROUNDKICK))
		{
			// 移動量の設定
			m_pPlayer->GetMovement()->Set(D3DXVECTOR3(0.0f, 15.0f, 0.0f));

			// 位置の取得
			D3DXVECTOR3 pos = m_pPlayer->GetPos();

			// サークルの生成
			auto pCircle = CMeshCircle::Create(D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f), pos, 0.0f, 20.0f);
			pCircle->SetCircle(0.0f, 10.0f, 60, true);

			CMeshField* pMeshField = CGame::GetField();

			CMeshFieldWave::Config config = { pos ,30.0f,50.0f,180.0f,20.0f,0.01f,120 };
			pMeshField->SetWave(config);
		}

		// モーションが終わったら
		if (pMotion->FinishMotion())
		{
			m_pPlayer->ChangeState(make_shared<CPlayerNormal>());
		}
	}
}
