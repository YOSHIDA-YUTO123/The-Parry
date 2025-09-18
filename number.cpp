//================================================
//
// 数字を操作する処理 [number.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "number.h"
#include "object2D.h"
#include"manager.h"
#include"renderer.h"
#include"textureManager.h"

//************************************************
// 静的メンバ変数宣言
//************************************************

using namespace Const;							// 名前空間Constを使用する

//================================================
// コンストラクタ
//================================================
CNumber::CNumber()
{
	m_pVtxBuffer = NULL;
	m_pos = VEC3_NULL;
	m_Size = VEC2_NULL;
	m_nTextureIdx = NULL;
}

//================================================
// デストラクタ
//================================================
CNumber::~CNumber()
{
}

//================================================
// 初期化処理
//================================================
HRESULT CNumber::Init(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//頂点バッファの生成・頂点情報の設定
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuffer,
		NULL)))
	{
		return E_FAIL;
	}

	// 位置と大きさの設定
	m_pos = pos;
	m_Size =  Size;

	// 頂点情報のポインタ
	VERTEX_2D* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(pos.x - Size.x, pos.y - Size.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(pos.x + Size.x, pos.y - Size.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(pos.x - Size.x, pos.y + Size.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(pos.x + Size.x, pos.y + Size.y, 0.0f);

	// rhwの設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラーの設定
	pVtx[0].col = WHITE;
	pVtx[1].col = WHITE;
	pVtx[2].col = WHITE;
	pVtx[3].col = WHITE;

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f / 9.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f / 9.0f, 1.0f);

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CNumber::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuffer != nullptr)
	{
		m_pVtxBuffer->Release();
		m_pVtxBuffer = nullptr;
	}
}

//================================================
// 更新処理
//================================================
void CNumber::Update(void)
{
	// 頂点情報のポインタ
	VERTEX_2D* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(m_pos.x - m_Size.x, m_pos.y - m_Size.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_pos.x + m_Size.x, m_pos.y - m_Size.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_pos.x - m_Size.x, m_pos.y + m_Size.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_pos.x + m_Size.x, m_pos.y + m_Size.y, 0.0f);

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();
}

//================================================
// 描画処理
//================================================
void CNumber::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// テクスチャの取得
	CTextureManager* pTexture = CManager::GetTexture();

	//頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// もとに戻しておく
	pDevice->SetTexture(0, pTexture->GetAdress(m_nTextureIdx));

	// プレイヤーの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2); // プリミティブの種類
}

//================================================
// 位置の設定処理
//================================================
void CNumber::SetPos(const D3DXVECTOR3 pos)
{
	// 頂点情報のポインタ
	VERTEX_2D* pVtx;
	
	// 位置の設定
	m_pos = pos;

	D3DXVECTOR2 Size = m_Size;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(pos.x - Size.x, pos.y - Size.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(pos.x + Size.x, pos.y - Size.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(pos.x - Size.x, pos.y + Size.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(pos.x + Size.x, pos.y + Size.y, 0.0f);

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();
}

//================================================
// UV座標の更新
//================================================
void CNumber::SetUV(const int nNum)
{
	// 頂点情報のポインタ
	VERTEX_2D* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	float fPosTexU = 0.1f * nNum;

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(fPosTexU, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(0.1f + fPosTexU, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(fPosTexU, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(0.1f + fPosTexU, 1.0f);

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();
}

//================================================
// 色の設定
//================================================
void CNumber::SetColor(const D3DXCOLOR col)
{
	// 頂点情報のポインタ
	VERTEX_2D* pVtx;

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

//================================================
// テクスチャのID
//================================================
void CNumber::SetTextureID(const char* pFileName)
{
	// テクスチャの取得
	CTextureManager* pTexture = CManager::GetTexture();

	if (pFileName == NULL)
	{
		m_nTextureIdx = -1;
		return;
	}
	m_nTextureIdx = pTexture->Register(pFileName);
}
