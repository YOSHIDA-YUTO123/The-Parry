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

using namespace Const; // 名前空間Constを使用
using namespace std;   // 名前空間stdを処理使用

////================================================
//// コンストラクタ
////================================================
//CCollider::CCollider()
//{
//}
//
////================================================
//// デストラクタ
////================================================
//CCollider::~CCollider()
//{
//}
//
////================================================
//// コンストラクタ
////================================================
//CColliderAABB::CColliderAABB()
//{
//	// 値のクリア
//	memset(&m_Data, NULL, sizeof(m_Data));
//}
//
////================================================
//// デストラクタ
////================================================
//CColliderAABB::~CColliderAABB()
//{
//}
//
////================================================
//// コライダーの作成処理
////================================================
//CColliderAABB* CColliderAABB::CreateCollider(const D3DXVECTOR3 Size)
//{
//	// コライダーの作成処理
//	CColliderAABB* pAABB;
//
//	// AABBの生成
//	pAABB = new CColliderAABB;
//
//	pAABB->m_Data.Size = Size;
//
//	return pAABB;
//}
//
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
