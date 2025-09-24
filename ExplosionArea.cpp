//==============================================
//
// 爆発のエリア [ExplosionArea.cpp]
// Author:YUTO YOSHIDA
//
//==============================================

//**********************************************
// インクルードファイル
//**********************************************
#include "ExplosionArea.h"
#include"Collider.h"
#include"Collision.h"
#include"obstaclemanager.h"

using namespace std;	// 名前空間stdの使用
using namespace Const;	// 名前空間Constの使用

//**********************************************
// 定数宣言
//**********************************************
constexpr float RANGE = 450.0f; // 爆発の範囲
constexpr int MAX_LIFE = 30;	// 寿命

//==============================================
// コンストラクタ
//==============================================
CExplosionArea::CExplosionArea()
{
	m_pSphere = nullptr;
	m_pos = VEC3_NULL;
	m_nLife = NULL;
}

//==============================================
// デストラクタ
//==============================================
CExplosionArea::~CExplosionArea()
{
}

//==============================================
// 生成処理
//==============================================
CExplosionArea* CExplosionArea::Create(const D3DXVECTOR3 pos)
{
	CExplosionArea* pArea = new CExplosionArea;

	pArea->m_pos = pos;

	// 初期化処理
	if (FAILED(pArea->Init()))
	{
		pArea->Uninit();
		pArea = nullptr;
		return nullptr;
	}

	return pArea;
}

//==============================================
// 初期化処理
//==============================================
HRESULT CExplosionArea::Init(void)
{
	// 円のコライダーの生成
	m_pSphere = make_unique<CColliderSphere>();

	// 円の判定の生成
	m_pSphere = CColliderSphere::Create(m_pos, RANGE);

	m_nLife = MAX_LIFE;

	// 障害物マネージャーの取得
	auto pObstacleManager = CObstacleManager::GetInstance();

	if (pObstacleManager != nullptr)
	{
		// リストに追加
		pObstacleManager->AddExplosion(this);
	}
	return S_OK;
}

//==============================================
// 終了処理
//==============================================
void CExplosionArea::Uninit(void)
{
	m_pSphere.reset();

	// 自分自身の破棄
	CObject::Release();
}

//==============================================
// 更新処理
//==============================================
void CExplosionArea::Update(void)
{
	// 寿命を減らす
	m_nLife--;

	// 寿命が尽きたら
	if (m_nLife <= 0)
	{
		// 障害物マネージャーの取得
		auto pObstacleManager = CObstacleManager::GetInstance();
		
		if (pObstacleManager != nullptr)
		{
			// 破棄
			pObstacleManager->EraseExplosion(this);
		}
	}
}

//==============================================
// 描画処理
//==============================================
void CExplosionArea::Draw(void)
{
	
}

//==============================================
// 当たり判定
//==============================================
bool CExplosionArea::Collision(CColliderCapsule* pCapsule,D3DXVECTOR3 *pPos)
{
	// nullだったら処理しない
	if (pCapsule == nullptr) return false;

	// 当たっている
	if (CCollisionCapsule::CollisionSphere(pCapsule, m_pSphere.get()))
	{
		if (pPos != nullptr)
		{
			// 自分の位置の設定
			*pPos = m_pos;
		}
		return true;
	}
	return false;
}
