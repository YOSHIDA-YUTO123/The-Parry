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
#include"manager.h"
#include"renderer.h"

using namespace Const; // 名前空間Constを使用

//================================================
// コンストラクタ
//================================================
CMeshDome::CMeshDome()
{

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

	//// ポリゴン数の設定
	//int nNumFanPolygon = nSegH;

	int nNumIdxFan = nNumFanVtx;

	// 頂点数の設定
	int nNumDomeVtx = (nSegH + 1) * (nSegV + 1);

	// ポリゴン数の設定
	int nNumDomePolygon = (((nSegH * nSegV) * 2)) + (4 * (nSegV - 1));

	// インデックス数の設定
	int nNumDomeIndex = nNumDomePolygon + 2;

	// 頂点情報の設定
	int nNumVtx = nNumFanVtx + nNumDomeVtx;

	int nNumIdx = nNumDomeIndex + nNumIdxFan;

	// 頂点の設定
	pMesh->SetVtxElement(nNumVtx, nNumDomePolygon, nNumIdx);

	// 分割数の設定処理
	pMesh->SetSegment(nSegH, nSegV);
	
	// 初期化処理
	pMesh->Init();

	// 設定処理
	pMesh->SetPosition(pos);
	pMesh->SetRotation(rot);

	pMesh->SetDome(nSegH, nSegV, fRadius, fHeight);

	return pMesh;
}

//================================================
// 初期化処理
//================================================
HRESULT CMeshDome::Init(void)
{
	// 初期化処理
	if (FAILED(CMesh::Init()))
	{
		return E_FAIL;
	}

	// テクスチャのIDの設定
	CMesh::SetTextureID("data/TEXTURE/sky/sky.jpg");

	return E_FAIL;
}

//================================================
// 終了処理
//================================================
void CMeshDome::Uninit(void)
{
	// 終了処理
	CMesh::Uninit();
}

//================================================
// 更新処理
//================================================
void CMeshDome::Update(void)
{
	UpdateRotation(D3DXVECTOR3(0.0f,0.001f,0.0f));
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

	int nSegH = GetSegH();
	int nSegV = GetSegV();

	// 頂点数の設定
	int nNumFanVtx = nSegH + 2;

	// ポリゴン数の設定
	int nNumFanPolygon = nSegH;

	// 頂点数の設定
	int nNumDomeVtx = (nSegH + 1) * (nSegV + 1);

	// ポリゴン数の設定
	int nNumDomePolygon = (((nSegH * nSegV) * 2)) + (4 * (nSegV - 1));

	// 描画処理
	CMesh::SetUpDraw();

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

	// ドームのてっぺんの位置の設定
	SetVtxBuffer(D3DXVECTOR3(0.0f, fHeight + (fHeight / nSegV + 1), 0.0f), 0, D3DXVECTOR2(0.5f, 0.5f));

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

		SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2(u, v));

		SetNormal(NormalizeNormal(nCntVtx), nCntVtx);
		nCntVtx++;
	}
	
	int OffsetIdx = nCntVtx;

	int nCntIdx = 0;

	// 頂点数分
	for (int nCnt = 0; nCnt < nSegH + 2; nCnt++)
	{
		SetIndexBuffer((WORD)nCnt, nCntIdx++);
	}

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
			
			SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2(fTexX * nCntX, fTexY * nCntZ));
			SetNormal(NormalizeNormal(nCntVtx), nCntVtx);

			nCntVtx++;//加算
		}
		fNowRadius += fRateRadius / (nCntZ + 1);
	}

	int IndxCount3 = nSegH + 1;//X

	int IdxCnt = m_nOffsetIdx;//配列

	int Num = 0;//

	int Index0 = 0;
	int Index1 = 0;

	//インデックスの設定
	for (int IndxCount1 = 0; IndxCount1 < nSegV; IndxCount1++)
	{
		for (int IndxCount2 = 0; IndxCount2 <= nSegH; IndxCount2++, IndxCount3++, Num++)
		{
			Index0 = IndxCount3 + OffsetIdx;
			Index1 = Num + OffsetIdx;

			// インデックスバッファの設定
			SetIndexBuffer((WORD)Index0, IdxCnt);
			SetIndexBuffer((WORD)Index1, IdxCnt + 1);
			IdxCnt += 2;
		}

		if (IndxCount1 < nSegV - 1)
		{
			// インデックスバッファの設定
			SetIndexBuffer((WORD)(Num - 1 + OffsetIdx), IdxCnt);
			SetIndexBuffer((WORD)(IndxCount3 + OffsetIdx), IdxCnt + 1);
			IdxCnt += 2;
		}
	}

}
