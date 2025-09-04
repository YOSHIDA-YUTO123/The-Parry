//===================================================
//
// 目印 [mark.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _MARK_H_
#define _MARK_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "billboard.h"

//***************************************************
// 目印クラスの定義
//***************************************************
class CMark : public CObjectBillboard
{
public:
	CMark();
	~CMark();

	static CMark* Create(const D3DXVECTOR3 pos);
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
	D3DXVECTOR3 m_col;	// 色
	D3DXVECTOR3 m_move;	// 移動量
	float m_fCounter;	// カウンター
};

#endif
