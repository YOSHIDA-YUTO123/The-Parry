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
#include"Collider.h"

using namespace Const; // 名前空間Constを使用
using namespace math; // 名前空間を使用
using namespace std; // 名前空間stdを処理使用

constexpr float HALF_VALUE = 0.5f; // 半分

//************************************************
// 静的メンバ変数宣言
//************************************************
unique_ptr<CCollisionAABB> CCollisionAABB::m_pAABB = nullptr;		// 当たり判定AABB
unique_ptr<CCollisionSphere> CCollisionSphere::m_pSphere = nullptr; // 当たり判定Sphere
unique_ptr<CCollisionFOV> CCollisionFOV::m_pFOV = nullptr;			// 当たり判定視界

//================================================
// コンストラクタ
//================================================
CCollision::CCollision(TYPE type) : m_type(type)
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
// 当たり判定の生成処理
//================================================
CCollision* CCollision::Create(const D3DXVECTOR3 pos, const TYPE type)
{
	// 当たり判定のポインタ
	CCollision* pCollision = nullptr;

	//// 種類の遷移
	//switch (type)
	//{
	//case TYPE::TYPE_AABB:
	//	pCollision = new CCollisionAABB;
	//	break;
	//case TYPE::TYPE_SPHERE:
	//	pCollision = new CCollisionSphere;
	//	break;
	//case TYPE::TYPE_FOV:
	//	pCollision = new CCollisionFOV;
	//	break;
	//default:
	//	break;
	//}

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
CCollisionAABB::CCollisionAABB() : CCollision(TYPE::TYPE_AABB)
{
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
void CCollisionAABB::Create(void)
{
	// nullだったら自分の生成(1つ以上作らない)
	if (m_pAABB == nullptr)
	{
		// AABBの作成処理
		m_pAABB.reset(new CCollisionAABB());
	}
}

//================================================
// AABB対AABBの当たり判定
//================================================
bool CCollisionAABB::Collision(CColliderAABB* pMyBox, CColliderAABB* pTargetBox)
{
	// 自分の位置と大きさの取得
	D3DXVECTOR3 pos = pMyBox->GetPos();

	// 相手の位置と大きさの取得
	D3DXVECTOR3 TargetPos = pTargetBox->GetPos();

	// 自分のデータの取得
	CColliderAABB::Data myData = pMyBox->GetData();

	// 相手のデータの取得
	CColliderAABB::Data targetData = pTargetBox->GetData();

	// ターゲットの大きさ
	D3DXVECTOR3 TargetSize = targetData.Size;
	D3DXVECTOR3 TargetPosOld = targetData.posOld;

	// 自分の前回の位置
	D3DXVECTOR3 posOldMin = myData.posOld - (myData.Size * HALF_VALUE);
	D3DXVECTOR3 posOldMax = myData.posOld + (myData.Size * HALF_VALUE);

	// 自分の位置
	D3DXVECTOR3 posMin = pos - (myData.Size * HALF_VALUE);
	D3DXVECTOR3 posMax = pos + (myData.Size * HALF_VALUE);

	// ターゲットの前回の位置(tはターゲットのt)
	D3DXVECTOR3 tPosOldMin = TargetPosOld - (TargetSize * HALF_VALUE);
	D3DXVECTOR3 tPosOldMax = TargetPosOld + (TargetSize * HALF_VALUE);

	// ターゲットの位置(tはターゲットのt)
	D3DXVECTOR3 tPosMin = TargetPos - (TargetSize * HALF_VALUE);
	D3DXVECTOR3 tPosMax = TargetPos + (TargetSize * HALF_VALUE);

	bool bHit = false;

	if (posOldMin.y <= tPosOldMax.y && posOldMax.y >= tPosOldMin.y)
	{
		// 左右のめり込み判定
		if (posMin.z < tPosMax.z && posMax.z > tPosMin.z)
		{
			// xが左から右にめり込んだ	
			if (posOldMax.x < tPosOldMin.x && posMax.x > tPosMin.x)
			{
				bHit = true;

				//pos.x = otherPos.x - otherSize.x * HALF_VALUE * g_Block[nCntBlock].Scal.x - Size.x * HALF_VALUE - 0.1f;
			}
			// xが右から左にめり込んだ	
			else if (posOldMin.x > tPosOldMax.x && posMin.x < tPosMax.x)
			{
				bHit = true;

				//pos.x = otherPos.x + otherSize.x * HALF_VALUE * g_Block[nCntBlock].Scal.x + Size.x * HALF_VALUE + 0.1f;
			}
		}

		// 前と後ろの判定
		if (posMin.x < tPosMax.x && posMax.x > tPosMin.x)
		{
			// zが前方からめり込んだ
			if (posOldMax.z < tPosOldMin.z && posMax.z > tPosMin.z)
			{
				bHit = true;

				//pos.z = otherPos.z - otherSize.z * HALF_VALUE * g_Block[nCntBlock].Scal.z - Size.z * HALF_VALUE - 0.1f;
			}
			// zが後方からめり込んだ
			else if (posOldMin.z > tPosOldMax.z && posMin.z < tPosMax.z)
			{
				bHit = true;

				//pos.z = otherPos.z + otherSize.z * HALF_VALUE * g_Block[nCntBlock].Scal.z + Size.z * HALF_VALUE + HALF_VALUE;
			}
		}
	}

	//if (pos.x - m_Size.x * HALF_VALUE <= otherPos.x + otherSize.x * HALF_VALUE
	//	&& pos.x + m_Size.x * HALF_VALUE >= otherPos.x - otherSize.x * HALF_VALUE)
	//{
	//	if (pos.z - m_Size.z * HALF_VALUE <= otherPos.z + otherSize.z * HALF_VALUE
	//		&& pos.z + m_Size.z * HALF_VALUE >= otherPos.z - otherSize.z * HALF_VALUE)
	//	{
	//		// 上から下
	//		if (m_posOld.y >= otherPos.y + otherSize.y
	//			&& pos.y < otherPos.y + otherSize.y)
	//		{
	//			bHit = true;

	//			//bLanding = true;
	//			//pos.y = otherPos.y + otherSize.y * g_Block[nCntBlock].Scal.y - Size.y;
	//			//pMove->y = 0.0f;
	//		}
	//		// 下から上
	//		else if (m_posOld.y + m_Size.y * HALF_VALUE <= otherPos.y - otherSize.y * HALF_VALUE
	//			&& pos.y + m_Size.y * HALF_VALUE > otherPos.y - otherSize.y * HALF_VALUE)
	//		{
	//			bHit = true;

	//			//pos.y = m_posOld.y;
	//			//pMove->y = 0.0f;
	//		}

	//	}
	//}
	
	return bHit;
}

//================================================
// コンストラクタ
//================================================
CCollisionSphere::CCollisionSphere() : CCollision(TYPE::TYPE_SPHERE)
{
}

//================================================
// デストラクタ
//================================================
CCollisionSphere::~CCollisionSphere()
{
}

//================================================
// 生成処理
//================================================
void CCollisionSphere::Create(void)
{
	// nullだったら自分の生成(1つ以上作らない)
	if (m_pSphere == nullptr)
	{
		// sphereの作成処理
		m_pSphere.reset(new CCollisionSphere());
	}
}

//================================================
// 当たり判定(円vs円)
//================================================
bool CCollisionSphere::Collision(CColliderSphere* myCollider, CColliderSphere* otherCollider)
{
	// 位置の取得
	D3DXVECTOR3 pos = myCollider->GetPos();

	// 相手の位置
	D3DXVECTOR3 otherPos = otherCollider->GetPos();

	// 相手の半径
	float fOtherRadius = otherCollider->GetRadius();

	// 自分の半径
	float fMyRadius = myCollider->GetRadius();

	// 差分を求める
	D3DXVECTOR3 diff = otherPos - pos;

	// 距離を求める
	float fDistance = (diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z);

	// 半径足す
	float fRadius = fMyRadius + fOtherRadius;

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
CCollisionFOV::CCollisionFOV() : CCollision(TYPE::TYPE_FOV)
{
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
void CCollisionFOV::Create(void)
{
	// nullだったら自分の生成(1つ以上作らない)
	if (m_pFOV == nullptr)
	{
		// 視界の作成処理
		m_pFOV.reset(new CCollisionFOV());
	}
}

////================================================
//// コライダーの作成
////================================================
//CCollisionFOV CCollisionFOV::CreateCollider(const D3DXVECTOR3 pos, const float fAngle, const float fAngleLeft, const float fAngleRight)
//{
//	//CCollisionFOV out;
//
//	//// 設定処理
//	//out.SetPos(pos);
//	//out.m_fNowAngle = fAngle;
//	//out.m_fAngleLeft = fAngleLeft;
//	//out.m_fAngleRight = fAngleRight;
//
//	//return out;
//}

//================================================
// 視界の判定
//================================================
bool CCollisionFOV::Collision(const D3DXVECTOR3 otherpos, CColliderFOV* pFOV)
{
	// 位置の取得
	D3DXVECTOR3 objectPos = pFOV->GetPos();

	// 前方までのベクトル
	D3DXVECTOR3 vecFront = GetVector(otherpos, objectPos);

	// データの取得
	CColliderFOV::Data data = pFOV->GetData();

	D3DXVECTOR3 LeftPos; // 左の位置

	// 左側の視界の端の位置を求める
	LeftPos.x = objectPos.x + sinf(data.fNowAngle + data.fAngleLeft) * data.fLength;
	LeftPos.y = 0.0f;
	LeftPos.z = objectPos.z + cosf(data.fNowAngle + data.fAngleLeft) * data.fLength;

	// 左側の視界のベクトルの作成
	D3DXVECTOR3 VecLeft = GetVector(LeftPos, objectPos);

	D3DXVECTOR3 RightPos; // 右の位置

	// 右側の視界の端の位置を求める
	RightPos.x = objectPos.x + sinf(data.fNowAngle + data.fAngleRight) * data.fLength;
	RightPos.y = 0.0f;
	RightPos.z = objectPos.z + cosf(data.fNowAngle + data.fAngleRight) * data.fLength;

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

//================================================
// コンストラクタ
//================================================
CCollisionManager::CCollisionManager()
{
}

//================================================
// デストラクタ
//================================================
CCollisionManager::~CCollisionManager()
{
}

//================================================
// すべての判定の生成
//================================================
void CCollisionManager::CreateAll(void)
{
	// AABBの作成処理
	CCollisionAABB::Create();

	// sphereの作成
	CCollisionSphere::Create();

	// 視界判定の生成
	CCollisionFOV::Create();
}

//================================================
// すべての判定の破棄
//================================================
void CCollisionManager::Uninit(void)
{

}
