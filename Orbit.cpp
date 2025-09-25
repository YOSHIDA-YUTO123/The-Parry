//================================================
//
// 軌跡 [orbit.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "Orbit.h"
#include "manager.h"
#include "renderer.h"
#include <string>
#include "textureManager.h"

using namespace std;	// 名前空間stdの使用
using namespace Const;	// 名前空間Constを使用する

//================================================
// コンストラクタ
//================================================
CMeshOrbit::CMeshOrbit()
{
	m_Bottom = VEC3_NULL;
	m_Top = VEC3_NULL;
	m_col = WHITE;
	D3DXMatrixIdentity(&m_mtxWorld);
	m_nNumIdx = NULL;
	m_nNumPolygon = NULL;
	m_nNumVtx = NULL;
	m_nSegH = 1;
	m_nSegV = 1;
	m_nTextureIdx = -1;
	m_pIdxBuffer = nullptr;
	m_pos = VEC3_NULL;
	m_pVtxBuffer = nullptr;
	m_rot = VEC3_NULL;
}

//================================================
// デストラクタ
//================================================
CMeshOrbit::~CMeshOrbit()
{
}

//================================================
// 軌跡の生成
//================================================
CMeshOrbit* CMeshOrbit::Create(const D3DXVECTOR3 Top, const D3DXVECTOR3 Bottom, const int nSegH,const D3DXCOLOR col)
{
	// 軌跡を生成
	CMeshOrbit* pMesh = new CMeshOrbit;

	// 縦の分割数
	const int nSegV = 1;

	// 頂点の総数
	pMesh->m_nNumVtx = (nSegH + 1) * (nSegV + 1);

	// インデックスの総数
	pMesh->m_nNumIdx = pMesh->m_nNumVtx;

	// ポリゴンの総数
	pMesh->m_nNumPolygon = pMesh->m_nNumVtx - 2;

	// 生成に失敗した
	if (FAILED(pMesh->Init()))
	{
		pMesh->Uninit();

		pMesh = nullptr;

		return nullptr;
	}

	// テクスチャ座(横)
	float fPosTexH = 1.0f / nSegH;

	VERTEX_3D* pVtx = NULL;

	// 頂点バッファをロック
	pMesh->m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// インデックスバッファへのポインタ
	WORD* pIdx;

	// インデックスバッファのロック
	pMesh->m_pIdxBuffer->Lock(0, 0, (void**)&pIdx, 0);

	// 頂点数分回す
	for (int nCnt = 0; nCnt < pMesh->m_nNumVtx; nCnt++)
	{
		// アルファ値の設定
		float fAlv = col.a * (1.0f - (float)nCnt / pMesh->m_nNumVtx);

		// 頂点の設定(いったん一か所に集める)
		pVtx[nCnt].pos = Top;

		// 法線、色、テクスチャの設定
		pVtx[nCnt].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[nCnt].col = D3DXCOLOR(col.r,col.g,col.b, fAlv);
		pVtx[nCnt].tex = D3DXVECTOR2(fPosTexH * nCnt, 1.0f);

		// インデックスバッファの設定
		pIdx[nCnt] = static_cast<WORD>(nCnt);
	}

	//インデックスバッファのアンロック
	pMesh->m_pIdxBuffer->Unlock();

	// 四個前から始める
	for (int nCnt = pMesh->m_nNumVtx - 4; nCnt >= 0; nCnt -= 2)
	{
		// 前の頂点座標を入れる
		D3DXVECTOR3 OldVtx0 = pVtx[nCnt].pos;
		D3DXVECTOR3 OldVtx1 = pVtx[nCnt + 1].pos;

		// 頂点座標の設定
		pVtx[nCnt + 2].pos = OldVtx0;
		pVtx[nCnt + 3].pos = OldVtx1;
	}

	// 新しい頂点を追加（先頭に入れる）
	pVtx[0].pos = Bottom;
	pVtx[1].pos = Top;

	// 頂点バッファをアンロック
	pMesh->m_pVtxBuffer->Unlock();

	// 設定処理
	pMesh->m_Bottom = Bottom;
	pMesh->m_Top = Top;
	pMesh->m_col = col;
	pMesh->m_nSegH = nSegH;

	return pMesh;
}

//================================================
// 初期化処理
//================================================
HRESULT CMeshOrbit::Init(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//頂点バッファの生成
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * m_nNumVtx,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuffer,
		NULL)))
	{
		return E_FAIL;
	}

	//インデックスバッファの生成
	if (FAILED(pDevice->CreateIndexBuffer(sizeof(WORD) * m_nNumIdx,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIdxBuffer,
		NULL)))
	{
		return E_FAIL;
	}

	// テクスチャのID
	SetTextureID("gradation/orbit.png");

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CMeshOrbit::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuffer != nullptr)
	{
		m_pVtxBuffer->Release();
		m_pVtxBuffer = nullptr;
	}

	// インデックスバッファの破棄
	if (m_pIdxBuffer != nullptr)
	{
		m_pIdxBuffer->Release();
		m_pIdxBuffer = nullptr;
	}

	// 自分自身の破棄
	CObject::Release();
}

//================================================
// 更新処理
//================================================
void CMeshOrbit::Update(void)
{
	VERTEX_3D* pVtx = NULL;

	// 頂点バッファをロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 四個前から始める
	for (int nCnt = m_nNumVtx - 4; nCnt >= 0; nCnt -= 2)
	{
		// 前の頂点座標を入れる
		D3DXVECTOR3 OldVtx0 = pVtx[nCnt].pos;
		D3DXVECTOR3 OldVtx1 = pVtx[nCnt + 1].pos;

		// 頂点座標の設定
		pVtx[nCnt + 2].pos = OldVtx0;
		pVtx[nCnt + 3].pos = OldVtx1;
	}

	// 新しい頂点を追加（先頭に入れる）
	pVtx[0].pos = m_Bottom;
	pVtx[1].pos = m_Top;

	// 頂点バッファをアンロック
	m_pVtxBuffer->Unlock();
}

//================================================
// 描画処理
//================================================
void CMeshOrbit::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// カリングをオンにする
	pDevice->SetRenderState(D3DRS_CULLMODE, TRUE);

	// aブレンディング
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	//計算用のマトリックス
	D3DXMATRIX mtxRot, mtxTrans;

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	//向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	//位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	//頂点バッファをデバイスのデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_3D));

	//インデックスバッファをデータストリームに設定
	pDevice->SetIndices(m_pIdxBuffer);

	//テクスチャフォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャが無かったら
	if (m_nTextureIdx == -1)
	{
		//テクスチャの設定
		pDevice->SetTexture(0, NULL);
	}
	else
	{
		//テクスチャの設定
		pDevice->SetTexture(0, pTexture->GetAdress(m_nTextureIdx));

	}

	//ポリゴンの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_nNumVtx, 0, m_nNumPolygon);

	// aブレンディングをもとに戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// カリングをオフにする
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//================================================
// 位置の設定処理
//================================================
void CMeshOrbit::SetPosition(const D3DXVECTOR3 Top, const D3DXVECTOR3 Bottom)
{
	m_Top = Top;
	m_Bottom = Bottom;
}

//================================================
// テクスチャのIDの設定
//================================================
void CMeshOrbit::SetTextureID(const char* pTextureName)
{
	// テクスチャマネージャーの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// ファイルパス
	string filePath = "data/TEXTURE/";

	// 文字列の連結
	filePath += pTextureName;

	if (pTexture != nullptr)
	{
		// テクスチャのIDの取得
		m_nTextureIdx = pTexture->Register(filePath.c_str());
	}
}
