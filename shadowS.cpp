//===================================================
//
// ステンシルシャドウ [shadowS.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "shadowS.h"
#include"manager.h"
#include"renderer.h"
#include"input.h"

using namespace Const; // 名前空間Constを使用

//===================================================
// コンストラクタ
//===================================================
CShadowS::CShadowS(int nPriority) : CObjectX(nPriority)
{
	m_pScal = nullptr;
	m_bShowShadow = false;
}

//===================================================
// デストラクタ
//===================================================
CShadowS::~CShadowS()
{
}

//===================================================
// 生成処理
//===================================================
CShadowS* CShadowS::Create(const D3DXVECTOR3 pos, D3DXVECTOR3* pScal)
{
	// 影の生成
	CShadowS* pShadowS = new CShadowS;

	if (FAILED(pShadowS->Init()))
	{
		pShadowS->Uninit();
		pShadowS = nullptr;

		return nullptr;
	}

	pShadowS->SetPosition(pos);
	pShadowS->m_pScal = pScal;
	return pShadowS;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CShadowS::Init(void)
{
	m_pScal = nullptr;

	// 初期化処理
	if (FAILED(CObjectX::Init()))
	{
		return E_FAIL;
	}

	// モデルのロード処理
	LoadModel("data/MODEL/ShadowS.x");

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

	// 頂点情報へのポインタ
	VERTEX_2D* pVtx = nullptr;

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
	pVtx[0].col = D3DXCOLOR(0.0f,0.0f,0.0f, 0.8f);
	pVtx[1].col = D3DXCOLOR(0.0f,0.0f,0.0f, 0.8f);
	pVtx[2].col = D3DXCOLOR(0.0f,0.0f,0.0f, 0.8f);
	pVtx[3].col = D3DXCOLOR(0.0f,0.0f,0.0f, 0.8f);

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
void CShadowS::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuffer != nullptr)
	{
		m_pVtxBuffer->Release();
		m_pVtxBuffer = nullptr;
	}

	// オブジェクトXの生成
	CObjectX::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CShadowS::Update(void)
{

}

//===================================================
// 描画処理
//===================================================
void CShadowS::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ワールドマトリックスの設定
	SetUpMatrix(*m_pScal);

#ifdef _DEBUG

	// キーボードの取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	// 影の表示
	if (pKeyboard != nullptr && pKeyboard->GetTrigger(DIK_F3))
	{
		m_bShowShadow = m_bShowShadow ? false : true;
	}

	if (m_bShowShadow == true)
	{
		// 影の描画処理
		SetUpDraw();
	}
#endif // DEBUG

	// ステンシルテストを有効にする
	pDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);

	// 画面クリア(ステンシルバッファのクリア)
	pDevice->Clear(0,
		NULL,D3DCLEAR_STENCIL,
		D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	// Zバッファへの書き込みを無効にする
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// カラーバッファへの書き込みを無効にする
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0x00000000);

	// ステンシルバッファの比較パラメータの設定
	pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);

	pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_DECR); // ステンシルテスト合格、zテスト合格
	pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_INCRSAT); // ステンシルテスト合格、zテスト不合格
	pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_ZERO); // ステンシルテスト不合格

	// 表面をカリング
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	// 影の描画処理
	SetUpDraw();

	// ステンシルバッファの参照値を設定("1"にする)
	pDevice->SetRenderState(D3DRS_STENCILREF, 0x01);

	// ステンシルバッファの比較パラメータの設定
	pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);

	pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCRSAT); // ステンシルテスト合格、zテスト合格
	pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_DECR);	  // ステンシルテスト合格、zテスト不合格
	pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_ZERO); // ステンシルテスト不合格

	// 裏面をカリング
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// 影の描画処理
	SetUpDraw();

	// カラーバッファへの書き込みを有効にする
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0x0000000F);

	// ステンシルバッファの参照値を設定("2"にする)
	pDevice->SetRenderState(D3DRS_STENCILREF, 0x02);

	// ステンシルバッファの比較パラメータの設定
	pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);

	pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP); // ステンシルテスト合格、zテスト合格
	pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP); // ステンシルテスト合格、zテスト不合格
	pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);  // ステンシルテスト不合格

	//頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャがない
	pDevice->SetTexture(0, NULL);
	
	// プレイヤーの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2); // プリミティブの種類	

	// ステンシルテストを有効にする
	pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);

	// Zバッファへの書き込みを無効にする
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// 表面をカリング
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
