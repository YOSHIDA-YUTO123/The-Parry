//===================================================
//
// 反撃発動可能UI [RevengeActiveUI.cpp]
// Author:YUTO YOSHIDA
// 
// << 反撃発動可能時にUIを表示するクラス	>>
// 
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "RevengeActiveUI.h"
#include "tutorial.h"
#include "game.h"
#include "player.h"
#include "manager.h"

//===================================================
// コンストラクタ
//===================================================
CRevengeActiveUI::CRevengeActiveUI()
{
}

//===================================================
// デストラクタ
//===================================================
CRevengeActiveUI::~CRevengeActiveUI()
{
}

//===================================================
// 生成処理
//===================================================
CRevengeActiveUI* CRevengeActiveUI::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const int UV, const int HV)
{
	CRevengeActiveUI* pUI = new CRevengeActiveUI;

	pUI->SetPosition(pos);
	pUI->SetSize(Size);
	pUI->SetAnim(UV, HV);

	// 初期化処理
	if (FAILED(pUI->Init()))
	{
		pUI->Uninit();
		pUI = nullptr;
		return nullptr;
	}

	return pUI;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CRevengeActiveUI::Init(void)
{
	// 初期化処理
	if (FAILED(CObject2DAnimMT::Init()))
	{
		return E_FAIL;
	}


	// テクスチャのIDの設定
	CObject2DAnimMT::SetTextureID("UI/revenge_ok.png", "UI/fire.png");
	CObject2DAnimMT::SetAnim(4, true);

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CRevengeActiveUI::Uninit(void)
{
	// 終了処理
	CObject2DAnimMT::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CRevengeActiveUI::Update(void)
{
	// 更新処理
	CObject2DAnimMT::Update();
}

//===================================================
// 描画処理
//===================================================
void CRevengeActiveUI::Draw(void)
{
	// プレイヤーのクラスへのポインタ
	CPlayer* pPlayer = nullptr;

	// モードの取得
	CScene::MODE mode = CManager::GetMode();

	if (mode == CScene::MODE_TUTORIAL)
	{
		// プレイヤー取得
		pPlayer = CTutorial::GetPlayer();
	}
	else if (mode == CScene::MODE_GAME)
	{
		// プレイヤー取得
		pPlayer = CGame::GetPlayer();
	}

	// 反撃発動可能かどうか取得
	bool bRevenge = pPlayer->GetRevengeState();

	// 発動可能だったら
	if (bRevenge)
	{
		// 描画処理
		CObject2DAnimMT::Draw();
	}
}
