//================================================
//
// メッシュシリンダー [cylinder.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "cylinder.h"
#include "player.h"
#include "manager.h"
#include "math.h"
#include "renderer.h"
#include <string>
#include "textureManager.h"

using namespace Const; // 名前空間Constを使用
using namespace math;  // 名前空間mathを使用
using namespace std;   // 名前空間stdを使用

//================================================
// コンストラクタ
//================================================
CMeshCylinder::CMeshCylinder(int nPriority) : CObject(nPriority)
{
	m_fHeight = NULL;
	m_Type = TYPE_WALL;
	m_pos = VEC3_NULL;
	m_col = WHITE;
	D3DXMatrixIdentity(&m_mtxWorld);
	m_nNumIdx = NULL;
	m_nNumPolygon = NULL;
	m_nNumVtx = NULL;
	m_nSegH = 1;
	m_nSegV = 1;
	m_nTextureIdx = -1;
	m_pIdxBuffer = nullptr;
	m_pVtxBuffer = nullptr;
	m_rot = VEC3_NULL;
	m_fRadius = NULL;
}

//================================================
// デストラクタ
//================================================
CMeshCylinder::~CMeshCylinder()
{
}

//================================================
// 生成処理
//================================================
CMeshCylinder* CMeshCylinder::Create(const D3DXVECTOR3 pos, const int nSegH, const int nSegV,const float fRadius, const float fHeight, const D3DXVECTOR3 rot)
{
	// メッシュフィールドを生成
	CMeshCylinder* pMesh = new CMeshCylinder;

	// 頂点数の設定
	pMesh->m_nNumVtx = (nSegH + 1) * (nSegV + 1);

	// ポリゴン数の設定
	pMesh->m_nNumPolygon = ((nSegH * nSegV) * 2) + (4 * (nSegV - 1));

	// インデックス数の設定
	pMesh->m_nNumIdx = pMesh->m_nNumPolygon + 2;

	pMesh->m_nSegH = nSegH;
	pMesh->m_nSegV = nSegV;
	pMesh->m_pos = pos;
	pMesh->m_fRadius = fRadius;
	pMesh->m_rot = rot;
	pMesh->m_fHeight = fHeight;

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
HRESULT CMeshCylinder::Init(void)
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

	// 頂点のカウント
	int nCntVtx = 0;

	float fTexPosX = 1.0f / m_nSegH;
	float fTexPosY = 1.0f / m_nSegV;

	VERTEX_3D* pVtx = NULL;

	// 頂点バッファをロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 縦の分割数分回す
	for (int nCntV = 0; nCntV <= m_nSegV; nCntV++)
	{
		for (int nCntH = 0; nCntH <= m_nSegH; nCntH++)
		{
			// 計算用の位置
			D3DXVECTOR3 posWk = VEC3_NULL;

			// 横の分割数
			float fAngle = (D3DX_PI * 2.0f) / m_nSegH * nCntH;

			posWk.x = sinf(fAngle) * m_fRadius;
			posWk.y = m_fHeight - (m_fHeight / m_nSegV * nCntV);
			posWk.z = cosf(fAngle) * m_fRadius;

			// 位置の設定
			pVtx[nCntVtx].pos = posWk;

			// 頂点までの方向を求める
			D3DXVECTOR3 nor = pVtx[nCntVtx].pos - m_pos;

			// 方向ベクトルにする
			D3DXVec3Normalize(&nor, &nor);

			// 法線の設定
			pVtx[nCntVtx].nor = nor;

			// 色の設定
			pVtx[nCntVtx].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);

			// テクスチャ座標の設定
			pVtx[nCntVtx].tex = D3DXVECTOR2((fTexPosX * nCntH), (fTexPosY * nCntV));

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

	//インデックスバッファのアンロック
	m_pIdxBuffer->Unlock();

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CMeshCylinder::Uninit(void)
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
void CMeshCylinder::Update(void)
{
}

//================================================
// 描画処理
//================================================
void CMeshCylinder::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

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
}

//================================================
// 当たり判定
//================================================
bool CMeshCylinder::Collision(D3DXVECTOR3 *pPos)
{
	// 横の分割数の取得
	int nSegH = m_nSegH;

	VERTEX_3D* pVtx = NULL;

	// 頂点バッファをロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 横の分割数分調べる
	for (int nCnt = 0; nCnt <= nSegH; nCnt++)
	{
		int nNextIndx = (nCnt + 1) % nSegH; // 次の頂点のインデックス
		int nIndx = nCnt;					// 今の頂点のインデックス

		// 頂点座標の取得
		D3DXVECTOR3 vtx0 = pVtx[nIndx].pos, vtx1 = pVtx[nNextIndx].pos;

		// 辺ベクトルの作成
		D3DXVECTOR3 edge = GetVector(vtx1, vtx0);

		// プレイヤーまでのベクトルを作成
		D3DXVECTOR3 playerVec = GetVector(*pPos, vtx0);

		D3DXVECTOR3 cross; // 外積

		// 外積する
		D3DXVec3Cross(&cross, &edge, &playerVec);

		// 正規化する
		D3DXVec3Normalize(&cross, &cross);

		// プレイヤーがシリンダーの外に出たら
		if (cross.y <= 0.0f)
		{
			D3DXVECTOR3 objectPos = *pPos;

			// シリンダーの中心までのベクトルを作成
			D3DXVECTOR3 CenterDir = GetVector(objectPos, m_pos);

			// 中心からプレイヤーまでの距離を求める
			float fDistance = GetDistance(objectPos - m_pos);

			// めり込んだ深さを求める
			float fDepth = (m_fRadius - 4.0f) - fDistance;

			// プレイヤーの位置から中心までの方向×めり込んだ深さを足す
			D3DXVECTOR3 pos = objectPos + CenterDir * fDepth;
			
			// 位置を設定
			pPos->x = pos.x;
			pPos->z = pos.z;

			return true;
		}
	}

	// 頂点バッファをアンロック
	m_pVtxBuffer->Unlock();

	return false;
}

//================================================
// テクスチャのIDの設定
//================================================
void CMeshCylinder::SetTextureID(const char* pTextureName)
{
	// テクスチャマネージャーの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// 省略用ファイルパス
	string filePath = "data/TEXTURE/";

	// 文字列の連結
	filePath += pTextureName;

	if (pTexture != nullptr)
	{
		// テクスチャのIDの設定
		m_nTextureIdx = pTexture->Register(filePath.c_str());
	}
}
