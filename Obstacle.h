//==============================================
//
// 障害物 [obstacle.h]
// Author:YUTO YOSHIDA
//
//==============================================

//**********************************************
// 多重インクルード防止
//**********************************************
#ifndef _OBSTACLE_H_
#define _OBSTACLE_H_

//**********************************************
// インクルードファイル
//**********************************************
#include"main.h"
#include "object.h"
#include<memory>

//**********************************************
// 前方宣言
//**********************************************
class CObjectX;
class CVelocity;
class CCollision;
class CCollisionAABB;
class CColliderAABB;
//**********************************************
// 障害物のクラスの定義
//**********************************************
class CObstacle : public CObject
{
public:
	CObstacle();
	~CObstacle();

	virtual HRESULT Init(void) override;
	virtual void Uninit(void) override;
	virtual void Update(void) override;
	virtual void Draw(void) override;
	virtual bool Collision(CColliderAABB* pCollider,D3DXVECTOR3 *pushPos) = 0;
	void Release(void);

	void SetPosition(const D3DXVECTOR3 pos);
	void SetRotaition(const D3DXVECTOR3 rot);
	void SetModelName(const char* pModelName);

	D3DXVECTOR3 GetPosition(void) const;
	D3DXVECTOR3 GetSize(void) const;
	D3DXMATRIX GetMatrix(void) const;
private:
	CObjectX* m_pObjectX;							  // Xファイルモデルクラスのポインタ
	std::unique_ptr<CVelocity> m_pMove;				  // 移動量
};

//**********************************************
// スパイクトラップクラスの定義
//**********************************************
class CSpikeTrap : public CObstacle
{
public:
	CSpikeTrap();
	~CSpikeTrap();
	static CSpikeTrap* Create(const D3DXVECTOR3 pos,const int nDamageFace);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	bool Collision(CColliderAABB* pCollider, D3DXVECTOR3* pushPos) override;
	D3DXVECTOR3 GetpushPos(void) const { return m_pushPos; }
private:
	std::unique_ptr<CColliderAABB> m_pAABB;  // 矩形のコライダー
	D3DXVECTOR3 m_posOld;					 // 前回の位置
	D3DXVECTOR3 m_CenterPos;				 // 真ん中の位置
	D3DXVECTOR3 m_pushPos;					 // 押し出された位置
	int m_nDamageFace;						 // ダメージを食らう面
};
#endif
