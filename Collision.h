//================================================
//
// 当たり判定 [collision.h]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// 多重インクルード防止
//************************************************
#ifndef _COLLISION_H_
#define _COLLISION_H_

//************************************************
// インクルードファイル
//************************************************
#include"main.h"
#include"object.h"

//************************************************
// 当たり判定AABBのクラスの定義
//************************************************
class CCollision
{
public:
	CCollision();
	virtual ~CCollision();

	void SetElement(const D3DXVECTOR3 pos, const D3DXVECTOR3 Size = VEC3_NULL);
	D3DXVECTOR3 GetPosition(void) const { return m_pos; }
	D3DXVECTOR3 GetSize(void) const { return GetVector; }
private:
	D3DXVECTOR3 m_pos;
	D3DXVECTOR3 GetVector;
};

//************************************************
// 当たり判定AABBクラス
//************************************************
class CCollisionAABB : public CCollision
{
public:
	CCollisionAABB();
	~CCollisionAABB();
	bool Collision(CCollisionAABB* other);
	void SetOldPos(const D3DXVECTOR3 posOld) { m_posOld = posOld; }
private:
	D3DXVECTOR3 m_posOld;
};

//************************************************
// 当たり判定(円)クラス
//************************************************
class CCollisionSphere : public CCollision
{
public:
	CCollisionSphere();
	~CCollisionSphere();
	bool Collision(CCollisionSphere* other);
	void SetRadius(const float fRadius) { m_fRadius = fRadius; }
private:
	float m_fRadius; // 半径
};

#endif
