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
	static void Create(void);
	static CCollisionAABB* GetInstance(void) { return m_pAABB.get(); }
	bool Collision(CColliderAABB* pMyBox, CColliderAABB* pTargetBox, D3DXVECTOR3* pushPos = nullptr,int *pFace = nullptr);
	D3DXVECTOR3 GetPushPos(void) { return m_pushPos; }
private:
	CCollisionAABB();
	static std::unique_ptr<CCollisionAABB> m_pAABB; // 自分のインスタンス
	D3DXVECTOR3 m_pushPos;
};

//************************************************
// 当たり判定(円)クラス
//************************************************
class CCollisionSphere : public CCollision
{
public:
	~CCollisionSphere();
	// コライダーの作成処理
	static void Create(void);
	static CCollisionSphere* GetInstance(void) { return m_pSphere.get(); }

	bool Collision(CColliderSphere* myCollider, CColliderSphere* otherCollider);
private:
	CCollisionSphere();
	static std::unique_ptr<CCollisionSphere> m_pSphere; // 自分のインスタンス
};

//************************************************
// 当たり判定(視界)クラス
//************************************************
class CCollisionFOV : public CCollision
{
public:
	~CCollisionFOV();
	static void Create(void);
	static CCollisionFOV* GetInstance(void) { return m_pFOV.get(); }

	bool Collision(const D3DXVECTOR3 otherpos, CColliderFOV* pFOV);
	//CCollisionFOV CreateCollider(const D3DXVECTOR3 pos, const float fAngle, const float fAngleLeft, const float fAngleRight);
private:
	CCollisionFOV();
	static std::unique_ptr<CCollisionFOV> m_pFOV; // 自分のインスタンス
};

//************************************************
// 当たり判定(カプセル)クラス
//************************************************
class CCollisionCapsule : public CCollision
{
public:
	~CCollisionCapsule();
	static void Create(void);
	static CCollisionCapsule* GetInstance(void) { return m_pCapsule.get(); }

	bool Collision(CColliderCapsule* myCapsule, CColliderCapsule* otherCapsule, D3DXVECTOR3* NearPos1 = nullptr, D3DXVECTOR3* NearPos2 = nullptr);
	bool CollisionSphere(CColliderCapsule* pCapsule, CColliderSphere* pSphere);
private:
	CCollisionCapsule();
	float ClosestPtSegmentSegment(D3DXVECTOR3 Start1, D3DXVECTOR3 End1, D3DXVECTOR3 Start2, D3DXVECTOR3 End2, float* pParam1, float* pParam2, D3DXVECTOR3* c1osestPos1, D3DXVECTOR3* c1osestPos2);
	static std::unique_ptr< CCollisionCapsule> m_pCapsule;
};
//************************************************
// 当たり判定のマネージャークラスの定義
//************************************************
class CCollisionManager
{
public:
	~CCollisionManager();
	static void CreateAll(void);
	void Uninit(void);
private:
	CCollisionManager();
};
#endif
