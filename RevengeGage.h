//================================================
//
// 反撃ゲージ [RevengeGage.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _REVENGEGAGE_H_
#define _REVENGEGAGE_H_

//************************************************
// インクルードファイル
//************************************************
#include "object2DMT.h"
#include<memory>

//*************************************************
// 前方宣言
//*************************************************
class CNumber;

//*************************************************
// 反撃ゲージのクラスの定義
//*************************************************
class CRevengeUI : public CObject2DMT
{
public:
	CRevengeUI();
	~CRevengeUI();

	static CRevengeUI* Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const float fValue);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Set(const float fValue) { m_fValue = fValue; }
private:
	float m_fValue;			// 現在の量
	float m_fMaxValue;		// 最大の量
};

#endif