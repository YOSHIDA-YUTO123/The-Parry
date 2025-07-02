//===================================================
//
// 3Dオブジェクトの描画 [object3D.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "object3D.h"
#include"manager.h"
#include"renderer.h"

//===================================================
// コンストラクタ
//===================================================
CObject3D::CObject3D(int nPriority) : CObject(nPriority)
{
	memset(m_mtxWorld, NULL, sizeof(D3DXMATRIX));
	m_pPos = nullptr;
	m_pRot = nullptr;
	m_pSize = nullptr;
	m_pVtxBuffer = NULL;
	m_nTextureIdx = NULL;
}

//===================================================
// デストラクタ
//===================================================
CObject3D::~CObject3D()
{
}

//===================================================
// 初期化処理
//===================================================
HRESULT CObject3D::Init(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// 位置、向き、大きさの設定
	m_pPos = new CPosition;
	m_pRot = new CRotation;
	m_pSize = new CSize3D;

	//頂点バッファの生成・頂点情報の設定
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuffer,
		NULL)))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CObject3D::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuffer != nullptr)
	{
		m_pVtxBuffer->Release();
		m_pVtxBuffer = nullptr;
	}

	// 位置の破棄
	if (m_pPos != nullptr)
	{
		delete m_pPos;
		m_pPos = nullptr;
	}

	// 向きの破棄
	if (m_pRot != nullptr)
	{
		delete m_pRot;
		m_pRot = nullptr;
	}

	// 大きさの破棄
	if (m_pSize != nullptr)
	{
		delete m_pSize;
		m_pSize = nullptr;
	}

	// 自分自身の破棄
	Release();
}

//===================================================
// 更新処理
//===================================================
void CObject3D::Update(void)
{
}

//===================================================
// 描画処理
//===================================================
void CObject3D::Draw(void)
{
	// レンダラーの取得
	CRenderer* pRenderer = CManager::GetRenderer();

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// 計算用マトリックス
	D3DXMATRIX mtxRot, mtxTrans;

	//	ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 向き
	D3DXVECTOR3 rot = m_pRot->Get();

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置
	D3DXVECTOR3 pos = m_pPos->Get();

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスを設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_3D));

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

#if 0
	LPDIRECT3DTEXTURE9 pTextureMT = pRenderer->GetTextureMT();

	// テクスチャ設定
	pDevice->SetTexture(0, pTextureMT);
#else
	// テクスチャ設定
	pDevice->SetTexture(0, pTexture->GetAdress(m_nTextureIdx));
#endif

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//===================================================
// 描画に必要な情報だけの描画処理
//===================================================
void CObject3D::SetDraw(void)
{
	// レンダラーの取得
	CRenderer* pRenderer = CManager::GetRenderer();

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_3D));

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャ設定
	pDevice->SetTexture(0, pTexture->GetAdress(m_nTextureIdx));

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

}

//===================================================
// 位置の取得
//===================================================
CPosition* CObject3D::GetPosition(void)
{
	if (m_pPos == nullptr) return nullptr;

	return m_pPos;
}

//===================================================
// 向きの取得
//===================================================
CRotation* CObject3D::GetRotaition(void)
{
	if (m_pRot == nullptr) return nullptr;

	return m_pRot;
}

//===================================================
// 大きさの取得
//===================================================
CSize3D* CObject3D::GetSize(void)
{
	if (m_pSize == nullptr) return nullptr;

	return m_pSize;
}

//===================================================
// 頂点のオフセットの設定処理
//===================================================
void CObject3D::SetOffsetVtx(const D3DXCOLOR col, const int nPosX, const int nPosY)
{
	// 頂点情報のポインタ
	VERTEX_3D* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 大きさの取得
	D3DXVECTOR3 Size = m_pSize->Get();

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-Size.x, Size.y, Size.z);
	pVtx[1].pos = D3DXVECTOR3(Size.x, Size.y, Size.z);
	pVtx[2].pos = D3DXVECTOR3(-Size.x, -Size.y, -Size.z);
	pVtx[3].pos = D3DXVECTOR3(Size.x, -Size.y, -Size.z);

	// 法線ベクトルの設定
	pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	// 頂点カラーの設定
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f / nPosX, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f / nPosY);
	pVtx[3].tex = D3DXVECTOR2(1.0f / nPosX, 1.0f / nPosY);

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();
}

//===================================================
// テクスチャのIDの設定
//===================================================
void CObject3D::SetTextureID(const char* pTextureName)
{
	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// テクスチャの名前の設定
	m_nTextureIdx = pTexture->Register(pTextureName);
}

//===================================================
// 位置の更新処理
//===================================================
void CObject3D::UpdatePosition(const D3DXVECTOR3 pos,const D3DXVECTOR3 Size)
{
	// 頂点情報のポインタ
	VERTEX_3D* pVtx;

	m_pPos->Set(pos);
	m_pSize->Set(Size);

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-Size.x, Size.y, Size.z);
	pVtx[1].pos = D3DXVECTOR3(Size.x, Size.y, Size.z);
	pVtx[2].pos = D3DXVECTOR3(-Size.x, -Size.y, -Size.z);
	pVtx[3].pos = D3DXVECTOR3(Size.x, -Size.y, -Size.z);

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();
}

//===================================================
// 色の更新処理
//===================================================
void CObject3D::UpdateCol(const D3DXCOLOR col)
{
	// 頂点情報のポインタ
	VERTEX_3D* pVtx;
	
	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点カラーの設定
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();
}

//===================================================
// 生成処理
//===================================================
CObject3D* CObject3D::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 size, const char* pTextureName)
{
	CObject3D* pObject3D = nullptr;

	// 3Dオブジェクトの生成
	pObject3D = new CObject3D;

	// 優先順位の取得
	int nPriority = pObject3D->GetPriority();

	// 現在のオブジェクトの最大数
	const int nNumAll = CObject::GetNumObject(nPriority);

	// オブジェクトが最大数まであったら
	if (nNumAll >= MAX_OBJECT && pObject3D != nullptr)
	{
		// 自分自身の破棄
		pObject3D->Release();

		// nullにしておく
		pObject3D = nullptr;

		return nullptr;
	}

	if (pObject3D == nullptr) return nullptr;

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	pObject3D->Init();
	pObject3D->m_pPos->Set(pos);
	pObject3D->m_pRot->Set(rot);
	pObject3D->m_pSize->Set(size);
	pObject3D->SetOffsetVtx();
	pObject3D->m_nTextureIdx = pTexture->Register(pTextureName);

	return pObject3D;
}
