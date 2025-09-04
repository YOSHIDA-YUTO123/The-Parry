//===================================================
//
// ゲームシーン [game.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "game.h"
#include "meshfield.h"
#include "dome.h"
#include"cylinder.h"
#include"player.h"
#include"objectX.h"
#include"enemy.h"
#include"Obstacle.h"
#include "manager.h"
#include"input.h"
#include"result.h"
#include"fade.h"
#include "debugproc.h"
#include "Gage.h"
#include "Observer.h"
#include"GageFrame.h"
#include "pause.h"
#include"GameCamera.h"
#include"light.h"
#include "RevengeGage.h"
#include "block.h"
#include "BlockManager.h"

using namespace Const; // 名前空間Constを使用
using namespace std; // 名前空間stdを使用

//***************************************************
// 静的メンバ変数宣言
//***************************************************
CMeshField* CGame::m_pMeshField = nullptr;					   // メッシュフィールドへのポインタ
CPlayer* CGame::m_pPlayer = nullptr;						   // プレイヤーへのポインタ
CMeshCylinder* CGame::m_pCylinder = nullptr;				   // メッシュシリンダーへのポインタ
CGame::STATE CGame::m_state = STATE_NORMAL;					   // ゲームの状態
CGameCamera* CGame::m_pCamera = nullptr;					   // ゲームカメラクラスへのポインタ
CGame::RESULTTYPE CGame::m_ResultType = CGame::RESULTTYPE_WIN; // リザルトの種類
unique_ptr<CGameManager> CGameManager::m_pInstance = nullptr;  // 自分のインスタンス
int CGameManager::m_nGameTime = 0;							   // ゲームの経過時間

//===================================================
// コンストラクタ
//===================================================
CGame::CGame() : CScene(MODE_GAME)
{
	m_nCounterState = NULL;
	m_pPauseManager = nullptr;
}

//===================================================
// デストラクタ
//===================================================
CGame::~CGame()
{

}

//===================================================
// 初期化処理
//===================================================
HRESULT CGame::Init(void)
{
	// ブロックマネージャーの取得
	auto pBlockManager = CBlockManager::GetInstance();

	// 取得できたら
	if (pBlockManager != nullptr)
	{
		// 終了
		pBlockManager->Uninit();
	}

	// ゲームマネージャーの生成
	CGameManager::Create();

	// ゲームのカメラの生成
	m_pCamera = new CGameCamera;
	m_pCamera->Init();

	// ライトの取得
	CLight* pLight = CManager::GetLight();
	pLight->Init();

	// ライトの設定処理
	pLight->SetDirectional(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, -0.56f, 0.74f), D3DXVECTOR3(0.0f, 100.0f, 0.0f));
	pLight->SetDirectional(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, -0.56f, -0.74f), D3DXVECTOR3(0.0f, 100.0f, 0.0f));

	// フィールドの設定
	m_pMeshField = CMeshField::Create(VEC3_NULL, 48, 48, D3DXVECTOR2(3500.0f, 3500.0f));

	// ドームの生成
	CMeshDome::Create(VEC3_NULL, 10, 10, 60000.0f, 20000.0f);

	// ドームの生成
	CMeshDome::Create(VEC3_NULL, 10, 10, 60000.0f, -20000.0f);

	// シリンダーの生成
	m_pCylinder = CMeshCylinder::Create(D3DXVECTOR3(0.0f, 0.0f, 0.0f), 32, 1, 1500.0f, 1500.0f);

	m_pCylinder->Set(CMeshCylinder::TYPE_WALL);

	// プレイヤーの生成
	m_pPlayer = CPlayer::Create(D3DXVECTOR3(1.0f, 0.0f, -1200.0f),D3DXVECTOR3(0.0f,D3DX_PI,0.0f));

	// 反撃UIの生成
	auto pRevenge = CRevengeUI::Create(D3DXVECTOR3(50.0f, 50.0f, 0.0f), D3DXVECTOR2(50.0f, 50.0f), CPlayer::MAX_REVENGE);

	// 反撃オブザーバーの生成
	auto pRevengeOb = new CRevengeUIObserver(pRevenge);

	// オブザーバーの設定
	m_pPlayer->SetRevengeObserver(pRevengeOb);

	// ゲージのフレームの生成
	auto gageFrame = CGageFrame::Create(D3DXVECTOR3(256.0f, 36.0f, 0.0f), D3DXVECTOR2(161.0f, 25.0f),CGageFrame::TYPE_HP_PLAYER);

	// 生成できていたら
	if (gageFrame != nullptr)
	{
		// テクスチャのIDの設定
		gageFrame->SetTextureID("data/TEXTURE/GageFrame/playerHpFrame.png");
	}

	// スタミナゲージの生成
	auto pStamina = CStaminaGage::Create(D3DXVECTOR3(110.0f, 81.0f, 0.0f), D3DXVECTOR2(298.0f, 8.0f), D3DXCOLOR(1.0f, 1.0f, 0.3f, 1.0f), CPlayer::MAX_STAMINA);

	if (pStamina != nullptr)
	{
		// スタミナオブザーバーの生成
		CStaminaObserver* pStaminaOb = new CStaminaObserver(pStamina);

		// オブザーバーの設定
		m_pPlayer->SetStaminaObserver(pStaminaOb);

	}

	// ゲージのフレームの生成
	gageFrame = CGageFrame::Create(D3DXVECTOR3(258.0f, 80.0f, 0.0f), D3DXVECTOR2(160.0f, 20.0f), CGageFrame::TYPE_STAMINA);

	// 生成できていたら
	if (gageFrame != nullptr)
	{
		// テクスチャのIDの設定
		gageFrame->SetTextureID("data/TEXTURE/GageFrame/staminaFrame.png");
	}

	// HPゲージの生成
	auto pGage = CHpGage::Create(D3DXVECTOR3(108.0f, 36.0f, 0.0f), D3DXVECTOR2(302.0f, 14.0f), D3DXCOLOR(0.0f,1.0f,0.0f,1.0f),D3DXCOLOR(1.0f,0.0f,0.0f,1.0f), CPlayer::MAX_LIFE,true);

	// Hpゲージのオブザーバーの設定
	CHpObserver *observer = new CHpObserver(pGage);

	// オブザーバーの設定
	m_pPlayer->SetHpObserver(observer);

	// アリーナの生成
	CObjectX::Create(VEC3_NULL, "data/MODEL/field/arena.x",VEC3_NULL);

	// 敵の生成
	auto pEnemy = CEnemy::Create(D3DXVECTOR3(0.0f, 0.0f, 1500.0f));

	// ゲージのフレームの生成
	gageFrame = CGageFrame::Create(D3DXVECTOR3(1000.0f, 36.0f, 0.0f), D3DXVECTOR2(200.0f, 25.0f), CGageFrame::TYPE_HP_ENEMY);

	// 生成できていたら
	if (gageFrame != nullptr)
	{
		// テクスチャのIDの設定
		gageFrame->SetTextureID("data/TEXTURE/GageFrame/enemyHpgageFrame.png");
	}

	// HPゲージの生成
	pGage = CHpGage::Create(D3DXVECTOR3(1115.0f, 36.0f, 0.0f), D3DXVECTOR2(308.0f, 17.0f), D3DXCOLOR(1.0f, 0.2f, 0.0f, 1.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), CEnemy::MAX_LIFE, false);

	// Hpゲージのオブザーバーの設定
	observer = new CHpObserver(pGage);

	// オブザーバーの設定
	pEnemy->SetObserver(observer);

	// 面の設定
	int face = CCollisionAABB::FACE_LEFT;

	// スパイクトラップ
	CSpikeTrap::Create(D3DXVECTOR3(1540.0f, 0.0f, 0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f) ,face);

	// 面の設定
	face = CCollisionAABB::FACE_RIGHT;

	// スパイクトラップ
	CSpikeTrap::Create(D3DXVECTOR3(-1540.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f,D3DX_PI, 0.0f), face);

	//// モデルの読み込み
	//CBlock::Create(D3DXVECTOR3(520.0f, 1510.0f, 2976.0f), "dust003.x");

	// ポーズマネージャーの生成
	CPauseManager::Create();

	// ポーズマネージャーの取得
	m_pPauseManager = CPauseManager::GetInstance();

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CGame::Uninit(void)
{
	m_pMeshField = nullptr;
	m_pCylinder = nullptr;
	m_pPlayer = nullptr;

	// カメラの破棄
	if (m_pCamera != nullptr)
	{
		m_pCamera->Uninit();
		delete m_pCamera;
		m_pCamera = nullptr;
	}

	// ポーズマネージャーの破棄
	if (m_pPauseManager != nullptr)
	{
		m_pPauseManager->Uninit();
		m_pPauseManager = nullptr;
	}
}

//===================================================
// 更新処理
//===================================================
void CGame::Update(void)
{
	// ゲームマネージャーの取得
	auto pGameManager = CGameManager::GetInstance();

	if (pGameManager != nullptr)
	{
		// 更新処理
		pGameManager->Update();
	}

	// カメラの更新
	if (m_pCamera != nullptr)
	{
		m_pCamera->Update();
	}

	// ポーズの更新
	if (m_pPauseManager != nullptr)
	{
		// ポーズの切り替え処理
		m_pPauseManager->EnablePause();

		// メニューの選択処理
		m_pPauseManager->SelectMenu();
	}

	// フェードの取得
	CFade* pFade = CManager::GetFade();

	switch (m_state)
	{
	case STATE_NORMAL:
		break;
	case STATE_END:
		m_nCounterState++;

		if (m_nCounterState >= 60 && pFade != nullptr)
		{
			switch (m_ResultType)
			{
			case RESULTTYPE_WIN: pFade->SetFade(make_unique<CResultWin>()); break;

			case RESULTTYPE_LOSE: pFade->SetFade(make_unique<CResultLose>()); break;

			default: break;
			}

			m_state = STATE_NORMAL;
		}
		break;
	default:
		break;
	}

#ifdef _DEBUG
	// キーボードの取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	if (pKeyboard->GetTrigger(DIK_F9))
	{
		SetState(STATE_END);
	}
#endif
}

//===================================================
// 描画処理
//===================================================
void CGame::Draw(void)
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

	CDebugProc::Print("デバッグ 非表示      : [ F2 ]\n");

	CDebugProc::Print("影の表示 : [ F3 ]\n");

	for (int nCntPriority = 0; nCntPriority < NUM_PRIORITY; nCntPriority++)
	{
		int nNumAll = CObject::GetNumObject(nCntPriority);

		// 文字の表示
		CDebugProc::Print("[ Priority = %d : Numobj = %d]\n", nCntPriority, nNumAll);
	}

	CDebugProc::Print("ワイヤーフレーム : [ F6 ]\n");

#endif // _DEBUG

	// カメラの設定
	if (m_pCamera != nullptr)
	{
		// カメラの設定
		m_pCamera->SetCamera();
	}
}

//===================================================
// コンストラクタ
//===================================================
CGameManager::CGameManager()
{
	m_nCounter = NULL;
}

//===================================================
// デストラクタ
//===================================================
CGameManager::~CGameManager()
{
	m_nGameTime = 0;
}

//===================================================
// 生成処理
//===================================================
void CGameManager::Create(void)
{
	// 自分が生成されていなかったら
	if (m_pInstance == nullptr)
	{
		// 自分の生成
		m_pInstance.reset(new CGameManager);
		m_pInstance->Init();
	}
}

//===================================================
// 初期化処理
//===================================================
void CGameManager::Init(void)
{
	m_nGameTime = 0;
}

//===================================================
// 終了処理
//===================================================
void CGameManager::Uninit(void)
{
	if (m_pInstance != nullptr)
	{
		// 破棄
		m_pInstance.reset();
	}
}

//===================================================
// 更新処理
//===================================================
void CGameManager::Update(void)
{
	// ゲームが終了状態じゃないなら
	if (CGame::GetState() != CGame::STATE_END)
	{
		// カウンターを加算
		m_nCounter++;

		// 一秒経過したら
		if (m_nCounter >= FRAME)
		{
			// カウンターをリセット
			m_nCounter = 0;

			// タイマーを加算
			m_nGameTime++;
		}
	}
}
