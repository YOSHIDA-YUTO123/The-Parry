//===================================================
//
// 竜巻パーティクル [PaticleTornado.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _PARTICLETORNADO_H_
#define _PARTICLETORNADO_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "particle.h"

//***************************************************
// 竜巻パーティクルクラスの定義
//***************************************************
class CParticleTornado : public CParticle3D
{
public:
	CParticleTornado();
	~CParticleTornado();

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

private:

};

#endif