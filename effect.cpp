//===================================================
//
// エフェクト [effect.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "effect.h"
#include "textureManager.h"
#include "manager.h"
#include "renderer.h"
#include "transform.h"

using namespace Const; // 名前空間Constを使用
using namespace std; // 名前空間stdを使用

//===================================================
// コンストラクタ
//===================================================
CEffect3D::CEffect3D(int nPriority) : CObjectBillboard(nPriority)
{
	m_pMove = nullptr;
	m_col = WHITE;
	m_nLife = NULL;
	m_fRadius = NULL;
	m_decAlv = NULL;
	m_decRadius = NULL;
}

//===================================================
// デストラクタ
//===================================================
CEffect3D::~CEffect3D()
{
}

//===================================================
// 初期化処理
//===================================================
HRESULT CEffect3D::Init(void)
{
	// 初期化処理
	if (FAILED(CObjectBillboard::Init()))
	{
		return E_FAIL;
	}

	// 種類の遷移
	switch (m_type)
	{
	case TYPE_NORAML:
		// IDの設定
		CObjectBillboard::SetTextureID("data/TEXTURE/effect000.jpg");
		break;
	case TYPE_HIT:
		// IDの設定
		CObjectBillboard::SetTextureID("data/TEXTURE/star_A.jpg");
		break;
	default:
		break;
	}

	// 移動クラスの生成
	m_pMove = make_unique<CVelocity>();

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CEffect3D::Uninit(void)
{
	m_pMove = nullptr;

	// 終了処理
	CObjectBillboard::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CEffect3D::Update(void)
{
	// 位置の取得
	D3DXVECTOR3 pos = GetPosition();

	if (m_pMove != nullptr)
	{
		// 移動量の更新
		pos += m_pMove->Get();
	}

	// 半径を減らす
	m_fRadius -= m_decRadius;

	// 透明度を上げる
	m_col.a -= m_decAlv;

	// 寿命
	m_nLife--;

	// 色の設定
	SetColor(m_col);

	// 大きさの設定
	SetSize(D3DXVECTOR2(m_fRadius, m_fRadius));

	// 位置の設定
	UpdateVertexPos(pos);

	if (m_nLife <= 0)
	{
		// 破棄
		Uninit();

		return;
	}
}

//===================================================
// 描画処理
//===================================================
void CEffect3D::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ゼットテスト
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// アルファテストを有効
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAREF, NULL);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	// aブレンディング
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// 描画処理
	CObjectBillboard::Draw();

	//ゼットテスト
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// アルファテストを無効
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	// aブレンディングをもとに戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

//===================================================
// 生成処理
//===================================================
CEffect3D* CEffect3D::Create(const D3DXVECTOR3 pos, const float fRadius, const D3DXCOLOR col, const TYPE type)
{
	CEffect3D* pEffect = nullptr;

	// 3Dエフェクトの生成
	pEffect = new CEffect3D;

	if (pEffect == nullptr) return nullptr;

	pEffect->SetPosition(pos);
	pEffect->SetSize(D3DXVECTOR2(fRadius, fRadius));
	pEffect->m_type = type;
	pEffect->Init();
	pEffect->m_fRadius = fRadius;
	pEffect->m_col = col;

	return pEffect;
}

//===================================================
// エフェクトの設定処理
//===================================================
void CEffect3D::SetEffect(const int nLife, const D3DXVECTOR3 move)
{
	// 減少値の計算
	m_decAlv = m_col.a / nLife;
	m_decRadius = m_fRadius / nLife;

	m_nLife = nLife;

	if (m_pMove == nullptr)
	{
		// 移動量の生成
		m_pMove = make_shared<CVelocity>();
	}

	m_pMove->Set(move);
}
