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
#include<vector>
#include<string>
#include"effect.h"

//***************************************************
// パーティクル3Dクラスの定義(基底クラス)
//***************************************************
class CParticle3D : public CObject
{
public:

	// パーティクルの情報(共通部分)
	struct Info
	{
		D3DXVECTOR3 pos;	// 位置
		D3DXCOLOR col;		// 色
		float fRadius;		// 半径
		float fSpeed;		// 速さ
		int nAngle;			// 放射範囲
		int nLife;			// 寿命
		int nNumParticle;	// 粒子の数
		int nTime;			// 発生させる時間
	};

	CParticle3D();
	virtual ~CParticle3D();

	virtual HRESULT Init(void) override;
	virtual void Uninit(void) override;
	virtual void Update(void) override = 0;
	virtual void Draw(void) override;
	void SetParticle(const float fSpeed, const int nLife, const int nNumParticle, const int nTime, const int nAngle);

protected:
	int GetMaxLife(void) const { return m_nMaxLife; }
	void SetMaxLife(const int nMaxLife) { m_nMaxLife = nMaxLife; }
	void SetInfo(const Info info) { m_Info = info; }
	Info GetInfo(void) const { return m_Info; }
private:
	Info m_Info;		// パーティクルの情報
	int m_nMaxLife;		// 最大のHP
};

//***************************************************
// パーティクル3D(通常)クラスの定義
//***************************************************
class CParticle3DNormal : public CParticle3D
{
public:
	CParticle3DNormal();
	~CParticle3DNormal();

	static CParticle3DNormal* Create(const D3DXVECTOR3 pos,const float fRadius,const D3DXCOLOR col);
	void SetParam(CEffect3D::TYPE type,const int nPosRange, const float fInertia = 0.01f);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override {};
private:
	CEffect3D::TYPE m_type;
	float m_fInertia; // 慣性
	int m_nPosRange;  // 位置の範囲
};
#endif