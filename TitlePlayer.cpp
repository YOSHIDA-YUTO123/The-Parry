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
#include"transform.h"
#include"player.h"

//===================================================
// コンストラクタ
//===================================================
CTitlePlayer::CTitlePlayer()
{
	m_pMove = nullptr;
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
	pPlayer->GetRotaition()->Set(rot);
	pPlayer->GetRotaition()->SetDest(rot);

	return pPlayer;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CTitlePlayer::Init(void)
{
	// キャラクタークラスへのポインタ
	CCharacter3D::LoadMotion("data/MOTION/motionPlayer.txt", TYPE_MAX);

	// 初期化処理
	CCharacter3D::Init();

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CTitlePlayer::Uninit(void)
{
	// 終了処理
	CCharacter3D::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CTitlePlayer::Update(void)
{
	// モーションの更新処理
	CCharacter3D::UpdateMotion();

	// 更新処理
	CCharacter3D::Update();
}

//===================================================
// 描画処理
//===================================================
void CTitlePlayer::Draw(void)
{
	// 描画処理
	CCharacter3D::Draw();
}
