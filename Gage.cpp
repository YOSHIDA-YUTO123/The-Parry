//================================================
//
// ゲージ [gage.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "Gage.h"
#include "GageFrame.h"

using namespace Const; // 名前空間Constを使用

//================================================
// コンストラクタ
//================================================
CGage::CGage(int nPriority) : CObject2D(nPriority)
{
	m_Observer = nullptr;
}

//================================================
// デストラクタ
//================================================
CGage::~CGage()
{
}

////================================================
//// 生成処理
////================================================
//CGage* CGage::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const D3DXCOLOR col)
//{
//	return nullptr;
//}

//================================================
// 初期化処理
//================================================
HRESULT CGage::Init(void)
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
void CGage::Uninit(void)
{
	// 終了処理
	CObject2D::Uninit();
}

//================================================
// 描画処理
//================================================
void CGage::Draw(void)
{
	// 描画処理
	CObject2D::Draw();
}

//================================================
// コンストラクタ
//================================================
CHpGage::CHpGage()
{
	m_nLife = NULL;
	m_nMaxLife = NULL;
	m_bDecRightToLeft = true;
}

//================================================
// デストラクタ
//================================================
CHpGage::~CHpGage()
{
}

//================================================
// HPゲージの生成処理
//================================================
CHpGage* CHpGage::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const D3DXCOLOR col, const int nLife, const bool bDecRightToLeft)
{
	// HPゲージの生成
	CHpGage* pGage = new CHpGage;

	// 設定処理
	pGage->Init();
	pGage->SetPosition(pos);
	pGage->SetSize(Size);
	pGage->SetSize(Size.x, Size.x, Size.y, Size.y);
	pGage->SetVtx(col);
	pGage->m_nLife = nLife;
	pGage->m_nMaxLife = nLife;
	pGage->m_bDecRightToLeft = bDecRightToLeft;

	return pGage;
}

//================================================
// 初期化処理
//================================================
HRESULT CHpGage::Init(void)
{
	// 初期化処理
	if (FAILED(CGage::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CHpGage::Uninit(void)
{
	// 終了処理
	CGage::Uninit();
}

//================================================
// 更新処理
//================================================
void CHpGage::Update(void)
{
	// 体力の割合を計算する
	float fRateLife = m_nLife / static_cast<float>(m_nMaxLife);

	// 大きさ
	D3DXVECTOR2 Size = CObject2D::GetSize();

	// 大きさの取得
	float fLength = fRateLife * Size.x;

	// 右から左に減るなら
	if (m_bDecRightToLeft)
	{
		// 設定処理
		CObject2D::SetSize(0.0f, fLength, Size.y, Size.y);
	}
	else
	{
		// 設定処理
		CObject2D::SetSize(fLength, 0.0f, Size.y, Size.y);
	}
}

//================================================
// 描画処理
//================================================
void CHpGage::Draw(void)
{
	// 描画処理
	CGage::Draw();
}
