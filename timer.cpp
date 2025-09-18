//================================================
//
// タイマー [timer.cpp]
// Author:YUTO YOSHIDA
//
//=================================================

//*************************************************
// インクルードファイル
//*************************************************
#include "timer.h"
#include "manager.h"
#include "renderer.h"
#include "object.h"
#include"number.h"

using namespace Const; // 名前空間Constを使用

//*************************************************
// 定数宣言
//*************************************************
namespace
{
	constexpr const char* NUMBER_TEXTURE = "data/TEXTURE/number/number005.png";
	constexpr const char* CORON_TEXTURE = "data/TEXTURE/number/coron.png";
};

//================================================
// コンストラクタ
//================================================
CTime::CTime(int nPriority) : CObject(nPriority)
{
	m_nTime = NULL;

	m_nMinute = NULL;
	m_nSecond = NULL;
	m_fWidth = NULL;
	m_fHeight = NULL;
	m_nCounter = NULL;
	m_pos = VEC3_NULL;
	m_bCountUp = false;

	memset(m_apMinute, NULL,sizeof(m_apMinute));
	memset(m_apSecond, NULL, sizeof(m_apSecond));

}

//================================================
// デストラクタ
//================================================
CTime::~CTime()
{
}

//================================================
// 初期化処理
//================================================
HRESULT CTime::Init(void)
{
	// 色の設定
	CTime::SetColor(WHITE);

	// 横幅の割合
	float fRateWidth = m_fWidth / NUM_DIGIT;

	for (int nCnt = 0; nCnt < NUM_DIGIT; nCnt++)
	{
		// 分のメモリの確保
		if (m_apMinute[nCnt] == nullptr)
		{
			m_apMinute[nCnt] = new CNumber;

			// 初期化処理
			m_apMinute[nCnt]->Init(D3DXVECTOR3(m_pos.x - (fRateWidth * 2.0f * nCnt), m_pos.y, 0.0f), D3DXVECTOR2(fRateWidth, m_fHeight));

			// テクスチャの割合
			m_apMinute[nCnt]->SetTextureID(NUMBER_TEXTURE);
		}
	}

	// オフセットの設定
	float offPosX = fRateWidth * NUM_DIGIT;

	// コロンの作成
	auto pCoron = CObject2D::Create(fRateWidth * 0.4f, m_fHeight * 0.75f, D3DXVECTOR3(offPosX + m_pos.x, m_pos.y, 0.0f));
	pCoron->SetTextureID(CORON_TEXTURE);

	offPosX = fRateWidth * (3.0f * NUM_DIGIT);

	for (int nCnt = 0; nCnt < NUM_DIGIT; nCnt++)
	{
		// 秒のメモリの確保
		if (m_apSecond[nCnt] == nullptr)
		{
			m_apSecond[nCnt] = new CNumber;

			// 初期化処理
			m_apSecond[nCnt]->Init(D3DXVECTOR3(offPosX + m_pos.x - (fRateWidth * 2.0f * nCnt), m_pos.y, 0.0f),D3DXVECTOR2(fRateWidth, m_fHeight));

			// テクスチャの割合
			m_apSecond[nCnt]->SetTextureID(NUMBER_TEXTURE);
		}
	}

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CTime::Uninit(void)
{
	for (int nCnt = 0; nCnt < NUM_DIGIT; nCnt++)
	{
		if (m_apMinute[nCnt] != NULL)
		{
			// 終了処理
			m_apMinute[nCnt]->Uninit();

			delete m_apMinute[nCnt];

			m_apMinute[nCnt] = NULL;
		}
	}

	for (int nCnt = 0; nCnt < NUM_DIGIT; nCnt++)
	{
		if (m_apSecond[nCnt] != NULL)
		{
			// 終了処理
			m_apSecond[nCnt]->Uninit();

			delete m_apSecond[nCnt];

			m_apSecond[nCnt] = NULL;
		}
	}

	Release();
}

//================================================
// 更新処理
//================================================
void CTime::Update(void)
{
	// カウントアップ
	UpdateCountUp();

	SetMinute(m_nTime);
	SetSecond(m_nTime);

	for (int nCnt = 0; nCnt < NUM_DIGIT; nCnt++)
	{
		if (m_apMinute[nCnt] != NULL)
		{
			// 更新処理
			m_apMinute[nCnt]->Update();
			int nData = 10;

			int nDivi = (int)pow((double)nData, (double)nCnt);

			// 桁の数字を求める
			int nNumber = m_nMinute / nDivi % nData;

			m_apMinute[nCnt]->SetUV(nNumber);
		}
	}

	for (int nCnt = 0; nCnt < NUM_DIGIT; nCnt++)
	{
		if (m_apSecond[nCnt] != NULL)
		{
			// 更新処理
			m_apSecond[nCnt]->Update();

			int nData = 10;

			int nDivi = (int)pow((double)nData, (double)nCnt);

			// 桁の数字を求める
			int nNumber = m_nSecond / nDivi % nData;

			m_apSecond[nCnt]->SetUV(nNumber);
		}
	}

}

//================================================
// 描画処理
//================================================
void CTime::Draw(void)
{
	for (int nCnt = 0; nCnt < NUM_DIGIT; nCnt++)
	{
		if (m_apMinute[nCnt] != NULL)
		{
			// 描画処理
			m_apMinute[nCnt]->Draw();
		}
	}

	for (int nCnt = 0; nCnt < NUM_DIGIT; nCnt++)
	{
		if (m_apSecond[nCnt] != NULL)
		{
			// 描画処理
			m_apSecond[nCnt]->Draw();
		}
	}

}

//================================================
// 大きさの設定処理
//================================================
void CTime::SetSize(const float fWidth, const float fHeight)
{
	m_fWidth = fWidth;
	m_fHeight = fHeight;
}

//================================================
// カウントダウンの更新処理
//================================================
void CTime::UpdateCountDown(void)
{
	// 1フレーム
	if (m_nCounter >= FRAME)
	{
		m_nCounter = 0;

		// 秒が0じゃなかったら
		if (m_nSecond > 0)
		{
			// 秒を減らす
			m_nSecond--;
		}
		// 分が0じゃなかったら
		else if (m_nSecond <= 0 && m_nMinute != 0)
		{
			// もう一周する
			m_nSecond = FRAME - 1;

			// 分を減らす
			m_nMinute--;
		}
	}


	// 60を超えたら
	if (m_nSecond >= FRAME)
	{
		m_nMinute++; // 分を1増やす

		// 余りが0じゃなかったら(60より上だったら)
		if (m_nSecond % FRAME != 0)
		{
			int Time = m_nSecond % FRAME; // 余りを求める

			m_nSecond = 0; // 0にする
			m_nSecond += Time; // 余りを加算する
		}
		else
		{
			m_nSecond = 0; // 0にする
		}
	}
}

//================================================
// カウントアップ
//================================================
void CTime::UpdateCountUp(void)
{
	//// 秒が0じゃなかったら
	//if (m_nSecond < 59)
	//{
	//	// 秒を減らす
	//	m_nSecond++;
	//}
	//// 分が0じゃなかったら
	//else if (m_nSecond >= 59 && m_nMinute != 99)
	//{
	//	// もう一周する
	//	m_nSecond = 0;

	//	// 分を減らす
	//	m_nMinute++;
	//}
}

//================================================
// タイマーの生成処理
//================================================
CTime* CTime::Create(const D3DXVECTOR3 pos, const float fWidth, const float fHeight, const int nTime, bool bCountUp)
{
	CTime* pTime = nullptr;

	// タイマーのメモリの確保
	pTime = new CTime;

	// NULLだったら飛ばす
	if (pTime == nullptr) return nullptr;

	// 大きさの設定処理
	pTime->SetSize(fWidth, fHeight);

	// 位置の設定
	pTime->SetPosition(pos);

	// 初期化
	pTime->Init();

	pTime->m_nTime = nTime;

	pTime->m_bCountUp = bCountUp;

	return pTime;
}
