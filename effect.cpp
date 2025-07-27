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
	// 値をクリアする
	ZeroMemory(&m_Data, sizeof(m_Data));
	m_Data.col = WHITE;
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

	//// 種類の遷移
	//switch (m_Data.type)
	//{
	//case TYPE_NORAML:
	//	// IDの設定
	//	CObjectBillboard::SetTextureID("data/TEXTURE/effect/effect000.jpg");
	//	break;
	//case TYPE_HIT:
	//	// IDの設定
	//	CObjectBillboard::SetTextureID("data/TEXTURE/effect/star_A.jpg");
	//	break;
	//default:
	//	break;
	//}

	// 移動クラスの生成
	m_Data.pMove = make_unique<CVelocity>();

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CEffect3D::Uninit(void)
{
	m_Data.pMove = nullptr;

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

	if (m_Data.pMove != nullptr)
	{
		m_Data.pMove->SetInertia3D(0.01f);

		// 移動量の更新
		pos += m_Data.pMove->Get();
	}

	// 半径を減らす
	m_Data.fRadius -= m_Data.decRadius;

	// 透明度を上げる
	m_Data.col.a -= m_Data.decAlv;

	// 寿命
	m_Data.nLife--;

	// 色の設定
	SetColor(m_Data.col);

	// 大きさの設定
	SetSize(D3DXVECTOR2(m_Data.fRadius, m_Data.fRadius));

	// 位置の設定
	UpdateVertexPos(pos);

	if (m_Data.nLife <= 0)
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
// エフェクトの設定処理
//===================================================
HRESULT CEffect3D::SetUp(const D3DXVECTOR3 pos, const float fRadius, const D3DXCOLOR col)
{
	SetPosition(pos);
	SetSize(D3DXVECTOR2(fRadius, fRadius));

	// 初期化に失敗したら
	if (FAILED(Init()))
	{
		return E_FAIL;
	}

	m_Data.fRadius = fRadius;
	m_Data.col = col;

	return S_OK;
}

//===================================================
// 生成処理
//===================================================
CEffect3D* CEffect3D::Create(const D3DXVECTOR3 pos, const float fRadius, const D3DXCOLOR col)
{
	CEffect3D* pEffect = nullptr;

	// 3Dエフェクトの生成
	pEffect = new CEffect3D;

	if (pEffect == nullptr) return nullptr;

	pEffect->SetPosition(pos);
	pEffect->SetSize(D3DXVECTOR2(fRadius, fRadius));
	pEffect->Init();
	pEffect->m_Data.fRadius = fRadius;
	pEffect->m_Data.col = col;

	return pEffect;
}

//===================================================
// エフェクトの設定処理
//===================================================
void CEffect3D::Set(const int nLife, const D3DXVECTOR3 move)
{
	// 減少値の計算
	m_Data.decAlv = m_Data.col.a / nLife;
	m_Data.decRadius = m_Data.fRadius / nLife;

	m_Data.nLife = nLife;

	if (m_Data.pMove == nullptr)
	{
		// 移動量の生成
		m_Data.pMove = make_shared<CVelocity>();
	}

	m_Data.pMove->Set(move);
}
