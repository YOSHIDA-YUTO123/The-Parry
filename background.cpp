//================================================
//
// 背景 [background.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "background.h"

using namespace Const; // 名前空間Constの使用


//================================================
// コンストラクタ
//================================================
CBackGround::CBackGround()
{
	m_type = TYPE_PAUSE;

	m_col = WHITE;
}

//================================================
// デストラクタ
//================================================
CBackGround::~CBackGround()
{
}

//================================================
// 生成処理
//================================================
CBackGround* CBackGround::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size,const D3DXCOLOR col, const TYPE type)
{
	// 背景の生成
	CBackGround* pBg = new CBackGround;

	pBg->m_type = type;
	pBg->Init();
	pBg->SetPosition(pos);
	pBg->SetSize(Size.x, Size.y);
	pBg->SetVtx(col);
	pBg->m_col = col;

	return pBg;
}

//================================================
// 初期化処理
//================================================
HRESULT CBackGround::Init(void)
{
	// 初期化処理
	if (FAILED(CObject2D::Init()))
	{
		return E_FAIL;
	}

	// 種類の遷移
	switch (m_type)
	{
	case TYPE_PAUSE:
		// テクスチャのID
		SetTextureID();

		// ポーズ中にしか描画しない
		CObject::SetType(CObject::TYPE_PAUSE);
		break;
	case TYPE_RESULT:
		// テクスチャのID
		//SetTextureID("data/TEXTURE/gradation/result_bg.jpg");
		break;
	default:
		break;
	}

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CBackGround::Uninit(void)
{
	// 終了処理
	CObject2D::Uninit();
}

//================================================
// 描画処理
//================================================
void CBackGround::Draw(void)
{
	// 描画処理
	CObject2D::Draw();
}
