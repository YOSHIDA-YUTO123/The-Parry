//================================================
//
// コライダー [collider.h]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// 多重インクルード防止
//************************************************
#ifndef _COLLIDER_H_
#define _COLLIDER_H_

//************************************************
// インクルードファイル
//************************************************
#include"main.h"

//************************************************
//// コライダークラスの定義
////************************************************
//class CCollider
//{
//public:
//	CCollider();
//	virtual ~CCollider();
//	void Update(const D3DXVECTOR3 pos) { m_pos = pos; }
//	D3DXVECTOR3 GetPos(void) const { return m_pos; }
//protected:
//	D3DXVECTOR3 m_pos; // 位置
//private:
//};
//
////************************************************
//// AABBのコライダークラスの定義
////************************************************
//class CColliderAABB : public CCollider
//{
//public:
//
//	// 当たり判定に必要なデータ
//	struct Data
//	{
//		D3DXVECTOR3 Size;	// 大きさ
//	};
//
//	CColliderAABB();
//	~CColliderAABB();
//	static CColliderAABB* CreateCollider(const D3DXVECTOR3 Size);
//	Data GetData(void) { return m_Data; }
//private:
//	Data m_Data; // データ
//};
//
////************************************************
//// Sphereのコライダークラスの定義
////************************************************
//class CColliderSphere : public CCollider
//{
//public:
//	CColliderSphere();
//	~CColliderSphere();
//	static CColliderSphere* CreateCollider(const float fRadius);
//private:
//	float m_fRadius;	// 半径
//};
//
////************************************************
//// FOVのコライダークラスの定義
////************************************************
//class CColliderFOV : public CCollider
//{
//public:
//	CColliderFOV();
//	~CColliderFOV();
//	static CColliderFOV* CreateCollider(const float fAngle, const float fAngleLeft, const float fAngleRight,const float fLength);
//private:
//	float m_fLength;		// 視界の長さ
//	float m_fNowAngle;		// 今の角度
//	float m_fAngleLeft;		// 視野左
//	float m_fAngleRight;	// 視野右
//
//};

#endif