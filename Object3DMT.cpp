//===================================================
//
// 3Dポリゴン(マルチテクスチャ) [Object3DMT.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "Object3DMT.h"
#include"manager.h"
#include"renderer.h"
#include"textureManager.h"
#include<string>

using namespace Const; // 名前空間Constを使用
using namespace std;   // 名前空間stdを使用

//===================================================
// コンストラクタ
//===================================================
CObject3DMT::CObject3DMT()
{
	m_pos = VEC3_NULL;
	m_rot = VEC3_NULL;
	m_Size = VEC3_NULL;
	D3DXMatrixIdentity(&m_mtxWorld);
	m_pVtxBuffer = nullptr;
	memset(&m_nTextureIdx, -1, sizeof(m_nTextureIdx));
}

//===================================================
// デストラクタ
//===================================================
CObject3DMT::~CObject3DMT()
{
}

//===================================================
// 生成処理
//===================================================
CObject3DMT* CObject3DMT::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 Size)
{
	CObject3DMT* pObject = new CObject3DMT;

	// 初期化処理
	if (FAILED(pObject->Init()))
	{
		pObject->Uninit();
		pObject = nullptr;
		return nullptr;
	}
	pObject->SetPosition(pos);
	pObject->SetRotaition(rot);
	pObject->m_Size = Size;
	pObject->SetVtx(WHITE);

	return pObject;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CObject3DMT::Init(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//頂点バッファの生成・頂点情報の設定
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D_MULT) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuffer,
		NULL)))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CObject3DMT::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuffer != nullptr)
	{
		m_pVtxBuffer->Release();
		m_pVtxBuffer = nullptr;
	}

	// 自分自身の破棄
	CObject::Release();
}

//===================================================
// 更新処理
//===================================================
void CObject3DMT::Update(void)
{

}

//===================================================
// 描画処理
//===================================================
void CObject3DMT::Draw(void)
{
	// レンダラーの取得
	CRenderer* pRenderer = CManager::GetRenderer();

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// テクスチャステージステートの設定
	pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// 計算用マトリックス
	D3DXMATRIX mtxRot, mtxTrans;

	//	ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 向き
	D3DXVECTOR3 rot = m_rot;

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置
	D3DXVECTOR3 pos = m_pos;

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスを設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_3D_MULT));

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3DMT);

	for (int nCnt = 0; nCnt < MAX_TEXTURE; nCnt++)
	{
		if (m_nTextureIdx[nCnt] != -1)
		{
			// テクスチャ設定
			pDevice->SetTexture(nCnt, pTexture->GetAdress(m_nTextureIdx[nCnt]));
		}
		else
		{
			// テクスチャ設定
			pDevice->SetTexture(nCnt, NULL);
		}
	}

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// もとに戻す
	pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
}

//===================================================
// 頂点バッファの設定処理
//===================================================
void CObject3DMT::SetVtx(const D3DXCOLOR col)
{
	// 頂点情報のポインタ
	VERTEX_3D_MULT* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 大きさの取得
	D3DXVECTOR3 Size = m_Size;

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-Size.x, Size.y, Size.z);
	pVtx[1].pos = D3DXVECTOR3(Size.x, Size.y, Size.z);
	pVtx[2].pos = D3DXVECTOR3(-Size.x, -Size.y, -Size.z);
	pVtx[3].pos = D3DXVECTOR3(Size.x, -Size.y, -Size.z);

	// 法線ベクトルの設定
	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	// 頂点カラーの設定
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// テクスチャ座標の設定
	pVtx[0].texMT = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].texMT = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].texMT = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].texMT = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();
}

//===================================================
// テクスチャIDの設定
//===================================================
void CObject3DMT::SetTextureID(const char* pTexture0, const char* pTexture1)
{
	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// 取得できなかったら処理しない
	if (pTexture == nullptr) return;

	// テクスチャのパス
	string TexturePath = "data/TEXTURE/";

	// 文字列をつなげる
	TexturePath += pTexture0;

	m_nTextureIdx[0] = pTexture->Register(TexturePath.c_str());

	// テクスチャのパス
	TexturePath = "data/TEXTURE/";

	// 文字列をつなげる
	TexturePath += pTexture1;

	m_nTextureIdx[1] = pTexture->Register(TexturePath.c_str());

}
