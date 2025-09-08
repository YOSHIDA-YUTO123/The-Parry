//===================================================
//
// パリィのエフェクト [ParryEffect.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _PARRYEFFECT_H_
#define _PARRYEFFECT_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "Object3DAnim.h"

//***************************************************
// パリィのエフェクトクラスの定義
//***************************************************
class CParryEffect : public CObject3DAnim
{
public:

	// エフェクトの種類
	typedef enum
	{
		TYPE_PARRY = 0,
		TYPE_ROUND_KICK,
		TYPE_SPARK,
		TYPE_MAX
	}TYPE;

	CParryEffect();
	~CParryEffect();

	static CParryEffect* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 Size, const D3DXVECTOR3 rot, const int nSegX, const int nSegY, const int nSpeed, const bool bLoop, const TYPE type);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

private:
	TYPE m_type; // 種類
};

#endif
