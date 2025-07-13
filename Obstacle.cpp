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

using namespace Const;							// 名前空間Constを使用する
using namespace std;							// 名前空間stdを使用する

//==============================================
// コンストラクタ
//==============================================
CObstacle::CObstacle()
{
	m_pObjectX = nullptr;
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
	// オブジェクトXの生成
	m_pObjectX = new CObjectX;

	m_pMove = make_unique<CVelocity>();

	// 初期化処理
	if (m_pObjectX != nullptr && FAILED(m_pObjectX->Init()))
	{
		return E_FAIL;
	}

	// 種類の設定
	SetType(TYPE::TYPE_OBSTACLE);

    return S_OK;
}

//==============================================
// 終了処理
//==============================================
void CObstacle::Uninit(void)
{
	// クリア
	m_pObjectX = nullptr;

	// 自分の破棄
	CObject::Release();
}

//==============================================
// 更新処理
//==============================================
void CObstacle::Update(void)
{
	// フィールドの取得
	CMeshField* pField = CManager::GetMeshField();

	// 位置の取得
	D3DXVECTOR3 pos = m_pObjectX->GetPosition();

	// 移動量
	pos += m_pMove->Get();

	// 高さ
	float fHeight = 0.0f;

	// 地面と当たったら
	if (pField->Collision(pos, &fHeight))
	{
		// 地面の高さに合わせる
		pos.y = fHeight;
	}

	// 重力の設定
	m_pMove->Gravity(-MAX_GRABITY);

	// いちの設定
	m_pObjectX->SetPosition(pos);

	if (CManager::GetInputKeyboard()->GetPress(DIK_G))
	{
		m_pObjectX->GetRotaition()->Set(D3DXVECTOR3(0.0f,D3DX_PI * 0.5f,0.0f));
	}
}

//==============================================
// 描画処理
//==============================================
void CObstacle::Draw(void)
{
	if (m_pObjectX != nullptr)
	{
		// 描画処理
		m_pObjectX->Draw();
	}
}

//==============================================
// 破棄
//==============================================
void CObstacle::Release(void)
{
	// Xオブジェクトの破棄
	if (m_pObjectX != nullptr)
	{
		m_pObjectX->Uninit();
		m_pObjectX = nullptr;

	}
}

//==============================================
// 位置の設定処理
//==============================================
void CObstacle::SetPosition(const D3DXVECTOR3 pos)
{
	// 位置の設定処理
	if (m_pObjectX != nullptr)
	{
		m_pObjectX->SetPosition(pos);
	}
}

//==============================================
// 向きの設定処理
//==============================================
void CObstacle::SetRotaition(const D3DXVECTOR3 rot)
{
	// 位置の設定処理
	if (m_pObjectX != nullptr)
	{
		m_pObjectX->GetRotaition()->Set(rot);
	}
}

//==============================================
// モデルのロードの設定
//==============================================
void CObstacle::SetModelName(const char* pModelName)
{
	// モデルのロード処理
	if (m_pObjectX != nullptr)
	{
		m_pObjectX->LoadModel(pModelName);
	}
}

//==============================================
// 位置の取得
//==============================================
D3DXVECTOR3 CObstacle::GetPosition(void) const
{
	// 位置の取得
	if(m_pObjectX != nullptr) return m_pObjectX->GetPosition();

	return VEC3_NULL;
}

//==============================================
// 大きさの取得
//==============================================
D3DXVECTOR3 CObstacle::GetSize(void) const
{
	// 大きさの取得
	if (m_pObjectX != nullptr) return m_pObjectX->GetSize();

	return VEC3_NULL;
}

//==============================================
// ワールドマトリックスの取得
//==============================================
D3DXMATRIX CObstacle::GetMatrix(void) const
{
	// ワールドマトリックス
	D3DXMATRIX MtxWorld;

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&MtxWorld);

	// ワールドマトリックスの取得
	if (m_pObjectX != nullptr)
	{
		MtxWorld = m_pObjectX->GetMatrix();
	}

	return MtxWorld;
}

//==============================================
// コンストラクタ
//==============================================
CSpikeTrap::CSpikeTrap()
{
	m_pAABB = nullptr;
	m_CenterPos = VEC3_NULL;
	m_pushPos = VEC3_NULL;
	m_posOld = VEC3_NULL;
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
CSpikeTrap* CSpikeTrap::Create(const D3DXVECTOR3 pos, const int nDamageFace)
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
	pObstacle->m_nDamageFace = nDamageFace;

	// マネージャーの生成
	CObstacleManager::Create();

	// 障害物マネージャーのインスタンスの取得
	CObstacleManager* pObstacleManager = CObstacleManager::GetInstance();

	// 障害物マネージャーが取得出来たら
	if (pObstacleManager != nullptr)
	{
		pObstacleManager->AddObstacle(pObstacle);
	}

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
	SetModelName("data/MODEL/obstacle/spiketrap.x");

 	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 Size = GetSize();

	// 中心座標の設定
	m_CenterPos.x = pos.x;
	m_CenterPos.y = pos.y + (Size.y * 0.5f);
	m_CenterPos.z = pos.z;

	// AABBの生成
	m_pAABB = CColliderAABB::Create(m_CenterPos,m_posOld, Size);

	return S_OK;
}

//==============================================
// 終了処理
//==============================================
void CSpikeTrap::Uninit(void)
{
	// 障害物マネージャーのインスタンスの取得
	CObstacleManager* pObstacleManager = CObstacleManager::GetInstance();

	// マネージャーの終了処理
	if (pObstacleManager != nullptr)
	{
		pObstacleManager->Uninit();
		pObstacleManager = nullptr;
	}

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

	// ワールドマトリックスの位置の取得
	D3DXVECTOR3 pos = math::GetPositionFromMatrix(GetMatrix());
	D3DXVECTOR3 Size = GetSize();

	// 前回の位置の設定
	m_posOld = m_CenterPos;

	// 中心座標の設定
	m_CenterPos.x = pos.x;
	m_CenterPos.y = pos.y + (Size.y * 0.5f);
	m_CenterPos.z = pos.z;

	CEffect3D::Create(m_CenterPos, 50.0f, VEC3_NULL, WHITE, 10);

	// 位置の設定処理
	m_pAABB->UpdateData(m_CenterPos, m_posOld);
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
	auto *pCollisionAABB = CCollisionAABB::GetInstance();

	// ダメージを食らう面
	int nDamageFace = 0;

	// 矩形の判定
	if (pCollisionAABB->Collision(m_pAABB.get(), pCollider, pushPos,&nDamageFace))
	{
		// ダメージを食らう面が同じだったら(またはすべて食らうなら)
		if (m_nDamageFace == nDamageFace || m_nDamageFace == -1)
		{
			return true;
		}
	}
	return false;
}
