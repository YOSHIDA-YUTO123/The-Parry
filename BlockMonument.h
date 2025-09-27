//===================================================
//
// 石碑ブロックの処理 [BlockMonumet.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _BLOCKMONUMENT_H_
#define _BLOCKMONUMENT_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "block.h"

//***************************************************
// 前方宣言
//***************************************************
class CMonumentUI;
class CMonumentMessageUI;

//***************************************************
// 石碑ブロッククラスの定義
//***************************************************
class CBlockMonument : public CBlock
{
public:

	CBlockMonument();
	~CBlockMonument();

	static CBlockMonument* Create(const D3DXVECTOR3 pos, const TYPE type, const D3DXVECTOR3 rot = Const::VEC3_NULL);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

private:
	void SetMonumentMessage(void);		
	CMonumentUI* m_pUI;					// 石碑のUIクラスへのポインタ
	CMonumentMessageUI* m_pMessageUI;	// 石碑のメッセージUIのクラスへのポインタ
	bool m_bShowMessage;				// メッセージを表示しているかどうか
};

#endif
