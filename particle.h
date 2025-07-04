//===================================================
//
// パーティクル [particle.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _PARTICLE_H_
#define _PARTICLE_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include"object.h"
#include"effect.h"
#include<vector>

//***************************************************
// オブジェクト3Dクラスの定義
//***************************************************
class CParticle3D : public CObject
{
public:
	CParticle3D();
	~CParticle3D();

	static CParticle3D* Create(const D3DXVECTOR3 pos, const D3DXCOLOR col, const int nLife, const float fRadius, const int nNumParticle,const int nTime,const float fSpeed);

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:

	std::vector<CEffect3D*> m_apEffect;		// エフェクトクラスへのポインタ
	D3DXVECTOR3 m_pos;						// 位置
	CVelocity *m_pMove;						// 移動量
	D3DXCOLOR m_col;						// 色
	float m_fRadius;						// 半径
	float m_fSpeed;							// 速さ
	int m_nLife;							// 寿命
	int m_nMaxLife;							// 最大のHP
	int m_nNumParticle;						// 粒子の数
	int m_nTime;							// パーティクルを出す時間
};
#endif