//===================================================
//
// 火花パーティクル [ParticleSpark.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _PARTICLESPARK_H_
#define _PARTICLESPARK_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "particle.h"

//***************************************************
// 火花パーティクルクラスの定義
//***************************************************
class CParticleSpark : public CParticle3D
{
public:
	CParticleSpark();
	~CParticleSpark();

	static CParticleSpark* Create(const D3DXVECTOR3 pos,const D3DXVECTOR2 Size, const D3DXCOLOR col);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
	D3DXVECTOR2 m_Size; // 大きさ
};

#endif
