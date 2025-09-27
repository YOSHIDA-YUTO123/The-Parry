//===================================================
//
// 石碑のUI [MonumentUI.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "MonumentUI.h"

//***************************************************
// 名前空間
//***************************************************
using namespace Const; // 名前空間Constの使用

//***************************************************
// 定数宣言
//***************************************************
constexpr float WIDTH = 120.0f;  // 横幅
constexpr float HEIGHT = 60.0f; // 縦幅
constexpr int FADE_TIME = 60;	 // フェードインする時間

//===================================================
// コンストラクタ
//===================================================
CMonumentUI::CMonumentUI()
{
	m_col = WHITE;
	m_fFlashTime = NULL;
	m_bShow = false;
}

//===================================================
// デストラクタ
//===================================================
CMonumentUI::~CMonumentUI()
{
}

//===================================================
// 生成処理
//===================================================
CMonumentUI* CMonumentUI::Create(const D3DXVECTOR3 pos)
{
	CMonumentUI* pMonumentUI = new CMonumentUI;

	// 位置の設定
	pMonumentUI->SetPosition(pos);

	// 初期化処理
	if (FAILED(pMonumentUI->Init()))
	{
		pMonumentUI->Uninit();
		pMonumentUI = nullptr;
		return nullptr;
	}

	return pMonumentUI;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CMonumentUI::Init(void)
{
	// 初期化処理
	if (FAILED(CObject2D::Init()))
	{
		return E_FAIL;
	}

	// 設定処理
	CObject2D::SetSize(WIDTH, HEIGHT);
	CObject2D::SetVtx(WHITE);

	// テクスチャのIDの設定
	CObject2D::SetTextureID("data/TEXTURE/UI/monumentUI000.png");

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CMonumentUI::Uninit(void)
{
	// 終了処理
	CObject2D::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CMonumentUI::Update(void)
{
	m_fFlashTime += 0.02f;

	// 色の点滅
	m_col.a = 1.0f - fabsf(sinf(m_fFlashTime));

	// 色の設定
	CObject2D::SetColor(m_col);

	// 更新処理
	CObject2D::Update();
}

//===================================================
// 描画処理
//===================================================
void CMonumentUI::Draw(void)
{
	// 表示状態だったら
	if (m_bShow)
	{
		// 描画処理
		CObject2D::Draw();
	}
}
