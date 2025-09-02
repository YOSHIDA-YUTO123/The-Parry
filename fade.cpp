//===================================================
//
// フェード処理 [fade.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "fade.h"
#include"manager.h"
#include"renderer.h"

using namespace Const; // 名前空間Constを使用
using namespace std; // 名前空間stdを使用

//===================================================
// コンストラクタ
//===================================================
CFade::CFade()
{
	m_pScene = nullptr;
	m_Fade = FADE_NONE;
	m_col = D3DXCOLOR(0.0f, 0.0, 0.0f, 1.0f);
}

//===================================================
// デストラクタ
//===================================================
CFade::~CFade()
{
}

//===================================================
// 生成処理
//===================================================
CFade* CFade::Create(void)
{
	// フェードの生成
	CFade* pFade = new CFade;

	if (FAILED(pFade->Init()))
	{
		pFade->Uninit();

		pFade = nullptr;

		return nullptr;
	}

	return pFade;
}

//===================================================
// フェード
//===================================================
void CFade::SetFade(unique_ptr<CScene> pNewScene, const D3DXCOLOR col)
{
	if (m_Fade != FADE_NONE)
	{
		// 破棄する
		pNewScene.reset();
		pNewScene = nullptr;

		return;
	}

	if (m_pScene != nullptr)
	{
		m_pScene->Uninit();
		m_pScene.reset();
		m_pScene = nullptr;
	}

	m_col = col;
	m_Fade = FADE_OUT;
	m_pScene = move(pNewScene);
}

//===================================================
// 初期化処理
//===================================================
HRESULT CFade::Init(void)
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

	m_Fade = FADE_IN;
	m_col = D3DXCOLOR(0.0f, 0.0, 0.0f, 1.0f);
	m_pScene = nullptr;

	// 頂点情報のポインタ
	VERTEX_2D* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(1280.0f, 0.0f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(0.0f, 720.0f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(1280.0f, 720.0f, 0.0f);

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

	// 頂点バッファのアンロック
	m_pVtxBuffer->Unlock();

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CFade::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuffer != nullptr)
	{
		m_pVtxBuffer->Release();
		m_pVtxBuffer = nullptr;
	}
}

//===================================================
// 更新処理
//===================================================
void CFade::Update(void)
{
	// 頂点情報のポインタ
	VERTEX_2D* pVtx;

	if (m_Fade != FADE_NONE)
	{
		if (m_Fade == FADE_IN)
		{
			m_col.a -= 0.03f;

			if (m_col.a <= 0.0f)
			{
				m_col.a = 0.0f;
				m_Fade = FADE_NONE;
			}

			// 頂点バッファのロック
			m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

			// 頂点カラーの設定
			pVtx[0].col = m_col;
			pVtx[1].col = m_col;
			pVtx[2].col = m_col;
			pVtx[3].col = m_col;

			// 頂点バッファのアンロック
			m_pVtxBuffer->Unlock();
		}
		else if (m_Fade == FADE_OUT)
		{
			// フェードアウト状態
			m_col.a += 0.03f;

			if (m_col.a >= 1.0f)
			{
				m_col.a = 1.0f;
				m_Fade = FADE_IN;

				// モード設定
				CManager::SetMode(move(m_pScene));
				return;
			}

			// 頂点バッファのロック
			m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

			// 頂点カラーの設定
			pVtx[0].col = m_col;
			pVtx[1].col = m_col;
			pVtx[2].col = m_col;
			pVtx[3].col = m_col;

			// 頂点バッファのアンロック
			m_pVtxBuffer->Unlock();
		}
	}
}

//===================================================
// 描画処理
//===================================================
void CFade::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャがない
	pDevice->SetTexture(0, NULL);

	// プレイヤーの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2); // プリミティブの種類	
}

