//===================================================
//
// リザルトのスコア [ResultScore.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "ResultScore.h"
#include "number.h"
#include "FadeInObject2D.h"

using namespace Const; // 名前空間Constの使用

//***************************************************
// 定数宣言
//***************************************************
const float FADE_TIME = 60; // フェード時間

//===================================================
// コンストラクタ
//===================================================
CResultScore::CResultScore() : CObject(7)
{
	// 最大の桁数分回す
	for (int nCnt = 0; nCnt < MAX_DIGIT; nCnt++)
	{
		m_pNumber[nCnt] = nullptr;
	}
	m_col = WHITE;
	m_fAddAlv = NULL;
	m_nScore = NULL;
	m_pos = VEC3_NULL;
	m_Size = VEC2_NULL;
}

//===================================================
// デストラクタ
//===================================================
CResultScore::~CResultScore()
{
}

//===================================================
// 生成処理
//===================================================
CResultScore* CResultScore::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const int nScore)
{
	CResultScore* pScore = new CResultScore;

	pScore->m_pos = pos;
	pScore->m_Size = Size;
	pScore->m_nScore = nScore;

	// 初期化処理
	if (FAILED(pScore->Init()))
	{
		pScore->Uninit();
		pScore = nullptr;
		return nullptr;
	}

	return pScore;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CResultScore::Init(void)
{
	// 透明にする
	m_col.a = 0.0f;
	m_fAddAlv = 1.0f / FADE_TIME;

	// スコアを保存
	int nScore = m_nScore;
	int nDigit = 0;

	// 横幅の割合を求める
	float fWidth = m_Size.x / MAX_DIGIT;

	// オフセット
	float offPosX = fWidth * 2.0f;

	// 桁数を求める
	while (nScore != 0)
	{
		nScore /= 10;
		nDigit++;
	}
	
	if (m_nScore == 0)
	{
		nDigit = 1;
	}

	// 桁数分回す
	for (int nCnt = 0; nCnt < nDigit; nCnt++)
	{
		m_pNumber[nCnt] = new CNumber;

		// 初期化処理
		m_pNumber[nCnt]->Init(D3DXVECTOR3(m_pos.x - (offPosX * nCnt), m_pos.y, 0.0f), D3DXVECTOR2(fWidth, m_Size.y));

		// テクスチャのIDの設定
		m_pNumber[nCnt]->SetTextureID("data/TEXTURE/number/number005.png");
	}

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CResultScore::Uninit(void)
{
	// 最大の桁数分回す
	for (int nCnt = 0; nCnt < MAX_DIGIT; nCnt++)
	{
		// ナンバーの破棄
		if (m_pNumber[nCnt] != nullptr)
		{
			m_pNumber[nCnt]->Uninit();
			delete m_pNumber[nCnt];
			m_pNumber[nCnt] = nullptr;
		}
	}

	// 自分自身の破棄
	CObject::Release();
}

//===================================================
// 更新処理
//===================================================
void CResultScore::Update(void)
{
	for (int nCnt = 0; nCnt < MAX_DIGIT; nCnt++)
	{
		// ナンバーオブジェクトの更新
		if (m_pNumber[nCnt] != nullptr)
		{
			m_col.a += m_fAddAlv;

			if (m_col.a >= 1.0f)
			{
				m_col.a = 1.0f;
			}
			// スコアを割る値
			int nData = 10;

			// 桁に応じた値を求める
			int nDivi = (int)pow((double)nData, (double)nCnt);

			// テクスチャ座標を求める
			int nNumber = m_nScore / nDivi % 10;

			// テクスチャ座標の設定
			m_pNumber[nCnt]->SetUV(nNumber);

			// 色の設定
			m_pNumber[nCnt]->SetColor(m_col);
		}
	}
}

//===================================================
// 描画処理
//===================================================
void CResultScore::Draw(void)
{
	// 最大の桁数分回す
	for (int nCnt = 0; nCnt < MAX_DIGIT; nCnt++)
	{
		// ナンバーの描画
		if (m_pNumber[nCnt] != nullptr)
		{
			m_pNumber[nCnt]->Draw();
		}
	}
}
