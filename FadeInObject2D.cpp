//===================================================
//
// フェードインする2Dオブジェクト [FadeInObject2D.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "FadeInObject2D.h"

using namespace Const; // 名前空間Constの使用

//===================================================
// コンストラクタ
//===================================================
CFadeInObject2D::CFadeInObject2D()
{
	m_fAddAlv = NULL;
	m_col = WHITE;
}

//===================================================
// デストラクタ
//===================================================
CFadeInObject2D::~CFadeInObject2D()
{
}

//===================================================
// 生成処理
//===================================================
CFadeInObject2D* CFadeInObject2D::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size,const int nTime)
{
	CFadeInObject2D* pObject2D = new CFadeInObject2D;

	// 初期化処理
	if (FAILED(pObject2D->Init()))
	{
		pObject2D->Uninit();
		pObject2D = nullptr;
		return nullptr;
	}

	pObject2D->SetPosition(pos);
	pObject2D->SetSize(Size.x, Size.y);
	pObject2D->SetVtx(WHITE);
	pObject2D->m_fAddAlv = 1.0f / nTime;

	return pObject2D;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CFadeInObject2D::Init(void)
{
	// 初期化処理
	if (FAILED(CObject2D::Init()))
	{
		return E_FAIL;
	}

	// 透明にする
	m_col.a = 0.0f;

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CFadeInObject2D::Uninit(void)
{
	// 終了処理
	CObject2D::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CFadeInObject2D::Update(void)
{
	// アルファ値を加算
	m_col.a += m_fAddAlv;

	// 範囲制限
	if (m_col.a >= 1.0f)
	{
		m_col.a = 1.0f;
	}

	// 色の設定
	CObject2D::SetColor(m_col);
}

//===================================================
// 描画処理
//===================================================
void CFadeInObject2D::Draw(void)
{
	// 描画処理
	CObject2D::Draw();
}
