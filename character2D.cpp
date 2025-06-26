//===================================================
//
// キャラクター2D [character.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "character2D.h"
#include "textureManager.h"
#include"manager.h"
#include"renderer.h"

//===================================================
// コンストラクタ
//===================================================
CCharacter2D::CCharacter2D(int nPriority) : CObject2D(nPriority)
{
	m_nIdxTexture = NULL;
	m_nLife = NULL;
	m_col = WHITE;
}

//===================================================
// デストラクタ
//===================================================
CCharacter2D::~CCharacter2D()
{

}

//===================================================
// 初期化処理
//===================================================
HRESULT CCharacter2D::Init(void)
{
	// 初期化
	if (FAILED(CObject2D::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CCharacter2D::Uninit(void)
{
	// 終了処理
	CObject2D::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CCharacter2D::Update(void)
{
	// 頂点座標の更新処理
	CObject2D::Update();
}

//===================================================
// 描画処理
//===================================================
void CCharacter2D::Draw(void)
{
	CTextureManager* pTexture = CManager::GetTexture();

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	pDevice->SetTexture(0, pTexture->GetAdress(m_nIdxTexture));

	// 描画処理
	CObject2D::Draw();
}


//===================================================
// テクスチャの設定
//===================================================
void CCharacter2D::SetTexture(const char* pFilePath)
{
	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// テクスチャの番号をもらう
	m_nIdxTexture = pTexture->Register(pFilePath);
}

//===================================================
// 点滅処理
//===================================================
void CCharacter2D::Flash(const int nCount, const float fSpeed)
{
	// 点滅時間が終わったら
	if (nCount <= 0)
	{
		// 色をもとに戻す
		m_col.a = 1.0f;

		// 色の設定
		CObject2D::SetColor(m_col);

		return;
	}

	// 点滅処理
	m_col.a = fabsf(sinf(nCount * fSpeed));

	// 色の設定
	CObject2D::SetColor(m_col);
}

//===================================================
// キャラクターのヒット後の処理
//===================================================
bool CCharacter2D::HitCharacter(const int nDamage)
{
	// 体力を減らす
	m_nLife -= nDamage;

	if (m_nLife <= 0)
	{
		// 終了処理
		Uninit();

		return false;
	}
	else
	{
		return true;
	}
}

//===================================================
// コンストラクタ
//===================================================
CAnimCharacter2D::CAnimCharacter2D()
{
	m_fPosTexU = m_fPosTexH = NULL;
	m_nCounterAnim = NULL;
	m_nPatternAnim = NULL;
	m_nPosX = NULL;
	m_nPosY = NULL;
	m_nAnimSpeed = NULL;
	m_nOffTexPosH = NULL;
}

//===================================================
// デストラクタ
//===================================================
CAnimCharacter2D::~CAnimCharacter2D()
{
}

//===================================================
// 初期化処理
//===================================================
HRESULT CAnimCharacter2D::Init(void)
{
	// 初期化処理
	if (FAILED(CCharacter2D::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CAnimCharacter2D::Uninit(void)
{
	// 終了処理
	CCharacter2D::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CAnimCharacter2D::Update(void)
{
	// アニメーションのカウンターを進める
	m_nCounterAnim++;

	m_fPosTexU = 1.0f / m_nPosX;
	m_fPosTexH = 1.0f / m_nPosY;

	if (m_nCounterAnim >= m_nAnimSpeed)
	{
		m_nCounterAnim = 0;

		m_nPatternAnim++;

		float UV = m_fPosTexU * m_nPatternAnim;
		float HV = m_nOffTexPosH * m_fPosTexH;

		// テクスチャ座標の更新
		CObject2D::SetUvPos(D3DXVECTOR2(UV, HV), D3DXVECTOR2(m_fPosTexU, m_fPosTexH));

		// 最大まで行ったら
		if (m_nPatternAnim >= (m_nPosX * m_nPosY))
		{
			m_nPatternAnim = 0;
		}
	}

	// 更新処理
	CCharacter2D::Update();
}

//===================================================
// 描画処理
//===================================================
void CAnimCharacter2D::Draw(void)
{
	// 描画処理
	CCharacter2D::Draw();
}

//===================================================
// テクスチャの分割数の設定
//===================================================
void CAnimCharacter2D::SetTexPos(const int nPosX, const int nPosY)
{
	m_nPosX = nPosX;
	m_nPosY = nPosY;
}
