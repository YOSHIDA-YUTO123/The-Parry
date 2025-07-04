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
	m_pMove = nullptr;
	m_pos = VEC3_NULL;
	m_nLife = NULL;
	m_nMaxLife = NULL;
	m_nTime = NULL;
	m_nNumParticle = NULL;
	m_fSpeed = NULL;
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
		pParticle->Uninit();

		// nullにしておく
		pParticle = nullptr;

		return nullptr;
	}

	if (pParticle == nullptr) return nullptr;

	// 初期化処理
	pParticle->Init();

	// 移動量の生成
	pParticle->m_pMove = new CVelocity;

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
	m_nLife = NULL;

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CParticle3D::Uninit(void)
{
	for (int nCnt = 0; nCnt < (int)m_apEffect.size(); nCnt++)
	{
		// エフェクトの破棄
		if (m_apEffect[nCnt] != nullptr)
		{
			m_apEffect[nCnt] = nullptr;
		}
	}

	// 移動量の破棄
	if (m_pMove != nullptr)
	{
		delete m_pMove;
		m_pMove = nullptr;
	}

	// 自分自身の破棄
	Release();
}

//===================================================
// 更新処理
//===================================================
void CParticle3D::Update(void)
{
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

		D3DXVECTOR3 moveWk = VEC3_NULL;

		moveWk.x = sinf(fAngleX) * sinf(fAngleY) * fMove;
		moveWk.y = cosf(fAngleX) * fMove;
		moveWk.z = cosf(fAngleX) * sinf(fAngleY) * fMove;

		m_pMove->Set(moveWk);

		// 半径をint型に変換
		int radius = (int)m_fRadius;

		// 半径を選出
		float fRadius = (float)(rand() % radius + m_fRadius * 0.5f);

		// 寿命の選出
		int nLife = rand() % m_nMaxLife - (int)(m_nMaxLife * 0.5f);


		if (m_nTime > 0)
		{
			CEffect3D* pEffect = nullptr;

			// エフェクトの生成
			pEffect = CEffect3D::Create(pos, fRadius, moveWk, m_col, nLife);

			m_apEffect.push_back(pEffect);
		}
	}

	m_nTime--;
	m_nLife--;

	// 終了処理
	Uninit();	
}

//===================================================
// 描画処理
//===================================================
void CParticle3D::Draw(void)
{

}
