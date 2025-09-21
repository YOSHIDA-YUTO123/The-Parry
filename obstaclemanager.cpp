//==============================================
//
// 障害物マネージャー [obstaclemanager.cpp]
// Author:YUTO YOSHIDA
//
//==============================================

//**********************************************
// インクルードファイル
//**********************************************
#include "obstaclemanager.h"
#include "Obstacle.h"
#include <string>
#include <fstream>
#include "player.h"
#include "game.h"
#include "Collider.h"
#include "ExplosionArea.h"
#include "meshfield.h"
#include "pause.h"

using namespace std; // 名前空間stdを使用

//**********************************************
// 定数宣言
//**********************************************
constexpr int POP_TIME = 1000; // ポップする時間

//**********************************************
// 静的メンバ変数宣言
//**********************************************
list<CObstacle*> CObstacleManager::m_apObstacleList = {};	// 障害物クラスへのポインタ
CObstacleManager* CObstacleManager::m_pManager = nullptr;	// 障害物マネージャーのポインタ

//==============================================
// コンストラクタ
//==============================================
CObstacleManager::CObstacleManager()
{
	// メモリのクリア
	ZeroMemory(&m_Point, sizeof(m_Point));
	m_nPopTime = NULL;
	m_nTNTPopPosID = -1;
}

//==============================================
// デストラクタ
//==============================================
CObstacleManager::~CObstacleManager()
{
}

//==============================================
// インスタンスの取得
//==============================================
CObstacleManager* CObstacleManager::GetInstance(void)
{
	return m_pManager;
}

//==============================================
// 障害物の取得(先頭)
//==============================================
list <CObstacle*>::iterator CObstacleManager::Begin(void)
{	
	// 先頭を返す
	return m_apObstacleList.begin();
}

//==============================================
// 障害物の取得(最後尾)
//==============================================
list<CObstacle*>::iterator CObstacleManager::End(void)
{
	// 最後尾を返す
	return m_apObstacleList.end();
}

//==============================================
// 障害物の並び替え
//==============================================
std::list<CObstacle*>::iterator CObstacleManager::Erase(std::list<CObstacle*>::iterator itr)
{
	return m_apObstacleList.erase(itr);
}

//==============================================
// 指定障害物の破棄
//==============================================
void CObstacleManager::Destroy(CObstacle* pObstacle)
{
	// 全て調べる
	for (auto itr = m_apObstacleList.begin(); itr != m_apObstacleList.end();)
	{
		if ((*itr) == pObstacle)
		{
			(*itr)->Uninit();
			itr = m_apObstacleList.erase(itr);
		}
		else
		{
			++itr;
		}
	}
}

//==============================================
// クリア処理
//==============================================
void CObstacleManager::Clear(CObstacle* pObstacle)
{
	// 全て調べる
	for (auto itr = m_apObstacleList.begin(); itr != m_apObstacleList.end();)
	{
		if ((*itr) == pObstacle)
		{
			(*itr)->Uninit();
			(*itr) = nullptr;
			break;
		}
		else
		{
			++itr;
		}
	}
}

//==============================================
// 初期化処理
//==============================================
void CObstacleManager::Create(void)
{
	// すでにインスタンスが存在するなら
	if (m_pManager != nullptr)
	{
		return;
	}

	// 障害物マネージャーの生成
	m_pManager = new CObstacleManager;

	// ポイントの設定
	m_pManager->m_Point[0] = D3DXVECTOR3(-713.0f,286.0f,1613.0f);
	m_pManager->m_Point[1] = D3DXVECTOR3(656.0f, 286.0f, 1574.0f);
	m_pManager->m_Point[2] = D3DXVECTOR3(795.0f, 286.0f, -1412.0f);
	m_pManager->m_Point[3] = D3DXVECTOR3(-685.0f, 286.0f, -1507.0f);
}

//==============================================
// 障害物の設定
//==============================================
void CObstacleManager::AddObstacle(CObstacle* pObstacle)
{
	// 障害物の設定
	m_apObstacleList.push_back(pObstacle);
}

//==============================================
// 爆発の破棄
//==============================================
void CObstacleManager::EraseExplosion(CExplosionArea* pExplosionArea)
{
	// 要素分調べる
	for (auto itr = m_apExplosionArea.begin(); itr != m_apExplosionArea.end(); ++itr)
	{
		// 指定したものと同じだったら
		if ((*itr) != nullptr && (*itr) == pExplosionArea)
		{
			(*itr)->Uninit();
			(*itr) = nullptr;
			itr = m_apExplosionArea.erase(itr);
			break;
		}
	}

}

//==============================================
// 破棄
//==============================================
void CObstacleManager::Uninit(void)
{
	// マネージャーの破棄
	if (m_pManager != nullptr)
	{
		delete m_pManager;

		m_pManager = nullptr;
	}

	// 空じゃないなら
	if (!m_apObstacleList.empty())
	{
		// 要素のクリア
		m_apObstacleList.clear();
	}
}

//==============================================
// 更新処理
//==============================================
void CObstacleManager::Update(void)
{
	// ポーズマネージャー
	auto pPauzeManager = CPauseManager::GetInstance();

	if (pPauzeManager != nullptr)
	{
		// ポーズ状態の取得
		bool bPause = pPauzeManager->GetPause();

		// ポーズ状態だったら
		if (bPause)
		{
			return;
		}
	}
	// タイムを減らす
	m_nPopTime--;

	// タイムが0になったら
	if (m_nPopTime <= 0)
	{
		// 再設定
		m_nPopTime = POP_TIME;

		// 出現位置を選出
		int nID = rand() % NUM_TNTPOP_POINT;

		// 位置の取得
		D3DXVECTOR3 pos = m_Point[nID];

		// プレイヤーの取得
		CPlayer* pPlayer = CGame::GetPlayer();

		if (pPlayer == nullptr) return;

		// プレイヤーの位置の取得
		D3DXVECTOR3 playerPos = pPlayer->GetPosition();

		// 爆発樽の生成
		CTNTBarrel::Create(pos, playerPos);
	}
}

//==============================================
// 障害物の現在の総数の取得
//==============================================
int CObstacleManager::GetObstacleSize(void)
{
	// 総数の取得
	int Size = static_cast<int>(m_apObstacleList.size());

	return Size;
}

//==============================================
// 爆発の当たり判定
//==============================================
bool CObstacleManager::CollisionExplotion(CColliderCapsule* pCapsule, D3DXVECTOR3* pPos)
{
	// 要素分調べる
	for (auto itr = m_apExplosionArea.begin(); itr != m_apExplosionArea.end(); ++itr)
	{
		// nullだったら処理を飛ばす
		if ((*itr) == nullptr) continue;

		// 爆発との当たり判定
		if ((*itr)->Collision(pCapsule, pPos))
		{
			return true;
		}
	}
	return false;
}

//==============================================
// インパクトの当たり判定
//==============================================
bool CObstacleManager::CollisionImpact(CObstacle* pObstacle)
{
	// メッシュフィールドの取得
	CMeshField* pMesh = CGame::GetField();

	// 取得できなかったら処理しない
	if (pMesh == nullptr) return false;

	// nullだったら処理しない
	if (pObstacle == nullptr) return false;

	// TNTじゃないなら
	if (pObstacle->GetType() != CObstacle::TYPE_TNT_BARREL) return false;

	// 位置の取得
	D3DXVECTOR3 pos = pObstacle->GetPosition();

	// インパクトの当たり判定
	if (pMesh->CollisionImpact(pos, 250.0f, CMeshFieldImpact::OBJ_ENEMY))
	{
		return true;
	}
	// インパクトの当たり判定
	else if (pMesh->CollisionImpact(pos, 250.0f, CMeshFieldImpact::OBJ_PLAYER))
	{
		return true;
	}

	return false;
}
