//===================================================
//
// オブジェクト2Dの描画処理 [object2D.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "object2D.h"
#include"renderer.h"
#include "manager.h"

//***************************************************
// 静的メンバ変数の宣言
//***************************************************

//===================================================
// コンストラクタ
//===================================================
CObject2D::CObject2D(int nPriority) : CObject(nPriority)
{
	m_pPos = nullptr;
	m_pRot = nullptr;
	m_pSize = nullptr;

	m_pVtxBuffer = NULL;
	m_fAngle = 0.0f;
	m_Length = 0.0f;
}
//===================================================
// デストラクタ
//===================================================
CObject2D::~CObject2D()
{
}

//===================================================
// 初期化処理
//===================================================
HRESULT CObject2D::Init(void)
{
	m_fAngle = 0.0f;
	m_Length = 0.0f;

	m_pPos = new CPosition;
	m_pRot = new CRotation;
	m_pSize = new CSize2D;

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

	return S_OK;
}
//===================================================
// 終了処理
//===================================================
void CObject2D::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuffer != NULL)
	{
		m_pVtxBuffer->Release();
		m_pVtxBuffer = NULL;
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
void CObject2D::Update(void)
{	
	CObject2D::UpdateVertex();
}
//===================================================
// 描画処理
//===================================================
void CObject2D::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	//頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	if (m_nTextureIdx != -1)
	{
		// NULLに戻す
		pDevice->SetTexture(0, pTexture->GetAdress(m_nTextureIdx));
	}
	else
	{
		// テクスチャがない
		pDevice->SetTexture(0, NULL);
	}
	// プレイヤーの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2); // プリミティブの種類	
}

//===================================================
// 頂点のオフセットの設定処理
//===================================================
void CObject2D::SetOffsetVtx(const D3DXCOLOR col, const int nPosX, const int nPosY)
{
	// 頂点情報のポインタ
	VERTEX_2D* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	D3DXVECTOR3 pos = m_pPos->Get();
	D3DXVECTOR3 rot = m_pRot->Get();

	D3DXVECTOR2 Size = m_pSize->Get();

	m_Length = sqrtf((Size.x * Size.x) + (Size.y * Size.y));
	m_fAngle = atan2f(Size.x, Size.y);

	// 頂点座標の設定
	pVtx[0].pos.x = pos.x + sinf(rot.z - (D3DX_PI - m_fAngle)) * m_Length;
	pVtx[0].pos.y = pos.y + cosf(rot.z - (D3DX_PI - m_fAngle)) * m_Length;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = pos.x + sinf(rot.z + (D3DX_PI - m_fAngle)) * m_Length;
	pVtx[1].pos.y = pos.y + cosf(rot.z + (D3DX_PI - m_fAngle)) * m_Length;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = pos.x + sinf(rot.z - m_fAngle) * m_Length;
	pVtx[2].pos.y = pos.y + cosf(rot.z - m_fAngle) * m_Length;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = pos.x + sinf(rot.z + m_fAngle) * m_Length;
	pVtx[3].pos.y = pos.y + cosf(rot.z + m_fAngle) * m_Length;
	pVtx[3].pos.z = 0.0f;

	// rhwの設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

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
// オブジェクト2Dの生成処理
//===================================================
CObject2D* CObject2D::Create(const float fWidth, const float fHeight, const D3DXVECTOR3 pos)
{
	CObject2D* pObject2D = nullptr;

	// 2Dオブジェクトの生成
	pObject2D = new CObject2D;

	// 優先順位の取得
	int nPriority = pObject2D->GetPriority();

	// 現在のオブジェクトの最大数
	const int nNumAll = CObject::GetNumObject(nPriority);

	// オブジェクトが最大数まであったら
	if (nNumAll >= MAX_OBJECT && pObject2D != nullptr)
	{
		// 自分自身の破棄
		pObject2D->Release();

		// nullにしておく
		pObject2D = nullptr;

		return nullptr;
	}

	if (pObject2D == nullptr) return nullptr;

	pObject2D->Init();
	pObject2D->m_pPos->Set(pos);
	pObject2D->m_pSize->Set(fWidth,fHeight);
	pObject2D->SetSize(fWidth, fHeight);
	pObject2D->SetOffsetVtx();
	pObject2D->SetTextureID();

	return pObject2D;
}
//===================================================
// オブジェクトの大きさの設定処理
//===================================================
void CObject2D::SetSize(const float fWidth, const float fHeight)
{
	// 頂点情報のポインタ
	VERTEX_2D* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	D3DXVECTOR3 pos = m_pPos->Get();
	D3DXVECTOR3 rot = m_pRot->Get();

	// 大きさの設定処理
	m_pSize->Set(fWidth, fHeight);

	m_Length = sqrtf((fWidth * fWidth) + (fHeight * fHeight));
	m_fAngle = atan2f(fWidth, fHeight);

	// 頂点座標の設定
	pVtx[0].pos.x = pos.x + sinf(rot.z - (D3DX_PI - m_fAngle)) * m_Length;
	pVtx[0].pos.y = pos.y + cosf(rot.z - (D3DX_PI - m_fAngle)) * m_Length;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = pos.x + sinf(rot.z + (D3DX_PI - m_fAngle)) * m_Length;
	pVtx[1].pos.y = pos.y + cosf(rot.z + (D3DX_PI - m_fAngle)) * m_Length;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = pos.x + sinf(rot.z - m_fAngle) * m_Length;
	pVtx[2].pos.y = pos.y + cosf(rot.z - m_fAngle) * m_Length;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = pos.x + sinf(rot.z + m_fAngle) * m_Length;
	pVtx[3].pos.y = pos.y + cosf(rot.z + m_fAngle) * m_Length;
	pVtx[3].pos.z = 0.0f;

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();
}

//===================================================
// 大きさの設定処理
//===================================================
void CObject2D::SetSize(const float leftWidth, const float rightWdth, const float topHeight, const float buttom)
{
	// 頂点情報のポインタ
	VERTEX_2D* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 位置
	D3DXVECTOR3 pos = m_pPos->Get();

	// 大きさの設定処理
	m_pSize->Set(rightWdth, topHeight);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(pos.x - leftWidth, pos.y - topHeight, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(pos.x + rightWdth, pos.y - topHeight, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(pos.x - leftWidth, pos.y + buttom, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(pos.x + rightWdth, pos.y + buttom, 0.0f);

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();
}
//===================================================
// 位置の更新
//===================================================
void CObject2D::UpdateVertex(void)
{
	// 頂点情報のポインタ
	VERTEX_2D* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	D3DXVECTOR3 pos = m_pPos->Get();
	D3DXVECTOR3 rot = m_pRot->Get();

	// 頂点座標の設定
	pVtx[0].pos.x = pos.x + sinf(rot.z - (D3DX_PI - m_fAngle)) * m_Length;
	pVtx[0].pos.y = pos.y + cosf(rot.z - (D3DX_PI - m_fAngle)) * m_Length;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = pos.x + sinf(rot.z + (D3DX_PI - m_fAngle)) * m_Length;
	pVtx[1].pos.y = pos.y + cosf(rot.z + (D3DX_PI - m_fAngle)) * m_Length;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = pos.x + sinf(rot.z - m_fAngle) * m_Length;
	pVtx[2].pos.y = pos.y + cosf(rot.z - m_fAngle) * m_Length;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = pos.x + sinf(rot.z + m_fAngle) * m_Length;
	pVtx[3].pos.y = pos.y + cosf(rot.z + m_fAngle) * m_Length;
	pVtx[3].pos.z = 0.0f;

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();
}
//===================================================
// テクスチャ座標の設定
//===================================================
void CObject2D::SetUvPos(const D3DXVECTOR2 OffPosTex,const D3DXVECTOR2 PosTex)
{
	// 頂点情報のポインタ
	VERTEX_2D* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(OffPosTex.x, OffPosTex.y);
	pVtx[1].tex = D3DXVECTOR2(OffPosTex.x + PosTex.x, OffPosTex.y);
	pVtx[2].tex = D3DXVECTOR2(OffPosTex.x, OffPosTex.y + PosTex.y);
	pVtx[3].tex = D3DXVECTOR2(OffPosTex.x + PosTex.x, OffPosTex.y + PosTex.y);

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();
}

//===================================================
// 色の設定処理
//===================================================
void CObject2D::SetColor(const D3DXCOLOR col)
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

//===================================================
// テクスチャの設定処理
//===================================================
void CObject2D::SetTextureID(const char* pTextureName)
{
	// テクスチャが無かったら
	if (pTextureName == NULL)
	{
		m_nTextureIdx = -1;
		return;
	}

	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	m_nTextureIdx = pTexture->Register(pTextureName);
}
