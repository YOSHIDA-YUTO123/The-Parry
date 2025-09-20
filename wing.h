//===================================================
//
// 羽 [Wing.h]
// Author:YUTO YOSHIDA
//
// << 鳥から羽を出すクラス >>
// 
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _WING_H_
#define _WING_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "objectX.h"
#include <memory>

//***************************************************
// 前方宣言
//***************************************************
class CVelocity;

//***************************************************
// 羽クラスの定義
//***************************************************
class CWing : public CObjectX
{
public:
	CWing();
	~CWing();

	static CWing* Create(const D3DXVECTOR3 pos);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
	std::unique_ptr<CVelocity> m_pMove; // 移動量
	D3DXVECTOR3 m_Scal;					// 大きさ
	D3DXVECTOR3 m_move;					// 移動量
	float m_fCounter;					// カウンター
	int m_nLife;						// 寿命
	int m_nMaxLife;						// 最大の寿命
};

#endif
