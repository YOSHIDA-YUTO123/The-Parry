//================================================
//
// メッシュインパクト [impact.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "Wave.h"
#include "manager.h"
#include "renderer.h"
#include "textureManager.h"
#include <string>
using namespace std;

using namespace Const;	// 名前空間Constを使用する

//================================================
// コンストラクタ
//================================================
CMeshWave::CMeshWave()
{
	m_col = WHITE;
	m_fHeight = NULL;
	m_fRadius = NULL;
	m_fSpeed = NULL;
	m_nLife = NULL;
	m_fDecAlv = NULL;
	D3DXMatrixIdentity(&m_mtxWorld);
	m_nNumIdx = NULL;
	m_nNumPolygon = NULL;
	m_nNumVtx = NULL;
	m_pos = VEC3_NULL;
	m_nSegH = 1;
	m_nSegV = 1;
	m_nTextureIdx = -1;
	m_pIdxBuffer = nullptr;
	m_pVtxBuffer = nullptr;
	m_rot = VEC3_NULL;
}

//================================================
// デストラクタ
//================================================
CMeshWave::~CMeshWave()
{
}

//================================================
// 生成処理
//================================================
CMeshWave* CMeshWave::Create(const D3DXVECTOR3 pos, const float fRadius, const float fHeight, const D3DXCOLOR col, const int nSegH)
{
	// メッシュウェーブを生成
	CMeshWave* pMesh = new CMeshWave;

	// zの分割数1固定
	const int nSegV = 1;

	// 頂点数の設定
	pMesh->m_nNumVtx = (nSegH + 1) * (nSegV + 1);

	// ポリゴン数の設定
	pMesh->m_nNumPolygon = ((nSegH * nSegV) * 2) + (4 * (nSegV - 1));

	// インデックス数の設定
	pMesh->m_nNumIdx = pMesh->m_nNumPolygon + 2;

	pMesh->m_nSegH = nSegH;
	pMesh->m_nSegV = nSegV;
	pMesh->m_col = col;
	pMesh->m_fRadius = fRadius;
	pMesh->m_fHeight = fHeight;
	pMesh->m_pos = pos;

	// 初期化処理
	pMesh->Init();

	return pMesh;
}

//================================================
// 初期化処理
//================================================
HRESULT CMeshWave::Init(void)
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

	int nCntVtx = 0; // 頂点数のカウンター

	float fPosTexV = 1.0f / m_nSegH; // 横の分割数

	VERTEX_3D* pVtx = NULL;

	// 頂点バッファをロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 縦の分割数分回す
	for (int nCntZ = 0; nCntZ <= m_nSegV; nCntZ++)
	{
		// 横の分割数分回す
		for (int nCntX = 0; nCntX <= m_nSegH; nCntX++)
		{
			// 一周の割合を求める
			float fAngle = (D3DX_PI * 2.0f) / m_nSegH * nCntX;

			// 計算用の位置
			D3DXVECTOR3 posWk = VEC3_NULL;

			posWk.x = sinf(fAngle) * m_fRadius;
			posWk.y = m_fHeight - (m_fHeight / m_nSegV * nCntZ);
			posWk.z = cosf(fAngle) * m_fRadius;

			// 位置の設定
			pVtx[nCntVtx].pos = posWk;

			// 法線の設定
			pVtx[nCntVtx].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

			// 色の設定
			pVtx[nCntVtx].col = WHITE;

			// テクスチャ座標の設定
			pVtx[nCntVtx].tex = D3DXVECTOR2((fPosTexV * nCntX), 1.0f);

			nCntVtx++;
		}
	}

	// 頂点バッファをアンロック
	m_pVtxBuffer->Unlock();

	int IndxNum = m_nSegH + 1; // インデックスの数値1

	int IdxCnt = 0; // 配列

	int Num = 0; // インデックスの数値2

		// インデックスバッファへのポインタ
	WORD* pIdx;

	// インデックスバッファのロック
	m_pIdxBuffer->Lock(0, 0, (void**)&pIdx, 0);

	//インデックスの設定
	for (int IndxCount1 = 0; IndxCount1 < m_nSegV; IndxCount1++)
	{
		for (int IndxCount2 = 0; IndxCount2 <= m_nSegH; IndxCount2++, IndxNum++, Num++)
		{
			// インデックスバッファの設定
			pIdx[IdxCnt] = static_cast<WORD>(IndxNum);
			pIdx[IdxCnt + 1] = static_cast<WORD>(Num);
			IdxCnt += 2;
		}

		// NOTE:最後の行じゃなかったら
		if (IndxCount1 < m_nSegV - 1)
		{
			pIdx[IdxCnt] = static_cast<WORD>(Num - 1);
			pIdx[IdxCnt + 1] = static_cast<WORD>(IndxNum);
			IdxCnt += 2;
		}
	}

	// インデックスバッファのアンロック
	m_pIdxBuffer->Unlock();

	// テクスチャのIDの設定
	SetTextureID("gradation/wave000.jpg");

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CMeshWave::Uninit(void)
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
void CMeshWave::Update(void)
{
	int nCntVtx = 0; // 頂点数のカウンター

	// 半径の更新
	m_fRadius += m_fSpeed;

	VERTEX_3D* pVtx = NULL;

	// 頂点バッファをロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 縦の分割数分回す
	for (int nCntZ = 0; nCntZ <= m_nSegV; nCntZ++)
	{
		// 横の分割数分回す
		for (int nCntX = 0; nCntX <= m_nSegH; nCntX++)
		{
			// 一周の割合を求める
			float fAngle = (D3DX_PI * 2.0f) / m_nSegH * nCntX;

			// 計算用の位置
			D3DXVECTOR3 posWk = VEC3_NULL;

			posWk.x = sinf(fAngle) * m_fRadius;
			posWk.y = m_fHeight - (m_fHeight / m_nSegV * nCntZ);
			posWk.z = cosf(fAngle) * m_fRadius;

			// 位置の設定
			pVtx[nCntVtx].pos = posWk;

			pVtx[nCntVtx].col = m_col;

			// 頂点数のカウンターを加算
			nCntVtx++;
		}
	}

	// 頂点バッファをアンロック
	m_pVtxBuffer->Unlock();

	// 透明度を下げる
	m_col.a -= m_fDecAlv;

	// 寿命を減らす
	m_nLife--;

	// 寿命が尽きた
	if (m_nLife <= 0)
	{
		Uninit();

		return;
	}
}

//================================================
// 描画処理
//================================================
void CMeshWave::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ポリゴンの両面化オン
	pDevice->SetRenderState(D3DRS_CULLMODE, TRUE);

	// aブレンディング
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// アルファテストを有効
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAREF, NULL);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

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

	// アルファテストを無効
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	// ポリゴンの両面化オフ
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//================================================
// 波の設定処理
//================================================
void CMeshWave::SetWave(const int nLife,const float fSpeed)
{
	m_fSpeed = fSpeed;
	m_nLife = nLife;
	m_fDecAlv = m_col.a / m_nLife;
}

//================================================
// テクスチャのIDの設定
//================================================
void CMeshWave::SetTextureID(const char* pTextureName)
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
