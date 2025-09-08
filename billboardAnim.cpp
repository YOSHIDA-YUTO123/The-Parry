//===================================================
//
// ビルボードアニメーションの描画 [billboardAnim.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "billboardAnim.h"
#include "manager.h"
#include"textureManager.h"
#include"renderer.h"

using namespace Const; // 名前空間Constを使用

//===================================================
// コンストラクタ
//===================================================
CBillboardAnimation::CBillboardAnimation(int nPriority) : CObject(nPriority)
{
	m_bLoop = false;
	m_fPosTexH = NULL;
	m_fPosTexU = NULL;
	m_nAnimSpeed = NULL;
	m_nCounterAnim = NULL;
	m_nPatternAnim = NULL;
	m_nH = 1;
	m_nV = 1;
}

//===================================================
// デストラクタ
//===================================================
CBillboardAnimation::~CBillboardAnimation()
{
}

//===================================================
// アニメーションの設定処理
//===================================================
void CBillboardAnimation::SetAnim(const int nAnimSpeed, const int U, const int V, const bool bLoop)
{
	m_bLoop = bLoop;
	m_nAnimSpeed = nAnimSpeed;
	m_nH = U;
	m_nV = V;
}

//===================================================
// テクスチャの座標の設定
//===================================================
void CBillboardAnimation::SetTextureVtx(const D3DXVECTOR2 tex, const D3DXVECTOR2 offtex)
{
	// 頂点情報のポインタ
	VERTEX_3D* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(offtex.x, offtex.y);
	pVtx[1].tex = D3DXVECTOR2(offtex.x + tex.x, offtex.y);
	pVtx[2].tex = D3DXVECTOR2(offtex.x, offtex.y + tex.y);
	pVtx[3].tex = D3DXVECTOR2(offtex.x + tex.x, offtex.y + tex.y);

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();
}

//===================================================
// 色の設定
//===================================================
void CBillboardAnimation::SetColor(const D3DXCOLOR col)
{
	// 頂点情報のポインタ
	VERTEX_3D* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点カラーの設定
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();
}

//===================================================
// テクスチャのIDの設定
//===================================================
void CBillboardAnimation::SetTextureID(const char* pTextureName)
{
	// テクスチャマネージャーの取得
	auto pTexture = CManager::GetTexture();

	// nullだったら-1
	if (pTextureName == nullptr)
	{
		m_nTextureIdx = -1;
		return;
	}

	// テクスチャのIDの登録
	m_nTextureIdx = pTexture->Register(pTextureName);
}

//===================================================
// 頂点座標の更新
//===================================================
void CBillboardAnimation::UpdateVertexPos(const D3DXVECTOR3 pos)
{
	// 頂点情報のポインタ
	VERTEX_3D* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 位置の設定
	m_pos = pos;

	// 大きさの取得
	D3DXVECTOR2 Size = m_Size;

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-Size.x, Size.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(Size.x, Size.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-Size.x, -Size.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(Size.x, -Size.y, 0.0f);

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();
}

//===================================================
// 生成処理
//===================================================
CBillboardAnimation* CBillboardAnimation::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const int nSpeed, const int U, const int V, const bool bLoop)
{
	CBillboardAnimation* pBillboard = new CBillboardAnimation;

	// アニメーションの設定
	pBillboard->SetAnim(nSpeed, U, V, bLoop);

	pBillboard->SetPosition(pos);
	pBillboard->SetSize(Size);

	// 初期化処理
	if (FAILED(pBillboard->Init()))
	{
		pBillboard->Uninit();
		pBillboard = nullptr;
		return nullptr;;
	}

	return pBillboard;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CBillboardAnimation::Init(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//頂点バッファの生成・頂点情報の設定
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuffer,
		NULL)))
	{
		return E_FAIL;
	}

	// 頂点情報のポインタ
	VERTEX_3D* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 大きさの取得
	D3DXVECTOR2 Size = m_Size;

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-Size.x, Size.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(Size.x, Size.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-Size.x, -Size.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(Size.x, -Size.y, 0.0f);

	// 法線ベクトルの設定
	pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	// 頂点カラーの設定
	pVtx[0].col = WHITE;
	pVtx[1].col = WHITE;
	pVtx[2].col = WHITE;
	pVtx[3].col = WHITE;

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f / m_nH, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f / m_nH);
	pVtx[3].tex = D3DXVECTOR2(1.0f / m_nH, 1.0f / m_nH);

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CBillboardAnimation::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuffer != nullptr)
	{
		m_pVtxBuffer->Release();
		m_pVtxBuffer = nullptr;
	}

	// 自分自身の破棄
	Release();
}

//===================================================
// 更新処理
//===================================================
void CBillboardAnimation::Update(void)
{
	// アニメーションのカウンターを進める
	m_nCounterAnim++;

	m_fPosTexU = 1.0f / m_nH;
	m_fPosTexH = 1.0f / m_nV;

	if (m_nCounterAnim >= m_nAnimSpeed)
	{
		m_nCounterAnim = 0;

		m_nPatternAnim++;

		// Y座標の割合
		int nRatePosY = m_nPatternAnim / m_nH;

		float UV = m_fPosTexU * m_nPatternAnim;
		float HV = nRatePosY * m_fPosTexH;

		// テクスチャ座標の更新
		SetTextureVtx(D3DXVECTOR2(m_fPosTexU, m_fPosTexH), D3DXVECTOR2(UV, HV));

		// 最大まで行ったら
		if (m_nPatternAnim >= (m_nH * m_nV))
		{
			// パターンを初期化
			m_nPatternAnim = 0;

			if (m_bLoop == false)
			{
				Uninit();
				return;
			}
		}
	}
}

//===================================================
// 描画処理
//===================================================
void CBillboardAnimation::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// 計算用のマトリックスを宣言
	D3DXMATRIX mtxRot, mtxTrans;

	// ライトを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// Zテスト
	//pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
	//pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// ビューマトリックスを宣言
	D3DXMATRIX mtxView;

	// ビューマトリックスの取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	// カメラの逆行列を設定
	m_mtxWorld._11 = mtxView._11;
	m_mtxWorld._12 = mtxView._21;
	m_mtxWorld._13 = mtxView._31;
	m_mtxWorld._21 = mtxView._12;
	m_mtxWorld._22 = mtxView._22;
	m_mtxWorld._23 = mtxView._32;
	m_mtxWorld._31 = mtxView._13;
	m_mtxWorld._32 = mtxView._23;
	m_mtxWorld._33 = mtxView._33;

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	//テクスチャの設定
	pDevice->SetTexture(0, pTexture->GetAdress(m_nTextureIdx));

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//// Zテスト
	//pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	//pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// ライトを有効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}
