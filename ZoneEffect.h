//===================================================
//
// 集中エフェクト3D [ZoneEffect3D.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _ZONEEFFECT3D_H_
#define _ZONEEFFECT3D_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"effect.h"

//***************************************************
// 集中エフェクト3Dのクラスの定義
//***************************************************
class CZoneEffect3D : public CEffect3D
{
public:
	CZoneEffect3D();
	~CZoneEffect3D();

	static CZoneEffect3D* Create(const D3DXVECTOR3 pos, const float fRadius, const D3DXCOLOR col);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
};

#endif