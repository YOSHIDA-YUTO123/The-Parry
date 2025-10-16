//================================================
//
// 移動量を制御するクラス [velocity.h]
// Author:YUTO YOSHIDA
//
//=================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _VELOCITY_H_
#define _VELOCITY_H_

//*************************************************
// インクルードファイル
//*************************************************

//*************************************************
// 移動のクラスの定義
//*************************************************
class CVelocity
{
public:
	CVelocity();
	~CVelocity();

	D3DXVECTOR3 Get(void) const;
	void Set(const D3DXVECTOR3 move) { m_move = move; }
	void Add(const D3DXVECTOR3 move) { m_move += move; }

	void SetInertia3D(const float fcoef, const bool bMoveY = false);
	void SetInertia2D(const float fcoef);
	void Gravity(const float gravity);
	void Bound(D3DXVECTOR3 nor, float coef = 0.8f);
	void Jump(const float jumpHeight);
private:
	D3DXVECTOR3 m_move; // 移動量
};
#endif