//================================================
//
// パーティクル2D [partile2D.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "particle2D.h"
#include "effect2D.h"
using namespace Const; // 名前空間Constの使用

//================================================
// コンストラクタ
//================================================
CParticle2D::CParticle2D()
{
	m_nPosRange = NULL;
	m_nPriority = 6;
	m_pos = VEC3_NULL;
	m_Speed = VEC3_NULL;
	m_col = WHITE;
	m_fAngle = NULL;
	m_fRadius = NULL;
	m_nLife = NULL;
	m_nTime = NULL;
	m_nNum = 1;
}

//================================================
// デストラクタ
//================================================
CParticle2D::~CParticle2D()
{
}

//================================================
// 生成処理
//================================================
CParticle2D* CParticle2D::Create(const D3DXVECTOR3 pos, const float fRadius, const D3DXCOLOR col)
{
	CParticle2D* pParticle = new CParticle2D;

	pParticle->m_pos = pos;
	pParticle->m_fRadius = fRadius;
	pParticle->m_col = col;

	// 初期化処理
	if (FAILED(pParticle->Init()))
	{
		pParticle->Uninit();
		pParticle = nullptr;
		return nullptr;
	}

	return pParticle;
}

//================================================
// 初期化処理
//================================================
HRESULT CParticle2D::Init(void)
{
	return S_OK;
}

//================================================
// 終了処理
//================================================
void CParticle2D::Uninit(void)
{
	// 自分自身の破棄
	CObject::Release();
}

//================================================
// 更新処理
//================================================
void CParticle2D::Update(void)
{
	// 発生数分回す
	for (int nCnt = 0; nCnt < m_nNum; nCnt++)
	{
		D3DXVECTOR3 pos = m_pos;

		if (m_nPosRange != 0)
		{
			// 位置の選出
			pos.x = m_pos.x + static_cast<float>(rand() % m_nPosRange) - m_nPosRange / 2;
			pos.y = m_pos.y + static_cast<float>(rand() % m_nPosRange) - m_nPosRange / 2;
		}

		// 移動量をint型に変換
		int nMoveX = static_cast<int>(m_Speed.x);
		int nMoveY = static_cast<int>(m_Speed.y);

		float fMoveX = 0.0f;
		float fMoveY = 0.0f;

		if (nMoveX != 0)
		{
			// 移動量を求める
			fMoveX = static_cast<float>(rand() / static_cast<float>(RAND_MAX)) * nMoveX + m_Speed.x * 0.5f;
		}
		if (nMoveY != 0)
		{
			// 移動量を求める
			fMoveY = static_cast<float>(rand() / static_cast<float>(RAND_MAX)) * nMoveY + m_Speed.y * 0.5f;
		}
		// 半径をint型に変換
		int nRadius = static_cast<int>(m_fRadius);

		float fRadius = 0.0f;

		if (nRadius != 0)
		{
			// 半径を求める
			fRadius = static_cast<float>(rand() % nRadius + m_fRadius * 0.5f);
		}

		// 角度をint型に変換
		int nAngle = static_cast<int>(m_fAngle * 2);

		float fAngle = 0.0f;

		if (nAngle != 0)
		{
			// 角度を求める
			fAngle = static_cast<float>(rand() % nAngle - m_fAngle) * 0.01f;
		}

		// 寿命を求める
		int nLife = rand() % m_nLife + m_nLife / 2;

		// 移動量
		D3DXVECTOR3 moveWk = VEC3_NULL;
 
		// 移動量を求める
		moveWk.x = sinf(fAngle) * fMoveX;
		moveWk.y = cosf(fAngle) * fMoveY;

		// 発生時間内だったら
		if (m_nTime > 0)
		{
			// エフェクトの生成
			CEffect2D *pEffect = CEffect2D::Create(pos, D3DXVECTOR2(fRadius, fRadius), m_col, m_nPriority);
			pEffect->SetParam(nLife, moveWk);
		}
	}

	m_nTime--;

	if (m_nTime <= 0)
	{
		Uninit();
	}
}

//================================================
// 描画処理
//================================================
void CParticle2D::Draw(void)
{
}

//================================================
// パラメータの設定処理
//================================================
void CParticle2D::SetParam(const float fAngle, const int nLife, const int nTime, const int nNum, const D3DXVECTOR3 Speed)
{
	m_fAngle = fAngle;
	m_nLife = nLife;
	m_nTime = nTime;
	m_nNum = nNum;
	m_Speed = Speed;
}

//================================================
// パラメータの設定処理
//================================================
void CParticle2D::SetParam(const int nPriority,const int nPosRenge)
{
	m_nPosRange = nPosRenge;
	m_nPriority = nPriority;
}
