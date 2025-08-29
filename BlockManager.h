//================================================
//
// ブロックマネージャー [BlockManager.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _BLOCKMANAGER_H_
#define _BLOCKMANAGER_H_

//************************************************
// インクルードファイル
//************************************************
#include "object.h"
#include<list>

//************************************************
// 前方宣言
//************************************************
class CBlock; 

//************************************************
// ブロックのマネージャクラスの定義
//************************************************
class CBlockManager
{
public:
	CBlockManager();
	~CBlockManager();

	void Delete(CBlock* pSelectBlock);
	void Update(void);
	void SetBlock(CBlock* pBlock);
private:
	CBlock* m_pSelectBlock;			  // 選択中のブロック
	std::list<CBlock*> m_apBlockList; // ブロックのリスト
};

#endif
