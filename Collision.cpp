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
#include"effect.h"

using namespace math; // 名前空間を使用

//================================================
// コンストラクタ
//================================================
CCollision::CCollision()
{
	m_pos = VEC3_NULL;
	m_type = TYPE::TYPE_AABB;
}

//================================================
// デストラクタ
//================================================
CCollision::~CCollision()
{
}

//================================================
// 当たり判定の生成処理
//================================================
CCollision* CCollision::Create(const D3DXVECTOR3 pos, const TYPE type)
{
	// 当たり判定のポインタ
	CCollision* pCollision = nullptr;

	// 種類の遷移
	switch (type)
	{
	case TYPE::TYPE_AABB:
		pCollision = new CCollisionAABB;
		break;
	case TYPE::TYPE_SPHERE:
		pCollision = new CCollisionSphere;
		break;
	case TYPE::TYPE_FOV:
		pCollision = new CCollisionFOV;
		break;
	default:
		break;
	}

	// 位置の設定
	pCollision->m_pos = pos;

	// 種類を代入
	pCollision->m_type = type;

	// 判定を返す
	return pCollision;
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
CCollisionAABB* CCollisionAABB::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 Size)
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
std::unique_ptr<CCollisionSphere> CCollisionSphere::Create(const D3DXVECTOR3 pos, const float fRadius)
{
	// ポインタの生成
	std::unique_ptr<CCollisionSphere> out = std::make_unique<CCollisionSphere>();

	// 設定処理
	out->SetPos(pos);
	out->m_fRadius = fRadius;
	return out;
}

//================================================
// コライダーの生成処理
//================================================
CCollisionSphere CCollisionSphere::CreateCollider(const D3DXVECTOR3 pos, const float fRadius)
{
	// アウトプット用
	CCollisionSphere sphere;

	// 判定に必要な情報の設定
	sphere.SetPos(pos);
	sphere.m_fRadius = fRadius;

	// コライダーを返す
	return sphere;
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

//================================================
// コンストラクタ
//================================================
CCollisionFOV::CCollisionFOV()
{
	m_fLength = NULL;
}

//================================================
// デストラクタ
//================================================
CCollisionFOV::~CCollisionFOV()
{
}

//================================================
// 視界の生成
//================================================
std::unique_ptr<CCollisionFOV> CCollisionFOV::Create(const D3DXVECTOR3 pos, const float fLength)
{
	// ポインタの生成
	std::unique_ptr<CCollisionFOV> out = std::make_unique<CCollisionFOV>();

	// 設定処理
	out->SetPos(pos);
	out->m_fLength = fLength;
	return out;
}

////================================================
//// 生成処理
////================================================
//CCollisionFOV* CCollisionFOV::Create(const D3DXVECTOR3 pos, const float fLength)
//{
//	//// 視界判定の生成
//	//CCollisionFOV* pCollision = new CCollisionFOV;
//
//	//pCollision->SetPos(pos);
//	//pCollision->m_fLength = fLength;
//
//	//return pCollision;
//}
//
//================================================
// 視界の判定
//================================================
bool CCollisionFOV::Collision(const D3DXVECTOR3 pos,const float fAngle, const float fAngleLeft, const float fAngleRight)
{
	// 位置の取得
	D3DXVECTOR3 objectPos = GetPosition();

	// 前方までのベクトル
	D3DXVECTOR3 vecFront = GetVector(pos, objectPos);

	D3DXVECTOR3 LeftPos; // 左の位置

	// 左側の視界の端の位置を求める
	LeftPos.x = objectPos.x + sinf(fAngle + fAngleLeft) * m_fLength;
	LeftPos.y = 0.0f;
	LeftPos.z = objectPos.z + cosf(fAngle + fAngleLeft) * m_fLength;

	// 左側の視界のベクトルの作成
	D3DXVECTOR3 VecLeft = GetVector(LeftPos, objectPos);

	D3DXVECTOR3 RightPos; // 右の位置

	// 右側の視界の端の位置を求める
	RightPos.x = objectPos.x + sinf(fAngle + fAngleRight) * m_fLength;
	RightPos.y = 0.0f;
	RightPos.z = objectPos.z + cosf(fAngle + fAngleRight) * m_fLength;

	// 右側の視界のベクトルの作成
	D3DXVECTOR3 VecRight = GetVector(RightPos, objectPos);

	D3DXVECTOR3 Cross0,Cross1; // 判定用外積ベクトル

#ifdef _DEBUG

	CEffect3D::Create(RightPos, 50.0f, VEC3_NULL, WHITE, 10);
	CEffect3D::Create(LeftPos, 50.0f, VEC3_NULL, WHITE, 10);
#endif // _DEBUG

	// 外積を出す
	D3DXVec3Cross(&Cross0, &VecLeft, &vecFront);
	D3DXVec3Cross(&Cross1, &vecFront, &VecRight);

	// 2本とも上向きだったら
	if (Cross0.y >= 0.0f && Cross1.y >= 0.0f)
	{
		return true;
	}

	return false;
}
