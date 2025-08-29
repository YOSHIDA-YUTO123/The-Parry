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

//***************************************************
// ブロッククラスの定義
//***************************************************
class CBlock : public CObjectX
{
public:
	CBlock();
	~CBlock();

	// "data/MODEL/"は省略
	static CBlock* Create(const D3DXVECTOR3 pos,const char *pModelFileName, const D3DXVECTOR3 rot = Const::VEC3_NULL);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
};

#endif