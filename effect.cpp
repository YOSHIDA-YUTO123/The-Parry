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
#include"renderer.h"

//===================================================
// コンストラクタ
//===================================================
CEffect3D::CEffect3D(int nPriority) : CObjectBillboard(nPriority)
{
	m_col = WHITE;
	m_decAlv = NULL;
	m_decRadius = NULL;
	m_fRadius = NULL;
	m_pMove = nullptr;
	m_nLife = NULL;
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

	// IDの設定
	CObjectBillboard::SetTextureID("data/TEXTURE/effect000.jpg");

	// 移動クラスの生成
	m_pMove = new CVelocity;

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CEffect3D::Uninit(void)
{
	// 移動の破棄
	if (m_pMove != nullptr)
	{
		delete m_pMove;
		m_pMove = nullptr;
	}
	// 終了処理
	CObjectBillboard::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CEffect3D::Update(void)
{
	// 位置の更新
	GetPosition()->UpdatePosition(m_pMove->Get());

	// 半径を減らす
	m_fRadius -= m_decRadius;

	// 透明度を上げる
	m_col.a -= m_decAlv;

	// 寿命
	m_nLife--;

	// 色の設定
	SetColor(m_col);

	// 大きさの設定
	GetSize()->Set(D3DXVECTOR3(m_fRadius, m_fRadius, 0.0f));

	// 位置の設定
	UpdateVertexPos(GetPosition()->Get());

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
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
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
CEffect3D* CEffect3D::Create(const D3DXVECTOR3 pos, const float fRadius, const D3DXVECTOR3 move, const D3DXCOLOR col, const int nLife)
{
	CEffect3D* pEffect = nullptr;

	// 3Dエフェクトの生成
	pEffect = new CEffect3D;

	// 優先順位の取得
	int nPriority = pEffect->GetPriority();

	// 現在のオブジェクトの最大数
	const int nNumAll = CObject::GetNumObject(nPriority);

	// オブジェクトが最大数まであったら
	if (nNumAll >= MAX_OBJECT && pEffect != nullptr)
	{
		// 自分自身の破棄
		pEffect->Uninit();

		// nullにしておく
		pEffect = nullptr;

		return nullptr;
	}

	if (pEffect == nullptr) return nullptr;

	pEffect->Init();
	pEffect->GetPosition()->Set(pos);
	pEffect->GetSize()->Set(D3DXVECTOR3(fRadius, fRadius, 0.0f));
	pEffect->SetOffsetVtx(col);
	pEffect->m_col = col;
	pEffect->m_pMove->Set(move);
	pEffect->m_decAlv = col.a / nLife;
	pEffect->m_decRadius = fRadius / nLife;
	pEffect->m_fRadius = fRadius;
	pEffect->m_nLife = nLife;

	return pEffect;
}
