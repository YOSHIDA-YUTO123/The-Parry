//==============================================
//
// 爆発のエリア [ExplosionArea.h]
// Author:YUTO YOSHIDA
//
//==============================================

//**********************************************
// 多重インクルード防止
//**********************************************
#ifndef _EXPLOSIONAREA_H_
#define _EXPLOSIONAREA_H_

//**********************************************
// インクルードファイル
//**********************************************
#include "object.h"
#include<memory>

//**********************************************
// 前方宣言
//**********************************************
class CColliderSphere;
class CColliderCapsule;

//**********************************************
// 爆破のエリアクラスの定義
//**********************************************
class CExplosionArea : public CObject
{
public:
	CExplosionArea();
	~CExplosionArea();

	static CExplosionArea* Create(const D3DXVECTOR3 pos);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	bool Collision(CColliderCapsule* pCapsule, D3DXVECTOR3* pPos = nullptr);
private:
	std::unique_ptr<CColliderSphere> m_pSphere; // 円のコライダー
	D3DXVECTOR3 m_pos;							// 位置
	int m_nLife;								// 寿命
};

#endif