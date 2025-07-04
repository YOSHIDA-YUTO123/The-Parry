//===================================================
//
// ポリゴンの描画処理 [renderer.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "renderer.h"
#include"object.h"
#include "object2D.h"
#include "manager.h"

//***************************************************
// 静的メンバ変数の宣言
//***************************************************
CDebugProc* CRenderer::m_pDebug = nullptr;	// デバッグオブジェクトへのポインタ

//===================================================
// コンストラクタ
//===================================================
CRenderer::CRenderer()
{
	m_pD3D = nullptr;			// Directxのデバイスの初期化
	m_pD3DDevice = nullptr;		// Directxのデバイスの初期化

	for (int nCnt = 0; nCnt < NUM_TEXTUREMT; nCnt++)
	{
		m_pRenderMT[nCnt] = nullptr;		// レンダリングターゲット用インターフェース
		m_pTextureMT[nCnt] = nullptr;		// レンダリングターゲット用テクスチャ
	}
	m_pVtxBuffMT = nullptr;

	m_pZBuffMT = nullptr;		// レンダリングターゲット用Zバッファ
	ZeroMemory(&m_viewport, sizeof(m_viewport)); // ビューポート
	m_bEffect = false;
}
//===================================================
// デストラクタ
//===================================================
CRenderer::~CRenderer()
{

}
//===================================================
// 初期化処理
//===================================================
HRESULT CRenderer::Init(HWND hWnd, BOOL bWindow)
{
	D3DDISPLAYMODE d3ddm;		 // ディスプレイモード
	D3DPRESENT_PARAMETERS d3dpp; // プレゼンテーションパラメータ

	// Direct3Dオブジェクトの生成
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (m_pD3D == NULL)
	{
		return E_FAIL;
	}

	// 現在のディスプレイモードを取得
	if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// デバイスのプレゼンテーションパラメータの設定
	ZeroMemory(&d3dpp, sizeof(d3dpp)); // パラメータのゼロクリア

	d3dpp.BackBufferWidth = SCREEN_WIDTH;							// ゲーム画面サイズ(幅)
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;							// ゲーム画面サイズ(高さ)
	d3dpp.BackBufferFormat = d3ddm.Format;							// バックバッファの形式
	d3dpp.BackBufferCount = 1;										// バックバッファの数
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;						// バックバッファの切り替え(映像信号に同期)
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;						// デプスバッファとして16bitを使う
	d3dpp.Windowed = bWindow;										// ウインドウモード
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;		// リフレッシュレート
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;		// インターバル

	// Direct3Dデバイスの生成描画処理と頂点処理をハードウェアで行う
	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp,
		&m_pD3DDevice)))
	{
		if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp,
			&m_pD3DDevice)))
		{
			if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&d3dpp,
				&m_pD3DDevice)))
			{
				return E_FAIL;
			}
		}
	}

	// レンダーステートの設定
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// サンプラーステートの設定
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// テクスチャステージステートの設定
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	// 初期化処理
	m_pDebug->Init();

	LPDIRECT3DSURFACE9 pRenderDef, pZBufferDef;

	for (int nCnt = 0; nCnt < NUM_TEXTUREMT; nCnt++)
	{
		// レンダラーターゲット用テクスチャの生成
		m_pD3DDevice->CreateTexture(
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			1,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_X8R8G8B8,
			D3DPOOL_DEFAULT,
			&m_pTextureMT[nCnt],
			NULL);

		// テクスチャのレンダリングターゲット用インターフェースの生成
		m_pTextureMT[nCnt]->GetSurfaceLevel(0, &m_pRenderMT[nCnt]);
	}

	// テクスチャレンダリング用Zバッファの生成
	m_pD3DDevice->CreateDepthStencilSurface(
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		D3DFMT_D16,
		D3DMULTISAMPLE_NONE,
		0,
		TRUE,
		&m_pZBuffMT,
		NULL);
	

	// 現在のレンダリングターゲットを取得(保存)
	m_pD3DDevice->GetRenderTarget(0, &pRenderDef);

	// 現在のZバッファの取得(保存)
	m_pD3DDevice->GetDepthStencilSurface(&pZBufferDef);

	for (int nCnt = 0; nCnt < NUM_TEXTUREMT; nCnt++)
	{
		// レンダリングターゲットを生成したテクスチャに設定
		m_pD3DDevice->SetRenderTarget(0, m_pRenderMT[nCnt]);
	}

	// Zバッファを生成したZバッファの設定
	m_pD3DDevice->SetDepthStencilSurface(m_pZBuffMT);

	// レンダリングターゲット用テクスチャのクリア
	m_pD3DDevice->Clear(0,
		NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	// レンダーターゲットをもとに戻す
	m_pD3DDevice->SetRenderTarget(0, pRenderDef);

	// Zバッファをもとに戻す
	m_pD3DDevice->SetDepthStencilSurface(pZBufferDef);

	// テクスチャレンダリング用ビューポートの生成
	m_viewport.X = 0;
	m_viewport.Y = 0;
	m_viewport.Width = SCREEN_WIDTH;
	m_viewport.Height = SCREEN_HEIGHT;
	m_viewport.MinZ = 0.0f;
	m_viewport.MaxZ = 1.0f;

	//頂点バッファの生成・頂点情報の設定
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * NUM_TEXTUREMT,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuffMT,
		NULL)))
	{
		return E_FAIL;
	}

	// 頂点情報のポインタ
	VERTEX_2D* pVtx;

	// 頂点バッファのロック
	m_pVtxBuffMT->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0; nCnt < NUM_TEXTUREMT; nCnt++)
	{
		D3DXVECTOR2 pos = D3DXVECTOR2(640.0f, 360.0f);

		float width = SCREEN_WIDTH * 0.5f;
		float height = SCREEN_HEIGHT * 0.5f;

		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(pos.x - width, pos.y - height, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pos.x + width, pos.y - height, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pos.x - width, pos.y + height, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pos.x + width, pos.y + height, 0.0f);

		// rhwの設定
		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

		float fA = (nCnt == 0) ? 1.0f : 0.5f;

		// 頂点カラーの設定
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fA);
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fA);
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fA);
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, fA);

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += 4;
	}
	// 頂点バッファのアンロック
	m_pVtxBuffMT->Unlock();

	return S_OK;
}
//===================================================
// 終了処理
//===================================================
void CRenderer::Uninit(void)
{
	// 終了処理
	m_pDebug->Uninit();

	// すべてのオブジェクトの破棄
	CObject::ReleaseAll();

	for (int nCnt = 0; nCnt < NUM_TEXTUREMT; nCnt++)
	{
		// レンダリングターゲット用インターフェースの破棄
		if (m_pRenderMT[nCnt]!= nullptr)
		{
			m_pRenderMT[nCnt]->Release();
			m_pRenderMT[nCnt] = nullptr;
		}

		// レンダリングターゲット用テクスチャの破棄
		if (m_pTextureMT[nCnt] != nullptr)
		{
			m_pTextureMT[nCnt]->Release();
			m_pTextureMT[nCnt] = nullptr;
		}
	}

	// レンダリングターゲット用Zバッファの破棄
	if (m_pZBuffMT != nullptr)
	{
		m_pZBuffMT->Release();
		m_pZBuffMT = nullptr;
	}

	// フィードバックエフェクト用頂点バッファの破棄
	if (m_pVtxBuffMT != nullptr)
	{
		m_pVtxBuffMT->Release();
		m_pVtxBuffMT = nullptr;
	}

	// Drectxデバイスの破棄
	if (m_pD3DDevice != NULL)
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}

	// Drectxデバイスの破棄
	if (m_pD3D != NULL)
	{
		m_pD3D->Release();
		m_pD3D = NULL;
	}
}
//===================================================
// 更新処理
//===================================================
void CRenderer::Update(void)
{
	if (CManager::GetInputKeyboard()->GetTrigger(DIK_H))
	{
		m_bEffect = m_bEffect ? false : true;
	}
	// すべてのオブジェクトの更新処理
	CObject::UpdateAll();
}
//===================================================
// 描画処理
//===================================================
void CRenderer::Draw(const int fps)
{
	// 画面クリア(バックバッファ&Zバッファのクリア)
	m_pD3DDevice->Clear(0,
		NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	CCamera* pCamera = CManager::GetCamera();

	D3DXVECTOR3 posV = pCamera->GetPosV(), posR = pCamera->GetPosR();
	D3DXVECTOR3 vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	LPDIRECT3DSURFACE9 pRenderDef, pZBuffer;
	D3DVIEWPORT9 viepowtDef;
	D3DXMATRIX mtxViewDef, mtxProjectionDef;

	// 現在のレンダーターゲットの取得
	m_pD3DDevice->GetRenderTarget(0, &pRenderDef);

	// 現在のZバッファの取得
	m_pD3DDevice->GetDepthStencilSurface(&pZBuffer);

	// 現在のビューポートの取得
	m_pD3DDevice->GetViewport(&viepowtDef);

	// 現在のビューマトリックスの取得
	m_pD3DDevice->GetTransform(D3DTS_VIEW, &mtxViewDef);

	// 現在のプロジェクションマトリックスの取得
	m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &mtxProjectionDef);

	// 描画開始
	if (SUCCEEDED(m_pD3DDevice->BeginScene()))
	{//描画開始が成功した場合

		if (m_bEffect == true)
		{
			// レンダリングターゲットの変更
			ChangeTarget(D3DXVECTOR3(0.0f,240.0f,400.0f), D3DXVECTOR3(0.0f,0.0f,0.0f), vecU);

			// 画面クリア(バックバッファ&Zバッファのクリア)
			m_pD3DDevice->Clear(0,
				NULL,
				(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
				D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

		}

		// すべてのオブジェクトの描画処理
		CObject::DrawAll();

		if (m_bEffect == true)
		{
			// サンプラーステートの設定
			m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
			m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

			//頂点バッファをデータストリームに設定
			m_pD3DDevice->SetStreamSource(0, m_pVtxBuffMT, 0, sizeof(VERTEX_2D));

			// 頂点フォーマットの設定
			m_pD3DDevice->SetFVF(FVF_VERTEX_2D);

			// NULLに戻す
			m_pD3DDevice->SetTexture(0, m_pTextureMT[1]);

			// プレイヤーの描画
			m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4, 2); // プリミティブの種類	

			// レンダーターゲットをもとに戻す
			m_pD3DDevice->SetRenderTarget(0, pRenderDef);

			// Zバッファをもとに戻す
			m_pD3DDevice->SetDepthStencilSurface(pZBuffer);

			//// 画面クリア(バックバッファ&Zバッファのクリア)
			m_pD3DDevice->Clear(0,
				NULL,
				(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
				D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);


			// ビューポートをもとに戻す
			m_pD3DDevice->SetViewport(&viepowtDef);

			m_pD3DDevice->SetTransform(D3DTS_VIEW, &mtxViewDef);

			m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &mtxProjectionDef);

			//頂点バッファをデータストリームに設定
			m_pD3DDevice->SetStreamSource(0, m_pVtxBuffMT, 0, sizeof(VERTEX_2D));

			// 頂点フォーマットの設定
			m_pD3DDevice->SetFVF(FVF_VERTEX_2D);

			// NULLに戻す
			m_pD3DDevice->SetTexture(0, m_pTextureMT[0]);

			// プレイヤーの描画
			m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2); // プリミティブの種類	

			LPDIRECT3DTEXTURE9 pTextureWK;
			LPDIRECT3DSURFACE9 pRenderWK;

			// テクスチャ0と1を入れ替える
			pTextureWK = m_pTextureMT[0];
				m_pTextureMT[0] = m_pTextureMT[1];
				m_pTextureMT[1] = pTextureWK;

			// レンダリング0と1を入れ替える
			pRenderWK = m_pRenderMT[0];
			m_pRenderMT[0] = m_pRenderMT[1];
			m_pRenderMT[1] = pRenderWK;	
		}
		CDebugProc::Print("FPS = %d\n", fps);

		CDebugProc::Print("デバッグ 非表示      : [ F2 ]\n");

		// 文字の表示
		CDebugProc::Print("オブジェクトの総数 : 最大数 : [ %d ]\n",MAX_OBJECT);

		for (int nCntPriority = 0; nCntPriority < NUM_PRIORITY; nCntPriority++)
		{
			int nNumAll = CObject::GetNumObject(nCntPriority);

			// 文字の表示
			CDebugProc::Print("[ Priority = %d : Numobj = %d]\n",nCntPriority,nNumAll);
		}

		CDebugProc::Print("ワイヤーフレーム : [ F6 ]");

		CDebugProc::Draw();

		CDebugProc::Reset();
		// 描画終了
		m_pD3DDevice->EndScene();
	}

	//バックバッファとフロントバッファの入れ替え
	m_pD3DDevice->Present(NULL, NULL, NULL, NULL);


}

//===================================================
// レンダリングターゲットの変更
//===================================================
void CRenderer::ChangeTarget(D3DXVECTOR3 posV, D3DXVECTOR3 posR, D3DXVECTOR3 vecU)
{
	D3DXMATRIX mtxView, mtxProjection; // ビューマトリックス、プロジェクションマトリックス
	float fAspect;

	// レンダリングターゲットを生成したテクスチャに設定
	m_pD3DDevice->SetRenderTarget(0, m_pRenderMT[0]);

	// 生成したZバッファに設定
	m_pD3DDevice->SetDepthStencilSurface(m_pZBuffMT);

	// テクスチャレンダリング用ビューポートの設定
	m_pD3DDevice->SetViewport(&m_viewport);

	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&mtxView);

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&mtxProjection);

	// ビューマトリックスの作成
	D3DXMatrixLookAtLH(
		&mtxView,
		&posV,
		&posR,
		&vecU);

	// ビューマトリックスの設定
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &mtxView);

	fAspect = (float)m_viewport.Width / (float)m_viewport.Height;

	// プロジェクションマトリックスの作成
	D3DXMatrixPerspectiveFovLH(&mtxProjection,
		D3DXToRadian(45.0f),
		fAspect,
		1.0f,
		100000.0f);

	// プロジェクションマトリックスの設定
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &mtxProjection);
}

//===================================================
// デバイスの取得処理
//===================================================
LPDIRECT3DDEVICE9 CRenderer::GetDevice(void)
{
	// デバイスのポインタを返す
	return m_pD3DDevice;
}

//===================================================
// ワイヤーフレームオン
//===================================================
void CRenderer::onWireFrame()
{
	m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
}

//===================================================
// ワイヤーフレームオフ
//===================================================
void CRenderer::offWireFrame()
{
	m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}
