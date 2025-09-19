//================================================
//
// エフェクト2D [effect2D.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "effect2D.h"
#include "manager.h"
#include "renderer.h"

using namespace Const; // 名前空間Constの使用

//================================================
// コンストラクタ
//================================================
CEffect2D::CEffect2D(const int nPriority) : CObject2D(nPriority)
{
	m_fDecAlv = NULL;
	m_DecSize = VEC2_NULL;
	m_col = WHITE;
	m_nLife = NULL;
	m_move = VEC3_NULL;
}

//================================================
// デストラクタ
//================================================
CEffect2D::~CEffect2D()
{
}

//================================================
// 生成処理
//================================================
CEffect2D* CEffect2D::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const D3DXCOLOR col,const int nPriority)
{
	CEffect2D* pEffect = new CEffect2D(nPriority);

	pEffect->SetPosition(pos);
	pEffect->SetSize(Size);
	pEffect->m_col = col;

	// 初期化処理
	if (FAILED(pEffect->Init()))
	{
		pEffect->Uninit();
		pEffect = nullptr;
		return nullptr;
	}

	// 頂点の設定
	pEffect->SetVtx(col);

	return pEffect;
}

//================================================
// 初期化処理
//================================================
HRESULT CEffect2D::Init(void)
{
	// 初期化処理
	if (FAILED(CObject2D::Init()))
	{
		return E_FAIL;
	}

	// エフェクトのテクスチャの読み込み
	CObject2D::SetTextureID("data/TEXTURE/effect/effect000.jpg");

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CEffect2D::Uninit(void)
{
	// 終了処理
	CObject2D::Uninit();
}

//================================================
// 更新処理
//================================================
void CEffect2D::Update(void)
{
	// 位置の取得
	D3DXVECTOR3 pos = CObject2D::GetPosition();

	// 移動量の更新
	pos += m_move;

	// 大きさの取得
	D3DXVECTOR2 Size = CObject2D::GetSize();

	// 大きさを減らす
	Size -= m_DecSize;

	// アルファ値を減らす
	m_col.a -= m_fDecAlv;

	// 色の設定
	CObject2D::SetColor(m_col);

	// 大きさの設定
	CObject2D::SetSize(Size);

	// 位置の設定
	CObject2D::SetPosition(pos);

	// 更新処理
	CObject2D::Update();

	m_nLife--;

	// 寿命が尽きたら
	if (m_nLife <= 0)
	{
		Uninit();
	}
}

//================================================
// 描画処理
//================================================
void CEffect2D::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// アルファテストを有効
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAREF, NULL);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	// aブレンディング
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// 描画処理
	CObject2D::Draw();

	// アルファテストを無効
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	// aブレンディングをもとに戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

//================================================
// パラメータの設定
//================================================
void CEffect2D::SetParam(const int nLife, const D3DXVECTOR3 move)
{
	m_nLife = nLife;

	// 大きさの取得
	D3DXVECTOR2 Size = CObject2D::GetSize();

	m_fDecAlv = m_col.a / nLife;
	m_DecSize.x = Size.x / nLife;
	m_DecSize.y = Size.y / nLife;

	m_move = move;
}
