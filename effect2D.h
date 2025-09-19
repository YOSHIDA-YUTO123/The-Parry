//================================================
//
// エフェクト2D [effect2D.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _EFFECT2D_H_
#define _EFFECT2D_H_

//************************************************
// インクルードファイル
//************************************************
#include "object2D.h"

//************************************************
// 2Dエフェクトのクラスの定義
//************************************************
class CEffect2D : public CObject2D
{
public:
	CEffect2D(const int nPriority = 6);
	~CEffect2D();

	static CEffect2D* Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size,const D3DXCOLOR col, const int nPriority = 6);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void SetParam(const int nLife, const D3DXVECTOR3 move);
private:
	D3DXCOLOR m_col;		// 色
	D3DXVECTOR3 m_move;		// 移動量
	D3DXVECTOR2 m_DecSize;	// 大きさの減少値
	float m_fDecAlv;		// アルファ値の減少値
	int m_nLife;			// 寿命
};

#endif