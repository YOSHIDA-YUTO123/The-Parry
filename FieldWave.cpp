//================================================
//
// フィールドのウェーブの処理をするクラス [FieldWave.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "FieldWave.h"
#include "meshfield.h"

//================================================
// コンストラクタ
//================================================
CMeshFieldWave::CMeshFieldWave()
{
	// 値のクリア
	ZeroMemory(&m_Confing, sizeof(m_Confing));
	ZeroMemory(&m_Info, sizeof(m_Info));

}

//================================================
// デストラクタ
//================================================
CMeshFieldWave::~CMeshFieldWave()
{
}

//================================================
// 生成処理
//================================================
CMeshFieldWave* CMeshFieldWave::Create(Config config)
{
	// 波の生成
	CMeshFieldWave* pWave = new CMeshFieldWave;

	// nullだったら
	if (pWave == nullptr) return nullptr;

	pWave->Init(); // 初期化処理

	// パラメーターの設定
	pWave->m_Confing = config;

	// 最初の高さを設定
	pWave->m_Info.fStartHeight = config.fHeight;

	return pWave;
}

//================================================
// 初期化処理
//================================================
void CMeshFieldWave::Init(void)
{
	m_Info.nCounter = 0;
}

//================================================
// 更新処理
//================================================
bool CMeshFieldWave::Update(CMeshField* pMeshField, const int nNumVtx)
{
	// 波のカウンターを進める
	m_Info.nCounter++;

	// 相対値を求める
	float fRate = (float)m_Info.nCounter / (float)m_Confing.nTime;

	// 速さに応じた波の幅を設定
	m_Info.fTime += m_Confing.fSpeed;

	// 波の高さをだんだん0に近づける
	m_Confing.fHeight = m_Info.fStartHeight + (0.0f - m_Info.fStartHeight) * fRate;

	for (int nCnt = 0; nCnt < nNumVtx; nCnt++)
	{
		// 頂点座標の取得
		D3DXVECTOR3 pos = pMeshField->GetVtxPos(nCnt);

		// 震源地から頂点までの差分
		D3DXVECTOR3 diff = m_Confing.epicenter - pos;

		// 距離をもとめる
		float dis = sqrtf((diff.x * diff.x) + (diff.z * diff.z));

		// 時間に応じた距離を設定
		float fTimeInRadius = m_Confing.fInRadius + m_Info.fTime;
		float fTimeOutRadius = m_Confing.fOutRadius + m_Info.fTime;

		// 範囲内だったら
		if (dis >= fTimeInRadius && dis <= fTimeOutRadius)
		{
			// 高さの設定
			float dest = m_Confing.fHeight + sinf(dis * m_Confing.fcoef);

			// 目的の高さに近づける
			pos.y += (dest - pos.y) * 0.1f;
		}
		else
		{
			// 目的の高さに近づける
			pos.y += (0.0f - pos.y) * 0.05f;
		}

		// 頂点座標の設定
		pMeshField->SetVtxPos(pos, nCnt);
	}

	// 最大値だったら
	if (m_Info.nCounter >= m_Confing.nTime)
	{
		// カウンターをリセット
		m_Info.nCounter = 0;

		return false;
	}

	return true;
}