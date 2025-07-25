//===================================================
//
// タイトルシーン [title.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "title.h"
#include"meshfield.h"
#include"manager.h"
#include"input.h"
#include"game.h"
#include"fade.h"
//#include "object2DMT.h"

using namespace Const; // 名前空間Constを使用
using namespace std;   // 名前空間stdを使用

//===================================================
// コンストラクタ
//===================================================
CTitle::CTitle() : CScene(MODE_TITLE)
{

}

//===================================================
// デストラクタ
//===================================================
CTitle::~CTitle()
{

}

//===================================================
// 初期化処理
//===================================================
HRESULT CTitle::Init(void)
{
	// フィールドの設定
	CMeshField::Create(VEC3_NULL, 48, 48, D3DXVECTOR2(5500.0f, 5500.0f));

	//auto pObj2D = CObject2DMT::Create(D3DXVECTOR3(640.0f, 360.0f, 0.0f), D3DXVECTOR2(350.0f, 150.0f));
	//pObj2D->SetTextureID("data/TEXTURE/GageFrame/playerHpFrame.png", "data/TEXTURE/gradation/wave000.png");

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CTitle::Uninit(void)
{

}

//===================================================
// 更新処理
//===================================================
void CTitle::Update(void)
{
	// キーボードの取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	if (pKeyboard->GetTrigger(DIK_RETURN))
	{
		CFade* pFade = CManager::GetFade();

		// 新しいモードの設定
		pFade->SetFade(make_unique<CGame>());
	}
}

//===================================================
// 描画処理
//===================================================
void CTitle::Draw(void)
{

}
