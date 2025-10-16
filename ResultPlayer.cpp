//================================================
//
// リザルトのプレイヤー [ResultPlayer.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "ResultPlayer.h"
#include "transform.h"

using namespace Const; // 名前空間Constの使用

//================================================
// コンストラクタ
//================================================
CResultPlayer::CResultPlayer()
{
}

//================================================
// デストラクタ
//================================================
CResultPlayer::~CResultPlayer()
{
}

//================================================
// 生成処理
//================================================
CResultPlayer* CResultPlayer::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot)
{
	CResultPlayer* pPlayer = new CResultPlayer;

	// 初期化処理
	if (FAILED(pPlayer->Init()))
	{
		pPlayer->Uninit();
		pPlayer = nullptr;
		return nullptr;
	}

	pPlayer->SetPosition(pos);
	pPlayer->SetRotation(rot);

	return pPlayer;
}

//================================================
// 初期化処理
//================================================
HRESULT CResultPlayer::Init(void)
{
	// 初期化処理
	if (FAILED(CCharacter3D::Init()))
	{
		return E_FAIL;
	}

	// モーションの読み込み
	CCharacter3D::LoadMotion("motionResultPlayer.txt", MOTIONTYPE_MAX);

	// キャラクターの設定
	CCharacter3D::SetCharacter(-1, 0.0f, D3DXVECTOR3(5.0f, 1.0f, 5.0f), VEC3_NULL);

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CResultPlayer::Uninit(void)
{
	// 終了処理
	CCharacter3D::Uninit();
}

//================================================
// 更新処理
//================================================
void CResultPlayer::Update(void)
{
	// モーションの更新処理
	CCharacter3D::UpdateMotion();

	// 更新処理
	CCharacter3D::Update();
}

//===============================================
// 描画処理
//===============================================
void CResultPlayer::Draw(void)
{
	// 描画処理
	CCharacter3D::Draw();
}
