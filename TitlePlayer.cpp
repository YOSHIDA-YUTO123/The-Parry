//===================================================
//
// タイトルのプレイヤー [TitlePlayer.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "TitlePlayer.h"
#include "transform.h"
#include "player.h"
#include "TitleMenu.h"
#include "playerstate.h"
#include "motion.h"
#include "PlayerMovement.h"
#include "transform.h"

using namespace std; // 名前空間stdを使用

//===================================================
// コンストラクタ
//===================================================
CTitlePlayer::CTitlePlayer() : CCharacter3D(TYPE_TITLEPLAYER)
{
	m_pMoveMent = nullptr;
}

//===================================================
// デストラクタ
//===================================================
CTitlePlayer::~CTitlePlayer()
{
}

//===================================================
// 生成処理
//===================================================
CTitlePlayer* CTitlePlayer::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot)
{
	// プレイヤーの生成
	auto pPlayer = new CTitlePlayer;

	// プレイヤーの初期化
	pPlayer->Init();

	pPlayer->SetPosition(pos);
	pPlayer->SetRotation(rot);
	pPlayer->SetRotDest(rot.y);

	return pPlayer;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CTitlePlayer::Init(void)
{
	// キャラクタークラスへのポインタ
	CCharacter3D::LoadMotion("motionTitlePlayer.txt", MOTIONTYPE_MAX);

	// 初期化処理
	CCharacter3D::Init();

	// 移動制御の生成
	m_pMoveMent = make_unique<CPlayerMovement>();
	m_pMoveMent->Init();

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CTitlePlayer::Uninit(void)
{
	// 移動クラスの破棄
	if (m_pMoveMent != nullptr)
	{
		m_pMoveMent->Uninit();
		m_pMoveMent = nullptr;
	}

	// 終了処理
	CCharacter3D::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CTitlePlayer::Update(void)
{
	// 位置の取得
	D3DXVECTOR3 pos = CCharacter3D::GetPosition();

	if (m_pMoveMent != nullptr)
	{
		// 位置の更新処理
		m_pMoveMent->UpdatePosition(&pos, nullptr);
	}

	// タイトルのマネージャーの取得
	auto pTitleMenuManager = CTitleMenuManager::GetInstance();

	// スタートボタンを押したら
	if (pTitleMenuManager != nullptr && pTitleMenuManager->CheckStart())
	{
		// モーションの取得
		CMotion* pMotion = CCharacter3D::GetMotion();

		pMotion->SetMotion(MOTIONTYPE_DASH, true, 5);

		if (m_pMoveMent != nullptr)
		{
			// 向きの取得
			float fAngle = CCharacter3D::GetRotation().y;

			m_pMoveMent->MoveForward(10.0f, fAngle + D3DX_PI);
		}
	}

	// モーションの更新処理
	CCharacter3D::UpdateMotion();

	// 更新処理
	CCharacter3D::Update();

	// 位置の設定
	CCharacter3D::SetPosition(pos);
}

//===================================================
// 描画処理
//===================================================
void CTitlePlayer::Draw(void)
{
	// 描画処理
	CCharacter3D::Draw();
}
