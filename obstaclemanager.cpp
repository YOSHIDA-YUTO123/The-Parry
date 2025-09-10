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

using namespace std; // 名前空間stdを使用

//**********************************************
// 定数宣言
//**********************************************
constexpr int POP_TIME = 600; // ポップする時間

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

	m_pManager->m_Point[0] = D3DXVECTOR3(-713.0f,286.0f,1613.0f);
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

	// 要素のクリア
	m_apObstacleList.clear();
}

//==============================================
// 更新処理
//==============================================
void CObstacleManager::Update(void)
{
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
