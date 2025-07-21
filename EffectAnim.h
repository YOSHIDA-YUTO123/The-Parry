//===================================================
//
// エフェクトアニメーション [ EeffectAnim.h ]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _EFFECTANIM_H_
#define _EFFECTANIM_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"billboardAnim.h"
#include<memory>

//***************************************************
// 前方宣言
//***************************************************
class CVelocity;

//***************************************************
// エフェクト3Dのアニメーションクラスの定義
//***************************************************
class CEffect3DAnim : public CBillboardAnimation
{
public:
	CEffect3DAnim();
	~CEffect3DAnim();

	static CEffect3DAnim* Create(const D3DXVECTOR3 pos, const float fRadius, const D3DXCOLOR col);
	void SetEffect(const D3DXVECTOR3 pos, const float fRadius, const D3DXCOLOR col);
	void SetEffect(const int nLife, const D3DXVECTOR3 move);

	virtual HRESULT Init(void) override;
	virtual void Uninit(void) override;
	virtual void Update(void) override;
	virtual void Draw(void) override;

private:
	std::shared_ptr<CVelocity> m_pMove;	// 移動量
	D3DXCOLOR m_col;					// 色
	float m_decRadius;					// 大きさの減少スピード
	float m_decAlv;						// α値の減少スピード
	float m_fRadius;					// 半径
	int m_nLife;						// 寿命
};

#endif