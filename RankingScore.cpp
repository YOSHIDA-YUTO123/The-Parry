//================================================
//
// ランキングのスコアを描画する [RankingScore.h]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "RankingScore.h"
#include "number.h"

using namespace Const; // 名前空間Constの使用

//************************************************
// 定数宣言
//************************************************
constexpr int ADD_FRAME = 120; // 加算フレーム
constexpr int ADD_SCPRE_OFFSET = 60; // 加算スコアのオフセット

//================================================
// コンストラクタ
//================================================
CRankingScore::CRankingScore() : CObject(7)
{
	// 最大の桁数分回す
	for (int nCnt = 0; nCnt < MAX_DIGIT; nCnt++)
	{
		m_apNumber[nCnt] = nullptr;
	}
	
	m_col = WHITE;
	m_pos = VEC3_NULL;
	m_DestSize = VEC2_NULL;
	m_Size = VEC2_NULL;
	m_nDestScore = NULL;
	m_nScore = NULL;
	m_fCounter = NULL;
	m_bHilight = false;
}

//================================================
// デストラクタ
//================================================
CRankingScore::~CRankingScore()
{
}

//================================================
// 生成処理
//================================================
CRankingScore* CRankingScore::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const int nScore, const bool bHilight)
{
	CRankingScore* pScore = new CRankingScore;
	
	pScore->m_pos = pos;
	pScore->m_DestSize = Size;
	pScore->m_nDestScore = nScore;
	pScore->m_bHilight = bHilight;
	
	// 初期化処理
	if (FAILED(pScore->Init()))
	{
		pScore->Uninit();
		pScore = nullptr;
		return nullptr;
	}
	return pScore;
}

//================================================
// 初期化処理
//================================================
HRESULT CRankingScore::Init(void)
{
	// 横幅を求める
	float fWidth = m_DestSize.x * 1.5f;

	// 最大の横幅を求める
	float fMaxWidth = fWidth * MAX_DIGIT;
	
	// 大きさの設定
	m_Size = m_DestSize * 50.0f;

	// 最大の桁数分回す
	for (int nCnt = 0; nCnt < MAX_DIGIT; nCnt++)
	{
		// 初期化処理
		if (m_apNumber[nCnt] == nullptr)
		{
			// ナンバーの生成処理
			m_apNumber[nCnt] = new CNumber;

			// 位置を計算
			float fPosX = m_pos.x - (fMaxWidth / MAX_DIGIT * nCnt);

			// 初期化処理
			HRESULT result = m_apNumber[nCnt]->Init(
				D3DXVECTOR3(fPosX, m_pos.y, m_pos.z),
				D3DXVECTOR2(m_Size.x,m_Size.y));

			// 初期化に失敗したら
			if (FAILED(result))
			{
				return E_FAIL;
			}

			// テクスチャのIDの設定
			m_apNumber[nCnt]->SetTextureID("data/TEXTURE/number/number005.png");
		}
	}
	return S_OK;
}

//================================================
// 終了処理
//================================================
void CRankingScore::Uninit(void)
{
	// 最大の桁数分回す
	for (int nCnt = 0; nCnt < MAX_DIGIT; nCnt++)
	{
		// 終了処理
		if (m_apNumber[nCnt] != nullptr)
		{
			m_apNumber[nCnt]->Uninit();
			delete m_apNumber[nCnt];
			m_apNumber[nCnt] = nullptr;
		}
	}

	// 自分自身の破棄
	CObject::Release();
}

//================================================
// 更新処理
//================================================
void CRankingScore::Update(void)
{
	// 目的の値を超えたら
	if (m_nDestScore <= m_nScore)
	{
		m_nScore = m_nDestScore;
	}
	else
	{
		// 加算値を求める
		int nAddScore = m_nDestScore / ADD_FRAME;

		if (nAddScore <= 0)
		{
			// 最低保障
			nAddScore = ADD_SCPRE_OFFSET;
		}

		// スコアを加算
		m_nScore += nAddScore;
	}

	// 最大の桁数分回す
	for (int nCnt = 0; nCnt < MAX_DIGIT; nCnt++)
	{
		// 使用されていないなら処理しない
		if (m_apNumber[nCnt] == nullptr) continue;
	
		// 強調表示するなら
		if (m_bHilight)
		{
			m_fCounter += 0.005f;

			// 色の設定
			m_col.g = fabsf(sinf(m_fCounter));
			m_col.b = fabsf(sinf(m_fCounter));

			// 色の設定
			m_apNumber[nCnt]->SetColor(m_col);
		}

		// スコアを割る値
		int nData = 10;

		// 桁に応じた値を求める
		int nDivi = (int)pow((double)nData, (double)nCnt);

		// テクスチャ座標を求める
		int nNumber = m_nScore / nDivi % 10;

		// テクスチャ座標の設定
		m_apNumber[nCnt]->SetUV(nNumber);

		// 目的の大きさに近づける
		m_Size += (m_DestSize - m_Size) * 0.01f;

		// 大きさの設定
		m_apNumber[nCnt]->SetSize(m_Size);

		// 更新処理
		m_apNumber[nCnt]->Update();
	}
}

//================================================
// 描画処理
//================================================
void CRankingScore::Draw(void)
{
	// 最大の桁数分回す
	for (int nCnt = 0; nCnt < MAX_DIGIT; nCnt++)
	{
		// 描画処理
		if (m_apNumber[nCnt] != nullptr)
		{
			m_apNumber[nCnt]->Draw();
		}
	}
}
