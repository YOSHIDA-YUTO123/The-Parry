//===================================================
//
// 2Dオブジェクト(マルチテクスチャ)の描画 [object2DMT.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "object2DMT.h"
#include "manager.h"
#include"renderer.h"
#include"textureManager.h"

using namespace Const; // 名前空間Constを使用

//===================================================
// コンストラクタ
//===================================================
CObject2DMT::CObject2DMT() : CObject(7)
{
	// すべてに-1を入れる
	memset(&m_nTextureIdx, -1, sizeof(m_nTextureIdx));
	m_rot = VEC3_NULL;
	m_pos = VEC3_NULL;
	m_pVtxBuffer = nullptr;
	m_Size = VEC2_NULL;
	m_col = WHITE;
}

//===================================================
// デストラクタ
//===================================================
CObject2DMT::~CObject2DMT()
{
}

//===================================================
// 生成処理
//===================================================
CObject2DMT* CObject2DMT::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size)
{
	// 2Dオブジェクトの生成
	CObject2DMT* pObject = new CObject2DMT;

	pObject->SetPosition(pos);
	pObject->SetRotaition(VEC3_NULL);
	pObject->SetSize(Size);

	if (FAILED(pObject->Init()))
	{// 初期化に失敗したら
		// 終了処理
		pObject->Uninit();
		pObject = nullptr;

		return nullptr;
	}

	return pObject;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CObject2DMT::Init(void)
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

	D3DXVECTOR3 pos = m_pos;
	D3DXVECTOR3 rot = m_rot;

	// 対角線の長さを求める
	float fLength = sqrtf((m_Size.x * m_Size.x) + (m_Size.y * m_Size.y));

	// 傾きを求める
	float fAngle = atan2f(m_Size.x, m_Size.y);

	// 頂点座標の設定
	pVtx[0].pos.x = pos.x + sinf(rot.z - (D3DX_PI - fAngle)) * fLength;
	pVtx[0].pos.y = pos.y + cosf(rot.z - (D3DX_PI - fAngle)) * fLength;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = pos.x + sinf(rot.z + (D3DX_PI - fAngle)) * fLength;
	pVtx[1].pos.y = pos.y + cosf(rot.z + (D3DX_PI - fAngle)) * fLength;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = pos.x + sinf(rot.z - fAngle) * fLength;
	pVtx[2].pos.y = pos.y + cosf(rot.z - fAngle) * fLength;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = pos.x + sinf(rot.z + fAngle) * fLength;
	pVtx[3].pos.y = pos.y + cosf(rot.z + fAngle) * fLength;
	pVtx[3].pos.z = 0.0f;

	// rhwの設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラーの設定
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// テクスチャ座標の設定
	pVtx[0].texMT = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].texMT = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].texMT = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].texMT = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CObject2DMT::Uninit(void)
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
void CObject2DMT::Update(void)
{
	// 頂点情報のポインタ
	VERTEX_2D_MULT* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	D3DXVECTOR3 pos = m_pos;
	D3DXVECTOR3 rot = m_rot;

	// 対角線の長さを求める
	float fLength = sqrtf((m_Size.x * m_Size.x) + (m_Size.y * m_Size.y));

	// 傾きを求める
	float fAngle = atan2f(m_Size.x, m_Size.y);

	// 頂点座標の設定
	pVtx[0].pos.x = pos.x + sinf(rot.z - (D3DX_PI - fAngle)) * fLength;
	pVtx[0].pos.y = pos.y + cosf(rot.z - (D3DX_PI - fAngle)) * fLength;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = pos.x + sinf(rot.z + (D3DX_PI - fAngle)) * fLength;
	pVtx[1].pos.y = pos.y + cosf(rot.z + (D3DX_PI - fAngle)) * fLength;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = pos.x + sinf(rot.z - fAngle) * fLength;
	pVtx[2].pos.y = pos.y + cosf(rot.z - fAngle) * fLength;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = pos.x + sinf(rot.z + fAngle) * fLength;
	pVtx[3].pos.y = pos.y + cosf(rot.z + fAngle) * fLength;
	pVtx[3].pos.z = 0.0f;

	// 頂点カラーの設定
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();
}

//===================================================
// 描画処理
//===================================================
void CObject2DMT::Draw(void)
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
// テクスチャのIDの登録
//===================================================
void CObject2DMT::SetTextureID(const char* pTexture1, const char* pTexture2)
{
	// テクスチャクラスの取得
	CTextureManager* pTexture = CManager::GetTexture();

	// IDの登録
	m_nTextureIdx[0] = pTexture->Register(pTexture1);

	// IDの登録
	m_nTextureIdx[1] = pTexture->Register(pTexture2);
}

//===================================================
// テクスチャ座標の設定
//===================================================
void CObject2DMT::SetTexture(const D3DXVECTOR2 tex0, const D3DXVECTOR2 tex1, const D3DXVECTOR2 tex2, const D3DXVECTOR2 tex3, const int nCnt)
{
	// 頂点情報のポインタ
	VERTEX_2D_MULT* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	if (nCnt == 0)
	{
		// テクスチャ座標の設定
		pVtx[0].tex = tex0;
		pVtx[1].tex = tex1;
		pVtx[2].tex = tex2;
		pVtx[3].tex = tex3;
	}
	else
	{
		// テクスチャ座標の設定
		pVtx[0].texMT = tex0;
		pVtx[1].texMT = tex1;
		pVtx[2].texMT = tex2;
		pVtx[3].texMT = tex3;
	}

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();
}
