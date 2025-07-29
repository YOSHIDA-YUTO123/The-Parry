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
#include "MoveSmoke.h"

using MOTION = CPlayer::TYPE; // プレイヤーの列挙型の使用
using namespace std;		  // 名前空間stdの使用
using namespace Const;		  // 名前空間Constの使用

//===================================================
// コンストラクタ
//===================================================
CPlayerState::CPlayerState(ID Id)
{
	m_ID = Id;
	m_pPlayer = nullptr;
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
void CPlayerState::SetOwner(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
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
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	pPlayer->Hit(m_nDamage);

	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(MOTION::TYPE_DAMAGE, true, 2);
	}
}

//===================================================
// 更新処理(Damage)
//==================================================
void CPlayerDamage::Update(void)
{
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	if (pMotion->GetType() == MOTION::TYPE_DAMAGE)
	{
		// モーションが終わったら
		if (pMotion != nullptr && pMotion->FinishMotion())
		{
			pPlayer->ChangeState(make_shared<CPlayerDownNeutral>());
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
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

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
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

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
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	// 移動制御の取得
	auto pMoveMent = pPlayer->GetMovement();

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// 移動クラスの取得
		if (pMoveMent != nullptr && pMotion->IsEventFrame(1,15,CPlayer::TYPE_AVOID))
		{
			// 向いている方向に進む処理
			pMoveMent->MoveForward(m_fSpeed);
		}

		// モーションが終わったら
		if (pMotion->FinishMotion())
		{
			pPlayer->ChangeState(make_shared<CPlayerNormal>());

			pMotion->SetMotion(pPlayer->TYPE_NEUTRAL, true, 5);

			return;
		}
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
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

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
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	// モーションがあるなら
	if (pMotion != nullptr)
	{
		// 位置の取得
		D3DXVECTOR3 pos = pPlayer->GetPosition();

		if (pMotion->IsEventFrame(1, 10, pPlayer->TYPE_ROUNDKICK))
		{
			// 向いている方向に進む
			pPlayer->GetMovement()->MoveForward(10.0f);
		}

		if (pMotion->IsEventFrame(15, 15, pPlayer->TYPE_ROUNDKICK))
		{
			// 移動量の設定
			pPlayer->GetMovement()->Set(D3DXVECTOR3(0.0f, 18.0f, 0.0f));

			// サークルの生成
			auto pCircle = CMeshCircle::Create(D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f), pos, 0.0f, 20.0f);
			pCircle->SetCircle(0.0f, 10.0f, 60, true);
		}

		if (pMotion->IsEventFrame(15, 17, pPlayer->TYPE_ROUNDKICK))
		{
			// エフェクトの生成
			auto pEffect = CMoveSmoke::Create(D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z), 100.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

			// エフェクトの設定処理
			pEffect->SetEffect(35, VEC3_NULL);
		}

		// モーションが終わったら
		if (pMotion->FinishMotion())
		{
			pPlayer->ChangeState(make_shared<CPlayerNormal>());
		}
	}
}

//===================================================
// コンストラクタ(ダッシュ)
//===================================================
CPlayerDash::CPlayerDash() : CPlayerState(ID_DASH)
{
}

//===================================================
// デストラクタ(ダッシュ)
//===================================================
CPlayerDash::~CPlayerDash()
{
}

//===================================================
// 初期化処理(ダッシュ)
//===================================================
void CPlayerDash::Init(void)
{
}

//===================================================
// 更新処理(ダッシュ)
//===================================================
void CPlayerDash::Update(void)
{
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	// 向きの取得
	D3DXVECTOR3 rot = pPlayer->GetRotaition()->Get();

	// プレイヤーの後ろ方向を設定
	float fMoveX = sinf(rot.y) * 2.0f;
	float fMoveZ = cosf(rot.y) * 2.0f;

	if (pMotion != nullptr)
	{
		// 10フレーム目になったら
		if (pMotion->IsEventFrame(10, 10, CPlayer::TYPE_DASH))
		{
			// 位置の取得
			D3DXVECTOR3 pos = pPlayer->GetModelPos(11);

			// エフェクトの生成
			auto pEffect = CMoveSmoke::Create(D3DXVECTOR3(pos.x,pos.y,pos.z), 100.0f, WHITE);

			// エフェクトの設定処理
			pEffect->SetEffect(60, D3DXVECTOR3(fMoveX, 0.0f, fMoveZ));
		}
		// 30フレーム目になったら
		else if (pMotion->IsEventFrame(30, 30, CPlayer::TYPE_DASH))
		{
			// 位置の取得
			D3DXVECTOR3 pos = pPlayer->GetModelPos(14);

			// エフェクトの生成
			auto pEffect = CMoveSmoke::Create(D3DXVECTOR3(pos.x, pos.y, pos.z), 100.0f, WHITE);

			// エフェクトの設定処理
			pEffect->SetEffect(60, D3DXVECTOR3(fMoveX, 0.0f, fMoveZ));
		}
	}
}

//===================================================
// コンストラクタ(ジャンプ)
//===================================================
CPlayerJump::CPlayerJump() : CPlayerState(ID_JUMP)
{

}

//===================================================
// デストラクタ(ジャンプ)
//===================================================
CPlayerJump::~CPlayerJump()
{
}

//===================================================
// 初期化処理(ジャンプ)
//===================================================
void CPlayerJump::Init(void)
{
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	if (pMotion != nullptr)
	{
		// モーションの再生
		pMotion->SetMotion(MOTION::TYPE_JUMP, true, 2);
	}
}

//===================================================
// 更新処理(ジャンプ)
//===================================================
void CPlayerJump::Update(void)
{

}

//===================================================
// コンストラクタ(着地)
//===================================================
CPlayerLanding::CPlayerLanding() : CPlayerState(ID_LANDING)
{

}

//===================================================
// デストラクタ(着地)
//===================================================
CPlayerLanding::~CPlayerLanding()
{
}

//===================================================
// 初期化処理(着地)
//===================================================
void CPlayerLanding::Init(void)
{
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	if (pMotion != nullptr)
	{
		// 着地モーションの再生
		pMotion->SetMotion(MOTION::TYPE_LANDING, true, 5);
	}

	// 位置の取得
	D3DXVECTOR3 pos = pPlayer->GetPosition();

	// サークルを生成
	auto pCircle = CMeshCircle::Create(D3DCOLOR_RGBA(220, 220, 220, 200), D3DXVECTOR3(pos.x, pos.y + 3.0f, pos.z), 0.0f, 50.0f, 32);

	// サークルの設定
	pCircle->SetCircle(0.0f, 10.0f, 30, true);
}

//===================================================
// 更新処理(着地)
//===================================================
void CPlayerLanding::Update(void)
{
	// プレイヤーの取得
	auto pPlayer = GetPlayer();

	// モーションの取得
	CMotion* pMotion = pPlayer->GetMotion();

	if (pMotion != nullptr)
	{
		// モーションが終わったら
		if (pMotion->IsFinishEndBlend())
		{
			pPlayer->ChangeState(make_shared<CPlayerNormal>());
		}
	}
}
