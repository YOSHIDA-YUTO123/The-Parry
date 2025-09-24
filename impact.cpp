//================================================
//
// メッシュインパクト [impact.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "impact.h"
#include "manager.h"
#include "renderer.h"
#include "slow.h"
#include "meshfield.h"
#include "game.h"
#include "textureManager.h"

using namespace std;	// 名前空間stdを使用する
using namespace Const;	// 名前空間Constを使用する

//================================================
// コンストラクタ
//================================================
CMeshCircle::CMeshCircle(const int nPriority) : CObject(nPriority)
{
	m_Incol = m_Outcol = WHITE;
	m_fDecAlv = NULL;
	m_bFiledCollision = true;
	m_fHeight = NULL;
	m_fInRadius = NULL;
	m_fOutRadius = NULL;
	m_fSpeed = NULL;
	m_nLife = NULL;
	m_pos = VEC3_NULL;
	m_nNumIdx = NULL;
	m_nNumPolygon = NULL;
	m_nNumVtx = NULL;
	D3DXMatrixIdentity(&m_mtxWorld);
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
CMeshCircle::~CMeshCircle()
{
}

//================================================
// 生成処理
//================================================
CMeshCircle* CMeshCircle::Create(const D3DXCOLOR col, const D3DXVECTOR3 pos,const float fInRadius,const float fOutRadius, const int nSegH)
{
	// メッシュインパクトを生成
	CMeshCircle* pMesh = new CMeshCircle;

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
	pMesh->m_fInRadius = fInRadius;
	pMesh->m_fOutRadius = fOutRadius;
	pMesh->m_Outcol = col;
	pMesh->m_Incol = D3DXCOLOR(col.r, col.g, col.b, col.a * 0.5f);
	pMesh->m_pos = pos;

	return pMesh;
}

//================================================
// サークルの設定処理
//================================================
void CMeshCircle::SetCircle(const float fHeight, const float fSpeed, const int nLife, const bool bField, const D3DXVECTOR3 rot)
{
	// 寿命に応じた透明度の減少値の計算
	m_fDecAlv = m_Outcol.a / nLife;

	// 要素の設定処理
	m_fHeight = fHeight;
	m_fSpeed = fSpeed;
	m_nLife = nLife;
	m_bFiledCollision = bField;
	m_rot = rot;

	// 初期化に失敗したら
	if (FAILED(Init()))
	{
		Uninit();
		return;
	}
}

//================================================
// 初期化処理
//================================================
HRESULT CMeshCircle::Init(void)
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

	// テクスチャの座標の割合
	float fTexPosX = 1.0f / m_nSegH;

	// 計算用の位置
	D3DXVECTOR3 posWk;

	VERTEX_3D* pVtx = NULL;

	// 頂点バッファをロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 横の分割数分回す
	for (int nCntH = 0; nCntH < m_nSegH; nCntH++)
	{
		// 一周の割合をもとめる
		float fAngle = (D3DX_PI * 2.0f) / m_nSegH * nCntH;

		// 位置の設定
		posWk.x = sinf(fAngle) * m_fInRadius;
		posWk.y = m_fHeight;
		posWk.z = cosf(fAngle) * m_fInRadius;

		// 位置の設定
		pVtx[nCntVtx].pos = posWk;

		// 法線の設定
		pVtx[nCntVtx].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		
		// 内側の色の設定
		pVtx[nCntVtx].col = m_Incol;

		// テクスチャ座標の設定
		pVtx[nCntVtx].tex = D3DXVECTOR2(fTexPosX * nCntH, 1.0f);

		nCntVtx++;
	}

	// 横の分割数分回す
	for (int nCntH = 0; nCntH < m_nSegH; nCntH++)
	{
		// 一周の割合をもとめる
		float fAngle = (D3DX_PI * 2.0f) / m_nSegH * nCntH;

		// 位置の設定
		posWk.x = sinf(fAngle) * m_fOutRadius;
		posWk.y = m_fHeight;
		posWk.z = cosf(fAngle) * m_fOutRadius;

		// 位置の設定
		pVtx[nCntVtx].pos = posWk;

		// 法線の設定
		pVtx[nCntVtx].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		// 内側の色の設定
		pVtx[nCntVtx].col = m_Outcol;

		// テクスチャ座標の設定
		pVtx[nCntVtx].tex = D3DXVECTOR2(fTexPosX * nCntH, 1.0f);

		nCntVtx++;
	}

	// 頂点バッファをアンロック
	m_pVtxBuffer->Unlock();

	// インデックスバッファへのポインタ
	WORD* pIdx;

	// インデックスバッファのロック
	m_pIdxBuffer->Lock(0, 0, (void**)&pIdx, 0);

	int IndxNum = m_nSegH + 1;//X

	int IdxCnt = 0;//配列

	int Num = 0;//

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
void CMeshCircle::Uninit(void)
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
void CMeshCircle::Update(void)
{
	int nCntVtx = 0; // 頂点数のカウンター

	// スローモーションの取得
	CSlow* pSlow = CManager::GetSlow();

	float fSlowLevel = pSlow->GetLevel(false);

	// 半径を拡大する
	m_fInRadius += m_fSpeed * fSlowLevel;
	m_fOutRadius += m_fSpeed * fSlowLevel;

	// メッシュフィールドの取得
	CMeshField* pMesh = CGame::GetField();

	// 頂点のy座標
	float fHeight = 0.0f;

	VERTEX_3D* pVtx = NULL;

	// 頂点バッファをロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 内側の頂点の更新
	for (int nCntH = 0; nCntH <= m_nSegH; nCntH++)
	{
		D3DXVECTOR3 posWk = VEC3_NULL;

		// 一周の割合を求める
		float fAngle = (D3DX_PI * 2.0f) / m_nSegH * nCntH;

		// 変形に頂点を撃つ
		posWk.x = sinf(fAngle) * m_fInRadius;
		posWk.y = m_fHeight;
		posWk.z = cosf(fAngle) * m_fInRadius;

		// 地面との当たり判定
		if (pMesh != nullptr && pMesh->Collision(posWk + m_pos, &fHeight) && m_bFiledCollision)
		{
			posWk.y = fHeight;
		}

		// 位置の設定
		pVtx[nCntVtx].pos = posWk;

		// 色の設定
		pVtx[nCntVtx].col = m_Incol;

		// 頂点のカウントを加算
		nCntVtx++;
	}

	// 外側の頂点の更新
	for (int nCntH = 0; nCntH <= m_nSegH; nCntH++)
	{
		D3DXVECTOR3 posWk = VEC3_NULL;

		// 一周の割合を求める
		float fAngle = (D3DX_PI * 2.0f) / m_nSegH * nCntH;

		// 変形に頂点を撃つ
		posWk.x = sinf(fAngle) * m_fOutRadius;
		posWk.y = 0.0f;
		posWk.z = cosf(fAngle) * m_fOutRadius;

		// 地面との当たり判定
		if (pMesh != nullptr && pMesh->Collision(posWk + m_pos, &fHeight) && m_bFiledCollision)
		{
			posWk.y = fHeight;
		}

		// 位置の設定
		pVtx[nCntVtx].pos = posWk;

		// 色の設定
		pVtx[nCntVtx].col = m_Outcol;

		// 頂点のカウントを加算
		nCntVtx++;
	}

	// 頂点バッファをアンロック
	m_pVtxBuffer->Unlock();

	// a値を減らす
	m_Incol.a -= m_fDecAlv;
	m_Outcol.a -= m_fDecAlv;

	// 寿命を減らす
	m_nLife--;

	// 寿命が尽きたら
	if (m_nLife <= 0)
	{
		// 終了処理
		Uninit();

		return;
	}
}

//================================================
// 描画処理
//================================================
void CMeshCircle::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// カリングをオンにする
	pDevice->SetRenderState(D3DRS_CULLMODE, TRUE);

	// ライトを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

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

	// ライトを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

//================================================
// テクスチャのIDの設定
//================================================
void CMeshCircle::SetTextureID(const char* pTextureName)
{
	// テクスチャマネージャーの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// ファイルパス
	string filePath = "data/TEXTURE/gradation/";

	// 文字列の連結
	filePath += pTextureName;

	if (pTexture != nullptr)
	{
		// テクスチャのIDの取得
		m_nTextureIdx = pTexture->Register(filePath.c_str());
	}
}