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
	~CCollisionAABB();
	static void Create(void);
	static CCollisionAABB* GetInstance(void) { return m_pAABB.get(); }
	bool Collision(CColliderAABB *pMyBox, CColliderAABB *pTargetBox);
private:
	CCollisionAABB();
	static std::unique_ptr<CCollisionAABB> m_pAABB; // 自分のインスタンス
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
// 当たり判定のマネージャークラスの定義
//************************************************
class CCollisionManager
{
public:
	CCollisionManager();
	~CCollisionManager();
	void CreateAll(void);
	void Uninit(void);
private:
};
#endif
