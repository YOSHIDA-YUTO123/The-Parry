//==============================================
//
// 障害物 [obstacle.cpp]
// Author:YUTO YOSHIDA
//
//==============================================

//**********************************************
// インクルードファイル
//**********************************************
#include "Obstacle.h"
#include "objectX.h"
#include "modelManager.h"
#include "manager.h"
#include "obstaclemanager.h"
#include "effect.h"
#include"Collider.h"
#include"meshfield.h"
#include"math.h"
#include "game.h"
#include"tutorial.h"
#include"transform.h"

using namespace Const;							// 名前空間Constを使用する
using namespace std;							// 名前空間stdを使用する

//==============================================
// コンストラクタ
//==============================================
CObstacle::CObstacle(const TYPE type)
{
	m_type = type;
	m_pMove = nullptr;
	m_pAABB = nullptr;
	m_CenterPos = VEC3_NULL;
	m_posOld = VEC3_NULL;
}

//==============================================
// デストラクタ
//==============================================
CObstacle::~CObstacle()
{
}

//==============================================
// 初期化処理
//==============================================
HRESULT CObstacle::Init(void)
{
	m_pMove = make_unique<CVelocity>();

	// 初期化処理
	if (FAILED(CObjectX::Init()))
	{
		return E_FAIL;
	}

	// 種類の設定
	CObject::SetType(CObject::TYPE::TYPE_OBSTACLE);

	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 Size = GetSize();

	// 中心座標の設定
	m_CenterPos.x = pos.x;
	m_CenterPos.y = pos.y + (Size.y * 0.5f);
	m_CenterPos.z = pos.z;

	// AABBの生成
	m_pAABB = CColliderAABB::Create(m_CenterPos, m_posOld, Size);

	// 障害物マネージャーのインスタンスの取得
	CObstacleManager* pObstacleManager = CObstacleManager::GetInstance();

	// 障害物マネージャーが取得出来たら
	if (pObstacleManager != nullptr)
	{
		pObstacleManager->AddObstacle(this);
	}

    return S_OK;
}

//==============================================
// 終了処理
//==============================================
void CObstacle::Uninit(void)
{
	CObjectX::Uninit();
}

//==============================================
// 更新処理
//==============================================
void CObstacle::Update(void)
{
	// フィールドへのポインタ
	CMeshField* pField = nullptr;

	// 現在のモードの取得
	CScene::MODE mode = CManager::GetMode();

	if (mode == CScene::MODE_TUTORIAL)
	{
		// フィールドの取得
		pField = CTutorial::GetField();
	}
	else if (mode == CScene::MODE_GAME)
	{
		// フィールドの取得
		pField = CGame::GetField();
	}

	// 位置の取得
	D3DXVECTOR3 pos = CObjectX::GetPosition();
	D3DXVECTOR3 Size = CObjectX::GetSize();

	if (m_pMove != nullptr)
	{
		// 移動量
		pos += m_pMove->Get();
	}

	// 高さ
	float fHeight = 0.0f;

	// 地面と当たったら
	if (pField != nullptr && pField->Collision(pos, &fHeight))
	{
		// 地面の高さに合わせる
		pos.y = fHeight;
	}

	// 前回の位置の設定
	m_posOld = m_CenterPos;

	// 中心座標の設定
	m_CenterPos.x = pos.x;
	m_CenterPos.y = pos.y + (Size.y * 0.5f);
	m_CenterPos.z = pos.z;

	// 位置の設定処理
	m_pAABB->UpdateData(m_CenterPos, m_posOld);
	
	// 重力の設定
	m_pMove->Gravity(-MAX_GRABITY);

	// いちの設定
	CObjectX::SetPosition(pos);
}

//==============================================
// 描画処理
//==============================================
void CObstacle::Draw(void)
{
	// 描画処理
	CObjectX::Draw();
}

//==============================================
// コライダーの生成
//==============================================
void CObstacle::CreateCollider(void)
{
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 Size = GetSize();

	// 中心座標の設定
	m_CenterPos.x = pos.x;
	m_CenterPos.y = pos.y + (Size.y * 0.5f);
	m_CenterPos.z = pos.z;

	// AABBの生成
	m_pAABB = CColliderAABB::Create(m_CenterPos, m_posOld, Size);
}

//==============================================
// コンストラクタ
//==============================================
CSpikeTrap::CSpikeTrap() : CObstacle(TYPE_SPIKE_TRAP)
{
	m_nDamageFace = NULL;
}

//==============================================
// デストラクタ
//==============================================
CSpikeTrap::~CSpikeTrap()
{
}

//==============================================
// 生成処理
//==============================================
CSpikeTrap* CSpikeTrap::Create(const D3DXVECTOR3 pos,const D3DXVECTOR3 rot, const int nDamageFace)
{
	CSpikeTrap* pObstacle = nullptr;

	// 障害物の生成
	pObstacle = new CSpikeTrap;
		
	if (pObstacle == nullptr) return nullptr;
		
	// 初期化に失敗したら
	if (FAILED(pObstacle->Init()))
	{
		// 終了処理
		pObstacle->Uninit();
		pObstacle = nullptr;
	
		return nullptr;
	}
	
	// オブジェクト
	pObstacle->SetPosition(pos);
	pObstacle->GetRotaition()->Set(rot);
	pObstacle->m_nDamageFace = nDamageFace;

	return pObstacle;
}

//==============================================
// 初期化処理
//==============================================
HRESULT CSpikeTrap::Init(void)
{
	// 初期化処理
	if (FAILED(CObstacle::Init()))
	{
		// 終了処理
		CObstacle::Release();

		return E_FAIL;
	}

	// モデルの読み込み
	CObjectX::LoadModel("data/MODEL/obstacle/spiketrap.x");

	// コライダーの生成	
	CreateCollider();

	return S_OK;
}

//==============================================
// 終了処理
//==============================================
void CSpikeTrap::Uninit(void)
{
	// 終了処理
	CObstacle::Uninit();
}

//==============================================
// 更新処理
//==============================================
void CSpikeTrap::Update(void)
{
	// 更新処理
	CObstacle::Update();
}

//==============================================
// 描画処理
//==============================================
void CSpikeTrap::Draw(void)
{
	// 描画処理
	CObstacle::Draw();
}

//==============================================
// 当たり判定
//==============================================
bool CSpikeTrap::Collision(CColliderAABB *pCollider, D3DXVECTOR3* pushPos)
{
	// 当たり判定(矩形)の取得
	auto pCollisionAABB = CCollisionAABB::GetInstance();

	// 自分のコライダーの取得
	auto pColliderAABB = CObstacle::GetCollider();

	// ダメージを食らう面
	int nDamageFace = 0;

	// 矩形の判定
	if (pCollisionAABB->Collision(pColliderAABB, pCollider, pushPos,&nDamageFace))
	{
		// ダメージを食らう面が同じだったら(またはすべて食らうなら)
		if (m_nDamageFace == nDamageFace || m_nDamageFace == -1)
		{
			return true;
		}
	}

	return false;
}

//==============================================
// コンストラクタ
//==============================================
CTNTBarrel::CTNTBarrel() : CObstacle(TYPE_TNT_BARREL)
{
}

//==============================================
// デストラクタ
//==============================================
CTNTBarrel::~CTNTBarrel()
{
}

//==============================================
// 生成処理
//==============================================
CTNTBarrel* CTNTBarrel::Create(const D3DXVECTOR3 pos)
{
	CTNTBarrel* pObstacle = nullptr;

	// 障害物の生成
	pObstacle = new CTNTBarrel;

	if (pObstacle == nullptr) return nullptr;

	// 初期化に失敗したら
	if (FAILED(pObstacle->Init()))
	{
		// 終了処理
		pObstacle->Uninit();
		pObstacle = nullptr;

		return nullptr;
	}

	// オブジェクト
	pObstacle->SetPosition(pos);

	return pObstacle;
}

//==============================================
// 初期化処理
//==============================================
HRESULT CTNTBarrel::Init(void)
{
	// 初期化処理
	if (FAILED(CObstacle::Init()))
	{
		// 終了処理
		CObstacle::Release();

		return E_FAIL;
	}

	// モデルの読み込み
	CObjectX::LoadModel("data/MODEL/obstacle/TNTBarrel.x");

	// コライダーの生成	
	CreateCollider();

	return S_OK;
}

//==============================================
// 終了処理
//==============================================
void CTNTBarrel::Uninit(void)
{
	// 終了処理
	CObstacle::Uninit();
}

//==============================================
// 更新処理
//==============================================
void CTNTBarrel::Update(void)
{
	// 更新処理
	CObstacle::Update();
}

//==============================================
// 描画処理
//==============================================
void CTNTBarrel::Draw(void)
{
	// 描画処理
	CObstacle::Draw();
}

//==============================================
// 当たり判定
//==============================================
bool CTNTBarrel::Collision(CColliderAABB* pCollider, D3DXVECTOR3* pushPos)
{
	// 当たり判定(矩形)の取得
	auto pCollisionAABB = CCollisionAABB::GetInstance();

	// 自分のコライダーの取得
	auto pColliderAABB = CObstacle::GetCollider();

	// 矩形の判定
	if (pCollisionAABB->Collision(pColliderAABB, pCollider, pushPos))
	{
		return true;
	}

	return false;
}
