//================================================
//
// 敵の移動制御処理 [enemyMovement.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _ENEMYMOVEMEMT_H_
#define _ENEMYMOVEMEMT_H_

//************************************************
// インクルードファイル
//************************************************
#include "main.h"
#include <memory>

//************************************************
// 前方宣言
//************************************************
class CVelocity;

//***************************************************
// 敵の移動処理クラスの定義
//***************************************************
class CEnemyMovement
{
public:

	static constexpr float INERTIA_BASE = 0.25f; // 慣性

	CEnemyMovement();
	~CEnemyMovement();

	void Init(void);
	void Uninit(void);

	/// <summary>
	/// 後ろに吹き飛ぶ処理
	/// </summary>
	/// <param name="AttackerDir"></param>
	/// <param name="blowOff"></param>
	/// <param name="jump"></param>
	/// <returns>アタッカーまでの角度</returns>
	float BlowOff(const D3DXVECTOR3 AttackerDir, const float blowOff, const float jump);
	void MoveForWard(const float fSpeed, const float fAngle);
	void SetMoveDir(const float dir, const float fSpeed, const float fAngleBase); // 移動方向の設定
	void Jump(const float fHeight);
	void UpdatePosition(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld);
	void SetInertia(const float fInertia) { m_fInertia = fInertia; }

	D3DXVECTOR3 Get(void) const;
	void Set(const D3DXVECTOR3 move);
	void Gravity(const float Gravity);
private:
	std::unique_ptr<CVelocity> m_pMove;		// 敵の移動制御用変数
	float m_fInertia;						// 慣性
};

#endif