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

	CCollision(TYPE type);
	virtual ~CCollision();

	static CCollision* Create(const D3DXVECTOR3 pos,const TYPE type);
	virtual bool Collision(CCollision* other) = 0;

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
	CCollisionAABB();
	~CCollisionAABB();

	static void Create(void);
	static CCollisionAABB* GetInstance(void) { return m_pAABB.get(); }
	bool Collision(CColliderAABB *pMyBox, CColliderAABB *pTargetBox);
	bool Collision(CCollision* other) { other = nullptr; return 0; }
private:
	static std::unique_ptr<CCollisionAABB> m_pAABB; // 自分のインスタンス
};

//************************************************
// 当たり判定(円)クラス
//************************************************
class CCollisionSphere : public CCollision
{
public:
	CCollisionSphere();
	~CCollisionSphere();
	static std::unique_ptr<CCollisionSphere> Create(const D3DXVECTOR3 pos, const float fRadius);

	// コライダーの作成処理
	CCollisionSphere CreateCollider(const D3DXVECTOR3 pos, const float fRadius);

	bool Collision(CCollision* other) override;
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
	static std::unique_ptr<CCollisionFOV> Create(const D3DXVECTOR3 pos,const float fLength);

	bool Collision(CCollision *fov) override;
	CCollisionFOV CreateCollider(const D3DXVECTOR3 pos, const float fAngle, const float fAngleLeft, const float fAngleRight);
private:
	float m_fLength;		// 視界の長さ
	float m_fNowAngle;		// 今の角度
	float m_fAngleLeft;		// 視野左
	float m_fAngleRight;	// 視野右
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
