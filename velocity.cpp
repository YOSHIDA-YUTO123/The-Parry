//================================================
//
// 移動量を制御するクラス [velocity.cpp]
// Author:YUTO YOSHIDA
//
//=================================================

//*************************************************
// インクルードファイル
//*************************************************
#include "velocity.h"
#include "slow.h"
#include "manager.h"
#include "math.h"

//=================================================
// コンストラクタ
//=================================================
CVelocity::CVelocity()
{
	m_move = Const::VEC3_NULL;
}

//=================================================
// デストラクタ
//=================================================
CVelocity::~CVelocity()
{
}

//=================================================
// 移動量の取得
//=================================================
D3DXVECTOR3 CVelocity::Get(void) const
{
	// スローモーションの取得
	CSlow* pSlow = CManager::GetSlow();

	return m_move * pSlow->GetLevel(false);
}

//=================================================
// 移動量の減衰処理3D
//=================================================
void CVelocity::SetInertia3D(const float fcoef, const bool bMoveY)
{
	// 0.0fに近づける
	m_move.x = LerpDest(0.0f, m_move.x, fcoef);

	// Yも減衰するなら
	if (bMoveY)
	{
		m_move.y = LerpDest(0.0f, m_move.y, fcoef);
	}
	m_move.z = LerpDest(0.0f, m_move.z, fcoef);
}

//=================================================
// 移動量の減衰処理2D
//=================================================
void CVelocity::SetInertia2D(const float fcoef)
{
	// 0.0fに近づける
	m_move.x = LerpDest(0.0f, m_move.x, fcoef);
	m_move.y = LerpDest(0.0f, m_move.y, fcoef);
}

//=================================================
// 重力の設定
//=================================================
void CVelocity::Gravity(const float gravity)
{
	// スローモーションの取得
	CSlow* pSlow = CManager::GetSlow();

	// スローモーションのレベル
	float fSlowLevel = pSlow->GetLevel(false);

	m_move.y += gravity * fSlowLevel;
}

//=================================================
// バウンドの設定
//=================================================
void CVelocity::Bound(D3DXVECTOR3 nor, float coef)
{
	// 範囲内にクランプする
	coef = Clamp(coef, 0.0f, 1.0f);

	float dot = D3DXVec3Dot(&m_move, &nor);

	D3DXVECTOR3 NewMove = m_move - (dot * 2.0f) * nor;

	m_move = NewMove * coef;
}

//=================================================
// ジャンプ処理
//=================================================
void CVelocity::Jump(const float jumpHeight)
{
	m_move.y = jumpHeight;
}
