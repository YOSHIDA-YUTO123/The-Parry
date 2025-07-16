//===================================================
//
// フェード処理 [fade.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "fade.h"
#include"manager.h"

using namespace Const; // 名前空間Constを使用

//===================================================
// コンストラクタ
//===================================================
CFade::CFade() : CObject2D(7)
{
	m_pScene = nullptr;
	m_Fade = FADE_NONE;
	m_col = D3DXCOLOR(0.0f, 0.0, 0.0f, 0.0f);
}

//===================================================
// デストラクタ
//===================================================
CFade::~CFade()
{
}

//===================================================
// 生成処理
//===================================================
CFade* CFade::Create(void)
{
	// フェードの生成
	CFade* pFade = new CFade;

	if (FAILED(pFade->Init()))
	{
		pFade->Uninit();

		pFade = nullptr;

		return nullptr;
	}

	return pFade;
}

//===================================================
// フェード
//===================================================
void CFade::SetFade(CScene* pNewScene)
{
	m_Fade = FADE_OUT;
	m_pScene = pNewScene;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CFade::Init(void)
{
	if (FAILED(CObject2D::Init()))
	{
		return E_FAIL;
	}

	// 位置の設定
	CObject2D::SetPosition(D3DXVECTOR3(640.0f, 360.0f, 0.0f));

	// 大きさの設定
	CObject2D::SetSize(SCREEN_WIDTH * 0.5f,SCREEN_HEIGHT * 0.5f);

	// 頂点の設定
	CObject2D::SetOffsetVtx(m_col, 1, 1);

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CFade::Uninit(void)
{
	// 終了処理
	CObject2D::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CFade::Update(void)
{
	if (m_Fade != FADE_NONE)
	{
		if (m_Fade == FADE_IN)
		{
			m_col.a -= 0.03f;

			if (m_col.a <= 0.0f)
			{
				m_col.a = 0.0f;
				m_Fade = FADE_NONE;
			}

			// 色の設定
			CObject2D::SetColor(m_col);
		}
		else if (m_Fade == FADE_OUT)
		{
			// フェードアウト状態
			m_col.a += 0.03f;

			if (m_col.a >= 1.0f)
			{
				m_col.a = 1.0f;
				m_Fade = FADE_IN;

				// モード設定
				CManager::SetMode(m_pScene);
				return;
			}

			// 色の設定
			CObject2D::SetColor(m_col);
		}
	}
}

//===================================================
// 描画処理
//===================================================
void CFade::Draw(void)
{
	CObject2D::Draw();
}

