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
#include"manager.h"
#include"renderer.h"

//================================================
// コンストラクタ
//================================================
CMeshWave::CMeshWave()
{
	m_fRadius = NULL;
	m_col = WHITE;
	m_fDecAlv = NULL;
	m_fSpeed = NULL;
	m_nLife = NULL;
	m_fHeight = NULL;
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
CMeshWave* CMeshWave::Create(const D3DXVECTOR3 pos, const float fRadius, const float fHeight, const float speed, const int nLife, const int nSegX, const D3DXCOLOR col, const D3DXVECTOR3 rot)
{
	// メッシュウェーブを生成
	CMeshWave* pMesh = new CMeshWave;

	// 優先順位の取得
	int nPriority = pMesh->GetPriority();

	// 現在のオブジェクトの最大数
	const int nNumAll = CObject::GetNumObject(nPriority);

	// オブジェクトが最大数まであったら
	if (nNumAll >= MAX_OBJECT && pMesh != nullptr)
	{
		// 自分のポインタの解放
		pMesh->Release();

		// nullにする
		pMesh = nullptr;

		// オブジェクトを消す
		return nullptr;
	}

	// zの分割数1固定
	const int nSegZ = 1;

	if (pMesh == nullptr) return nullptr;

	// 頂点数の設定
	int nNumVtx = (nSegX + 1) * (nSegZ + 1);

	// ポリゴン数の設定
	int nNumPolygon = ((nSegX * nSegZ) * 2) + (4 * (nSegZ - 1));

	// インデックス数の設定
	int nNumIndex = nNumPolygon + 2;

	// 頂点の設定
	pMesh->SetVtxElement(nNumVtx, nNumPolygon, nNumIndex);
	pMesh->SetSegment(nSegX, nSegZ);

	// 初期化処理
	pMesh->Init();

	// 設定処理
	pMesh->SetPosition(pos);
	pMesh->SetRotation(rot);
	pMesh->m_fHeight = fHeight;
	pMesh->m_fRadius = fRadius;
	pMesh->m_nLife = nLife;
	pMesh->m_fSpeed = speed;
	pMesh->m_col = col;
	pMesh->m_fDecAlv = col.a / nLife;

	pMesh->SetWave(nSegX,fRadius, fHeight);

	return pMesh;
}

//================================================
// 初期化処理
//================================================
HRESULT CMeshWave::Init(void)
{
	// 初期化処理
	if (FAILED(CMesh::Init()))
	{
		return E_FAIL;
	}

	// テクスチャのIDの設定
	CMesh::SetTextureID("data/TEXTURE/dog.png");

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CMeshWave::Uninit(void)
{
	// 終了処理
	CMesh::Uninit();
}

//================================================
// 更新処理
//================================================
void CMeshWave::Update(void)
{
	int nCntVtx = 0; // 頂点数のカウンター

	// 横の分割数の取得
	int nSegX = GetSegX();

	float fPosTexV = 1.0f / nSegX; // 横の分割数

	// Zの分割数の取得
	int nSegZ = GetSegZ();

	// 半径の更新
	m_fRadius += m_fSpeed;

	for (int nCntZ = 0; nCntZ <= nSegZ; nCntZ++)
	{
		for (int nCntX = 0; nCntX <= nSegX; nCntX++)
		{
			// 一周の割合を求める
			float fAngle = (D3DX_PI * 2.0f) / nSegX * nCntX;

			// 計算用の位置
			D3DXVECTOR3 posWk = VEC3_NULL;

			posWk.x = sinf(fAngle) * m_fRadius;
			posWk.y = m_fHeight - (m_fHeight / nSegZ * nCntZ);
			posWk.z = cosf(fAngle) * m_fRadius;

			// 法線の正規化
			D3DXVECTOR3 nor = NormalizeNormal(nCntVtx);

			// 頂点座標の設定
			SetVtxPos(posWk,nCntVtx);

			// 頂点カラーの設定
			SetColor(m_col, nCntVtx);

			// 法線の設定
			SetNormal(nor,nCntVtx);

			// 頂点数のカウンターを加算
			nCntVtx++;
		}
	}

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

	// 描画処理
	CMesh::Draw();

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
void CMeshWave::SetWave(const int nSegX, const float fRadius, const float fHeight)
{
	int nCntVtx = 0; // 頂点数のカウンター

	float fPosTexV = 1.0f / nSegX; // 横の分割数
	
	// Zの分割数の取得
	int nSegZ = GetSegZ();

	for (int nCntZ = 0; nCntZ <= nSegZ; nCntZ++)
	{
		for (int nCntX = 0; nCntX <= nSegX; nCntX++)
		{
			// 一周の割合を求める
			float fAngle = (D3DX_PI * 2.0f) / nSegX * nCntX;

			// 計算用の位置
			D3DXVECTOR3 posWk = VEC3_NULL;

			posWk.x = sinf(fAngle) * fRadius;
			posWk.y = fHeight - (fHeight / nSegZ * nCntZ);
			posWk.z = cosf(fAngle) * fRadius;

			// 頂点バッファの設定
			SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2((fPosTexV * nCntX), 1.0f));

			// 法線の正規化
			D3DXVECTOR3 nor = NormalizeNormal(nCntVtx);

			SetNormal(nor, nCntVtx);

			nCntVtx++;
		}
	}

	int IndxNum = nSegX + 1; // インデックスの数値1

	int IdxCnt = 0; // 配列

	int Num = 0; // インデックスの数値2

	//インデックスの設定
	for (int IndxCount1 = 0; IndxCount1 < nSegZ; IndxCount1++)
	{
		for (int IndxCount2 = 0; IndxCount2 <= nSegX; IndxCount2++, IndxNum++, Num++)
		{
			// インデックスバッファの設定
			SetIndexBuffer((WORD)IndxNum, IdxCnt);
			SetIndexBuffer((WORD)Num, IdxCnt + 1);
			IdxCnt += 2;
		}

		// NOTE:最後の行じゃなかったら
		if (IndxCount1 < nSegZ - 1)
		{
			SetIndexBuffer((WORD)Num - 1, IdxCnt);
			SetIndexBuffer((WORD)IndxNum, IdxCnt + 1);
			IdxCnt += 2;
		}
	}
}
