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

	typedef enum
	{
		TYPE_AABB = 0,
		TYPE_SPHERE,
		TYPE_FOV,
		TYPE_MAX
	}TYPE;

	CCollision();
	virtual ~CCollision();

	static CCollision* Create(const D3DXVECTOR3 pos,const TYPE type);
	void SetPos(const D3DXVECTOR3 pos) { m_pos = pos; }
	D3DXVECTOR3 GetPosition(void) const { return m_pos; }
private:
	D3DXVECTOR3 m_pos; // 位置
	TYPE m_type;	   // 当たり判定の種類
};

//************************************************
// 当たり判定AABBクラス
//************************************************
class CCollisionAABB : public CCollision
{
public:
	CCollisionAABB();
	~CCollisionAABB();

	static CCollisionAABB* Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 Size);
	bool Collision(CCollisionAABB* other);
	void SetOldPos(const D3DXVECTOR3 posOld) { m_posOld = posOld; }
private:
	D3DXVECTOR3 m_posOld;
	D3DXVECTOR3 m_Size;
};

//************************************************
// 当たり判定(円)クラス
//************************************************
class CCollisionSphere : public CCollision
{
public:
	CCollisionSphere();
	~CCollisionSphere();
	static CCollisionSphere* Create(const D3DXVECTOR3 pos, const float fRadius);

	// コライダーの作成処理
	CCollisionSphere CreateCollider(const D3DXVECTOR3 pos, const float fRadius);

	bool Collision(CCollisionSphere* other);
	void SetRadius(const float fRadius) { m_fRadius = fRadius; }
private:
	float m_fRadius; // 半径
};

//************************************************
// 当たり判定(視界)クラス
//************************************************
class CCollisionFOV : public CCollision
{
public:
	CCollisionFOV();
	~CCollisionFOV();
	static CCollisionFOV* Create(const D3DXVECTOR3 pos,const float fLength);

	bool Collision(const D3DXVECTOR3 pos, const float fAngle,const float fAngleLeft,const float fAngleRight);
private:
	float m_fLength; // 視界の長さ
};

#endif
