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

using namespace math; // 名前空間Constの使用

//***************************************************
// 定数宣言
//***************************************************
constexpr int AREA_POP_TIME = 1800; // 30秒
constexpr int POP_BIRD = 10; // 10匹

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
// エリアにいるかどうか
//===================================================
void CBirdManager::InAreaRenge(const D3DXVECTOR3 otherPos)
{
	int nIdx = -1;

	// ポイントの総数分回す
	for (int nCnt = 0; nCnt < MAX_POINT; nCnt++)
	{
		// 位置の取得
		D3DXVECTOR3 pos = m_Point[nCnt].pos;

		// 距離を求める
		float fDistance = GetDistance(pos - otherPos);

		if (fDistance <= m_Point[nCnt].fRadius)
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
		D3DXVECTOR3 pos = m_Point[nPoint].pos;

		// 半径
		int nRadius = static_cast<int>(m_Point[nPoint].fRadius);

		// ランダムな位置の設定
		float fRandomPosX = static_cast<float>(rand() % (nRadius * 2) - m_Point[nPoint].fRadius);
		float fRandomPosZ = static_cast<float>(rand() % (nRadius * 2) - m_Point[nPoint].fRadius);

		// 鳥の生成
		CBird::Create(D3DXVECTOR3(pos.x + fRandomPosX,pos.y,pos.z + fRandomPosZ));
	}

	m_nAreaPopTime = AREA_POP_TIME;
}

//===================================================
// コンストラクタ
//===================================================
CBirdManager::CBirdManager()
{
	m_nAreaPopTime = NULL;
	m_apBirdList = {};					   
	ZeroMemory(&m_Point, sizeof(m_Point));
}

//===================================================
// デストラクタ
//===================================================
CBirdManager::~CBirdManager()
{
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
		m_Point[nCnt].fRadius = 300.0f;
	}

	m_Point[0].pos = { 0.0f,0.0f,0.0f };
	m_Point[1].pos = { 100.0f,0.0f,1000.0f };
	m_Point[2].pos = { 1000.0f,0.0f,1000.0f };
	m_Point[3].pos = { 1000.0f,0.0f,-1000.0f };

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
	m_nAreaPopTime--;

	// 情報の描画
	CDebugProc::Print("鳥出現まで %d秒\n", m_nAreaPopTime / 60);
}

//===================================================
// 描画処理
//===================================================
void CBirdManager::Draw(void)
{
}
