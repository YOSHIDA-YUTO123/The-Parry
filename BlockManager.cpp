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
#include "BlockMonument.h"

using namespace std;	// 名前空間stdを使用
using namespace Const;	// 名前空間Constを使用

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
	fstream file("data/TXT/tutorialStage.txt");
	string line, input;

	D3DXVECTOR3 pos;	// 位置
	string filepath;	// ファイルパス
	int nReverse;		// 反転するかどうか
	int type = 0;		// 種類			
	D3DXVECTOR3 Angle = VEC3_NULL;	// 角度

	// ファイルが開けなかったら
	if (!file.is_open())
	{
		MessageBox(NULL, "ファイルが開けませんでした", "blockManager", MB_OK);
		return E_FAIL;
	}

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
		if (line.find("TYPE") != string::npos)
		{
			// = から先を求める
			input = line.substr(equal_pos + 1);

			// 数値を読み込む準備
			istringstream value_Input = pLoad->SetInputvalue(input);

			value_Input >> type;
		}
		if (line.find("REVERSE") != string::npos)
		{
			Angle = VEC3_NULL;

			// 数値を読み込む準備
			istringstream value_Input = pLoad->SetInputvalue(input);

			// 数値を代入する
			value_Input >> nReverse;

			if (nReverse == 0)
			{
				Angle.y = 0.0f;
			}
			else
			{
				// 反転
				Angle.y = D3DX_PI;
			}
		}
		if (line.find("ROT") != string::npos)
		{
			Angle = VEC3_NULL;

			// 数値を読み込む準備
			istringstream value_Input = pLoad->SetInputvalue(input);

			// 数値を代入する
			value_Input >> Angle.x;
			value_Input >> Angle.y;
			value_Input >> Angle.z;
		}
		if (line.find("END_BLOCKSET") != string::npos)
		{ 
			CBlock* pBlock = nullptr;
			// 種類の遷移
			switch (type)
			{
			case CBlock::TYPE_MONUMENT_000:
				// ブロックの生成
				pBlock = CBlockMonument::Create(pos, static_cast<CBlock::TYPE>(type), Angle);
				break;
			case CBlock::TYPE_MONUMENT_001:
				// ブロックの生成
				pBlock = CBlockMonument::Create(pos, static_cast<CBlock::TYPE>(type), Angle);
				break;
			default:
				// ブロックの生成
				pBlock = CBlock::Create(pos, static_cast<CBlock::TYPE>(type), Angle);
				break;
			}

			pBlock->SetTextureMT("data/TEXTURE/effect/grid000.png");
			m_apBlockList.push_back(pBlock);
		}
	}

	pLoad.reset();
	file.close();
	file.clear();

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
// ブロックの取得
//================================================
CBlock* CBlockManager::GetBlock(const int nIdx)
{
	// ブロックの総数
	int nNumBlock = static_cast<int>(m_apBlockList.size());

	if (nIdx < 0 || nIdx > nNumBlock - 1)
	{
		return nullptr;
	}

	return m_apBlockList[nIdx];
}

//================================================
// 終了処理
//================================================
void CBlockManager::Uninit(void)
{
	m_apBlockList.clear();
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
		if ((*itr) != nullptr && (*itr)->Collision(pAABB, pPushPos))
		{
			bHit = true;
		}
	}
	return bHit;
}
