//===================================================
//
// フェードエフェクト [FadeEffect.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "FadeEffect.h"

//===================================================
// コンストラクタ
//===================================================
CFadeEffect::CFadeEffect()
{
	m_Fade = FADE_NONE;
	m_col = D3DXCOLOR(1.0f, 1.0, 1.0f, 0.0f);
}

//===================================================
// デストラクタ
//===================================================
CFadeEffect::~CFadeEffect()
{
}

//===================================================
// 生成処理
//===================================================
CFadeEffect* CFadeEffect::Create(const FADE fade)
{
	CFadeEffect* pFade = new CFadeEffect;

	pFade->SetPosition(D3DXVECTOR3(640.0f, 360.0f, 0.0f));

	pFade->m_Fade = fade;

	// 初期化処理
	if (FAILED(pFade->Init()))
	{
		// フェードの破棄
		pFade->Uninit();
		pFade = nullptr;
		return nullptr;
	}

	pFade->SetSize(640.0f, 360.0f);
	pFade->SetVtx(pFade->m_col);

	return pFade;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CFadeEffect::Init(void)
{
	// 初期化処理
	if (FAILED(CObject2D::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CFadeEffect::Uninit(void)
{
	// 終了処理
	CObject2D::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CFadeEffect::Update(void)
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
		}
		else if (m_Fade == FADE_OUT)
		{
			// フェードアウト状態
			m_col.a += 0.03f;

			if (m_col.a >= 1.0f)
			{
				m_col.a = 1.0f;
				m_Fade = FADE_NONE;

				return;
			}
		}

		// 色の設定
		CObject2D::SetColor(m_col);
	}
}

//===================================================
// 描画処理
//===================================================
void CFadeEffect::Draw(void)
{
	// 描画処理
	CObject2D::Draw();
}
