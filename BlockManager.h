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
#include<vector>
#include<memory>

//************************************************
// 前方宣言
//************************************************
class CBlock; 
class CColliderAABB;

//************************************************
// ブロックのマネージャクラスの定義
//************************************************
class CBlockManager
{
public:
	~CBlockManager();

	static void Create(void);
	static CBlockManager* GetInstance(void) { return m_pInstance.get(); }
	void SetBlock(CBlock* pBlock);
	CBlock* GetBlock(const int nIdx);
	int GetNumAll(void) const { return static_cast<int>(m_apBlockList.size()); }
	void Uninit(void);
	bool Collision(CColliderAABB* pAABB, D3DXVECTOR3* pPushPos);
	HRESULT Load(void);
private:
	CBlockManager();
	static std::unique_ptr<CBlockManager> m_pInstance; // 自分のインスタンス
	std::vector<CBlock*> m_apBlockList; // ブロックのリスト
};

#endif
