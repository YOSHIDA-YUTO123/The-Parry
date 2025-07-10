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

using namespace std; // 名前空間stdを使用

//**********************************************
// 静的メンバ変数宣言
//**********************************************
vector<CObstacle*> CObstacleManager::m_apObstacle = {};					// 障害物クラスへのポインタ
CObstacleManager* CObstacleManager::m_pManager = nullptr;	// 障害物マネージャーのポインタ

//==============================================
// コンストラクタ
//==============================================
CObstacleManager::CObstacleManager()
{

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
// 障害物の取得
//==============================================
CObstacle* CObstacleManager::GetObstacle(const int nIdx)
{
	// 総数の取得
	int Size = static_cast<int>(m_apObstacle.size());

	// インデックスがサイズオーバーしていなかったら
	if (nIdx >= Size)
	{
		return nullptr;
	}

	return m_apObstacle[nIdx];
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
}

//==============================================
// 障害物の設定
//==============================================
void CObstacleManager::AddObstacle(CObstacle* pObstacle)
{
	// 障害物の設定
	m_apObstacle.push_back(pObstacle);
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
	m_apObstacle.clear();
}

//==============================================
// 障害物の現在の総数の取得
//==============================================
int CObstacleManager::GetObstacleSize(void)
{
	// 総数の取得
	int Size = static_cast<int>(m_apObstacle.size());

	return Size;
}
