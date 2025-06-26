//===================================================
//
// パーティクル [particle.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "particle.h"
#include "effect.h"
#include "math.h"
#include"meshfield.h"
#include "manager.h"

//===================================================
// コンストラクタ
//===================================================
CParticle3D::CParticle3D()
{
	m_col = WHITE;
	m_fRadius = NULL;
	m_move = VEC3_NULL;
	m_pos = VEC3_NULL;
	m_nLife = NULL;
	m_nMaxLife = NULL;
	m_nTime = NULL;
	m_nNumParticle = NULL;
	m_fSpeed = NULL;
	memset(m_apEffect, NULL, sizeof(m_apEffect));
}

//===================================================
// デストラクタ
//===================================================
CParticle3D::~CParticle3D()
{
}

//===================================================
// 生成処理
//===================================================
CParticle3D* CParticle3D::Create(const D3DXVECTOR3 pos, const D3DXCOLOR col, const int nLife, const float fRadius, const int nNumParticle, const int nTime, const float fSpeed)
{
	CParticle3D* pParticle = nullptr;

	// 3Dオブジェクトの生成
	pParticle = new CParticle3D;

	// 優先順位の取得
	int nPriority = pParticle->GetPriority();

	// 現在のオブジェクトの最大数
	const int nNumAll = CObject::GetNumObject(nPriority);

	// オブジェクトが最大数まであったら
	if (nNumAll >= MAX_OBJECT && pParticle != nullptr)
	{
		// 自分自身の破棄
		pParticle->Release();

		// nullにしておく
		pParticle = nullptr;

		return nullptr;
	}

	if (pParticle == nullptr) return nullptr;

	// 初期化処理
	pParticle->Init();

	// 設定処理
	pParticle->m_pos = pos;
	pParticle->m_fRadius = fRadius;
	pParticle->m_nLife = nLife;
	pParticle->m_nMaxLife = nLife;
	pParticle->m_col = col;
	pParticle->m_nNumParticle = nNumParticle;
	pParticle->m_nTime = nTime;
	pParticle->m_fSpeed = fSpeed;

	return pParticle;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CParticle3D::Init(void)
{
	m_col = WHITE;
	m_fRadius = NULL;
	m_move = VEC3_NULL;
	m_nLife = NULL;

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CParticle3D::Uninit(void)
{
	for (int nCnt = 0; nCnt < MAX_PARTICLE; nCnt++)
	{
		// エフェクトの破棄
		if (m_apEffect[nCnt] != nullptr)
		{
			m_apEffect[nCnt] = nullptr;
		}
	}

	// 自分自身の破棄
	Release();
}

//===================================================
// 更新処理
//===================================================
void CParticle3D::Update(void)
{
	// 範囲内にクランプする
	m_nNumParticle = Clamp(m_nNumParticle, 0, MAX_PARTICLE);

	for (int nCnt = 0; nCnt < m_nNumParticle; nCnt++)
	{
		D3DXVECTOR3 pos = m_pos;
		
		// 角度の選出
		float fAngleX = (float)(rand() % 629 - 314) * 0.01f;
		float fAngleY = (float)(rand() % 629 - 314) * 0.01f;

		// 速さをint型に変換
		int speed = (int)m_fSpeed;

		// 移動量
		float fMove = (float)(rand() % speed + m_fSpeed * 0.5f);

		m_move.x = sinf(fAngleX) * sinf(fAngleY) * fMove;
		m_move.y = cosf(fAngleX) * fMove;
		m_move.z = cosf(fAngleX) * sinf(fAngleY) * fMove;

		// 半径をint型に変換
		int radius = (int)m_fRadius;

		// 半径を選出
		float fRadius = (float)(rand() % radius + m_fRadius * 0.5f);

		// 寿命の選出
		int nLife = rand() % m_nMaxLife - (int)(m_nMaxLife * 0.5f);

		if (m_nTime > 0 && m_apEffect[nCnt] == nullptr)
		{
			// エフェクトの生成
			m_apEffect[nCnt] = CEffect3D::Create(pos, fRadius, m_move, m_col, nLife);
		}
	}

	m_nTime--;
	m_nLife--;

	if (m_nLife <= 0)
	{
		Uninit();
		return;
	}
}

//===================================================
// 描画処理
//===================================================
void CParticle3D::Draw(void)
{

}
