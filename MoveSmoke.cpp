//===================================================
//
// 移動時の煙 [MoveSmoke.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "MoveSmoke.h"

//===================================================
// コンストラクタ
//===================================================
CMoveSmoke::CMoveSmoke()
{
}

//===================================================
// デストラクタ
//===================================================
CMoveSmoke::~CMoveSmoke()
{
}

//===================================================
// 生成処理
//===================================================
CMoveSmoke* CMoveSmoke::Create(const D3DXVECTOR3 pos, const float fRadius, const D3DXCOLOR col)
{
	CMoveSmoke* pEffect = nullptr;

	// 3Dエフェクトの生成
	pEffect = new CMoveSmoke;

	if (pEffect == nullptr) return nullptr;

	pEffect->SetEffect(pos, fRadius, col);
	pEffect->SetSize(D3DXVECTOR2(fRadius, fRadius));
	pEffect->Init();

	return pEffect;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CMoveSmoke::Init(void)
{
	// アニメーションの設定
	CMoveSmoke::SetAnim(5, 4, 3, true);

	// 初期化処理
	if (FAILED(CEffect3DAnim::Init()))
	{
		return E_FAIL;
	}

	// IDの設定
	CMoveSmoke::SetTextureID("data/TEXTURE/explosion/explosion001.png");

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CMoveSmoke::Uninit(void)
{
	// 終了処理
	CEffect3DAnim::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CMoveSmoke::Update(void)
{
	// 更新処理
	CEffect3DAnim::Update();
}

//===================================================
// 描画処理
//===================================================
void CMoveSmoke::Draw(void)
{
	// 描画処理
	CEffect3DAnim::Draw();
}
