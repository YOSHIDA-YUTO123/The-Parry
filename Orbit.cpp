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
#include"renderer.h"

using namespace Const;							// 名前空間Constを使用する

//================================================
// コンストラクタ
//================================================
CMeshOrbit::CMeshOrbit()
{
	m_Bottom = VEC3_NULL;
	m_Top = VEC3_NULL;
	m_col = WHITE;
	m_nLife = NULL;
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
CMeshOrbit* CMeshOrbit::Create(const D3DXVECTOR3 Top, const D3DXVECTOR3 Bottom, const int nSegH,const D3DXCOLOR col,const int nLife)
{
	// 軌跡を生成
	CMeshOrbit* pMesh = new CMeshOrbit;

	// 優先順位の取得
	int nPriority = pMesh->GetPriority();

	// 現在のオブジェクトの最大数
	const int nNumAll = CObject::GetNumObject(nPriority);

	// オブジェクトが最大数まであったら
	if (nNumAll >= MAX_OBJECT && pMesh != nullptr)
	{
		// 自分のポインタの解放
		pMesh->Uninit();

		// nullにする
		pMesh = nullptr;

		// オブジェクトを消す
		return nullptr;
	}

	// nullなら処理を通さない
	if (pMesh == nullptr) return nullptr;

	// 縦の分割数
	const int nSegV = 1;

	// 頂点の総数
	int nNumVtx = (nSegH + 1) * (nSegV + 1);

	// インデックスの総数
	int nNumIdx = nNumVtx;

	// ポリゴンの総数
	int nNumPolygon = nNumVtx - 2;

	// 分割数の設定
	pMesh->SetSegment(nSegH, nSegV);

	// 頂点の要素の設定
	pMesh->SetVtxElement(nNumVtx, nNumPolygon, nNumIdx);

	// 生成に失敗した
	if (FAILED(pMesh->Init()))
	{
		pMesh->Uninit();

		pMesh = nullptr;

		return nullptr;
	}

	// テクスチャ座(横)
	float fPosTexH = 1.0f / nSegH;

	// 頂点数分回す
	for (int nCnt = 0; nCnt < nNumVtx; nCnt++)
	{
		// アルファ値の設定
		float fAlv = col.a * (1.0f - (float)nCnt / nNumVtx);

		// 頂点の設定
		pMesh->SetVtxBuffer(Bottom, nCnt, D3DXVECTOR2(fPosTexH * nCnt, 1.0f),D3DXVECTOR3(0.0f,1.0f,0.0f),D3DXCOLOR(col.r,col.g,col.b, fAlv));

		// インデックスバッファの設定
		pMesh->SetIndexBuffer((WORD)nCnt,nCnt);
	}

	// 設定処理
	pMesh->m_Bottom = Bottom;
	pMesh->m_Top = Top;
	pMesh->m_col = col;
	pMesh->m_nLife = nLife;

	return pMesh;
}

//================================================
// 初期化処理
//================================================
HRESULT CMeshOrbit::Init(void)
{
	// 初期化処理
	if (FAILED(CMesh::Init()))
	{
		return E_FAIL;
	}

	// テクスチャのID
	SetTextureID("data/TEXTURE/orbit.png");

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CMeshOrbit::Uninit(void)
{
	// 終了処理
	CMesh::Uninit();
}

//================================================
// 更新処理
//================================================
void CMeshOrbit::Update(void)
{
	int nSegH = GetSegH(); 	// 横の分割数の取得
	int nSegV = 1;			// 縦の分割数

	// 頂点の総数
	int nNumVtx = (nSegH + 1) * (nSegV + 1);


	// 四個前から始める
	for (int nCnt = nNumVtx - 4; nCnt >= 0; nCnt -= 2)
	{
		// 前の頂点座標を入れる
		D3DXVECTOR3 OldVtx0 = GetVtxPos(nCnt);
		D3DXVECTOR3 OldVtx1 = GetVtxPos(nCnt + 1);

		// 頂点座標の設定
		SetVtxPos(OldVtx0, nCnt + 2);
		SetVtxPos(OldVtx1, nCnt + 3);
	}

	// 新しい頂点を追加（先頭に入れる）
	SetVtxPos(m_Bottom, 0);
	SetVtxPos(m_Top, 1);

	m_nLife--;

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

	// 描画処理
	CMesh::Draw();

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
bool CMeshOrbit::SetPosition(const D3DXVECTOR3 Top, const D3DXVECTOR3 Bottom)
{
	m_Top = Top;
	m_Bottom = Bottom;

	if (m_nLife <= 0)
	{
		Uninit();
		return false;
	}

	return true;
}
