//================================================
//
// ランキングマネージャー [RankingManager.cpp]
// Author: YUTO YOSHIDA
// 
// << ランキングのスコアの読み込み、ソート、生成を行う >>
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "RankingManager.h"
#include "LoadManager.h"
#include <algorithm>
#include "RankingScore.h"

using namespace Const;	// 名前空間Constの使用
using namespace std;	// 名前空間stdの使用

//================================================
// コンストラクタ
//================================================
CRankingManager::CRankingManager()
{
	// 値のクリア
	ZeroMemory(&m_aScore, sizeof(m_aScore));
	m_nResultScore = NULL;
	m_nUpdateScoreIdx = -1;
}

//================================================
// デストラクタ
//================================================
CRankingManager::~CRankingManager()
{
}

//================================================
// 生成処理
//================================================
void CRankingManager::Create(void)
{
	// ランキングマネージャーの生成
	auto pManager = new CRankingManager;
	pManager->Init();
}

//================================================
// ランキングの設定処理
//================================================
void CRankingManager::SetRanking(void)
{
	// 順位数分回す
	for (int nCnt = 0; nCnt < NUM_RANK; nCnt++)
	{
		// インデックスが一致しているか比較
		const bool bHilight = (m_nUpdateScoreIdx == nCnt) ? true : false;

		// テクスチャのパス
		static const char* pTexturePath[NUM_RANK] =
		{
			"data/TEXTURE/UI/rank_001.png",
			"data/TEXTURE/UI/rank_002.png",
			"data/TEXTURE/UI/rank_003.png",
			"data/TEXTURE/UI/rank_004.png",
			"data/TEXTURE/UI/rank_005.png",
		};
		// オブジェクト2Dの生成
		CObject2D::Create(45.0f,45.0f, D3DXVECTOR3(350.0f, 130.0f + nCnt * 100.0f, 0.0f))->SetTextureID(pTexturePath[nCnt]);

		// ランキングのスコアの生成
		CRankingScore::Create(D3DXVECTOR3(950.0f, 130.0f + nCnt * 100.0f, 0.0f), D3DXVECTOR2(45.0f, 45.0f), m_aScore[nCnt], bHilight);
	}
}

//================================================
// 初期化処理
//================================================
HRESULT CRankingManager::Init(void)
{
	// スコアのロード
	Load();

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CRankingManager::Uninit(void)
{
	// 自分自身の破棄
	CObject::Release();
}

//================================================
// 更新処理
//================================================
void CRankingManager::Update(void)
{
}

//================================================
// 描画処理
//================================================
void CRankingManager::Draw(void)
{
}

//================================================
// スコアのロード
//================================================
void CRankingManager::Load(void)
{
	// ファイルの読み込み
	fstream ranking_file("data/TXT/ranking.txt");
	string line;
	string input;

	int nCnt = 0;

	// ファイルが開けたら
	if (ranking_file.is_open())
	{
		// ロードマネージャーの生成
		unique_ptr<CLoadManager> pLoad = make_unique<CLoadManager>();

		// ファイルを一行ずつ読み取る
		while (getline(ranking_file, line))
		{
			// ランキングの順位数を超えたら抜ける
			if (nCnt >= NUM_RANK)
			{
				continue;
			}

			istringstream value_Input(line);

			// 値を代入
			value_Input >> m_aScore[nCnt];

			nCnt++;
		}

		// ファイルをクリア
		pLoad.reset();
		ranking_file.close();
		ranking_file.clear();
	}
	else
	{
		// メッセージボックス
		MessageBox(NULL, "ファイルが開けたら", "data/TXT/ranking.txt", MB_OK);
	}

	// ファイルを開く
	ifstream result_file("data/TXT/result_score.txt");

	// ファイルが開けたら
	if (result_file.is_open())
	{
		// ロードマネージャーの生成
		unique_ptr<CLoadManager> pLoad = make_unique<CLoadManager>();

		// ファイルを一行ずつ読み取る
		while (getline(result_file, line))
		{
			size_t equal_pos = line.find("="); // =の位置

			// [=] から先を求める
			input = line.substr(equal_pos + 1);

			if (line.find("RESULT_SCORE") != string::npos)
			{
				// 数値を読み込む準備
				istringstream value_Input = pLoad->SetInputvalue(input);

				// スコアを読み込む
				value_Input >> m_nResultScore;
				break;
			}
		}

		// ファイルをクリア
		pLoad.reset();
		result_file.close();
		result_file.clear();
	}
	else
	{
		// メッセージボックス
		MessageBox(NULL, "ファイルが開けたら", "data/TXT/ranking.txt", MB_OK);
	}

	// スコアのソート
	Sort();

	// ランキングの設定
	SetRanking();

	// セーブ
	Save();
}

//================================================
// スコアのソート
//================================================
void CRankingManager::Sort(void)
{
	// 最後のスコアと比較
	if (m_aScore[NUM_RANK - 1] < m_nResultScore)
	{
		m_aScore[NUM_RANK - 1] = m_nResultScore;
	}

	// スコアをソートする
	sort(m_aScore, m_aScore + NUM_RANK, greater<int>());

	// 順位数分調べる
	for (int nCnt = 0; nCnt < NUM_RANK; nCnt++)
	{
		// スコアが一致したら
		if (m_aScore[nCnt] == m_nResultScore)
		{
			// インデックスの取得
			m_nUpdateScoreIdx = nCnt;
			break;
		}
	}
}

//================================================
// スコアのセーブ処理
//================================================
void CRankingManager::Save(void)
{
	// ファイルを開く
	ofstream file("data/TXT/ranking.txt");

	// ファイルが開けたら
	if (file.is_open())
	{
		// 順位の数分回す
		for (int nCnt = 0; nCnt < NUM_RANK; nCnt++)
		{
			file << m_aScore[nCnt] << "\n";
		}

		file.close();
		file.clear();
	}
}
