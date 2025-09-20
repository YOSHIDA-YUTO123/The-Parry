//===================================================
//
// オープニングのスキップUIを表示する [SkipUI.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "SkipUI.h"
#include "object2D.h"

using namespace Const; // 名前空間Constの使用

namespace
{
	constexpr float FLASH_SPEED = 0.01f; // 点滅時間
}

//===================================================
// コンストラクタ
//===================================================
CSkipUI::CSkipUI()
{
	m_fFlashCounter = NULL;
}

//===================================================
// デストラクタ
//===================================================
CSkipUI::~CSkipUI()
{
}

//===================================================
// 生成処理
//===================================================
CSkipUI* CSkipUI::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const int nTime)
{
	CSkipUI* pSkip = new CSkipUI;

	pSkip->SetPosition(pos);
	pSkip->SetAddAvl(nTime);

	// 初期化処理
	if (FAILED(pSkip->Init()))
	{
		pSkip->Uninit();
		pSkip = nullptr;
		return nullptr;
	}

	pSkip->SetSize(Size.x, Size.y);
	pSkip->SetVtx(WHITE);

	return pSkip;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CSkipUI::Init(void)
{
	// 初期化処理
	if (FAILED(CFadeInObject2D::Init()))
	{
		return E_FAIL;
	}

	// テクスチャのIDの設定
	CObject2D::SetTextureID("data/TEXTURE/UI/skip.png");

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CSkipUI::Uninit(void)
{
	// 終了処理
	CFadeInObject2D::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CSkipUI::Update(void)
{
	// 更新処理
	CFadeInObject2D::Update();

	// フェードが終わったかどうか
	bool bFinish = CFadeInObject2D::GetFinish();

	if (bFinish)
	{
		// カウンターを進める
		m_fFlashCounter += FLASH_SPEED;

		// 色の取得
		D3DXCOLOR col = CFadeInObject2D::GetColor();

		// 点滅
		col.a = 1.0f - fabsf(sinf(m_fFlashCounter) * 0.5f);

		// 色の設定
		CFadeInObject2D::SetColor(col);
	}
}

//===================================================
// 描画処理
//===================================================
void CSkipUI::Draw(void)
{
	// 描画処理
	CFadeInObject2D::Draw();
}
