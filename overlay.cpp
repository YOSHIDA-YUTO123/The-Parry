//================================================
//
// オーバーレイ [overlay.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "overlay.h"

using namespace Const; // 名前空間Constを使用

//================================================
// コンストラクタ
//================================================
COverlay::COverlay() : CObject2D(4)
{
    m_nShowTime = NULL;
    m_col = WHITE;
}

//================================================
// デストラクタ
//================================================
COverlay::~COverlay()
{
}

//================================================
// 生成処理
//================================================
COverlay* COverlay::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const int nShowTime)
{
	COverlay* pOverlay = nullptr;

	// オーバーレイの生成
	pOverlay = new COverlay;

	if (pOverlay == nullptr) return nullptr;

    pOverlay->m_col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	pOverlay->Init();
	pOverlay->SetPosition(pos);
	pOverlay->SetSize(Size.x, Size.y);
	pOverlay->SetVtx(pOverlay->m_col);
    pOverlay->m_nShowTime = nShowTime;

	return pOverlay;
}

//================================================
// 初期化処理
//================================================
HRESULT COverlay::Init(void)
{
    // 初期化処理
    if (FAILED(CObject2D::Init()))
    {
        return E_FAIL;
    }

    return S_OK;
}

//================================================
// 終了処理
//================================================
void COverlay::Uninit(void)
{
    // 終了処理
    CObject2D::Uninit();
}

//================================================
// 更新処理
//================================================
void COverlay::Update(void)
{
    m_nShowTime--;

    // 使用時間が終了したら
    if (m_nShowTime <= 0)
    {
        // 消す
        Uninit();

        return;
    }
}

//================================================
// 描画処理
//================================================
void COverlay::Draw(void)
{
    // 描画処理
    CObject2D::Draw();
}
