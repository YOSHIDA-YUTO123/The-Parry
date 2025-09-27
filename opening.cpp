//===================================================
//
// オープニングシーン [opening.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "opening.h"
#include "manager.h"
#include "input.h"
#include "OpningEnemy.h"
#include "OpeningCamera.h"
#include "meshfield.h"
#include "objectX.h"
#include "light.h"
#include "debugproc.h"
#include "FadeEffect.h"
#include "dome.h"
#include "fade.h"
#include "game.h"
#include "BirdManager.h"
#include "Obstacle.h"
#include "FadeInObject2D.h"
#include "SkipUI.h"
#include "gear.h"
#include "renderer.h"

using namespace Const; // 名前空間Constの使用
using namespace std;   // 名前空間stdの使用

//***************************************************
// 静的メンバ変数宣言
//***************************************************
CFadeEffect* COpening::m_pFadeEffect = nullptr;				// フェードエフェクトへのポインタ
COpening::STATE COpening::m_state = COpening::STATE_NORMAL;	// オープニングの状態
COpeningCamera* COpening::m_pCamera;						// カメラのポインタ
CMeshField* COpening::m_pMeshField = nullptr;				// メッシュフィールドへのポインタ

//===================================================
// コンストラクタ
//===================================================
COpening::COpening() : CScene(MODE_OPENING)
{
	m_bTutorial = false;
}

//===================================================
// デストラクタ
//===================================================
COpening::~COpening()
{
	// レンダラーの取得
	CRenderer* pRenderer = CManager::GetRenderer();

	if (pRenderer != nullptr)
	{
		// エフェクトをOFFにする
		pRenderer->offEffect();
	}
}

//===================================================
// 初期化処理
//===================================================
HRESULT COpening::Init(void)
{
	// 鳥のマネージャの取得
	CBirdManager::Create();

	// ゲームのカメラの生成
	m_pCamera = new COpeningCamera;
	m_pCamera->Init();
	m_pCamera->EnableTutorial(m_bTutorial);

	// ライトの取得
	CLight* pLight = CManager::GetLight();
	pLight->Init();

	// ポイントライトの設定処理
	pLight->SetPoint(D3DXVECTOR3(-200.0f, 300.0f, 0.0f), 1000.0f, D3DCOLOR_RGBA(255, 255, 255, 255), D3DCOLOR_RGBA(255, 255, 255, 255));

	// ドームの生成
	CMeshDome::Create(VEC3_NULL, 10, 10, 60000.0f, 20000.0f);

	// ドームの生成
	CMeshDome::Create(VEC3_NULL, 10, 10, 60000.0f, -20000.0f);

	// フェードの生成
	m_pFadeEffect = CFadeEffect::Create(CFadeEffect::FADE_NONE);

	// スキップUIの生成
	CSkipUI::Create(D3DXVECTOR3(1000.0f, 650.0f, 0.0f), D3DXVECTOR2(150.0f, 50.0f), 60);

	// フィールドの設定
	m_pMeshField = CMeshField::Create(VEC3_NULL, 48, 48, D3DXVECTOR2(3500.0f, 3500.0f));

	// アリーナの生成
	CObjectX::Create(VEC3_NULL, "data/MODEL/field/arena.x", VEC3_NULL);

	// フィールドの設定
	CMeshField::Create(D3DXVECTOR3(5500.0f, 0.0f, 0.0f), 48, 48, D3DXVECTOR2(1500.0f, 1500.0f));

	// アリーナの生成
	CObjectX::Create(D3DXVECTOR3(5500.0f,0.0f,0.0f), "data/MODEL/field/TitleArena.x", D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	// 鳥のマネージャーの取得
	auto pBirdManager = CBirdManager::GetInstance();

	if (pBirdManager != nullptr)
	{
		// オープニングの鳥
		pBirdManager->SetOpening();
	}

	// スパイクトラップ
	CSpikeTrap::Create(D3DXVECTOR3(-1540.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, D3DX_PI, 0.0f), -1);

	// スパイクトラップ
	CSpikeTrap::Create(D3DXVECTOR3(1540.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), -1);

	// 歯車の生成
	CGear::Create(D3DXVECTOR3(-2320.0f, 1700.0f, 100.0f));

	// 歯車の生成
	CGear::Create(D3DXVECTOR3(2100.0f, 1700.0f, 100.0f));

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void COpening::Uninit(void)
{
	if (m_pCamera != nullptr)
	{
		m_pCamera->Uninit();
		delete m_pCamera;
		m_pCamera = nullptr;
	}
}

//===================================================
// 更新処理
//===================================================
void COpening::Update(void)
{
	// キーボードの取得
	auto pKeyboard = CManager::GetInputKeyboard();

	// パッドの取得
	auto pJoyPad = CManager::GetInputJoypad();

	// フェードの取得
	CFade* pFade = CManager::GetFade();

	if (pKeyboard->GetTrigger(DIK_RETURN) || pJoyPad->GetTrigger(pJoyPad->JOYKEY_A))
	{
		// ゲームシーンへ以降
		pFade->SetFade(make_unique<CGame>());
	}
	if (m_pCamera != nullptr)
	{
		m_pCamera->Update();
	}
}

//===================================================
// 描画処理
//===================================================
void COpening::Draw(void)
{
#ifdef _DEBUG
	// カメラの位置
	D3DXVECTOR3 cameraposV = m_pCamera->GetPosV();
	D3DXVECTOR3 cameraposR = m_pCamera->GetPosR();
	D3DXVECTOR3 camerarot = m_pCamera->GetRotaition();

	// デバッグ情報
	CDebugProc::Print("視点の座標 : [ %.2f ] [ %.2f ] [ %.2f ] \n", cameraposV.x, cameraposV.y, cameraposV.z);
	CDebugProc::Print("注視点の座標 : [ %.2f ] [ %.2f ] [ %.2f ] \n", cameraposR.x, cameraposR.y, cameraposR.z);
	CDebugProc::Print("カメラの向き : [ %.2f ] [ %.2f ] [ %.2f ] \n", camerarot.x, camerarot.y, camerarot.z);
#endif // _DEBUG

	if (m_pCamera != nullptr)
	{
		m_pCamera->SetCamera();
	}
}
