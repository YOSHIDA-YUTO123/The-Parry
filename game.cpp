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
#include "cylinder.h"
#include "player.h"
#include "objectX.h"
#include "enemy.h"
#include "Obstacle.h"
#include "manager.h"
#include "input.h"
#include "result.h"
#include "fade.h"
#include "debugproc.h"
#include "Gage.h"
#include "Observer.h"
#include "GageFrame.h"
#include "pause.h"
#include "GameCamera.h"
#include "light.h"
#include "RevengeGage.h"
#include "block.h"
#include "BlockManager.h"
#include "obstaclemanager.h"
#include "LoadManager.h"
#include "Object3DAnim.h"
#include "ParryEffect.h"
#include "bird.h"
#include "BirdManager.h"
#include "gear.h"
#include "sound.h"
#include "enemy.h"
#include "Object2DAnimMT.h"
#include "RevengeActiveUI.h"
#include "billboardAnim.h"
#include "wing.h"
#include "ArenaDust.h"
#include "renderer.h"

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
CEnemy* CGame::m_pEnemy = nullptr;							   // 敵クラスへのポインタ

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

	// マネージャーの生成
	CObstacleManager::Create();

	// ゲームマネージャーの生成
	CGameManager::Create();

	// 鳥のマネージャの取得
	CBirdManager::Create();

	// ゲームのロード
	Load();

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

	// アリーナの生成
	CObjectX::Create(VEC3_NULL, "data/MODEL/field/arena.x",VEC3_NULL);

	// ポーズマネージャーの生成
	CPauseManager::Create();

	// ポーズマネージャーの取得
	m_pPauseManager = CPauseManager::GetInstance();

	// 歯車の生成
	CGear::Create(D3DXVECTOR3(-2320.0f,1700.0f,100.0f));

	// 歯車の生成
	CGear::Create(D3DXVECTOR3(2100.0f, 1700.0f, 100.0f));

	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pSound != nullptr)
	{
		// 音の再生
		pSound->Play(CSound::SOUND_LABEL_GAME_BGM, 0.5f);
	}

	// 反撃ゲージのUIの生成
	CRevengeActiveUI::Create(D3DXVECTOR3(55.0f,90.0f,0.0f), D3DXVECTOR2(48.0f, 18.0f), 4, 4);

	// 瓦礫の生成
	CArenaDust::Create(D3DXVECTOR3(970.0f,0.0f,561.0f), 0.0f);

	// 瓦礫の生成
	CArenaDust::Create(D3DXVECTOR3(810.0f, 0.0f, -922.0f), 0.5f);

	// 瓦礫の生成
	CArenaDust::Create(D3DXVECTOR3(-882.0f, 0.0f, -762.0f), 0.0f);

	// 瓦礫の生成
	CArenaDust::Create(D3DXVECTOR3(-1061.0f, 0.0f, 762.0f), 0.0f);

	//// パリィエフェクトの生成
	//CParryEffect::Create(D3DXVECTOR3(0.0f,200.0f,0.0f), D3DXVECTOR3(150.0f, 150.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 5, 2, 4, true);

	//CParryEffect::Create(D3DXVECTOR3(0.0f, 300.0f, 0.0f), D3DXVECTOR3(150.0f, 150.0f, 0.0f), VEC3_NULL, 5, 3, 3, true);

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CGame::Uninit(void)
{
	// 状態のリセット
	m_state = STATE_NORMAL;

	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pSound != nullptr)
	{
		// 音の停止
		pSound->StopSound();
	}

	m_pMeshField = nullptr;
	m_pCylinder = nullptr;
	m_pPlayer = nullptr;
	m_pEnemy = nullptr;

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

	// 障害物マネージャーのインスタンスの取得
	CObstacleManager* pObstacleManager = CObstacleManager::GetInstance();

	if (pObstacleManager != nullptr)
	{
		// 更新処理
		pObstacleManager->Update();
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

	if (pKeyboard->GetTrigger(DIK_0))
	{
		CWing::Create(D3DXVECTOR3(0.0f, 500.0f, 0.0f));
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
// ロード処理
//===================================================
void CGame::Load(void)
{
	// ファイルを開く
	fstream file("data/TXT/game.txt");
	string line, input;

	string filepath;	// ファイルパス

	// nullじゃなかったら
	if (file.is_open())
	{
		// ロードマネージャーの生成
		unique_ptr<CLoadManager> pLoad = make_unique<CLoadManager>();

		// ファイルを一行ずつ読み取る
		while (getline(file, line))
		{
			size_t equal_pos = line.find("="); // =の位置

			// [=] から先を求める
			input = line.substr(equal_pos + 1);

			if (line.find("OBSTACLESET") != string::npos)
			{
				// 障害物の読み込み処理
				LoadObstacle(file, line,pLoad.get());
			}
			if (line.find("CHARACTERSET") != string::npos)
			{
				// キャラクター読み込み処理
				LoadCharacter(file, line, pLoad.get());
			}
		}

		// 破棄
		pLoad.reset();
		file.close();
		file.clear();
	}
}

//===================================================
// 障害物のロード処理
//===================================================
void CGame::LoadObstacle(std::fstream& file, std::string line,CLoadManager *pLoad)
{
	D3DXVECTOR3 pos = VEC3_NULL; // 位置
	float fAngle = 0.0f;	// 角度
	int nType = -1;			// 種類
	int nReverse = 0;		// 反転するかどうか
	int nDamageFase = -999;	// ダメージを受ける面

	string input;

	while (1)
	{
		// ファイルを読み取る
		getline(file, line);

		size_t equal_pos = line.find("="); // =の位置

		// [=] から先を求める
		input = line.substr(equal_pos + 1);

		if (line.find("TYPE") != string::npos)
		{
			// 数値を読み込む準備
			istringstream value_Input = pLoad->SetInputvalue(input);

			value_Input >> nType;
		}
		if (line.find("POS") != string::npos)
		{
			// 数値を読み込む準備
			istringstream value_Input = pLoad->SetInputvalue(input);

			value_Input >> pos.x >> pos.y >> pos.z;
		}
		if (line.find("REVERSE") != string::npos)
		{
			// 数値を読み込む準備
			istringstream value_Input = pLoad->SetInputvalue(input);

			value_Input >> nReverse;

			// 1だったら反転
			fAngle = (nReverse == 0) ? 0.0f : D3DX_PI;
		}
		if (line.find("DAMAGE_FACE") != string::npos)
		{
			// 数値を読み込む準備
			istringstream value_Input = pLoad->SetInputvalue(input);

			value_Input >> nDamageFase;
		}
		if (line.find("END_OBSTACLESET") != string::npos)
		{
			// 種類の遷移
			switch (nType)
			{
			case CObstacle::TYPE_SPIKE_TRAP:
				// スパイクトラップ
				CSpikeTrap::Create(pos, D3DXVECTOR3(0.0f, fAngle, 0.0f), nDamageFase);
				break;
			case CObstacle::TYPE_TNT_BARREL:
				break;
			default:
				break;
			}
			break;
		}
	}
}

//===================================================
//キャラクターのロード処理
//===================================================
void CGame::LoadCharacter(std::fstream& file, std::string line, CLoadManager* pLoad)
{
	D3DXVECTOR3 pos = VEC3_NULL; // 位置
	D3DXVECTOR3 ShadowScal = { 1.0f,1.0f,1.0f };
	D3DXVECTOR3 Size = VEC3_NULL;
	float fAngle = 0.0f;		 // 向き
	float fSpeed = 0.0f;		 // 速さ
	int nType = -1;				 // 種類
	int nLife = -1;				 // 体力

	string input;

	while (1)
	{
		// ファイルを読み取る
		getline(file, line);

		size_t equal_pos = line.find("="); // =の位置

		// [=] から先を求める
		input = line.substr(equal_pos + 1);

		if (line.find("TYPE") != string::npos)
		{
			// 数値を読み込む準備
			istringstream value_Input = pLoad->SetInputvalue(input);

			value_Input >> nType;
		}
		if (line.find("POS") != string::npos)
		{
			// 数値を読み込む準備
			istringstream value_Input = pLoad->SetInputvalue(input);

			value_Input >> pos.x >> pos.y >> pos.z;
		}
		if (line.find("ANGLE") != string::npos)
		{
			// 数値を読み込む準備
			istringstream value_Input = pLoad->SetInputvalue(input);

			value_Input >> fAngle;
		}
		if (line.find("LIFE") != string::npos)
		{
			// 数値を読み込む準備
			istringstream value_Input = pLoad->SetInputvalue(input);

			value_Input >> nLife;
		}
		if (line.find("SPEED") != string::npos)
		{
			// 数値を読み込む準備
			istringstream value_Input = pLoad->SetInputvalue(input);

			value_Input >> fSpeed;
		}
		if (line.find("SHADOW") != string::npos)
		{
			// 数値を読み込む準備
			istringstream value_Input = pLoad->SetInputvalue(input);

			value_Input >> ShadowScal.x >> ShadowScal.y >> ShadowScal.z;
		}
		if (line.find("SIZE") != string::npos)
		{
			// 数値を読み込む準備
			istringstream value_Input = pLoad->SetInputvalue(input);

			value_Input >> Size.x >> Size.y >> Size.z;
		}

		if (line.find("END_CHARACTERSET") != string::npos)
		{
			switch (nType)
			{
			case CCharacter3D::TYPE_PLAYER:
				PlayerConfig(nLife,fSpeed,ShadowScal,Size,pos,fAngle);
				break;
			case CCharacter3D::TYPE_ENEMY:
				EnemyConfig(nLife, fSpeed, ShadowScal, Size, pos, fAngle);
				break;
			case CCharacter3D::TYPE_BIRD:
				CBird::Create(pos, CBird::TYPE_NORMAL)->SetCharacter(nLife, fSpeed, ShadowScal, Size);
				break;
			default:
				break;
			}
			break;
		}
	}
}

//===================================================
// プレイヤーの構成
//===================================================
void CGame::PlayerConfig(const int nLife, const float fSpeed, const D3DXVECTOR3 ShadowScal, const D3DXVECTOR3 Size, const D3DXVECTOR3 pos, const float fAngle)
{
	// プレイヤーの生成
	m_pPlayer = CPlayer::Create(nLife, fSpeed, ShadowScal, Size, pos, D3DXVECTOR3(0.0f, fAngle, 0.0f));

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
	auto pGage = CHpGage::Create(D3DXVECTOR3(108.0f, 36.0f, 0.0f), D3DXVECTOR2(302.0f, 14.0f), D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), nLife, true);

	// Hpゲージのオブザーバーの設定
	CHpObserver* observer = new CHpObserver(pGage);

	// オブザーバーの設定
	m_pPlayer->SetHpObserver(observer);
}

//===================================================
// 敵の構成
//===================================================
void CGame::EnemyConfig(const int nLife, const float fSpeed, const D3DXVECTOR3 ShadowScal, const D3DXVECTOR3 Size, const D3DXVECTOR3 pos, const float fAngle)
{
	// 敵の生成
	m_pEnemy = CEnemy::Create(nLife,fSpeed,ShadowScal,Size,pos,D3DXVECTOR3(0.0f,fAngle,0.0f));

	// ゲージのフレームの生成
	auto gageFrame = CGageFrame::Create(D3DXVECTOR3(1000.0f, 36.0f, 0.0f), D3DXVECTOR2(200.0f, 25.0f), CGageFrame::TYPE_HP_ENEMY);

	// 生成できていたら
	if (gageFrame != nullptr)
	{
		// テクスチャのIDの設定
		gageFrame->SetTextureID("data/TEXTURE/GageFrame/enemyHpgageFrame.png");
	}

	// HPゲージの生成
	auto pGage = CHpGage::Create(D3DXVECTOR3(1115.0f, 36.0f, 0.0f), D3DXVECTOR2(308.0f, 17.0f), D3DXCOLOR(1.0f, 0.2f, 0.0f, 1.0f), D3DXCOLOR(1.0f, 0.5f, 1.0f, 1.0f), nLife, false);

	// Hpゲージのオブザーバーの設定
	auto observer = new CHpObserver(pGage);

	// オブザーバーの設定
	m_pEnemy->SetObserver(observer);
}

//===================================================
// プレイヤーの情報のセーブ
//===================================================
void CGameManager::SavePlayerInfo(const int nPerfect, const int nNormal, const int nWeak)
{
	// ファイルを開く
	ofstream file("data/TXT/playerInfo.txt");

	// ファイルが開けたら
	if (file.is_open())
	{
		file << "PARFECT = " << nPerfect << "\n";
		file << "NORMAL = " << nNormal << "\n";
		file << "WEAK = " << nWeak;

		// ファイルを閉じる
		file.clear();
		file.close();
	}
	else
	{
		MessageBox(NULL, "ファイルが開けませんでした", "data/TXT/playerInfo.txt", MB_OK);
	}
}

//===================================================
// コンストラクタ
//===================================================
CGameManager::CGameManager()
{
	m_nGameTime = NULL;
	m_nCounter = NULL;
}

//===================================================
// デストラクタ
//===================================================
CGameManager::~CGameManager()
{

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
	// ポーズのマネージャーの取得
	CPauseManager* pPauseManager = CPauseManager::GetInstance();

	bool bPause = false;

	if (pPauseManager != nullptr)
	{
		// ポーズの状態の取得
		bPause = pPauseManager->GetPause();
	}
	// ゲームが終了状態じゃないなら&&ポーズ状態じゃないなら
	if (CGame::GetState() != CGame::STATE_END && !bPause)
	{
		// カウンターを加算
		m_nCounter++;

		// 一秒経過したら
		if (m_nCounter >= FRAME)
		{
			// カウンターをリセット
			m_nCounter = 0;

			// タイマーを加算
 			m_pInstance->m_nGameTime++;
		}
	}
}
