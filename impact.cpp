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
#include"slow.h"
#include"meshfield.h"
#include "game.h"

using namespace Const;							// 名前空間Constを使用する

//================================================
// コンストラクタ
//================================================
CMeshCircle::CMeshCircle()
{
	m_Incol = m_Outcol = WHITE;
	m_fDecAlv = NULL;
	m_bFiledCollision = true;
	m_fHeight = NULL;
	m_fInRadius = NULL;
	m_fOutRadius = NULL;
	m_fSpeed = NULL;
	m_nLife = NULL;
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

	if (pMesh == nullptr) return nullptr;

	// 頂点数の設定
	int nNumVtx = (nSegH + 1) * (nSegV + 1);

	// ポリゴン数の設定
	int nNumPolygon = ((nSegH * nSegV) * 2) + (4 * (nSegV - 1));

	// インデックス数の設定
	int nNumIndex = nNumPolygon + 2;

	// 頂点の設定
	pMesh->SetVtxElement(nNumVtx, nNumPolygon, nNumIndex);
	pMesh->SetSegment(nSegH, nSegV);

	// 初期化処理
	pMesh->Init();

	// 設定処理
	pMesh->SetPosition(pos);
	pMesh->m_Outcol = col;
	pMesh->m_Incol = D3DXCOLOR(col.r, col.g, col.b, col.a * 0.5f);
	pMesh->m_fInRadius = fInRadius;
	pMesh->m_fOutRadius = fOutRadius;

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
	SetRotation(rot);

	// 横の分割数の取得
	int nSegH = GetSegH();

	// サークルの設定
	SetVtx(nSegH, m_fInRadius, m_fOutRadius);
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
	//CMesh::SetTextureID("data/TEXTURE/gradation/gradation100.jpg");

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

	int nSegH = GetSegH(); // 横の分割数の取得

	float fTexPosX = 1.0f / nSegH; // テクスチャ座標の割合を求める

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

	// 内側の頂点の更新
	for (int nCntX = 0; nCntX <= nSegH; nCntX++)
	{
		D3DXVECTOR3 posWk = VEC3_NULL;

		// 一周の割合を求める
		float fAngle = (D3DX_PI * 2.0f) / nSegH * nCntX;

		D3DXVECTOR3 vtxpos = GetVtxPos(nCntVtx);

		// 変形に頂点を撃つ
		posWk.x = sinf(fAngle) * m_fInRadius;
		posWk.y = vtxpos.y;
		posWk.z = cosf(fAngle) * m_fInRadius;

		D3DXVECTOR3 pos = GetPosition();

		// 地面との当たり判定
		if (pMesh != nullptr && pMesh->Collision(posWk + pos, &fHeight) && m_bFiledCollision)
		{
			posWk.y = fHeight;
		}

		// 頂点座標の設定
		SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2(fTexPosX * nCntX, 1.0f),D3DXVECTOR3(0.0f,1.0f,0.0f),m_Incol);

		// 頂点のカウントを加算
		nCntVtx++;
	}

	// 外側の頂点の更新
	for (int nCntX = 0; nCntX <= nSegH; nCntX++)
	{
		D3DXVECTOR3 posWk = VEC3_NULL;

		float fAngle = (D3DX_PI * 2.0f) / nSegH * nCntX;

		D3DXVECTOR3 vtxpos = GetVtxPos(nCntVtx);

		posWk.x = sinf(fAngle) * m_fOutRadius;
		posWk.y = vtxpos.y;
		posWk.z = cosf(fAngle) * m_fOutRadius;

		D3DXVECTOR3 pos = GetPosition();

		// 地面との当たり判定
		if (pMesh != nullptr && pMesh->Collision(posWk + pos, &fHeight) && m_bFiledCollision)
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

	//// Zテスト
	//pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	//pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

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

	// カリングをオフにする
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// ライトを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	//// Zテスト
	//pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	//pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

//================================================
// インパクトの設定処理
//================================================
void CMeshCircle::SetVtx(const int nSegH,const float InRadius, const float OutRadius)
{
	int nCntVtx = 0; // 頂点数のカウンター

	float fTexPosX = 1.0f / nSegH; // 横のテクスチャ座標を求める

	D3DXVECTOR3 posWk = VEC3_NULL;

	// 内側の頂点の生成
	for (int nCntX = 0; nCntX <= nSegH; nCntX++)
	{
		// 一周の割合を求める
		float fAngle = (D3DX_PI * 2.0f) / nSegH * nCntX;

		// 円形に点を撃つ
		posWk.x = sinf(fAngle) * InRadius;
		posWk.y = m_fHeight;
		posWk.z = cosf(fAngle) * InRadius;

		// 頂点座標の設定
		SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2(fTexPosX * nCntX, 1.0f));

		// 頂点のカウンターを加算
		nCntVtx++;
	}

	// 外側の頂点の生成
	for (int nCntX = 0; nCntX <= nSegH; nCntX++)
	{
		float fAngle = (D3DX_PI * 2.0f) / nSegH * nCntX;

		posWk.x = sinf(fAngle) * OutRadius;
		posWk.y = 0.0f;
		posWk.z = cosf(fAngle) * OutRadius;

		SetVtxBuffer(posWk, nCntVtx, D3DXVECTOR2(fTexPosX * nCntX, 1.0f));

		nCntVtx++;
	}

	int IndxNum = nSegH + 1; // インデックスの数値1

	int IdxCnt = 0; // 配列

	int Num = 0; // インデックスの数値2

	// Zの分割数
	int nSegV = GetSegV();

	//インデックスの設定
	for (int IndxCount1 = 0; IndxCount1 < nSegV; IndxCount1++)
	{
		for (int IndxCount2 = 0; IndxCount2 <= nSegH; IndxCount2++, IndxNum++, Num++)
		{
			// インデックスバッファの設定
			SetIndexBuffer((WORD)IndxNum, IdxCnt);
			SetIndexBuffer((WORD)Num, IdxCnt + 1);
			IdxCnt += 2;
		}

		// NOTE:最後の行じゃなかったら
		if (IndxCount1 < nSegV - 1)
		{
			SetIndexBuffer((WORD)Num - 1, IdxCnt);
			SetIndexBuffer((WORD)IndxNum, IdxCnt + 1);
			IdxCnt += 2;
		}
	}
}

