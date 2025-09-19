//===================================================
//
// 反撃発動可能UI [RevengeActiveUI.h]
// Author:YUTO YOSHIDA
// 
// << 反撃発動可能時にUIを表示するクラス	>>
// 
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _REVENGEACTIVEUI_H_
#define _REVENGEACTIVEUI_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "Object2DAnimMT.h"

//***************************************************
// 反撃の発動可能時のUIのクラスの定義
//***************************************************
class CRevengeActiveUI : public CObject2DAnimMT
{
public:
	CRevengeActiveUI();
	~CRevengeActiveUI();

	static CRevengeActiveUI* Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const int UV, const int HV);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
};

#endif
