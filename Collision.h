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
#include<memory>

//************************************************
// 前方宣言
//************************************************
class CColliderAABB;
class CCollider;
class CColliderSphere;
class CColliderFOV;
class CColliderCapsule;

//************************************************
// 当たり判定のクラスの定義
//************************************************
class CCollision
{
public:

	typedef enum
	{
		TYPE_AABB = 0,
		TYPE_SPHERE,
		TYPE_FOV,
		TYPE_CAPSULE,
		TYPE_MAX
	}TYPE;

	CCollision(TYPE type);
	virtual ~CCollision();

	static CCollision* Create(const D3DXVECTOR3 pos,const TYPE type);

	void SetPos(const D3DXVECTOR3 pos) { m_pos = pos; }

	D3DXVECTOR3 GetPosition(void) const { return m_pos; }
	TYPE GetType(void) const { return m_type; }
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

	// 面の種類
	typedef enum
	{
		FACE_ALL = 0,
		FACE_RIGHT,
		FACE_LEFT,
		FACE_FRONT,
		FACE_BACK,
		FACE_MAX
	}FACE;

	~CCollisionAABB();
	static bool Collision(CColliderAABB* pMyBox, CColliderAABB* pTargetBox, D3DXVECTOR3* pushPos = nullptr,int *pFace = nullptr);
private:
	CCollisionAABB();
};

//************************************************
// 当たり判定(円)クラス
//************************************************
class CCollisionSphere : public CCollision
{
public:
	~CCollisionSphere();

	static bool Collision(CColliderSphere* myCollider, CColliderSphere* otherCollider);
private:
	CCollisionSphere();
};

//************************************************
// 当たり判定(視界)クラス
//************************************************
class CCollisionFOV : public CCollision
{
public:
	~CCollisionFOV();

	static bool Collision(const D3DXVECTOR3 otherpos, CColliderFOV* pFOV);
private:
	CCollisionFOV();
};

//************************************************
// 当たり判定(カプセル)クラス
//************************************************
class CCollisionCapsule : public CCollision
{
public:
	~CCollisionCapsule();

	static bool Collision(CColliderCapsule* myCapsule, CColliderCapsule* otherCapsule, D3DXVECTOR3* NearPos1 = nullptr, D3DXVECTOR3* NearPos2 = nullptr);
	static bool CollisionSphere(CColliderCapsule* pCapsule, CColliderSphere* pSphere);
private:
	CCollisionCapsule();
	static float ClosestPtSegmentSegment(D3DXVECTOR3 Start1, D3DXVECTOR3 End1, D3DXVECTOR3 Start2, D3DXVECTOR3 End2, float* pParam1, float* pParam2, D3DXVECTOR3* c1osestPos1, D3DXVECTOR3* c1osestPos2);
};

#endif
