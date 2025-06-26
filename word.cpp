//================================================
//
// 文字を操作する処理 [word.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "word.h"
#include "object.h"
#include "manager.h"
#include "renderer.h"

//================================================
// コンストラクタ
//================================================
CWord::CWord()
{
	m_pVtxBuffer = NULL;
	m_fHeight = NULL;
	m_fWidth = NULL;
	m_pos = VEC3_NULL;
	m_nTextureIdx = NULL;
}

//================================================
// デストラクタ
//================================================
CWord::~CWord()
{
}

//================================================
// 初期化処理
//================================================
HRESULT CWord::Init(const int nPosX, const int nPosY, const D3DXVECTOR3 pos, const float fWidth, const float fHeight)
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

	m_pos = pos;
	m_fWidth = fWidth;
	m_fHeight = fHeight;

	// 頂点情報のポインタ
	VERTEX_2D* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(pos.x - fWidth, pos.y - fHeight, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(pos.x + fWidth, pos.y - fHeight, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(pos.x - fWidth, pos.y + fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(pos.x + fWidth, pos.y + fHeight, 0.0f);

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
	pVtx[1].tex = D3DXVECTOR2(1.0f / nPosX, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f / nPosY);
	pVtx[3].tex = D3DXVECTOR2(1.0f / nPosX, 1.0f / nPosY);

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CWord::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuffer != NULL)
	{
		m_pVtxBuffer->Release();
		m_pVtxBuffer = NULL;
	}
}

//================================================
// 更新処理
//================================================
void CWord::Update(void)
{
}

//================================================
// 描画処理
//================================================
void CWord::Draw(void)
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
// 大きさの設定処理
//================================================
void CWord::SetSize(const float fWIdth, const float fHeight)
{
	m_fWidth = fWIdth;
	m_fHeight = fHeight;
}

//================================================
// 位置の設定処理
//================================================
void CWord::SetPos(const D3DXVECTOR3 pos)
{
	// 頂点情報のポインタ
	VERTEX_2D* pVtx;

	m_pos = pos;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(pos.x - m_fWidth, pos.y - m_fHeight, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(pos.x + m_fWidth, pos.y - m_fHeight, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(pos.x - m_fWidth, pos.y + m_fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(pos.x + m_fWidth, pos.y + m_fHeight, 0.0f);

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();
}

//================================================
// UVの設定処理
//================================================
void CWord::SetUV(const int nNum)
{
	// 頂点情報のポインタ
	VERTEX_2D* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	float offposX = 1.0f / CWord::WORD_MAX;

	float fPosTexU = offposX * nNum;

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(fPosTexU, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(offposX + fPosTexU, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(fPosTexU, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(offposX + fPosTexU, 1.0f);

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();
}

//================================================
// 色の設定処理
//================================================
void CWord::SetColor(const D3DXCOLOR col)
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
// テクスチャのID設定処理
//================================================
void CWord::SetTextureID(const char* pFileName)
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
