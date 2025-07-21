//===================================================
//
// 移動時の煙 [MoveSmoke.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _MOVESMOKE_H_
#define _MOVESMOKE_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "EffectAnim.h"

//***************************************************
// 移動時の煙クラスの定義
//***************************************************
class CMoveSmoke : public CEffect3DAnim
{
public:
	CMoveSmoke();
	~CMoveSmoke();

	static CMoveSmoke* Create(const D3DXVECTOR3 pos, const float fRadius, const D3DXCOLOR col);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
};

#endif