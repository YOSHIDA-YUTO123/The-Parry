//==============================================
//
// 歯車 [gear.h]
// Author:YUTO YOSHIDA
//
//==============================================

//**********************************************
// 多重インクルード防止
//**********************************************
#ifndef _GEAR_H_
#define _GEAR_H_

//**********************************************
// インクルードファイル
//**********************************************
#include "objectX.h"

//**********************************************
// 歯車クラスの定義
//**********************************************
class CGear : public CObjectX
{
public:
	CGear();
	~CGear();

	static CGear* Create(const D3DXVECTOR3 pos);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
	float m_fRotValue;	// 回転量
};

#endif