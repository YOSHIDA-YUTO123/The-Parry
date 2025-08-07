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
	m_pTemporary = nullptr;
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
CHpGage* CHpGage::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const D3DXCOLOR col, const D3DXCOLOR temporaryColor, const int nLife, const bool bDecRightToLeft)
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

	pGage->m_pTemporary = new CTemporaryGage;
	pGage->m_pTemporary->Init();
	pGage->m_pTemporary->SetGage(pos, Size, temporaryColor, bDecRightToLeft);

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
	m_pTemporary = nullptr;

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

	if (m_pTemporary != nullptr)
	{
		// 長さの設定
		m_pTemporary->SetLength(fLength);
	}

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

//================================================
// コンストラクタ
//================================================
CTemporaryGage::CTemporaryGage(int nPriority) : CGage(nPriority)
{
	m_fDestLength = NULL;
	m_fLength = NULL;
	m_bDecRightToLeft = true;
}

//================================================
// デストラクタ
//================================================
CTemporaryGage::~CTemporaryGage()
{
}

//================================================
// 初期化処理
//================================================
HRESULT CTemporaryGage::Init(void)
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
void CTemporaryGage::Uninit(void)
{
	// 終了処理
	CGage::Uninit();
}

//================================================
// 更新処理
//================================================
void CTemporaryGage::Update(void)
{
	// 目的の長さに近づける
	m_fLength += (m_fDestLength - m_fLength) * 0.05f;

	// 大きさ
	D3DXVECTOR2 Size = CObject2D::GetSize();

	// 右から左に減るなら
	if (m_bDecRightToLeft)
	{
		// 設定処理
		CObject2D::SetSize(0.0f, m_fLength, Size.y, Size.y);
	}
	else
	{
		// 設定処理
		CObject2D::SetSize(m_fLength, 0.0f, Size.y, Size.y);
	}
}

//================================================
// 描画処理
//================================================
void CTemporaryGage::Draw(void)
{
	// 描画処理
	CGage::Draw();
}

//================================================
// ゲージの設定処理
//================================================
void CTemporaryGage::SetGage(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const D3DXCOLOR col, const bool bDecRightToLeft)
{
	// ゲージの設定処理
	CObject2D::SetPosition(pos);
	CObject2D::SetSize(Size);
	CObject2D::SetSize(Size.x, Size.x, Size.y, Size.y);
	CObject2D::SetVtx(col);
	m_fLength = m_fDestLength = Size.x;	 // 長さの設定
	m_bDecRightToLeft = bDecRightToLeft; // 右から減るか左から減るか
}

//================================================
// コンストラクタ
//================================================
CStaminaGage::CStaminaGage() 
{
}

//================================================
// デストラクタ
//================================================
CStaminaGage::~CStaminaGage()
{
}

//================================================
// 生成処理
//================================================
CStaminaGage* CStaminaGage::Create(const D3DXVECTOR3 pos, D3DXVECTOR2 Size, const D3DXCOLOR col,const float fStamina)
{
	// スタミナゲージの生成
	CStaminaGage* pGage = new CStaminaGage;

	if (FAILED(pGage->Init()))
	{// 初期化に失敗したら
		// 終了処理
		pGage->Uninit();
		pGage = nullptr;

		return nullptr;
	}

	// 設定処理
	pGage->SetPosition(pos);
	pGage->SetSize(Size);
	pGage->SetSize(Size.x, Size.x, Size.y, Size.y);
	pGage->SetVtx(col);
	pGage->m_fMaxStamina = fStamina;
	pGage->m_fStamina = fStamina;

	return pGage;
}

//================================================
// 初期化処理
//================================================
HRESULT CStaminaGage::Init(void)
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
void CStaminaGage::Uninit(void)
{
	// 終了処理
	CGage::Uninit();
}

//================================================
// 更新処理
//================================================
void CStaminaGage::Update(void)
{
	// スタミナの割合を求める
	float fRate = m_fStamina / m_fMaxStamina;

	// 大きさの取得
	D3DXVECTOR2 Size = CObject2D::GetSize();

	// 割合の大きさを求める
	float fLength = Size.x * fRate;

	// 設定処理
	CObject2D::SetSize(0.0f, fLength, Size.y, Size.y);
}

//================================================
// 描画処理
//================================================
void CStaminaGage::Draw(void)
{
	// 描画処理
	CGage::Draw();
}
