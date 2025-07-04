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

//================================================
// コンストラクタ
//================================================
CMeshCircle::CMeshCircle()
{
	ZeroMemory(&m_Config, sizeof(m_Config));
	m_Incol = m_Outcol = WHITE;
	m_fDecAlv = NULL;
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
CMeshCircle* CMeshCircle::Create(const Confing confing, const D3DXCOLOR col, const D3DXVECTOR3 pos, const int nSegX,const D3DXVECTOR3 rot)
{
	// メッシュインパクトを生成
	CMeshCircle* pMesh = new CMeshCircle;

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
	pMesh->m_Config = confing;
	pMesh->m_Outcol = col;
	pMesh->m_Incol = D3DXCOLOR(col.r, col.g, col.b, col.a * 0.5f);
	pMesh->m_fDecAlv = col.a / confing.nLife;

	pMesh->SetCircle(nSegX, confing.fInRadius, confing.fOutRadius);

	return pMesh;
}

//================================================
// 初期化処理
//================================================
HRESULT CMeshCircle::Init(void)
{
	// 初期化処理
	if (FAILED(CMesh::Init()))
	{
		return E_FAIL;
	}

	// テクスチャのIDの設定
	CMesh::SetTextureID();

	return E_FAIL;
}

//================================================
// 終了処理
//================================================
void CMeshCircle::Uninit(void)
{
	// 終了処理
	CMesh::Uninit();
}

//================================================
// 更新処理
//================================================
void CMeshCircle::Update(void)
{
	int nCntVtx = 0; // 頂点数のカウンター

	int nSegX = GetSegX(); // 横の分割数の取得

	float fTexPosX = 1.0f / nSegX; // テクスチャ座標の割合を求める

	// スローモーションの取得
	CSlow* pSlow = CManager::GetSlow();

	float fSlowLevel = pSlow->GetLevel(false);

	// 半径を拡大する
	m_Config.fInRadius += m_Config.fSpeed * fSlowLevel;
	m_Config.fOutRadius += m_Config.fSpeed * fSlowLevel;

	// メッシュフィールドの取得
	CMeshField* pMesh = CManager::GetMeshField();

	// 頂点のy座標
	float fHeight = 0.0f;

	// 内側の頂点の更新
	for (int nCntX = 0; nCntX <= nSegX; nCntX++)
	{
		D3DXVECTOR3 posWk = VEC3_NULL;

		// 一周の割合を求める
		float fAngle = (D3DX_PI * 2.0f) / nSegX * nCntX;

		D3DXVECTOR3 vtxpos = GetVtxPos(nCntVtx);

		// 変形に頂点を撃つ
		posWk.x = sinf(fAngle) * m_Config.fInRadius;
		posWk.y = vtxpos.y;
		posWk.z = cosf(fAngle) * m_Config.fInRadius;

		D3DXVECTOR3 pos = GetPosition();

		// 地面との当たり判定
		if (pMesh->Collision(posWk + pos, &fHeight) && m_Config.bFiledCollision)
		{
			posWk.y = fHeight;
		}

		// 頂点座標の設定
		SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2(fTexPosX * nCntX, 1.0f),D3DXVECTOR3(0.0f,1.0f,0.0f),m_Incol);

		// 頂点のカウントを加算
		nCntVtx++;
	}

	// 外側の頂点の更新
	for (int nCntX = 0; nCntX <= nSegX; nCntX++)
	{
		D3DXVECTOR3 posWk = VEC3_NULL;

		float fAngle = (D3DX_PI * 2.0f) / nSegX * nCntX;

		D3DXVECTOR3 vtxpos = GetVtxPos(nCntVtx);

		posWk.x = sinf(fAngle) * m_Config.fOutRadius;
		posWk.y = vtxpos.y;
		posWk.z = cosf(fAngle) * m_Config.fOutRadius;

		D3DXVECTOR3 pos = GetPosition();

		// 地面との当たり判定
		if (pMesh->Collision(posWk + pos, &fHeight) && m_Config.bFiledCollision)
		{
			posWk.y = fHeight;
		}

		SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2(fTexPosX * nCntX, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), m_Outcol);

		nCntVtx++;
	}

	// a値を減らす
	m_Incol.a -= m_fDecAlv;
	m_Outcol.a -= m_fDecAlv;

	// 寿命を減らす
	m_Config.nLife--;

	// 寿命が尽きたら
	if (m_Config.nLife <= 0)
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

	pDevice->SetRenderState(D3DRS_CULLMODE, TRUE);

	// aブレンディング
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// 描画処理
	CMesh::Draw();

	// aブレンディングをもとに戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//================================================
// インパクトの設定処理
//================================================
void CMeshCircle::SetCircle(const int nSegX,const float InRadius, const float OutRadius)
{
	int nCntVtx = 0; // 頂点数のカウンター

	float fTexPosX = 1.0f / nSegX; // 横のテクスチャ座標を求める

	D3DXVECTOR3 posWk = VEC3_NULL;

	// 内側の頂点の生成
	for (int nCntX = 0; nCntX <= nSegX; nCntX++)
	{
		// 一周の割合を求める
		float fAngle = (D3DX_PI * 2.0f) / nSegX * nCntX;

		// 円形に点を撃つ
		posWk.x = sinf(fAngle) * InRadius;
		posWk.y = m_Config.fHeight;
		posWk.z = cosf(fAngle) * InRadius;

		// 頂点座標の設定
		SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2(fTexPosX * nCntX, 1.0f));

		// 頂点のカウンターを加算
		nCntVtx++;
	}

	// 外側の頂点の生成
	for (int nCntX = 0; nCntX <= nSegX; nCntX++)
	{
		float fAngle = (D3DX_PI * 2.0f) / nSegX * nCntX;

		posWk.x = sinf(fAngle) * OutRadius;
		posWk.y = 0.0f;
		posWk.z = cosf(fAngle) * OutRadius;

		SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2(fTexPosX * nCntX, 1.0f));

		nCntVtx++;
	}

	int IndxNum = nSegX + 1; // インデックスの数値1

	int IdxCnt = 0; // 配列

	int Num = 0; // インデックスの数値2

	// Zの分割数
	int nSegZ = GetSegZ();

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

