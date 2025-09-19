//===================================================
//
// マルチテクスチャ2Dオブジェクトのアニメーション [Object2DAnimMT.cpp]
// Author:YUTO YOSHIDA
// 
// << マルチテクスチャの2Dポリゴンのテクスチャ	>>
// << アニメーションに特化したクラス			>>
// 
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "Object2DAnimMT.h"
#include "manager.h"
#include "renderer.h"
#include "textureManager.h"
#include <string>

using namespace Const;	// 名前空間Constを使用
using namespace std;	// 名前空間stdを使用

//===================================================
// コンストラクタ
//===================================================
CObject2DAnimMT::CObject2DAnimMT() : CObject(7)
{
	m_pVtxBuffer = nullptr;
	m_pos = VEC3_NULL;
	m_Size = VEC2_NULL;
	m_nCounter = NULL;
	m_nHV = 1;
	m_nUV = 1;
	m_nPattern = NULL;
	m_nSpeed = NULL;
	m_bLoop = false;

	// テクスチャの数分回す
	for (int nCnt = 0; nCnt < MAX_TEXTURE; nCnt++)
	{
		m_nTextureIdx[nCnt] = -1;
	}
}

//===================================================
// デストラクタ
//===================================================
CObject2DAnimMT::~CObject2DAnimMT()
{
}

//===================================================
// 生成処理
//===================================================
CObject2DAnimMT* CObject2DAnimMT::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const int UV, const int HV)
{
	CObject2DAnimMT* pObject2D = new CObject2DAnimMT;

	pObject2D->m_pos = pos;
	pObject2D->m_Size = Size;
	pObject2D->m_nUV = UV;
	pObject2D->m_nHV = HV;

	// 初期化処理
	if (FAILED(pObject2D->Init()))
	{
		pObject2D->Uninit();
		pObject2D = nullptr;
		return nullptr;
	}

	return pObject2D;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CObject2DAnimMT::Init(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//頂点バッファの生成・頂点情報の設定
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_2D_MULT) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2DMT,
		D3DPOOL_MANAGED,
		&m_pVtxBuffer,
		NULL)))
	{
		return E_FAIL;
	}

	// 頂点情報のポインタ
	VERTEX_2D_MULT* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(m_pos.x - m_Size.x, m_pos.y - m_Size.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_pos.x + m_Size.x, m_pos.y - m_Size.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_pos.x - m_Size.x, m_pos.y + m_Size.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_pos.x + m_Size.x, m_pos.y + m_Size.y, 0.0f);

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
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// テクスチャ座標の設定
	pVtx[0].texMT = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].texMT = D3DXVECTOR2(1.0f / m_nUV, 0.0f);
	pVtx[2].texMT = D3DXVECTOR2(0.0f, 1.0f / m_nHV);
	pVtx[3].texMT = D3DXVECTOR2(1.0f / m_nUV, 1.0f / m_nHV);

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CObject2DAnimMT::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuffer != nullptr)
	{
		m_pVtxBuffer->Release();
		m_pVtxBuffer = nullptr;
	}

	// 自分自身の破棄
	CObject::Release();
}

//===================================================
// 更新処理
//===================================================
void CObject2DAnimMT::Update(void)
{
	// 頂点情報のポインタ
	VERTEX_2D_MULT* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(m_pos.x - m_Size.x, m_pos.y - m_Size.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_pos.x + m_Size.x, m_pos.y - m_Size.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_pos.x - m_Size.x, m_pos.y + m_Size.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_pos.x + m_Size.x, m_pos.y + m_Size.y, 0.0f);

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();

	// アニメーションのカウンターを進める
	m_nCounter++;

	float fPosTexU = 1.0f / m_nUV;
	float fPosTexH = 1.0f / m_nHV;

	if (m_nCounter >= m_nSpeed)
	{
		m_nCounter = 0;

		m_nPattern++;

		// Y座標の割合
		int nRatePosY = m_nPattern / m_nUV;

		float UV = fPosTexU * m_nPattern;
		float HV = nRatePosY * fPosTexH;

		// 頂点バッファのロック
		m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

		// テクスチャ座標の設定
		pVtx[0].texMT = D3DXVECTOR2(UV, HV);
		pVtx[1].texMT = D3DXVECTOR2(UV + fPosTexU, HV);
		pVtx[2].texMT = D3DXVECTOR2(UV, HV + fPosTexH);
		pVtx[3].texMT = D3DXVECTOR2(UV + fPosTexU, HV + fPosTexH);

		// 頂点バッファのアンロック
		m_pVtxBuffer->Unlock();

		// 最大まで行ったら
		if (m_nPattern >= (m_nUV * m_nHV))
		{
			// パターンを初期化
			m_nPattern = 0;

			// ループしないなら
			if (!m_bLoop)
			{
				// 破棄
				Uninit();

				return;
			}
		}
	}
}

//===================================================
// 描画処理
//===================================================
void CObject2DAnimMT::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// テクスチャステージステートの設定
	pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);

	//頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_2D_MULT));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2DMT);

	// テクスチャの数分回す
	for (int nCnt = 0; nCnt < MAX_TEXTURE; nCnt++)
	{
		// テクスチャがあったら
		if (m_nTextureIdx[nCnt] != -1)
		{
			// テクスチャを設定
			pDevice->SetTexture(nCnt, pTexture->GetAdress(m_nTextureIdx[nCnt]));
		}
		else
		{
			// テクスチャがない
			pDevice->SetTexture(nCnt, NULL);
		}
	}

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2); // プリミティブの種類	

	// もとに戻す
	pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
}

//===================================================
// 設定処理
//===================================================
void CObject2DAnimMT::SetAnim(const int nSpeed, const bool bLoop)
{
	m_bLoop = bLoop;
	m_nSpeed = nSpeed;
}

//===================================================
// 設定処理
//===================================================
void CObject2DAnimMT::SetAnim(const int nUV, const int nHV)
{
	m_nHV = nHV;
	m_nUV = nUV;
}

//===================================================
// テクスチャのIDの設定
//===================================================
void CObject2DAnimMT::SetTextureID(const char* pTextureName0, const char* pTextureName1)
{
	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// 取得できなかったら処理しない
	if (pTexture == nullptr) return;

	// 省略用パス
	string filePath = "data/TEXTURE/";

	// 文字列の連結
	filePath += pTextureName0;

	// IDの設定
	m_nTextureIdx[0] = pTexture->Register(filePath.c_str());

	// 省略用パス
	filePath = "data/TEXTURE/";

	// 文字列の連結
	filePath += pTextureName1;

	// IDの設定
	m_nTextureIdx[1] = pTexture->Register(filePath.c_str());
}
