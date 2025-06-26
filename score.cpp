//================================================
//
// スコア [score.cpp]
// Author:YUTO YOSHIDA
//
//=================================================

//*************************************************
// インクルードファイル
//*************************************************
#include "score.h"
#include"manager.h"
#include"renderer.h"

//*************************************************
// 静的メンバ変数の宣言
//*************************************************
CNumber* CScore::m_apNumber[MAX_DIGIT] = {};	// ナンバーオブジェクトへのポインタ

//=================================================
// コンストラクタ
//=================================================
CScore::CScore(int nPriority) : CObject(nPriority)
{
	m_fHeight = NULL;
	m_fWidth = NULL;
	m_pos = VEC3_NULL;
	m_nScore = NULL;
}

//=================================================
// デストラクタ
//=================================================
CScore::~CScore()
{
}

//=================================================
// 初期化処理
//=================================================
HRESULT CScore::Init(void)
{
	// 横幅の割合を求める
	float fWidth = m_fWidth / MAX_DIGIT;

	// オフセット
	float offPosX = fWidth * 2.0f;

	for (int nCnt = 0; nCnt < MAX_DIGIT; nCnt++)
	{
		// ナンバーのメモリの確保
		if (m_apNumber[nCnt] == NULL)
		{
			m_apNumber[nCnt] = new CNumber;

			// 初期化処理
			m_apNumber[nCnt]->Init(9, 1, D3DXVECTOR3(m_pos.x - (offPosX * nCnt), m_pos.y, 0.0f), fWidth, m_fHeight);
			m_apNumber[nCnt]->SetTextureID("data/TEXTURE/number005.png");
		}
	}

	return S_OK;
}

//=================================================
// 終了処理
//=================================================
void CScore::Uninit(void)
{
	for (int nCnt = 0; nCnt < MAX_DIGIT; nCnt++)
	{
		// ナンバーオブジェクトの解放
		if (m_apNumber[nCnt] != NULL)
		{
			m_apNumber[nCnt]->Uninit();

			delete m_apNumber[nCnt];

			m_apNumber[nCnt] = NULL;
		}
	}

	Release();
}

//=================================================
// 更新処理
//=================================================
void CScore::Update(void)
{
	// スコアの制限
	if (m_nScore >= MAX_SCORE)
	{
		m_nScore = MAX_SCORE;
	}
	else if (m_nScore <= MIN_SCORE)
	{
		m_nScore = MIN_SCORE;
	}

	for (int nCnt = 0; nCnt < MAX_DIGIT; nCnt++)
	{
		// ナンバーオブジェクトの更新
		if (m_apNumber[nCnt] != NULL)
		{
			m_apNumber[nCnt]->Update();

			// スコアを割る値
			int nData = 10;

			// 桁に応じた値を求める
			int nDivi = (int)pow((double)nData, (double)nCnt);

			// テクスチャ座標を求める
			int nNumber = m_nScore / nDivi % 10;

			m_apNumber[nCnt]->SetUV(nNumber);
		}
	}
}

//=================================================
// 描画処理
//=================================================
void CScore::Draw(void)
{
	for (int nCnt = 0; nCnt < MAX_DIGIT; nCnt++)
	{
		// ナンバーオブジェクトの描画
		if (m_apNumber[nCnt] != NULL)
		{
			// 描画処理
			m_apNumber[nCnt]->Draw();
		}
	}

}

//=================================================
// 大きさの設定処理
//=================================================
void CScore::SetSize(const float fWidth, const float fHeight)
{
	m_fWidth = fWidth;
	m_fHeight = fHeight;
}

//=================================================
// 作成処理
//=================================================
CScore* CScore::Create(const TYPE type, const D3DXVECTOR3 pos, const float fWidth, const float fHeight)
{
	// スコアオブジェクトへのポインタ
	CScore* pScore = NULL;

	// 種類の遷移
	switch (type)
	{
	case TYPE::TYPE_NORMAL:
		// スコアのメモリの確保
		pScore = new CScore;
		break;
	case TYPE::TYPE_LERPER:
		// スコアのメモリの確保
		pScore = new CScoreLerper;
		break;
	default:
		return nullptr;
		break;
	}

	// 自分の優先順位の取得
	const int nPriority = pScore->GetPriority();

	// 現在のオブジェクトの最大数
	const int nNumAll = CObject::GetNumObject(nPriority);

	// オブジェクトが最大数まであったら
	if (nNumAll >= MAX_OBJECT && pScore != nullptr)
	{
		// 自分のポインタの解放
		pScore->Release();

		// nullにする
		pScore = nullptr;

		// オブジェクトを消す
		return nullptr;
	}

	// NULLだったら飛ばす
	if (pScore == nullptr) return nullptr;

	// 大きさの設定処理
	pScore->SetSize(fWidth, fHeight);

	// 位置の設定
	pScore->SetPosition(pos);

	// 初期化処理
	pScore->Init();

	return pScore;
}

//=================================================
// コンストラクタ
//=================================================
CScoreLerper::CScoreLerper(int nPriority) : CScore(nPriority)
{
	m_nDestScore = NULL;
}

//=================================================
// デストラクタ
//=================================================
CScoreLerper::~CScoreLerper()
{
}

//=================================================
// 初期化処理
//=================================================
HRESULT CScoreLerper::Init(void)
{
	// スコアの初期化処理
	if (FAILED(CScore::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//=================================================
// 終了処理
//=================================================
void CScoreLerper::Uninit(void)
{
	// 終了処理
	CScore::Uninit();
}

//=================================================
// 更新処理
//=================================================
void CScoreLerper::Update(void)
{
	// スコアの取得
	int score = GetScore();

	// 割合を計算
	float fRateTime = (float)m_nCounter / (float)m_nTime;
	 
	// カウンターの制限
	if (m_nCounter >= m_nTime)
	{
		m_nCounter = m_nTime;
	}
	else
	{
		m_nCounter++;
	}

	// スコアを目的のスコアに近づける
	score += (int)((m_nDestScore - score) * fRateTime);

	// スコアの設定
	CScore::SetScore(score);

	// 更新処理
	CScore::Update();
}

//=================================================
// 描画処理
//=================================================
void CScoreLerper::Draw(void)
{
	// 描画処理
	CScore::Draw();
}

//=================================================
// スコアの設定処理
//=================================================
void CScoreLerper::SetDestScore(const int value, const int nTime)
{
	m_nDestScore += value;
	m_nTime = nTime;
	m_nCounter = NULL;
}
