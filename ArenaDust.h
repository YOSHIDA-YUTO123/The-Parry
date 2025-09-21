//===================================================
//
// ステージの瓦礫 [ArenaDust.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _ARENADUST_H_
#define _ARENADUST_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "objectX.h"

//***************************************************
// ステージの瓦礫のクラスの定義
//***************************************************
class CArenaDust : public CObjectX
{
public:
	CArenaDust();
	~CArenaDust();

	static CArenaDust* Create(const D3DXVECTOR3 pos, const float fAngle);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
	D3DXVECTOR3 m_move; // 移動量
};
#endif