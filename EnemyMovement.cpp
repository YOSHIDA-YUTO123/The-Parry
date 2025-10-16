//================================================
//
// 敵の移動制御処理 [enemyMovement.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "EnemyMovement.h"
#include "transform.h"
#include "velocity.h"

//===================================================
// コンストラクタ
//===================================================
CEnemyMovement::CEnemyMovement()
{
	m_pMove = nullptr;
	m_fInertia = INERTIA_BASE;
}

//===================================================
// デストラクタ
//===================================================
CEnemyMovement::~CEnemyMovement()
{
}

//===================================================
// 初期化処理
//===================================================
void CEnemyMovement::Init()
{
	// nullなら
	if (m_pMove == nullptr)
	{
		// 敵の移量を参照
		m_pMove = std::make_unique<CVelocity>();
	}
}

//===================================================
// 終了処理
//===================================================
void CEnemyMovement::Uninit(void)
{
	m_pMove.reset();
}

//===================================================
// 吹き飛び処理
//===================================================
float CEnemyMovement::BlowOff(const D3DXVECTOR3 AttackerDir, const float blowOff, const float jump)
{
	// nullだったら処理しない
	if (m_pMove == nullptr) return NULL;

	// 角度を求める
	float fAngle = atan2f(AttackerDir.x, AttackerDir.z);

	// 移動量
	D3DXVECTOR3 move;

	// 移動量の設定
	move.x = sinf(fAngle) * blowOff;
	move.y = jump;
	move.z = cosf(fAngle) * blowOff;

	m_pMove->Set(move);

	return fAngle;
}

//===================================================
// 向いている方向に進む処理
//===================================================
void CEnemyMovement::MoveForWard(const float fSpeed, const float fAngle)
{
	// nullだったら処理しない
	if (m_pMove == nullptr) return;

	// 移動量の取得
	D3DXVECTOR3 move = m_pMove->Get();

	// 移動量の設定
	move.x = sinf(fAngle) * fSpeed;
	move.z = cosf(fAngle) * fSpeed;

	// 移動量の設定
	m_pMove->Set(move);
}

//===================================================
// 移動方向の設定処理
//===================================================
void CEnemyMovement::SetMoveDir(const float dir, const float fSpeed, const float fAngleBase)
{
	// nullだったら処理しない
	if (m_pMove == nullptr) return;

	// 移動量の取得
	D3DXVECTOR3 moveWk = m_pMove->Get();

	// 移動量の設定
	moveWk.x = sinf(fAngleBase + dir) * fSpeed;
	moveWk.z = cosf(fAngleBase + dir) * fSpeed;

	// 移動量の設定
	m_pMove->Set(moveWk);
}

//===================================================
// ジャンプ
//===================================================
void CEnemyMovement::Jump(const float fHeight)
{
	// nullだったら処理しない
	if (m_pMove == nullptr) return;

	m_pMove->Jump(fHeight);
}

//===================================================
// 位置の更新処理
//===================================================
void CEnemyMovement::UpdatePosition(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld)
{
	// nullだったら処理しない
	if (m_pMove == nullptr) return;

	// 移動量の減衰
	m_pMove->SetInertia3D(m_fInertia);

	if (*pPosOld != nullptr)
	{
		// 前回の位置の設定
		*pPosOld = *pPos;
	}

	if (*pPos != nullptr)
	{
		// 移動量の取得
		*pPos += m_pMove->Get();
	}
}

//===================================================
// 移動量の取得
//===================================================
D3DXVECTOR3 CEnemyMovement::Get(void) const
{
	// nullだったら処理しない
	if (m_pMove == nullptr) return Const::VEC3_NULL;

	return m_pMove->Get();
}

//===================================================
// 移動量の設定
//===================================================
void CEnemyMovement::Set(const D3DXVECTOR3 move)
{
	// nullだったら処理しない
	if (m_pMove == nullptr) return;

	// 移動量の設定
	m_pMove->Set(move);
}

//===================================================
// 重力の設定
//===================================================
void CEnemyMovement::Gravity(const float Gravity)
{
	// nullだったら処理しない
	if (m_pMove == nullptr) return;

	// 重力の設定
	m_pMove->Gravity(Gravity);
}
