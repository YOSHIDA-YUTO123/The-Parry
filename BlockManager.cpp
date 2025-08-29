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

//================================================
// コンストラクタ
//================================================
CBlockManager::CBlockManager()
{
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
// 選択中のブロックの消去
//================================================
void CBlockManager::Delete(CBlock* pSelectBlock)
{
	// 選択中のブロックのイテレータを取得
	auto itr = std::find(m_apBlockList.begin(), m_apBlockList.end(), pSelectBlock);

	if (itr != m_apBlockList.end())
	{
		// 終了処理
		(*itr)->Uninit();
		m_apBlockList.erase(itr);
	}
}

//================================================
// 更新処理
//================================================
void CBlockManager::Update(void)
{
	for (auto itr = m_apBlockList.begin(); itr != m_apBlockList.end();)
	{
		CBlock* pBlock = *itr;

		// 死亡フラグがたっていたら
		if (pBlock->GetDeath())
		{
			itr = m_apBlockList.erase(itr);
		}
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
