//===================================================
//
// ブロック [block.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _BLOCK_H_
#define _BLOCK_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "objectX.h"
#include <memory>

//***************************************************
// 前方宣言
//***************************************************
class CColliderAABB;

//***************************************************
// ブロッククラスの定義
//***************************************************
class CBlock : public CObjectX
{
public:

	// ブロックの種類
	typedef enum 
	{
		TYPE_WALL000 = 0,
		TYPE_WALL001,
		TYPE_WALL002,
		TYPE_PILLAR000,
		TYPE_GATE000,
		TYPE_SPEAR_RACK,
		TYPE_SWORD_RACK,
		TYPE_SWORD_RACK001,
		TYPE_WEPON_RACK,
		TYPE_WOODEN_BOX000,
		TYPE_WOODEN_BOX001,
		TYPE_TORCH,
		TYPE_MONUMENT_000,
		TYPE_MONUMENT_001,
		TYPE_MAX
	}TYPE;

	CBlock();
	~CBlock();

	// "data/MODEL/"は省略
	static CBlock* Create(const D3DXVECTOR3 pos, const TYPE type, const D3DXVECTOR3 rot = Const::VEC3_NULL);

	virtual HRESULT Init(void) override;
	virtual void Uninit(void) override;
	virtual void Update(void) override;
	virtual void Draw(void) override;

	bool Collision(CColliderAABB* pAABB, D3DXVECTOR3* pPushPos);
protected:
	void SetType(const TYPE type) { m_type = type; }
	TYPE GetType(void) const { return m_type; }
private:
	static const char* m_pFilePath[TYPE_MAX];	// ファイルパス

	std::unique_ptr<CColliderAABB> m_pAABB;		// AABBのコライダー
	D3DXVECTOR3 m_CenterPos;					// 中心座標
	TYPE m_type;								// 種類
};

#endif