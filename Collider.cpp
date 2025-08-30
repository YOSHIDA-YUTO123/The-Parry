//================================================
//
// コライダー [collider.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "Collider.h"
#include"object.h"
#include"math.h"

using namespace Const; // 名前空間Constを使用
using namespace std;   // 名前空間stdを処理使用
using namespace math;  // 名前空間mathを処理使用

//================================================
// コンストラクタ
//================================================
CCollider::CCollider()
{
}

//================================================
// デストラクタ
//================================================
CCollider::~CCollider()
{
}

//================================================
// コンストラクタ
//================================================
CColliderAABB::CColliderAABB()
{
	// 値のクリア
	memset(&m_Data, NULL, sizeof(m_Data));
}

//================================================
// デストラクタ
//================================================
CColliderAABB::~CColliderAABB()
{
}

//================================================
// コライダーの作成処理
//================================================
unique_ptr<CColliderAABB> CColliderAABB::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 posOld, const D3DXVECTOR3 Size)
{
	// AABBの生成
	unique_ptr<CColliderAABB> pAABB = make_unique<CColliderAABB>();

	// 設定処理
	pAABB->m_pos = pos;
	pAABB->m_Data.Size = Size;
	pAABB->m_Data.posOld = posOld;

	return pAABB;
}

//================================================
// コライダーの生成処理
//================================================
CColliderAABB CColliderAABB::CreateCollider(const D3DXVECTOR3 pos, const D3DXVECTOR3 posOld, const D3DXVECTOR3 Size)
{
	// AABB
	CColliderAABB aabb;

	// 設定処理
	aabb.m_pos = pos;
	aabb.m_Data.posOld = posOld;
	aabb.m_Data.Size = Size;

	return aabb;
}

//================================================
// データの更新処理
//================================================
void CColliderAABB::UpdateData(const D3DXVECTOR3 pos, const D3DXVECTOR3 posOld)
{
	m_pos = pos;
	m_Data.posOld = posOld;
}

////================================================
//// コンストラクタ
////================================================
//CColliderSphere::CColliderSphere()
//{
//	m_fRadius = NULL;
//}
//
////================================================
//// デストラクタ
////================================================
//CColliderSphere::~CColliderSphere()
//{
//}
//
////================================================
//// コライダーの作成処理
////================================================
//CColliderSphere* CColliderSphere::CreateCollider(const float fRadius)
//{
//	// コライダーの作成処理
//	CColliderSphere* pSphere;
//
//	// sphereの生成
//	pSphere = new CColliderSphere;
//
//	pSphere->m_fRadius = fRadius;
//
//	return pSphere;
//}
//
////================================================
//// コンストラクタ
////================================================
//CColliderFOV::CColliderFOV()
//{
//	m_fAngleLeft = NULL;
//	m_fAngleRight = NULL;
//	m_fLength = NULL;
//	m_fNowAngle = NULL;
//}
//
////================================================
//// デストラクタ
////================================================
//CColliderFOV::~CColliderFOV()
//{
//
//}
//
////================================================
//// コライダーの生成
////================================================
//CColliderFOV* CColliderFOV::CreateCollider(const float fAngle, const float fAngleLeft, const float fAngleRight,const float fLength)
//{
//	// コライダーの作成処理
//	CColliderFOV* pFOV;
//
//	// 視界の生成
//	pFOV = new CColliderFOV;
//	pFOV->m_fAngleLeft = fAngleLeft;
//	pFOV->m_fAngleRight = fAngleRight;
//	pFOV->m_fLength = fLength;
//	pFOV->m_fNowAngle = fAngle;
//
//	return pFOV;
//}

//================================================
// コンストラクタ
//================================================
CColliderSphere::CColliderSphere()
{
	m_fRadius = NULL;
}

//================================================
// デストラクタ
//================================================
CColliderSphere::~CColliderSphere()
{
}

//================================================
// 円のコライダーの作成
//================================================
unique_ptr<CColliderSphere> CColliderSphere::Create(const D3DXVECTOR3 pos, const float fRadius)
{
	// sphereの生成
	unique_ptr<CColliderSphere> pSphere = make_unique<CColliderSphere>();

	// 設定処理
	pSphere->m_pos = pos;
	pSphere->m_fRadius = fRadius;

	return pSphere;
}

//================================================
// コライダーの作成処理
//================================================
CColliderSphere CColliderSphere::CreateCollider(const D3DXVECTOR3 pos, const float fRadius)
{
	// コライダー
	CColliderSphere sphere;

	// 設定処理
	sphere.m_pos = pos;
	sphere.m_fRadius = fRadius;

	return sphere;
}

//================================================
// コンストラクタ
//================================================
CColliderFOV::CColliderFOV()
{
	// メモリのクリア
	memset(&m_Data, NULL, sizeof(m_Data));
}

//================================================
// デストラクタ
//================================================
CColliderFOV::~CColliderFOV()
{
}

//================================================
// 生成処理
//================================================
unique_ptr<CColliderFOV> CColliderFOV::Create(const D3DXVECTOR3 pos, const float fAngle, const float fAngleLeft, const float fAngleRight, const float fLength)
{
	// 視界の生成
	unique_ptr<CColliderFOV> pFOV = make_unique<CColliderFOV>();

	// 設定処理
	pFOV->m_pos = pos;
	pFOV->m_Data.fAngleLeft = fAngleLeft;
	pFOV->m_Data.fAngleRight = fAngleRight;
	pFOV->m_Data.fLength = fLength;
	pFOV->m_Data.fNowAngle = fAngle;

	return pFOV;
}

//================================================
// コライダーの生成
//================================================
CColliderFOV CColliderFOV::CreateCollider(const D3DXVECTOR3 pos, const float fAngle, const float fAngleLeft, const float fAngleRight, const float fLength)
{
	// 視界の
	CColliderFOV FOV = {};

	// 設定処理
	FOV.m_pos = pos;
	FOV.m_Data.fAngleLeft = fAngleLeft;
	FOV.m_Data.fAngleRight = fAngleRight;
	FOV.m_Data.fLength = fLength;
	FOV.m_Data.fNowAngle = fAngle;

	return FOV;
}

//================================================
// コンストラクタ
//================================================
CColliderCapsule::CColliderCapsule()
{
	ZeroMemory(&m_Data, sizeof(m_Data));
}

//================================================
// デストラクタ
//================================================
CColliderCapsule::~CColliderCapsule()
{
}

//================================================
// 生成処理
//================================================
unique_ptr<CColliderCapsule> CColliderCapsule::Create(const D3DXVECTOR3 StartPos, const D3DXVECTOR3 EndPos, const float fRadius, const int nID)
{
	// カプセルの生成
	auto pCapsule = make_unique<CColliderCapsule>();

	// 設定処理
	pCapsule->m_Data.StartPos = StartPos;
	pCapsule->m_Data.EndPos = EndPos;
	pCapsule->m_Data.fRadius = fRadius;
	pCapsule->m_Data.nID = nID;
	return pCapsule;
}

//================================================
// コライダーの作成処理
//================================================
CColliderCapsule CColliderCapsule::CreateCollider(const D3DXVECTOR3 StartPos, const D3DXVECTOR3 EndPos, const float fRadius)
{
	CColliderCapsule capsule = {};

	// 設定処理
	capsule.m_Data.StartPos = StartPos;
	capsule.m_Data.EndPos = EndPos;
	capsule.m_Data.fRadius = fRadius;

	return capsule;
}

//================================================
// コライダーの更新
//================================================
void CColliderCapsule::UpdateData(const Data data)
{
	m_Data = data;
}
