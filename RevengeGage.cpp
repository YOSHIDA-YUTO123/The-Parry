//================================================
//
// 反撃ゲージ [RevengeGage.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "RevengeGage.h"
#include "manager.h"
#include "renderer.h"
#include "number.h"
#include "enemy.h"

using namespace Const; // 名前空間Constを使用
using namespace std;   // 名前空間stdを使用

constexpr float MAX_HV = 0.5f; // 最大のテクスチャ座標

//================================================
// コンストラクタ
//================================================
CRevengeUI::CRevengeUI()
{
	m_fValue = NULL;
	m_fMaxValue = NULL;
}

//================================================
// デストラクタ
//================================================
CRevengeUI::~CRevengeUI()
{
	
}

//================================================
// 生成処理
//================================================
CRevengeUI* CRevengeUI::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const float fValue)
{
	// ゲージの生成
	auto pGage = new CRevengeUI;

	pGage->m_fValue = 0.0f;
	pGage->m_fMaxValue = fValue;

	pGage->SetPosition(pos);
	pGage->SetSize(Size);

	// 初期化処理
	if (FAILED(pGage->Init()))
	{
		pGage->Uninit();
		pGage = nullptr;
		return nullptr;
	}

	return pGage;
}

//================================================
// 初期化処理
//================================================
HRESULT CRevengeUI::Init(void)
{
	// 初期化処理
	if (FAILED(CObject2DMT::Init()))
	{
		return E_FAIL;
	}

	// テクスチャのIDの設定
	CObject2DMT::SetTextureID("data/TEXTURE/UI/Revenge.png", "data/TEXTURE/UI/RevengeGage.png");

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CRevengeUI::Uninit(void)
{
	// 終了処理
	CObject2DMT::Uninit();
}

//================================================
// 更新処理
//================================================
void CRevengeUI::Update(void)
{
	// 割合を求める
	float fRate = m_fValue / m_fMaxValue;

	// 現在の座標を求める
	float fHV = fRate * MAX_HV;

	// 更新処理
	CObject2DMT::Update();

	// テクスチャの設定
	CObject2DMT::SetTexture(
		D3DXVECTOR2(0.0f, 0.0f + fHV),
		D3DXVECTOR2(1.0f, 0.0f + fHV),
		D3DXVECTOR2(0.0f, 0.5f + fHV),
		D3DXVECTOR2(1.0f, 0.5f + fHV),
		1);
}

//================================================
// 描画処理
//================================================
void CRevengeUI::Draw(void)
{
	// 描画処理
	CObject2DMT::Draw();
}
