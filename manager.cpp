//===================================================
//
// マネージャー [manager.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "manager.h"
#include"renderer.h"
#include "testword.h"
#include "dome.h"
#include "enemy.h"
#include "object3D.h"
#include<ctime>
#include"Obstacle.h"
#include"Collision.h"
#include"sound.h"
#include"input.h"
#include"textureManager.h"
#include"modelManager.h"
#include"camera.h"
#include"light.h"
#include"slow.h"
#include"scene.h"
#include"object.h"
#include"title.h"
#include "fade.h"

using namespace Const;			// 名前空間Constを使用する
using namespace std;			// 名前空間stdを使用する

//***************************************************
// 静的メンバ変数の宣言
//***************************************************
CRenderer* CManager::m_pRenderer = nullptr;				// レンダラーへのポインタ
CInputKeyboard *CManager::m_pInputKeyboard = nullptr;	// キーボードへのポインタ
CInputJoypad* CManager::m_pInputJoypad = nullptr;		// パッドへのポインタ
CInputMouse* CManager::m_pInputMouse = nullptr;			// マウスへのポインタ
CSound* CManager::m_pSound = nullptr;					// サウンドのポインタ
int CManager::m_nFrameCounter = 0;						// フレームのカウンター
bool CManager::m_bShowDebug = true;						// デバッグ表示をするかしないか
CTextureManager* CManager::m_pTexture = nullptr;		// テクスチャクラスへのポインタ
CCamera* CManager::m_pCamera = nullptr;					// カメラのポインタ
CLight* CManager::m_pLight = nullptr;					// カメラへのポインタ
CModelManager* CManager::m_pModel = nullptr;			// モデルのクラスへのポインタ
bool CManager::m_bPause = false;						// ポーズ
CSlow* CManager::m_pSlow = nullptr;						// スローモーションのポインタ
unique_ptr<CScene> CManager::m_pScene = nullptr;		// シーンクラスへのポインタ
CFade* CManager::m_pFade = nullptr;						// フェードクラスへのポインタ

//===================================================
// コンストラクタ
//===================================================
CManager::CManager()
{
	m_fps = 0;
}
//===================================================
// デストラクタ
//===================================================
CManager::~CManager()
{
}

//===================================================
// 初期化処理
//===================================================
HRESULT CManager::Init(HINSTANCE hInstance,HWND hWnd, BOOL bWindow)
{
	// 乱数の種の設定
	srand(static_cast<unsigned int>(time(nullptr)));

	// レンダラーを生成
	m_pRenderer = new CRenderer;

	// サウンドの初期化処理
	m_pSound = new CSound;

	// キーボードを生成
	m_pInputKeyboard = new CInputKeyboard;

	// パッドを生成
	m_pInputJoypad = new CInputJoypad;

	// マウスを生成
	m_pInputMouse = new CInputMouse;

	// 初期化処理
	if (FAILED(m_pRenderer->Init(hWnd, bWindow))) return E_FAIL;

	// サウンドの初期化処理
	if (FAILED(m_pSound->Init(hWnd))) return E_FAIL;

	// キーボードの初期化処理
	if (FAILED(m_pInputKeyboard->Init(hInstance, hWnd))) return E_FAIL;

	// パッドの初期化処理
	if (FAILED(m_pInputJoypad->Init())) return E_FAIL;

	// マウスの初期化処理
	if (FAILED(m_pInputMouse->Init(hWnd))) return E_FAIL;

	// 当たり判定のマネージャーの生成
 	CCollisionManager::CreateAll();

	// テクスチャの生成
	m_pTexture = new CTextureManager;

	// すべてのテクスチャの読み込み処理
	m_pTexture->Load();

	// モデルの生成
	m_pModel = new CModelManager;
	m_pModel->Load();

	// カメラの生成
	m_pCamera = new CCamera;
	m_pCamera->Init();

	// ライトの生成
	m_pLight = new CLight;
	m_pLight->Init();
	m_pLight->SetLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(1.0f,1.0f, 1.0f,1.0f), D3DXVECTOR3(0.0f, -0.56f, 0.74f), D3DXVECTOR3(0.0f, 100.0f, 0.0f));
	m_pLight->SetLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, -0.56f, -0.74f), D3DXVECTOR3(0.0f, 100.0f, 0.0f));

	//m_pLight->SetLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR3(-0.18f, -0.29f, -0.74f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	//m_pLight->SetLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.5f, -1.0f, -0.74f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	// スローモーションの生成処理
	m_pSlow = new CSlow;

	// フェードの生成
	m_pFade = CFade::Create();

	SetMode(make_unique<CTitle>());

	//// フィールドの設定
	//m_pMeshField = CMeshField::Create(VEC3_NULL ,48,48, D3DXVECTOR2(5500.0f,5500.0f));

	//// ドームの生成
	//CMeshDome::Create(VEC3_NULL,10,10,60000.0f,20000.0f);

	//// ドームの生成
	//CMeshDome::Create(VEC3_NULL, 10, 10, 60000.0f, -20000.0f);

	//// シリンダーの生成
	//m_pCylinder = CMeshCylinder::Create(D3DXVECTOR3(0.0f, 0.0f, 0.0f), 32, 1, 1900.0f, 1900.0f);

	//// プレイヤーの生成
	//m_pPlayer = CPlayer::Create(D3DXVECTOR3(1.0f,0.0f,-1500.0f));

	//CObjectX::Create(VEC3_NULL, "data/MODEL/field/arena.x");

	//CEnemy::Create(D3DXVECTOR3(0.0f,0.0f,1500.0f));

	//// 面の設定
	//int face = CCollisionAABB::FACE_LEFT;

	//// スパイクトラップ
	//CSpikeTrap::Create(D3DXVECTOR3(1840.0f, 0.0f, 0.0f), face);

#ifdef _DEBUG

#else
	// カーソルを非表示にする
	ShowCursor(false);
#endif // _DEBUG

	// 結果を返す
	return S_OK;
}
//===================================================
// 終了処理
//===================================================
void CManager::Uninit(void)
{
	// すべてのサウンドの停止
	m_pSound->StopSound();

	// スローの破棄
	if (m_pSlow != nullptr)
	{
		delete m_pSlow;
		m_pSlow = nullptr;
	}

	// モデルの破棄
	if (m_pModel != nullptr)
	{
		m_pModel->UnLoad();
		delete m_pModel;
		m_pModel = nullptr;
	}

	// カメラの破棄
	if (m_pCamera != nullptr)
	{
		// 終了処理
		m_pCamera->Uninit();

		delete m_pCamera;

		m_pCamera = nullptr;
	}

	// ライトの破棄
	if (m_pLight != nullptr)
	{
		delete m_pLight;

		m_pLight = nullptr;
	}

	// テクスチャの破棄
	if (m_pTexture != nullptr)
	{
		// 終了処理
		m_pTexture->UnLoad();

		delete m_pTexture;

		m_pTexture = nullptr;

	}

	// サウンドの破棄
	if (m_pSound != nullptr)
	{
		// キーボードの終了処理
		m_pSound->Uninit();

		delete m_pSound;

		m_pSound = nullptr;
	}

	// キーボードの破棄
	if (m_pInputKeyboard != nullptr)
	{
		// キーボードの終了処理
		m_pInputKeyboard->Uninit();

		delete m_pInputKeyboard;

		m_pInputKeyboard = nullptr;
	}

	// パッドの破棄
	if (m_pInputJoypad != nullptr)
	{
		// パッドの終了処理
		m_pInputJoypad->Uninit();

		delete m_pInputJoypad;

		m_pInputJoypad = nullptr;
	}

	// パッドの破棄
	if (m_pInputMouse != nullptr)
	{
		// マウスの終了処理
		m_pInputMouse->Uninit();

		delete m_pInputMouse;

		m_pInputMouse = nullptr;
	}

	// レンダラーの破棄
	if (m_pRenderer != nullptr)
	{
		// オブジェクトの終了処理
		m_pRenderer->Uninit();

		delete m_pRenderer;

		m_pRenderer = nullptr;
	}

	// シーンの破棄
	if (m_pScene != nullptr)
	{
		m_pScene->Uninit();
		m_pScene = nullptr;
	}

	// フェードの破棄
	if (m_pFade != nullptr)
	{
		m_pFade->Uninit();
		delete m_pFade;
		m_pFade = nullptr;
	}
}
//===================================================
// 更新処理
//===================================================
void CManager::Update(void)
{
	// フレームのカウンターを増やす
	m_nFrameCounter++;

	// 1フレーム経過したら
	if (m_nFrameCounter > FRAME)
	{
		// カウンターをリセット
		m_nFrameCounter = 0;
	}

	EnablePause();

	if (m_bPause == false)
	{
		if (m_pScene != nullptr)
		{
			// シーンの更新処理
			m_pScene->Update();
		}

		if (m_pFade != nullptr)
		{
			// フェードの更新処理
			m_pFade->Update();
		}

		if (m_pRenderer != nullptr)
		{
			// 更新処理
			m_pRenderer->Update();
		}
	}

	if (m_pInputKeyboard != nullptr)
	{
		// キーボードの更新処理
		m_pInputKeyboard->Update();
	}

	if (m_pInputJoypad != nullptr)
	{
		// パッドの更新処理
		m_pInputJoypad->Update();
	}

	if (m_pInputMouse != nullptr)
	{
		// マウスの更新処理
		m_pInputMouse->Update();
	}

	if (m_pCamera != nullptr)
	{
		// カメラの更新処理
		m_pCamera->Update();
	}

	if (m_pLight != nullptr)
	{	// ライトの更新処理
		m_pLight->Update();
	}

	if (m_pSlow != nullptr)
	{		// スローモーションの更新処理
		m_pSlow->Update();
	}

	if (m_pInputKeyboard->GetTrigger(DIK_F2))
	{
		m_bShowDebug = m_bShowDebug ? false : true;
	}

	// ワイヤーフレームの状態
	static bool bWireFrame = false;

	if (m_pInputKeyboard->GetTrigger(DIK_F6))
	{
		bWireFrame = bWireFrame ? false : true;
	}

#ifdef _DEBUG

	if (bWireFrame == true)
	{
		m_pRenderer->onWireFrame();
	}
	else
	{
		m_pRenderer->offWireFrame();
	}

#endif // _DEBUG
}
//===================================================
// 描画処理
//===================================================
void CManager::Draw(void)
{
	// シーンの描画処理
	if (m_pScene != nullptr)
	{
		m_pScene->Draw();
	}

	if (m_pRenderer != nullptr)
	{
		// 描画処理
		m_pRenderer->Draw(m_fps);
	}
}
//===================================================
// レンダラーの取得処理
//===================================================
CRenderer* CManager::GetRenderer(void)
{
	// レンダラーがNULLだったらNULLを返す
	if (m_pRenderer == nullptr) return nullptr;

	return m_pRenderer;
}
//===================================================
// キーボードの取得処理
//===================================================
CInputKeyboard* CManager::GetInputKeyboard(void)
{
	// NULLだったら
	if (m_pInputKeyboard == nullptr) return nullptr;

	return m_pInputKeyboard;
}
//===================================================
// パッドの取得処理
//===================================================
CInputJoypad* CManager::GetInputJoypad(void)
{
	// NULLだったら
	if (m_pInputJoypad == nullptr) return nullptr;

	return m_pInputJoypad;
}
//===================================================
// マウスの取得処理
//===================================================
CInputMouse* CManager::GetInputMouse(void)
{
	// NULLだったら
	if (m_pInputMouse == nullptr) return nullptr;

	return m_pInputMouse;
}
//===================================================
// サウンドの取得処理
//===================================================
CSound* CManager::GetSound(void)
{
	// NULLだったら
	if (m_pSound == nullptr) return nullptr;

	return m_pSound;
}

//===================================================
// フレームのカウンター
//===================================================
int CManager::GetFrameCounter(void)
{
	// 現在のフレームの取得
	return m_nFrameCounter;
}

//===================================================
// テクスチャクラスへのポインタ
//===================================================
CTextureManager* CManager::GetTexture(void)
{
	// NULLだったら
	if (m_pTexture == nullptr) return nullptr;

	return m_pTexture;
}

//===================================================
// カメラの取得
//===================================================
CCamera* CManager::GetCamera(void)
{
	// NULLだったら
	if (m_pCamera == nullptr) return nullptr;

	return m_pCamera;
}

//===================================================
// ライトの取得
//===================================================
CLight* CManager::GetLight(void)
{
	// NULLだったら
	if (m_pLight == nullptr) return nullptr;

	return m_pLight;
}

//===================================================
// モデルクラスの取得
//===================================================
CModelManager* CManager::GetModel(void)
{
	// NULLだったら
	if (m_pModel == nullptr) return nullptr;

	return m_pModel;
}

//===================================================
// ポーズの設定
//===================================================
void CManager::EnablePause(void)
{
	if (m_pInputKeyboard->GetTrigger(DIK_P))
	{
		m_bPause = m_bPause ? false : true;
	}
}

//===================================================
// シーンの設定処理
//===================================================
void CManager::SetMode(unique_ptr<CScene> pNewScene)
{
	if (m_pScene != nullptr)
	{
		// シーンが同じだったら設定しない
		if (m_pScene->GetMode() == pNewScene->GetMode())
		{
			return;
		}
	}

	// 今のシーンの破棄
	if (m_pScene != nullptr)
	{
		m_pScene->Uninit();
		m_pScene.reset();
		m_pScene = nullptr;

		// すべてのオブジェクトの破棄
		CObject::ReleaseAll();
	}
	
	// しっかり破棄できているなら
	if (m_pScene == nullptr)
	{
		// 新しいシーンを設定
		m_pScene = std::move(pNewScene);

		m_pCamera->Init();

		// シーンの初期化処理
		m_pScene->Init();
	}
}