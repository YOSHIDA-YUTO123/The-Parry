//================================================
//
// ロックオン時にビルボードを描画する [RockOn.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _ROCKON_H_
#define _ROCKON_H_

//************************************************
// インクルードファイル
//************************************************
#include "billboard.h"

//************************************************
// ロックオンクラスの定義
//************************************************
class CRockOn : public CObjectBillboard
{
public:
	CRockOn();
	~CRockOn();

	static CRockOn* Create(void);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
	const D3DXVECTOR2 SIZE = { 40.0f,40.0f };
	D3DXVECTOR2 m_DestSize; // 目的の大きさ
};

#endif