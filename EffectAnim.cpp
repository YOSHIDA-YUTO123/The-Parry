//===================================================
//
// エフェクトアニメーション [ EeffectAnim.cpp ]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "EffectAnim.h"
#include "textureManager.h"
#include "manager.h"
#include "renderer.h"
#include "velocity.h"

//===================================================
// コンストラクタ
//===================================================
CEffect3DAnim::CEffect3DAnim()
{
	m_pMove = nullptr;
	m_col = Const::WHITE;
	m_nLife = NULL;
	m_fRadius = NULL;
	m_decAlv = NULL;
	m_decRadius = NULL;
}

//===================================================
// デストラクタ
//===================================================
CEffect3DAnim::~CEffect3DAnim()
{
}

//===================================================
// 生成処理
//===================================================
CEffect3DAnim* CEffect3DAnim::Create(const D3DXVECTOR3 pos, const float fRadius, const D3DXCOLOR col)
{
	CEffect3DAnim* pEffect = nullptr;

	// 3Dエフェクトの生成
	pEffect = new CEffect3DAnim;

	if (pEffect == nullptr) return nullptr;

	pEffect->SetPosition(pos);
	pEffect->SetSize(D3DXVECTOR2(fRadius, fRadius));
	pEffect->Init();
	pEffect->m_fRadius = fRadius;
	pEffect->m_col = col;

	return pEffect;
}

//===================================================
// 設定処理
//===================================================
void CEffect3DAnim::SetEffect(const D3DXVECTOR3 pos, const float fRadius, const D3DXCOLOR col)
{
	// 位置の設定処理
	SetPosition(pos);

	m_fRadius = fRadius;

	m_col = col;
}

//===================================================
// 設定処理
//===================================================
void CEffect3DAnim::SetEffect(const int nLife, const D3DXVECTOR3 move)
{
	// 減少値の計算
	m_decAlv = m_col.a / nLife;
	m_decRadius = m_fRadius / nLife;

	m_nLife = nLife;

	if (m_pMove == nullptr)
	{
		// 移動量の生成
		m_pMove = std::make_shared<CVelocity>();
	}

	m_pMove->Set(move);
}

//===================================================
// 初期化処理
//===================================================
HRESULT CEffect3DAnim::Init(void)
{
	// 初期化処理
	if (FAILED(CBillboardAnimation::Init()))
	{
		return E_FAIL;
	}

	// 移動クラスの生成
	m_pMove = std::make_unique<CVelocity>();

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CEffect3DAnim::Uninit(void)
{
	m_pMove = nullptr;

	// 終了処理
	CBillboardAnimation::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CEffect3DAnim::Update(void)
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

	// 更新処理
	CBillboardAnimation::Update();
}

//===================================================
// 描画処理
//===================================================
void CEffect3DAnim::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ゼットテスト
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	//// アルファテストを有効
	//pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	//pDevice->SetRenderState(D3DRS_ALPHAREF, NULL);
	//pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	//// aブレンディング
	//pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	//pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// 描画処理
	CBillboardAnimation::Draw();

	//ゼットテスト
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	//// アルファテストを無効
	//pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	//// aブレンディングをもとに戻す
	//pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	//pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

}
