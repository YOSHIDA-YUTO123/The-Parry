//===================================================
//
// 勝利時のリザルトのマネージャー [WinResultManager.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "WinResultManager.h"
#include "FadeInObject2D.h"
#include "ResultScore.h"
#include "LoadManager.h"
#include "timer.h"
#include "game.h"

using namespace std; // 名前空間stdの使用

//***************************************************
// 定数宣言
//***************************************************
const int CREATE_TIME = 60; // メニューを生成する時間
const int NUM_SCORE = 3;	 // スコアの数

//***************************************************
// 静的メンバ変数宣言
//***************************************************
CWinResultManager* CWinResultManager::m_pInstance = nullptr; // 自分のインスタンス

//===================================================
// コンストラクタ
//===================================================
CWinResultManager::CWinResultManager()
{
	m_menu = MENU_PARRY_PARFECT;
	m_nCounter = NULL;
	m_nPerfectCnt = NULL;
	m_nNormalCnt = NULL;
	m_nWeakCnt = NULL;
	m_nScore = NULL;
}

//===================================================
// デストラクタ
//===================================================
CWinResultManager::~CWinResultManager()
{
	m_pInstance = nullptr;
}

//===================================================
// 生成処理
//===================================================
void CWinResultManager::Create(void)
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new CWinResultManager;
		m_pInstance->Init();
	}
	else
	{
		return;
	}

	// ファイルを開く
	fstream file("data/TXT/playerInfo.txt");
	string line, input;

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

			if (line.find("PARFECT") != string::npos)
			{
				// 数値を読み込む準備
				istringstream value_Input = pLoad->SetInputvalue(input);

				value_Input >> m_pInstance->m_nPerfectCnt;
			}
			if (line.find("NORMAL") != string::npos)
			{
				// 数値を読み込む準備
				istringstream value_Input = pLoad->SetInputvalue(input);

				value_Input >> m_pInstance->m_nNormalCnt;
			}
			if (line.find("WEAK") != string::npos)
			{
				// 数値を読み込む準備
				istringstream value_Input = pLoad->SetInputvalue(input);

				value_Input >> m_pInstance->m_nWeakCnt;
			}
		}

		// 破棄
		pLoad.reset();
		file.close();
		file.clear();
	}

	// ゲームマネージャーの取得
	auto pGameManager = CGameManager::GetInstance();

	// ゲームの時間の取得
	const int nTime = pGameManager->GetTime();

	// スコアの計算
	int nPerfectScore = 100000 * m_pInstance->m_nPerfectCnt;
	int nNormalScore = 10000 * m_pInstance->m_nNormalCnt;
	int nWeakScore = 1000 * m_pInstance->m_nWeakCnt;
	int nTimeScore = 1234 * nTime;

	m_pInstance->m_nScore = nPerfectScore + nNormalScore + nWeakScore + nTimeScore;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CWinResultManager::Init(void)
{
	m_nCounter = CREATE_TIME;

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CWinResultManager::Uninit(void)
{
	// 終了処理
	CObject::Release();
}

//===================================================
// 更新処理
//===================================================
void CWinResultManager::Update(void)
{
	// テクスチャのパス
	static const char* pTextureName[MENU_MAX] =
	{
		"data/TEXTURE/result/result_list_parfect.png",
		"data/TEXTURE/result/result_list_normal.png",
		"data/TEXTURE/result/result_list_safe.png",
		"data/TEXTURE/result/result_list_cleartime.png",
		"data/TEXTURE/result/result_score.png",
	};

	// スコアの値
	static const int nScore[NUM_SCORE] =
	{
		m_nPerfectCnt,
		m_nNormalCnt,
		m_nWeakCnt,
	};

	int nCnt = m_menu;

	// 最大になったら
	if (nCnt >= MENU_MAX)
	{
		// 処理しない
		return;
	}
	else
	{
		// カウンターをインクリメント
		m_nCounter++;
	}

	// 生成時間になったら
	if (m_nCounter >= CREATE_TIME)
	{
		m_nCounter = NULL;

		// フェード時間の設定
		const int nFadeTime = (nCnt == MENU_CLEAR_TIME) ? 0 : 60;

		 // 生成処理
		 CFadeInObject2D::Create(
			D3DXVECTOR3(450.0f, 150.0f + nCnt * 100.0f, 0.0f),
			D3DXVECTOR2(150.0f,50.0f),
			 nFadeTime)
			 ->SetTextureID(pTextureName[nCnt]);

		 if (nCnt < NUM_SCORE)
		 {
			 // リザルトのスコアの生成
			 CResultScore::Create(D3DXVECTOR3(650.0f, 150.0f + nCnt * 100.0f, 0.0f), D3DXVECTOR2(200.0f, 45.0f), nScore[nCnt]);
		 }
		 else if(nCnt == MENU_CLEAR_TIME)
		 {
			 // ゲームマネージャーの取得
			 auto pGameManager = CGameManager::GetInstance();

			 // ゲームの時間の取得
			 const int nTime = pGameManager->GetTime();

			 // タイマーの生成
			 CTime::Create(D3DXVECTOR3(750.0f, 150.0f + nCnt * 100.0f, 0.0f), 80.0f, 45.0f, nTime, true);
		 }
		 else
		 {
			 // リザルトのスコアの生成
			 CResultScore::Create(D3DXVECTOR3(950.0f, 150.0f + nCnt * 100.0f, 0.0f), D3DXVECTOR2(250.0f, 50.0f), m_nScore);
		 }
		 // 次の種類へ
		 nCnt++;

		// 種類を設定
		m_menu = static_cast<MENU>(nCnt);
	}
}

//===================================================
// 描画処理
//===================================================
void CWinResultManager::Draw(void)
{
}
