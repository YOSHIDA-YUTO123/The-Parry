//===================================================
//
// 集中パーティクル [ZoneParticle.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _ZONEPARTICLE_H_
#define _ZONEPARTICLE_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"particle.h"

//***************************************************
// 集中パーティクルクラスの定義
//***************************************************
class CZoneParticle3D : public CParticle3D
{
public:
	CZoneParticle3D();
	~CZoneParticle3D();

	static CZoneParticle3D* Create(const D3DXVECTOR3 pos, const float fRadius, const D3DXCOLOR col);
	
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override {};

	void SetZone(D3DXVECTOR3 DestPos, const int nRandomPosRange);
private:
	D3DXVECTOR3 m_DestPos; // 目的の位置へのポインタ
	int m_nRandomPosRange; // ランダムな位置の範囲
};

#endif