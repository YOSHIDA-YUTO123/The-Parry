//================================================
//
// 当たり判定 [collision.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "Collision.h"
#include"math.h"

using namespace math; // 名前空間を使用

//================================================
// コンストラクタ
//================================================
CCollision::CCollision()
{
	m_pos = VEC3_NULL;
}

//================================================
// デストラクタ
//================================================
CCollision::~CCollision()
{
}

//================================================
// コンストラクタ
//================================================
CCollisionAABB::CCollisionAABB()
{
	m_Size = VEC3_NULL;
	m_posOld = VEC3_NULL;
}

//================================================
// デストラクタ
//================================================
CCollisionAABB::~CCollisionAABB()
{
}

//================================================
// AABBの作成処理
//================================================
CCollisionAABB* CCollisionAABB::CreateAABB(const D3DXVECTOR3 pos, const D3DXVECTOR3 Size)
{
	// AABBの作成処理
	CCollisionAABB* pAABB = new CCollisionAABB;

	// 設定処理
	pAABB->SetPos(pos);
	pAABB->m_Size = Size;

	return pAABB;
}

//================================================
// AABB対AABBの当たり判定
//================================================
bool CCollisionAABB::Collision(CCollisionAABB* other)
{
	// 自分の位置と大きさの取得
	D3DXVECTOR3 pos = GetPosition();

	// 相手の位置と大きさの取得
	D3DXVECTOR3 otherPos = other->GetPosition();
	D3DXVECTOR3 otherSize = other->m_Size;

	bool bHit = false;

	const float HALF_VALUE = 0.5f;

	if (m_posOld.y <= otherPos.y + otherSize.y
		&& m_posOld.y + m_Size.y >= otherPos.y)
	{
		// 左右のめり込み判定
		if (pos.z - m_Size.z * HALF_VALUE < otherPos.z + otherSize.z * HALF_VALUE
			&& pos.z + m_Size.z * HALF_VALUE > otherPos.z - otherSize.z * HALF_VALUE)
		{
			// xが左から右にめり込んだ	
			if (m_posOld.x + m_Size.x * HALF_VALUE < otherPos.x - otherSize.x * HALF_VALUE
				&& pos.x + m_Size.x * HALF_VALUE > otherPos.x - otherSize.x * HALF_VALUE)
			{
				bHit = true;

				//pos.x = otherPos.x - otherSize.x * HALF_VALUE * g_Block[nCntBlock].Scal.x - Size.x * HALF_VALUE - 0.1f;
			}
			// xが右から左にめり込んだ	
			else if (m_posOld.x - m_Size.x * HALF_VALUE > otherPos.x + otherSize.x * HALF_VALUE
				&& pos.x - m_Size.x * HALF_VALUE < otherPos.x + otherSize.x * HALF_VALUE)
			{
				bHit = true;

				//pos.x = otherPos.x + otherSize.x * HALF_VALUE * g_Block[nCntBlock].Scal.x + Size.x * HALF_VALUE + 0.1f;
			}
		}

		// 前と後ろの判定
		if (pos.x - m_Size.x * HALF_VALUE < otherPos.x + otherSize.x * HALF_VALUE
			&& pos.x + m_Size.x * HALF_VALUE > otherPos.x - otherSize.x * HALF_VALUE)
		{
			// zが前方からめり込んだ
			if (m_posOld.z + m_Size.z * HALF_VALUE < otherPos.z - otherSize.z * HALF_VALUE
				&& pos.z + m_Size.z * HALF_VALUE > otherPos.z - otherSize.z * HALF_VALUE)
			{
				bHit = true;

				//pos.z = otherPos.z - otherSize.z * HALF_VALUE * g_Block[nCntBlock].Scal.z - Size.z * HALF_VALUE - 0.1f;
			}
			// zが後方からめり込んだ
			else if (m_posOld.z - m_Size.z * HALF_VALUE > otherPos.z + otherSize.z * HALF_VALUE
				&& pos.z - m_Size.z * HALF_VALUE < otherPos.z + otherSize.z * HALF_VALUE)
			{
				bHit = true;

				//pos.z = otherPos.z + otherSize.z * HALF_VALUE * g_Block[nCntBlock].Scal.z + Size.z * HALF_VALUE + HALF_VALUE;
			}
		}
	}

	if (pos.x - m_Size.x * HALF_VALUE <= otherPos.x + otherSize.x * HALF_VALUE
		&& pos.x + m_Size.x * HALF_VALUE >= otherPos.x - otherSize.x * HALF_VALUE)
	{
		if (pos.z - m_Size.z * HALF_VALUE <= otherPos.z + otherSize.z * HALF_VALUE
			&& pos.z + m_Size.z * HALF_VALUE >= otherPos.z - otherSize.z * HALF_VALUE)
		{
			// 上から下
			if (m_posOld.y >= otherPos.y + otherSize.y
				&& pos.y < otherPos.y + otherSize.y)
			{
				bHit = true;

				//bLanding = true;
				//pos.y = otherPos.y + otherSize.y * g_Block[nCntBlock].Scal.y - Size.y;
				//pMove->y = 0.0f;
			}
			// 下から上
			else if (m_posOld.y + m_Size.y * HALF_VALUE <= otherPos.y - otherSize.y * HALF_VALUE
				&& pos.y + m_Size.y * HALF_VALUE > otherPos.y - otherSize.y * HALF_VALUE)
			{
				bHit = true;

				//pos.y = m_posOld.y;
				//pMove->y = 0.0f;
			}

		}
	}
	
	return bHit;
}

//================================================
// コンストラクタ
//================================================
CCollisionSphere::CCollisionSphere()
{
	m_fRadius = NULL;
}

//================================================
// デストラクタ
//================================================
CCollisionSphere::~CCollisionSphere()
{
}

//================================================
// 円の判定の生成処理
//================================================
CCollisionSphere* CCollisionSphere::CreateSphere(const D3DXVECTOR3 pos, const float fRadius)
{
	// 円の作成処理
	CCollisionSphere* pSphere = new CCollisionSphere;

	// 設定処理
	pSphere->SetPos(pos);
	pSphere->m_fRadius = fRadius;

	return pSphere;
}

//================================================
// 当たり判定(円vs円)
//================================================
bool CCollisionSphere::Collision(CCollisionSphere* other)
{
	// 位置の取得
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 otherPos = other->GetPosition();

	// 差分を求める
	D3DXVECTOR3 diff = otherPos - pos;

	// 距離を求める
	float fDistance = (diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z);

	// 半径足す
	float fRadius = m_fRadius + other->m_fRadius;

	// 半径を2乗する
	fRadius = fRadius * fRadius;

	// 距離が半径以下だったら当たっている
	if (fDistance <= fRadius)
	{
		return true;
	}

	return false;
}
