//===================================================
//
// 爆発,煙 [explosion.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "explosion.h"
#include "manager.h"
#include"renderer.h"

using namespace Const;							// 名前空間Constを使用する

//===================================================
// コンストラクタ
//===================================================
CExplosion::CExplosion(int nPriority) : CBillboardAnimation(nPriority)
{
	m_col = WHITE;
}

//===================================================
// デストラクタ
//===================================================
CExplosion::~CExplosion()
{
}

//===================================================
// 生成処理
//===================================================
CExplosion* CExplosion::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 size, const D3DXCOLOR col, const int U, const int V, const int nAnimSpeed)
{
	// 爆発の生成
	CExplosion* pExplotion = new CExplosion;

	if (pExplotion == nullptr) return nullptr;

	pExplotion->m_col = col;
	pExplotion->SetAnim(nAnimSpeed, U, V,false);
	pExplotion->SetPosition(pos);
	pExplotion->SetSize(size);
	pExplotion->Init();

	return pExplotion;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CExplosion::Init(void)
{
	// 初期化処理
	if (FAILED(CBillboardAnimation::Init()))
	{
		return E_FAIL;
	}

	// テクスチャのIDの設定
	SetTextureID("data/TEXTURE/explosion001.png");

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CExplosion::Uninit(void)
{
	// 終了処理
	CBillboardAnimation::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CExplosion::Update(void)
{
	// 更新処理
	CBillboardAnimation::Update();
}

//===================================================
// 描画処理
//===================================================
void CExplosion::Draw(void)
{
	// 更新処理
	CBillboardAnimation::Draw();
}
