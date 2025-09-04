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
#include<memory>

//************************************************
// コライダークラスの定義
//************************************************
class CCollider
{
public:
	CCollider();
	virtual ~CCollider();
	void SetPosition(const D3DXVECTOR3 pos) { m_pos = pos; }

	D3DXVECTOR3 GetPos(void) const { return m_pos; }
protected:
	D3DXVECTOR3 m_pos; // 位置
private:
};

//************************************************
// AABBのコライダークラスの定義
//************************************************
class CColliderAABB : public CCollider
{
public:

	// 当たり判定に必要なデータ(前回の位置,大きさ...)
	struct Data
	{
		D3DXVECTOR3 posOld; // 前回の位置
		D3DXVECTOR3 Size;	// 大きさ
	};

	CColliderAABB();
	~CColliderAABB();
	static std::unique_ptr<CColliderAABB> Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 posOld, const D3DXVECTOR3 Size);
	static CColliderAABB CreateCollider(const D3DXVECTOR3 pos, const D3DXVECTOR3 posOld, const D3DXVECTOR3 Size);
	void UpdateData(const D3DXVECTOR3 pos, const D3DXVECTOR3 posOld);
	Data GetData(void) { return m_Data; }
private:
	Data m_Data; // データ
};

//************************************************
// Sphereのコライダークラスの定義
//************************************************
class CColliderSphere : public CCollider
{
public:
	CColliderSphere();
	~CColliderSphere();
	static std::unique_ptr<CColliderSphere> Create(const D3DXVECTOR3 pos,const float fRadius);
	static CColliderSphere CreateCollider(const D3DXVECTOR3 pos, const float fRadius);
	float GetRadius(void) const { return m_fRadius; }
private:
	float m_fRadius;	// 半径
};

//************************************************
// FOVのコライダークラスの定義
//************************************************
class CColliderFOV : public CCollider
{
public:

	struct Data
	{
		float fLength;		// 視界の長さ
		float fNowAngle;	// 今の角度
		float fAngleLeft;	// 視野左
		float fAngleRight;	// 視野右
	};

	CColliderFOV();
	~CColliderFOV();
	static std::unique_ptr<CColliderFOV> Create(const D3DXVECTOR3 pos,const float fAngle, const float fAngleLeft, const float fAngleRight,const float fLength);
	static CColliderFOV CreateCollider(const D3DXVECTOR3 pos, const float fAngle, const float fAngleLeft, const float fAngleRight, const float fLength);
	Data GetData(void) const { return m_Data; }
	void UpdateData(const float fNowAngle) { m_Data.fNowAngle = fNowAngle; }
private:
	Data m_Data; // データ
};

//************************************************
// カプセルコライダークラスの定義
//************************************************
class CColliderCapsule : public CCollider
{
public:

	// 当たり判定のデータ
	struct Data
	{
		D3DXVECTOR3 StartPos; // 始点
		D3DXVECTOR3 EndPos;	// 終点
		float fRadius;		// 半径
		int nID;			// 自分の番号
	};

	CColliderCapsule();
	~CColliderCapsule();
	static std::unique_ptr<CColliderCapsule> Create(const D3DXVECTOR3 StartPos, const D3DXVECTOR3 EndPos, const float fRadius,const int nID);
	static CColliderCapsule CreateCollider(const D3DXVECTOR3 StartPos, const D3DXVECTOR3 EndPos, const float fRadius);
	Data GetData(void) const { return m_Data; }
	void UpdateData(const Data data);
private:
	Data m_Data; // データ
};
#endif