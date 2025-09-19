//===================================================
//
// 鳥のマネージャー [BirdManager.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "BirdManager.h"
#include "bird.h"
#include "math.h"
#include "debugproc.h"

using namespace math;  // 名前空間mathの使用
using namespace Const; // 名前空間Constの使用

//***************************************************
// 定数宣言
//***************************************************
constexpr int AREA_POP_TIME = 1800; // 30秒
constexpr int POINT_POP_TIME = 900; // 15秒
constexpr int POP_BIRD_CENTER = 3; // 10匹
constexpr int POP_BIRD = 8; // 10匹

//***************************************************
// 静的メンバ変数宣言
//***************************************************
CBirdManager* CBirdManager::m_pInstance = nullptr; // 自分のインスタンス

//===================================================
// 対象の位置からの距離を判定
//===================================================
bool CBirdManager::CheckDistance(const D3DXVECTOR3 otherPos, const float fRadius)
{
	// 判定結果
	bool bResult = false;

	// 鳥の出現エリアにいるか判定
	InAreaRenge(otherPos);

	// 要素を調べる
	for (auto itr = m_apBirdList.begin(); itr != m_apBirdList.end(); ++itr)
	{
		// nullだったら処理を飛ばす
		if ((*itr) == nullptr) continue;

		// 一定の範囲近づいたか判定
		if ((*itr)->CheckDistance(otherPos, fRadius))
		{
			bResult = true;
		}
	}

	return bResult;
}

//===================================================
// オープニングの鳥の設定
//===================================================
void CBirdManager::SetOpening(void)
{
	// 鳥の出現
	for (int nCnt = 0; nCnt < POP_BIRD_CENTER; nCnt++)
	{
		// 位置
		D3DXVECTOR3 pos = m_Arena[0].pos;

		// 半径
		int nRadius = static_cast<int>(m_Arena[0].fRadius);

		// ランダムな位置の設定
		float fRandomPosX = static_cast<float>(rand() % (nRadius * 2) - m_Arena[0].fRadius);
		float fRandomPosZ = static_cast<float>(rand() % (nRadius * 2) - m_Arena[0].fRadius);

		// 鳥の生成
		CBird::Create(D3DXVECTOR3(pos.x + fRandomPosX, pos.y, pos.z + fRandomPosZ),CBird::TYPE_NORMAL,false);
	}

	// 鳥の出現
	for (int nCnt = 0; nCnt < POP_BIRD; nCnt++)
	{
		CBird::Create(D3DXVECTOR3(static_cast<float>(rand() % 2800 - 1400.0f), 0.0f, static_cast<float>(rand() % 2800 - 1400.0f)), CBird::TYPE_NORMAL,false);
		CBird::Create(D3DXVECTOR3(static_cast<float>(rand() % 2000 - 1000.0f), static_cast<float>(rand() % 200 + 600.0f), static_cast<float>(rand() % 2000 - 1000.0f)), CBird::TYPE_FLY_MOVE, false);
	}
}

//===================================================
// エリアにいるかどうか
//===================================================
void CBirdManager::InAreaRenge(const D3DXVECTOR3 otherPos)
{
	int nIdx = -1;

	// ポイントの総数分回す
	for (int nCnt = 0; nCnt < MAX_POINT; nCnt++)
	{
		// 位置の取得
		D3DXVECTOR3 pos = m_Arena[nCnt].pos;

		// 距離を求める
		float fDistance = GetDistance(pos - otherPos);

		// 出現エリアの範囲外だったら
		if (fDistance <= m_Arena[nCnt].fRadius)
		{
			nIdx = nCnt;
			break;
		}
	}

	// 0秒じゃないなら
	if (m_nAreaPopTime > 0) return;

	// 出現ポイント
	int nPoint = -1;

	while (1)
	{
		// ポイントを選出
		nPoint = rand() % MAX_POINT;

		// プレイヤーがいる場所じゃないなら
		if (nPoint != nIdx)
		{
			break;
		}
	}

	// 出現数分回す
	for (int nCnt = 0; nCnt < POP_BIRD; nCnt++)
	{
		// 位置
		D3DXVECTOR3 pos = m_Arena[nPoint].pos;

		// 半径
		int nRadius = static_cast<int>(m_Arena[nPoint].fRadius);

		// ランダムな位置の設定
		float fRandomPosX = static_cast<float>(rand() % (nRadius * 2) - m_Arena[nPoint].fRadius);
		float fRandomPosZ = static_cast<float>(rand() % (nRadius * 2) - m_Arena[nPoint].fRadius);

		// 鳥の生成
		CBird::Create(D3DXVECTOR3(pos.x + fRandomPosX,pos.y,pos.z + fRandomPosZ), CBird::TYPE_NORMAL);
		CBird::Create(D3DXVECTOR3(static_cast<float>(rand() % 2000 - 1000.0f), static_cast<float>(rand() % 200 + 600.0f), static_cast<float>(rand() % 2000 - 1000.0f)), CBird::TYPE_FLY_MOVE, false);
	}

	m_nAreaPopTime = AREA_POP_TIME;
}

//===================================================
// コンストラクタ
//===================================================
CBirdManager::CBirdManager()
{
	m_nAreaPopTime = NULL;
	m_nPointPopTime = POINT_POP_TIME;
	m_apBirdList = {};					   
	ZeroMemory(&m_Arena, sizeof(m_Arena));
	ZeroMemory(&m_Point, sizeof(m_Point));
}

//===================================================
// デストラクタ
//===================================================
CBirdManager::~CBirdManager()
{
	m_pInstance = nullptr;
}

//===================================================
// 生成処理
//===================================================
void CBirdManager::Create(void)
{
	// 自分が生成されていなかったら
	if (m_pInstance == nullptr)
	{
		// 鳥のマネージャー生成
		m_pInstance = new CBirdManager;
		m_pInstance->Init();
	}
}

//===================================================
// インスタンスを指定してリストから切り離す
//===================================================
void CBirdManager::Erase(CBird* pBird)
{
	// 要素を調べる
	for (auto itr = m_apBirdList.begin(); itr != m_apBirdList.end(); ++itr)
	{
		// nullだったら処理しない
		if ((*itr) == nullptr) continue;

		if ((*itr) == pBird)
		{
			(*itr)->Uninit();
			(*itr) = nullptr;
			itr = m_apBirdList.erase(itr);
			break;
		}
	}
}

//===================================================
// 初期化処理
//===================================================
HRESULT CBirdManager::Init(void)
{
	// ポイントの総数分回す
	for (int nCnt = 0; nCnt < MAX_POINT; nCnt++)
	{
		m_Arena[nCnt].fRadius = 500.0f;
	}

	m_Arena[0].pos = { 0.0f,0.0f,0.0f };
	m_Arena[1].pos = { 100.0f,0.0f,1000.0f };
	m_Arena[2].pos = { 1000.0f,0.0f,1000.0f };
	m_Arena[3].pos = { 1000.0f,0.0f,-1000.0f };

	m_Point[0] = { 1760.0f,736.0f,68.0f };
	m_Point[1] = { -1682.0f,736.0f,37.0f };

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CBirdManager::Uninit(void)
{
	// 終了処理
	CObject::Release();
}

//===================================================
// 更新処理
//===================================================
void CBirdManager::Update(void)
{
	// エリアに出現する時間
	m_nAreaPopTime--;

	// 出現位置に出現する時間
	m_nPointPopTime--;

	if (m_nPointPopTime <= 0)
	{
		// ポイントを選出
		int nPoint = rand() % MAX_POINT;

		// 生成処理
		CBird::Create(m_Point[nPoint], CBird::TYPE_NORMAL,false);

		m_nPointPopTime = POINT_POP_TIME;
	}
	// 情報の描画
	CDebugProc::Print("鳥出現まで %d秒\n", m_nAreaPopTime / 60);
}

//===================================================
// 描画処理
//===================================================
void CBirdManager::Draw(void)
{
}
