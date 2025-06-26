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
	GetVector = VEC3_NULL;
}

//================================================
// デストラクタ
//================================================
CCollision::~CCollision()
{
}

//================================================
// 要素の設定処理
//================================================
void CCollision::SetElement(const D3DXVECTOR3 pos, const D3DXVECTOR3 Size)
{
	m_pos = pos;
	GetVector = Size;
}

//================================================
// コンストラクタ
//================================================
CCollisionAABB::CCollisionAABB()
{
	m_posOld = VEC3_NULL;
}

//================================================
// デストラクタ
//================================================
CCollisionAABB::~CCollisionAABB()
{
}

//================================================
// AABB対AABBの当たり判定
//================================================
bool CCollisionAABB::Collision(CCollisionAABB* other)
{
	// 自分の位置と大きさの取得
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 Size = GetSize();

	// 相手の位置と大きさの取得
	D3DXVECTOR3 otherPos = other->GetPosition();
	D3DXVECTOR3 otherSize = other->GetSize();

	bool bHit = false;

	const float HALF_VALUE = 0.5f;

	if (m_posOld.y <= otherPos.y + otherSize.y
		&& m_posOld.y + Size.y >= otherPos.y)
	{
		// 左右のめり込み判定
		if (pos.z - Size.z * HALF_VALUE < otherPos.z + otherSize.z * HALF_VALUE
			&& pos.z + Size.z * HALF_VALUE > otherPos.z - otherSize.z * HALF_VALUE)
		{
			// xが左から右にめり込んだ	
			if (m_posOld.x + Size.x * HALF_VALUE < otherPos.x - otherSize.x * HALF_VALUE
				&& pos.x + Size.x * HALF_VALUE > otherPos.x - otherSize.x * HALF_VALUE)
			{
				bHit = true;

				//pos.x = otherPos.x - otherSize.x * HALF_VALUE * g_Block[nCntBlock].Scal.x - Size.x * HALF_VALUE - 0.1f;
			}
			// xが右から左にめり込んだ	
			else if (m_posOld.x - Size.x * HALF_VALUE > otherPos.x + otherSize.x * HALF_VALUE
				&& pos.x - Size.x * HALF_VALUE < otherPos.x + otherSize.x * HALF_VALUE)
			{
				bHit = true;

				//pos.x = otherPos.x + otherSize.x * HALF_VALUE * g_Block[nCntBlock].Scal.x + Size.x * HALF_VALUE + 0.1f;
			}
		}

		// 前と後ろの判定
		if (pos.x - Size.x * HALF_VALUE < otherPos.x + otherSize.x * HALF_VALUE
			&& pos.x + Size.x * HALF_VALUE > otherPos.x - otherSize.x * HALF_VALUE)
		{
			// zが前方からめり込んだ
			if (m_posOld.z + Size.z * HALF_VALUE < otherPos.z - otherSize.z * HALF_VALUE
				&& pos.z + Size.z * HALF_VALUE > otherPos.z - otherSize.z * HALF_VALUE)
			{
				bHit = true;

				//pos.z = otherPos.z - otherSize.z * HALF_VALUE * g_Block[nCntBlock].Scal.z - Size.z * HALF_VALUE - 0.1f;
			}
			// zが後方からめり込んだ
			else if (m_posOld.z - Size.z * HALF_VALUE > otherPos.z + otherSize.z * HALF_VALUE
				&& pos.z - Size.z * HALF_VALUE < otherPos.z + otherSize.z * HALF_VALUE)
			{
				bHit = true;

				//pos.z = otherPos.z + otherSize.z * HALF_VALUE * g_Block[nCntBlock].Scal.z + Size.z * HALF_VALUE + HALF_VALUE;
			}
		}
	}

	if (pos.x - Size.x * HALF_VALUE <= otherPos.x + otherSize.x * HALF_VALUE
		&& pos.x + Size.x * HALF_VALUE >= otherPos.x - otherSize.x * HALF_VALUE)
	{
		if (pos.z - Size.z * HALF_VALUE <= otherPos.z + otherSize.z * HALF_VALUE
			&& pos.z + Size.z * HALF_VALUE >= otherPos.z - otherSize.z * HALF_VALUE)
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
			else if (m_posOld.y + Size.y * HALF_VALUE <= otherPos.y - otherSize.y * HALF_VALUE
				&& pos.y + Size.y * HALF_VALUE > otherPos.y - otherSize.y * HALF_VALUE)
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
// 当たり判定(円vs円)
//================================================
bool CCollisionSphere::Collision(CCollisionSphere* other)
{
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 otherPos = other->GetPosition();

	D3DXVECTOR3 diff = otherPos - pos;

	float fDistance = (diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z);

	float fRadius = m_fRadius + other->m_fRadius;

	fRadius = fRadius * fRadius;

	if (fDistance <= fRadius)
	{
		return true;
	}

	return false;
}
