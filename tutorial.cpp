//===================================================
//
// チュートリアル [tutorial.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "tutorial.h"
#include "meshfield.h"
#include"GameCamera.h"
#include"manager.h"
#include"light.h"
#include"player.h"
#include"block.h"
#include"BlockManager.h"
#include "particle.h"
#include "TrainingEnemy.h"
#include "object2D.h"
#include "object3D.h"
#include "Object3DMT.h"
#include "RevengeGage.h"
#include"Observer.h"
#include "GageFrame.h"
#include"Gage.h"
#include"input.h"
#include"fade.h"
#include "game.h"
#include "opening.h"
#include "mark.h"
#include "Obstacle.h"
#include"Collider.h"
#include"Collision.h"
#include "obstaclemanager.h"
#include "GateUI.h"
#include "wall.h"
#include "RevengeActiveUI.h"

using namespace std;	// 名前空間stdの使用
using namespace Const;  // 名前空間Const

//***************************************************
// 定数宣言
//***************************************************
const D3DXVECTOR3 PLAYER_POS = { 0.0f,0.0f,1200.0f }; // プレイヤーの初期位置
const D3DXVECTOR3 GATE_POS = { 0.0f,0.0f,-1200.0f };  // ゲートの位置
constexpr float GATE_RADIUS = 100.0f;				  // ゲートの半径

//***************************************************
// 静的メンバ変数宣言
//***************************************************
CGameCamera* CTutorial::m_pCamera = nullptr;			// カメラクラスへのポインタ
CMeshField* CTutorial::m_pMeshField = nullptr;			// メッシュフィールドへのポインタ
CPlayer* CTutorial::m_pPlayer = nullptr;				// プレイヤーへのポインタ
CTrainingEnemy* CTutorial::m_pTrainingEnemy = nullptr;  // 練習用の敵のクラスへのポインタ

//===================================================
// コンストラクタ
//===================================================
CTutorial::CTutorial() : CScene(MODE_TUTORIAL)
{
	m_pSphere = nullptr;
}

//===================================================
// デストラクタ
//===================================================
CTutorial::~CTutorial()
{
}

//==================================================
// 初期化処理
//===================================================
HRESULT CTutorial::Init(void)
{
	// マネージャーの生成
	CObstacleManager::Create();

	m_pCamera = new CGameCamera;
	m_pCamera->Init();
	m_pCamera->SetCamera(D3DXVECTOR3(0.0f, 250.0f, 350.0f), VEC3_NULL, D3DXVECTOR3(1.59f, -3.12f, 0.0f));

	// ライトの取得
	CLight* pLight = CManager::GetLight();
	pLight->Init();

	// ライトの設定処理
	// ポイントライトの設定処理
	pLight->SetPoint(D3DXVECTOR3(0.0f, 400.0f, -1000.0f), 2500.0f, D3DCOLOR_RGBA(255, 170, 170, 255), D3DCOLOR_RGBA(255, 170, 170, 255));
	pLight->SetPoint(D3DXVECTOR3(500.0f, 400.0f, 0.0f), 2500.0f, D3DCOLOR_RGBA(255, 170, 170, 255), D3DCOLOR_RGBA(255, 170, 170, 255));
	pLight->SetPoint(D3DXVECTOR3(-500.0f, 400.0f, 0.0f), 2500.0f, D3DCOLOR_RGBA(255, 170, 170, 255), D3DCOLOR_RGBA(255, 170, 170, 255));
	pLight->SetPoint(D3DXVECTOR3(0.0f, 200.0f, 515.0f), 300.0f, D3DCOLOR_RGBA(255, 255, 255, 255), D3DCOLOR_RGBA(255, 255, 255, 255));

	// フィールドの生成
	m_pMeshField = CMeshField::Create(VEC3_NULL, 32, 32, D3DXVECTOR2(3000.0f, 3000.0f));

	// ブロックマネージャーの取得
	auto pBlockManager = CBlockManager::GetInstance();

	// 取得できたら
	if (pBlockManager != nullptr)
	{
		// ロード
		pBlockManager->Load();
	}

	// プレイヤーの生成
	m_pPlayer = CPlayer::Create(999,6.0f,D3DXVECTOR3(4.0f,1.0f,4.0f),D3DXVECTOR3(50.0f,200.0f,50.0f),PLAYER_POS);

	// 反撃UIの生成
	auto pRevenge = CRevengeUI::Create(D3DXVECTOR3(50.0f, 50.0f, 0.0f), D3DXVECTOR2(50.0f, 50.0f), CPlayer::MAX_REVENGE);

	// 反撃オブザーバーの生成
	auto pRevengeOb = new CRevengeUIObserver(pRevenge);

	// オブザーバーの設定
	m_pPlayer->SetRevengeObserver(pRevengeOb);

	// ゲージのフレームの生成
	auto gageFrame = CGageFrame::Create(D3DXVECTOR3(256.0f, 36.0f, 0.0f), D3DXVECTOR2(161.0f, 25.0f), CGageFrame::TYPE_HP_PLAYER);

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
	auto pGage = CHpGage::Create(D3DXVECTOR3(108.0f, 36.0f, 0.0f), D3DXVECTOR2(302.0f, 14.0f), D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), 999, true);

	// Hpゲージのオブザーバーの設定
	CHpObserver* observer = new CHpObserver(pGage);

	// オブザーバーの設定
	m_pPlayer->SetHpObserver(observer);

	// 敵の生成
	m_pTrainingEnemy = CTrainingEnemy::Create(D3DXVECTOR3(0.0f,0.0f,-500.0f));

	// 生成処理
	CObject2D::Create(180.0f, 180.0f, D3DXVECTOR3(1080.0f, 200.0f, 0.0f))->SetTextureID("data/TEXTURE/tutorial/tuto.png");

	// 生成処理
	CObject3DMT::Create(D3DXVECTOR3(0.0f,100.0f,530.0f), D3DXVECTOR3(0.0f, D3DX_PI, 0.0f), D3DXVECTOR3(15.0f, 65.0f, 0.0f))->SetTextureID("UI/monument.png","arena/wall.jpg");

	// 生成処理
	CObject3DMT::Create(D3DXVECTOR3(413.0f, 100.0f, -263.0f), D3DXVECTOR3(0.0f, D3DX_PI * 0.5f, 0.0f), D3DXVECTOR3(55.0f, 45.0f, 0.0f))->SetTextureID("UI/monument001.png", "");

	// 目印
	CMark::Create(D3DXVECTOR3(0.0f, 250.0f, 500.0f));

	// 目印
	CMark::Create(D3DXVECTOR3(471.0f, 250.0f, -267.0f));

	// 目印
	CMark::Create(D3DXVECTOR3(0.0f, 200.0f, -1100.0f));

	// ゲートのUIの生成
	CGateUI::Create(D3DXVECTOR3(640.0f, 500.0f, 0.0f), D3DXVECTOR2(350.0f,50.0f), D3DXVECTOR3(0.0f, 0.0f, -1100.0f));

	// 面の設定
	int face = CCollisionAABB::FACE_RIGHT;

	// スパイクトラップ
	CSpikeTrap::Create(D3DXVECTOR3(-1200.0f, 0.0f, -800.0f), D3DXVECTOR3(0.0f, D3DX_PI, 0.0f), face);

	// ゲート
	m_pSphere = CColliderSphere::Create(GATE_POS, GATE_RADIUS);

	// 反撃ゲージのUIの生成
	CRevengeActiveUI::Create(D3DXVECTOR3(55.0f, 90.0f, 0.0f), D3DXVECTOR2(48.0f, 18.0f), 4, 4);

	return S_OK;
}

//==================================================
// 終了処理
//===================================================
void CTutorial::Uninit(void)
{
	m_pTrainingEnemy = nullptr;
	m_pMeshField = nullptr;
	m_pPlayer = nullptr;

	// 障害物マネージャーのインスタンスの取得
	CObstacleManager* pObstacleManager = CObstacleManager::GetInstance();

	// マネージャーの終了処理
	if (pObstacleManager != nullptr)
	{
		pObstacleManager->Uninit();
		pObstacleManager = nullptr;
	}

	// カメラの破棄
	if (m_pCamera != nullptr)
	{
		m_pCamera->Uninit();
		delete m_pCamera;
		m_pCamera = nullptr;
	}
}

//==================================================
// 更新処理
//===================================================
void CTutorial::Update(void)
{
	auto pBlockManager = CBlockManager::GetInstance();

	int nNumBlock = pBlockManager->GetNumAll();

	static float fTime = 0.0f;
	fTime += 15.0f;

	for (int nCnt = 0; nCnt < nNumBlock; nCnt++)
	{
		// ブロックの取得
		CBlock* pBlock = pBlockManager->GetBlock(nCnt);

		D3DXVECTOR3 diff = pBlock->GetPosition() - PLAYER_POS;

		// 距離
		float fDistance = fabsf(D3DXVec3Length(&diff));

		if (fDistance <= fTime)
		{
			pBlock->SetTextureMT("");
		}
	}

	// パーティクルの生成
	auto pParticle = CParticle3DNormal::Create(D3DXVECTOR3(-225.0f, 325.0f, -1115.0f), 15.0f, D3DCOLOR_RGBA(240, 122, 27, 255));
	pParticle->SetParticle(2.0f, 100, 10, 1, 40);
	pParticle->SetParam(CEffect3D::TYPE_FIRE,0);

	// パーティクルの生成
	pParticle = CParticle3DNormal::Create(D3DXVECTOR3(255.0f, 325.0f, -1115.0f), 15.0f, D3DCOLOR_RGBA(240, 122, 27, 255));
	pParticle->SetParticle(2.0f, 100, 10, 1, 40);
	pParticle->SetParam(CEffect3D::TYPE_FIRE,0);

	// キーボードの取得
	auto pKeyboard = CManager::GetInputKeyboard();
	auto pJoypad = CManager::GetInputJoypad();

	// カメラの更新
	if (m_pCamera != nullptr)
	{
		m_pCamera->Update();
	}

	// ゲートの範囲内に入ったら
	if (CCollisionSphere::Collision(m_pPlayer->GetSphereCollider(), m_pSphere.get()))
	{
		if (pKeyboard->GetTrigger(DIK_RETURN) || pJoypad->GetTrigger(pJoypad->JOYKEY_START))
		{
			// フェードの取得
			CFade* pFade = CManager::GetFade();

			if (pFade != nullptr)
			{
				// ゲームシーンへ遷移
				pFade->SetFade(make_unique<COpening>());
			}
		}
	}
}

//==================================================
// 描画処理
//===================================================
void CTutorial::Draw(void)
{
	// カメラの設定
	if (m_pCamera != nullptr)
	{
		m_pCamera->SetCamera();
	}
}
