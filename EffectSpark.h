//===================================================
//
// 火花エフェクト [EffectSpark.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef EFFECTSPARK_H_
#define EFFECTSPARK_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"billboard.h"
#include<memory>

//***************************************************
// 前方宣言
//***************************************************
class CVelocity;

//***************************************************
// 火花エフェクトクラスの定義
//***************************************************
class CEffectSpark : public CObjectBillboard
{
public:
	CEffectSpark();
	~CEffectSpark();

	static CEffectSpark* Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const D3DXCOLOR col);
	void SetParam(const int nLife, const D3DXVECTOR3 move,const float fGravity);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
 	std::unique_ptr<CVelocity> m_pMove;	// 移動量
	D3DXCOLOR m_col;	// 色
	float m_fDecAlv;	// アルファ値の減少スピード
	float m_fGravity;	// 重力
	int m_nLife;		// 寿命
};

#endif
