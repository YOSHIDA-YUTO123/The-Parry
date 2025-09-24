//================================================
//
// メッシュドーム [dome.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "dome.h"
#include "manager.h"
#include "renderer.h"
#include "textureManager.h"
#include <string>

using namespace std;
using namespace Const; // 名前空間Constを使用

//================================================
// コンストラクタ
//================================================
CMeshDome::CMeshDome()
{
	m_nOffsetIdx = NULL;
	m_fHeight = NULL;
	m_fRadius = NULL;
	D3DXMatrixIdentity(&m_mtxWorld);
	m_nNumIdx = NULL;
	m_nNumPolygon = NULL;
	m_nNumVtx = NULL;
	m_nSegH = 1;
	m_nSegV = 1;
	m_nTextureIdx = -1;
	m_pIdxBuffer = nullptr;
	m_pVtxBuffer = nullptr;
	m_pos = VEC3_NULL;
	m_rot = VEC3_NULL;
}

//================================================
// デストラクタ
//================================================
CMeshDome::~CMeshDome()
{
}

//================================================
// 生成処理
//================================================
CMeshDome* CMeshDome::Create(const D3DXVECTOR3 pos, const int nSegH, const int nSegV, const float fRadius, const float fHeight, const D3DXVECTOR3 rot)
{
	// メッシュドームを生成
	CMeshDome* pMesh = new CMeshDome;

	if (pMesh == nullptr) return nullptr;

	// 頂点数の設定
	int nNumFanVtx = nSegH + 2;

	int nNumIdxFan = nNumFanVtx;

	// 頂点数の設定
	int nNumDomeVtx = (nSegH + 1) * (nSegV + 1);

	// ポリゴン数の設定
	int nNumDomePolygon = (((nSegH * nSegV) * 2)) + (4 * (nSegV - 1));

	// インデックス数の設定
	int nNumDomeIndex = nNumDomePolygon + 2;

	// 頂点情報の設定
	int nNumVtx = nNumFanVtx + nNumDomeVtx;

	// インデックス数の計算
	int nNumIdx = nNumDomeIndex + nNumIdxFan;

	// 要素の設定
	pMesh->m_nNumIdx = nNumIdx;
	pMesh->m_nNumVtx = nNumVtx;
	pMesh->m_nNumPolygon = nNumDomePolygon;
	pMesh->m_nSegH = nSegH;
	pMesh->m_nSegV = nSegV;
	pMesh->m_pos = pos;
	pMesh->m_rot = rot;
	pMesh->m_fHeight = fHeight;
	pMesh->m_fRadius = fRadius;

	// 初期化処理
	if (FAILED(pMesh->Init()))
	{
		pMesh->Uninit();
		pMesh = nullptr;
		return nullptr;
	}

	return pMesh;
}

//================================================
// 初期化処理
//================================================
HRESULT CMeshDome::Init(void)
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

	// ドームの設定
	SetDome(m_nSegH, m_nSegV, m_fRadius, m_fHeight);

	// テクスチャのIDの設定
	SetTextureID("sky.jpg");

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CMeshDome::Uninit(void)
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
void CMeshDome::Update(void)
{
	// 回転する
	m_rot.y += 0.001f;
}

//================================================
// 描画処理
//================================================
void CMeshDome::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	pDevice->SetRenderState(D3DRS_CULLMODE, TRUE);

	int nSegH = m_nSegH;
	int nSegV = m_nSegV;

	// 頂点数の設定
	int nNumFanVtx = nSegH + 2;

	// ポリゴン数の設定
	int nNumFanPolygon = nSegH;

	// 頂点数の設定
	int nNumDomeVtx = (nSegH + 1) * (nSegV + 1);

	// ポリゴン数の設定
	int nNumDomePolygon = (((nSegH * nSegV) * 2)) + (4 * (nSegV - 1));

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
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, 0, 0, nNumFanVtx, 0, nNumFanPolygon);

	//ポリゴンの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, nNumDomeVtx, m_nOffsetIdx, nNumDomePolygon);

	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

//================================================
// ドームの設定処理
//================================================
void CMeshDome::SetDome(const int nSegH, const int nSegV, const float fRadius, const float fHeight)
{
	int nCntVtx = 0;

	// テクスチャのオフセット
	float fTexX = 0.5f / nSegH;
	float fTexY = 0.5f / nSegV;

	float fNowRadius = fRadius / (nSegV + 1);

	VERTEX_3D* pVtx = NULL;

	// 頂点バッファをロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 頂上の座標の設定
	pVtx[0].pos = D3DXVECTOR3(0.0f, fHeight + (fHeight / nSegV + 1), 0.0f);

	// 法線の設定
	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	// 色の設定
	pVtx[0].col = WHITE;

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.5f, 0.5f);

	nCntVtx++;

	// 蓋の部分の作成
	for (int nCntX = 0; nCntX <= nSegH; nCntX++)
	{
		float fAngle = (D3DX_PI * 2.0f) / nSegH * nCntX;

		D3DXVECTOR3 posWk = VEC3_NULL;

		posWk.x = sinf(fAngle) * fNowRadius;
		posWk.y = fHeight;
		posWk.z = cosf(fAngle) * fNowRadius;

		float u = 0.5f + 0.5f * cosf(fAngle);
		float v = 0.5f + 0.5f * sinf(fAngle);

		// 頂上の座標の設定
		pVtx[nCntVtx].pos = posWk;

		// 法線の設定
		pVtx[nCntVtx].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		// 色の設定
		pVtx[nCntVtx].col = WHITE;

		// テクスチャ座標の設定
		pVtx[nCntVtx].tex = D3DXVECTOR2(u, v);

		nCntVtx++;
	}
	
	// インデックスのオフセットの保存
	int OffsetIdx = nCntVtx;

	int nCntIdx = 0;

	// インデックスバッファへのポインタ
	WORD* pIdx;

	// インデックスバッファのロック
	m_pIdxBuffer->Lock(0, 0, (void**)&pIdx, 0);

	// 頂点数分
	for (int nCnt = 0; nCnt < nSegH + 2; nCnt++)
	{
		// インデックスの設定
		pIdx[nCntIdx++] = static_cast<WORD>(nCnt);
	}

	//インデックスバッファのアンロック
	m_pIdxBuffer->Unlock();

	// インデックスのオフセットを設定
	m_nOffsetIdx = nCntIdx;

	// テクスチャのオフセット
	fTexX = 1.0f / nSegH;
	fTexY = 1.0f / nSegV;

	// 半径
	fNowRadius = fRadius / (nSegV + 1);
	float fRateRadius = fNowRadius;

	//縦
	for (int nCntZ = 0; nCntZ <= nSegV; nCntZ++)
	{
		//横
		for (int nCntX = 0; nCntX <= nSegH; nCntX++)
		{
			float fAngel = (D3DX_PI * 2.0f) / nSegH * nCntX;

			D3DXVECTOR3 posWk = VEC3_NULL;

			posWk.x = sinf(fAngel) * fNowRadius;
			posWk.y = fHeight - (fHeight / nSegV) * nCntZ;
			posWk.z = cosf(fAngel) * fNowRadius;
			
			// 頂上の座標の設定
			pVtx[nCntVtx].pos = posWk;

			// 法線の設定
			pVtx[nCntVtx].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

			// 色の設定
			pVtx[nCntVtx].col = WHITE;

			// テクスチャ座標の設定
			pVtx[nCntVtx].tex = D3DXVECTOR2(fTexX * nCntX, fTexY * nCntZ);

			nCntVtx++;//加算
		}

		// 半径を拡大
		fNowRadius += fRateRadius / (nCntZ + 1);
	}

	// 頂点バッファをアンロック
	m_pVtxBuffer->Unlock();

	int IndxCount3 = nSegH + 1;//X

	int IdxCnt = m_nOffsetIdx;//配列

	int Num = 0;//

	int Index0 = 0;
	int Index1 = 0;

	// インデックスバッファのロック
	m_pIdxBuffer->Lock(0, 0, (void**)&pIdx, 0);

	//インデックスの設定
	for (int IndxCount1 = 0; IndxCount1 < nSegV; IndxCount1++)
	{
		for (int IndxCount2 = 0; IndxCount2 <= nSegH; IndxCount2++, IndxCount3++, Num++)
		{
			Index0 = IndxCount3 + OffsetIdx;
			Index1 = Num + OffsetIdx;

			// インデックスバッファの設定
			pIdx[IdxCnt] = static_cast<WORD>(Index0);
			pIdx[IdxCnt + 1] = static_cast<WORD>(Index1);
			IdxCnt += 2;
		}

		if (IndxCount1 < nSegV - 1)
		{
			// インデックスバッファの設定
			pIdx[IdxCnt] = static_cast<WORD>((Num - 1 + OffsetIdx));
			pIdx[IdxCnt + 1] = static_cast<WORD>(IndxCount3 + OffsetIdx);
			IdxCnt += 2;
		}
	}
	//インデックスバッファのアンロック
	m_pIdxBuffer->Unlock();
}

//================================================
// テクスチャのIDの設定
//================================================
void CMeshDome::SetTextureID(const char* pTextureName)
{
	// テクスチャマネージャーの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// ファイルパス
	string filePath = "data/TEXTURE/sky/";

	// 文字列の連結
	filePath += pTextureName;

	if (pTexture != nullptr)
	{
		// テクスチャのIDの取得
		m_nTextureIdx = pTexture->Register(filePath.c_str());
	}
}
