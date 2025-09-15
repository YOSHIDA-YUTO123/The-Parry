//===================================================
//
// タイトルロゴ [TitleLogo.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "TitleLogo.h"
#include"math.h"

using namespace Const; // 名前空間Constを使用
using namespace math;  // 名前空間mathの使用

//***************************************************
// 定数宣言
//***************************************************
constexpr int FADE_TIME = 120;	// フェードイン時間

//===================================================
// コンストラクタ
//===================================================
CTitleLogo::CTitleLogo()
{
	m_fAddAlv = NULL;
	m_col = WHITE;
}

//===================================================
// デストラクタ
//===================================================
CTitleLogo::~CTitleLogo()
{
}

//===================================================
// 生成処理
//===================================================
CTitleLogo* CTitleLogo::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size)
{
	CTitleLogo* pLogo = nullptr;

	// タイトルロゴの生成
	pLogo = new CTitleLogo;

	if (FAILED(pLogo->Init()))
	{
		pLogo->Uninit();
		pLogo = nullptr;
		return nullptr;
	}

	pLogo->SetPosition(pos);
	pLogo->SetSize(Size.x, Size.y);
	pLogo->SetVtx(WHITE);

	return pLogo;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CTitleLogo::Init(void)
{
	// 初期化処理
	if (FAILED(CObject2D::Init()))
	{
		return E_FAIL;
	}

	// テクスチャのIDの設定
	SetTextureID("data/TEXTURE/title/title000.png");

	m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
	m_fAddAlv = 1.0f / FADE_TIME;

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CTitleLogo::Uninit(void)
{
	// 終了処理
	CObject2D::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CTitleLogo::Update(void)
{
	// アルファ値を設定
	m_col.a += m_fAddAlv;

	// 範囲の制限
	m_col.a = Clamp(m_col.a, 0.0f, 1.0f);

	// 色の設定
	CObject2D::SetColor(m_col);
}

//===================================================
// 描画処理
//===================================================
void CTitleLogo::Draw(void)
{
	// 描画処理
	CObject2D::Draw();
}
