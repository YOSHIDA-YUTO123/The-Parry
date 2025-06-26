//===================================================
//
// エフェクト [effect.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _EFFECT_H_
#define _EFFECT_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include"billboard.h"
#include"transform.h"

//***************************************************
// オブジェクト3Dクラスの定義
//***************************************************
class CEffect3D : public CObjectBillboard
{
public:
	CEffect3D(int nPriority = 4);
	~CEffect3D();

	static CEffect3D* Create(const D3DXVECTOR3 pos, const float fRadius,const D3DXVECTOR3 move,const D3DXCOLOR col,const int nLife);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	CVelocity* GetVelocity(void) { return m_pMove; }
private:
	CVelocity *m_pMove;		// 移動量
	D3DXCOLOR m_col;		// 色
	float m_fRadius;		// 半径
	float m_decRadius;		// 大きさの減少スピード
	float m_decAlv;			// α値の減少スピード
	int m_nLife;			// 寿命
};
#endif