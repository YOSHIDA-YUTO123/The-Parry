//================================================
//
// ブロックマネージャー [BlockManager.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "BlockManager.h"
#include"block.h"
#include"LoadManager.h"

using namespace std; // 名前空間stdを使用

//************************************************
// 静的メンバ変数宣言
//************************************************
unique_ptr<CBlockManager> CBlockManager::m_pInstance = nullptr; // 自分のインスタンス

//================================================
// コンストラクタ
//================================================
CBlockManager::CBlockManager()
{
}

//================================================
// ブロックのロード処理
//================================================
HRESULT CBlockManager::Load(void)
{
	// ファイルを開く
	fstream file("data/MODEL/tutorialStage.txt");
	string line, input;

	D3DXVECTOR3 pos; // 位置
	string filepath; // ファイルパス
	int nReverse;    // 反転するかどうか
	float fAngle = 0.0f; // 向き

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

			if (line.find("POS") != string::npos)
			{
				// = から先を求める
				input = line.substr(equal_pos + 1);

				// 数値を読み込む準備
				istringstream value_Input = pLoad->SetInputvalue(input);

				// 数値を代入する
				value_Input >> pos.x;
				value_Input >> pos.y;
				value_Input >> pos.z;
			}
			if (line.find("FILE_NAME") != string::npos)
			{
				// 数値を読み込む準備
				istringstream value_Input = pLoad->SetInputvalue(input);

				// 数値を代入する
				value_Input >> filepath;
			}
			if (line.find("REVERSE") != string::npos)
			{
				// 数値を読み込む準備
				istringstream value_Input = pLoad->SetInputvalue(input);

				// 数値を代入する
				value_Input >> nReverse;

				if (nReverse == 0)
				{
					fAngle = 0.0f;
				}
				else
				{
					// 反転
					fAngle = D3DX_PI;
				}
			}
			if (line.find("END_BLOCKSET") != string::npos)
			{
				// ブロックの生成
				CBlock *pBlock = CBlock::Create(pos, filepath.c_str(),D3DXVECTOR3(0.0f, fAngle,0.0f));
				m_apBlockList.push_back(pBlock);
			}
		}

		pLoad.reset();
		file.close();
		file.clear();
	}
	else
	{
		MessageBox(NULL, "ファイルが開けませんでした", "blockManager", MB_OK);
		return E_FAIL;
	}
	return S_OK;
}

//================================================
// デストラクタ
//================================================
CBlockManager::~CBlockManager()
{
	// 要素のクリア
	m_apBlockList.clear();
}

//================================================
// 生成処理
//================================================
void CBlockManager::Create(void)
{
	// 自分がnullだったら
	if (m_pInstance == nullptr)
	{
		// 自分の生成
		m_pInstance.reset(new CBlockManager);
	}
}

//================================================
// ブロックの設定処理
//================================================
void CBlockManager::SetBlock(CBlock* pBlock)
{
	// ブロックをリストに追加する
	m_apBlockList.push_back(pBlock);
}

//================================================
// 当たり判定
//================================================
bool CBlockManager::Collision(CColliderAABB* pAABB,D3DXVECTOR3* pPushPos)
{
	bool bHit = false;

	// 要素をすべて調べる
	for (auto itr = m_apBlockList.begin(); itr != m_apBlockList.end(); ++itr)
	{
		// 当たり判定
		if ((*itr)->Collision(pAABB, pPushPos))
		{
			bHit = true;
		}
	}
	return bHit;
}
