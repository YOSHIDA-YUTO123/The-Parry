//================================================
//
// パーティクル2D [partile2D.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _PARTICLE2D_H_
#define _PARTICLE2D_H_

//************************************************
// インクルードファイル
//************************************************
#include "object.h"

//************************************************
// パーティクル2Dのクラスの定義
//************************************************
class CParticle2D : public CObject
{
public:
	CParticle2D();
	~CParticle2D();

	static CParticle2D* Create(const D3DXVECTOR3 pos, const float fRadius, const D3DXCOLOR col);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void SetParam(const float fAngle, const int nLife, const int nTime, const int nNum,const D3DXVECTOR3 Speed);
	void SetParam(const int nPriority,const int nPosRange);
private:
	D3DXCOLOR m_col;		// 色
	D3DXVECTOR3 m_pos;		// 位置
	D3DXVECTOR3 m_Speed;	// 速さ
	float m_fRadius;		// 半径
	float m_fAngle;			// 角度
	int m_nPosRange;		// 位置の範囲
	int m_nPriority;		// 優先順位
	int m_nNum;				// 発生量
	int m_nLife;			// 寿命
	int m_nTime;			// 発生時間
};

#endif