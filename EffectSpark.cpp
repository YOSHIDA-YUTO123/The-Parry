//===================================================
//
// 火花エフェクト [EffectSpark.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "EffectSpark.h"
#include "manager.h"
#include "renderer.h"
#include "transform.h"
#include "velocity.h"

//===================================================
// コンストラクタ
//===================================================
CEffectSpark::CEffectSpark()
{
    m_col = Const::WHITE;
    m_fDecAlv = NULL;
	m_pMove = nullptr;
    m_nLife = NULL;
	m_fGravity = Const::MAX_GRABITY;
}

//===================================================
// デストラクタ
//===================================================
CEffectSpark::~CEffectSpark()
{

}

//===================================================
// 生成処理
//===================================================
CEffectSpark* CEffectSpark::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const D3DXCOLOR col)
{
    // 火花エフェクト
    CEffectSpark* pEffect = new CEffectSpark;

    pEffect->SetPosition(pos);
    pEffect->SetSize(Size);

    // 初期化処理
    if (FAILED(pEffect->Init()))
    {
        pEffect->Uninit();
        pEffect = nullptr;
        return nullptr;
    }

    pEffect->SetColor(col);
	pEffect->m_col = col;

    return pEffect;
}

//===================================================
// パラメータの設定処理
//===================================================
void CEffectSpark::SetParam(const int nLife, const D3DXVECTOR3 move, const float fGravity)
{
    m_nLife = nLife;
    m_fDecAlv = m_col.a / nLife;

	if (m_pMove == nullptr)
	{
		m_pMove = std::make_unique<CVelocity>();
	}

	m_pMove->Set(move);

	m_fGravity = fGravity;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CEffectSpark::Init(void)
{
    // 初期化処理
    if (FAILED(CObjectBillboard::Init()))
    {
        return E_FAIL;
    }

	// テクスチャのIDの設定
	CObjectBillboard::SetTextureID("data/TEXTURE/effect/effect000.jpg");

    return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CEffectSpark::Uninit(void)
{
	m_pMove = nullptr;

    // 終了処理
    CObjectBillboard::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CEffectSpark::Update(void)
{
	// 位置の取得
	D3DXVECTOR3 pos = CObjectBillboard::GetPosition();

	if (m_pMove != nullptr)
	{
		// 位置の更新
		pos += m_pMove->Get();
	}

	// 重力の設定
	m_pMove->Gravity(-m_fGravity);

	m_col.a -= m_fDecAlv;

	// 色の設定
	SetColor(m_col);

	m_nLife--;

	SetPosition(pos);

	if (m_nLife <= 0)
	{
		Uninit();

		return;
	}
}

//===================================================
// 描画処理
//===================================================
void CEffectSpark::Draw(void)
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

	// ビューマトリックスを宣言
	D3DXMATRIX mtxView;

	// ビューマトリックスの取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	// 計算用ワールドマトリックス
	D3DXMATRIX mtxWk, mtxTrans, mtxRot, mtxRotResult;

	// 初期化
	D3DXMatrixIdentity(&mtxWk);

	// カメラの逆行列を設定
	mtxWk._11 = mtxView._11;
	mtxWk._12 = mtxView._21;
	mtxWk._13 = mtxView._31;
	mtxWk._21 = mtxView._12;
	mtxWk._22 = mtxView._22;
	mtxWk._23 = mtxView._32;
	mtxWk._31 = mtxView._13;
	mtxWk._32 = mtxView._23;
	mtxWk._33 = mtxView._33;

	// 軸、上方向ベクトルの宣言
	D3DXVECTOR3 axis, vecUp = { 0.0f,1.0f,0.0f };

	// 移動ベクトルの取得
	D3DXVECTOR3 vecMove = m_pMove->Get();
	D3DXVec3Normalize(&vecMove, &vecMove);

	// 上方向と移動方向のベクトルの外積
	D3DXVec3Cross(&axis, &vecUp, &vecMove);

	// クォータニオン
	D3DXQUATERNION quat;

	// 傾きを算出
	float fAngle = D3DXVec3Dot(&vecUp, &vecMove);

	// 内積を角度に変換
	fAngle = acosf(fAngle);

	// 軸と回転量からクォータニオンを作成
	D3DXQuaternionRotationAxis(&quat, &axis, fAngle);

	// クォータニオンから回転行列を作成
	D3DXMatrixRotationQuaternion(&mtxRot, &quat);

	// 回転行列とビルボード回転行列を掛け合わせる
	D3DXMatrixMultiply(&mtxRotResult, &mtxWk, &mtxRot);

	// 位置の取得
	D3DXVECTOR3 pos = CObjectBillboard::GetPosition();

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);

	// ワールドマトリックスの設定処理
	CObjectBillboard::SetMatrix(mtxRotResult, mtxTrans);

	// 描画の設定処理
	CObjectBillboard::SetUpDraw();

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
