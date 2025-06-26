//================================================
//
// トランスフォームを制御する処理 [transform.h]
// Author:YUTO YOSHIDA
//
//=================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

//*************************************************
// インクルードファイル
//*************************************************
#include"main.h"

//*************************************************
// 位置のクラスの定義
//*************************************************
class CPosition
{
public:
	CPosition();
	~CPosition();

	D3DXVECTOR3 Get(void) const { return m_pos; }
	void Set(const D3DXVECTOR3 pos) { m_pos = pos; }

	void UpdatePosition(const D3DXVECTOR3 move);
private:
	D3DXVECTOR3 m_pos; // 位置
};

//*************************************************
// 回転のクラスの定義
//*************************************************
class CRotation
{
public:
	CRotation();
	~CRotation();

	D3DXVECTOR3 Get(void) const { return m_rot; }
	void Set(const D3DXVECTOR3 rot) { m_rot = rot; }
	void Add(const D3DXVECTOR3 rot) { m_rot += rot; }
	void SetDest(const D3DXVECTOR3 dest) { m_Dest = dest; }
	D3DXVECTOR3 GetDest(void) const { return m_Dest; }
	void SetSmoothAngle(const float coef);
private:
	D3DXVECTOR3 m_rot;	   // 向き
	D3DXVECTOR3 m_Dest; // 目的の向き
};

//*************************************************
// 移動のクラスの定義
//*************************************************
class CVelocity
{
public:
	CVelocity();
	~CVelocity();

	D3DXVECTOR3 Get(void) const { return m_move; }
	void Set(const D3DXVECTOR3 move) { m_move = move; }
	void Add(const D3DXVECTOR3 move) { m_move += move; }

	void SetInertia3D(const float fcoef);
	void SetInertia2D(const float fcoef);
	void Gravity(const float gravity);
	void Bound(D3DXVECTOR3 nor, float coef = 0.8f);
	void Jump(const float jumpHeight) { m_move.y = jumpHeight; }
private:
	D3DXVECTOR3 m_move; // 移動量
};

//*************************************************
// 大きさの(円形)クラスの定義
//*************************************************
class CSizeCircle
{
public:
	CSizeCircle();
	~CSizeCircle();

	float Get(void) const { return m_fRadius; }
	void Set(const float fRadius) { m_fRadius = fRadius; }
private:
	float m_fRadius; // 大きさ
};

//*************************************************
// 大きさの2Dクラスの定義
//*************************************************
class CSize2D
{
public:
	CSize2D();
	~CSize2D();

	D3DXVECTOR2 Get(void) const { return D3DXVECTOR2(m_fWidth, m_fHeight); }
	void Set(const float fWidth,const float fHeight) { m_fWidth = fWidth , m_fHeight = fHeight; }
private:
	float m_fWidth,m_fHeight; // 大きさ
};

//*************************************************
// 大きさの3Dクラスの定義
//*************************************************
class CSize3D
{
public:
	CSize3D();
	~CSize3D();

	D3DXVECTOR3 Get(void) const { return GetVector; }
	void Set(const D3DXVECTOR3 SIze) { GetVector = SIze; }
private:
	D3DXVECTOR3 GetVector;
};

#endif