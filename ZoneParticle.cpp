//===================================================
//
// 集中パーティクル [ZoneParticle.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "ZoneParticle.h"
#include"math.h"

using namespace math;  // 名前空間mathを使用する
using namespace Const; // 名前空間Constを使用する

//===================================================
// コンストラクタ
//===================================================
CZoneParticle3D::CZoneParticle3D()
{
	m_DestPos = VEC3_NULL;
	m_nRandomPosRange = NULL;
}

//===================================================
// デストラクタ
//===================================================
CZoneParticle3D::~CZoneParticle3D()
{
}

//===================================================
// 生成処理
//===================================================
CZoneParticle3D* CZoneParticle3D::Create(const D3DXVECTOR3 pos, const float fRadius, const D3DXCOLOR col)
{
	// パーティクルの生成
	auto pParticle = new CZoneParticle3D;

	// 情報の設定
	pParticle->Init();

	// 情報の取得
	Info info = pParticle->GetInfo();
	info.pos = pos;
	info.fRadius = fRadius;
	info.col = col;

	// 情報の設定処理
	pParticle->SetInfo(info);

	return pParticle;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CZoneParticle3D::Init(void)
{
	// 初期化処理
	if (FAILED(CParticle3D::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CZoneParticle3D::Uninit(void)
{
	// 終了処理
	CParticle3D::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CZoneParticle3D::Update(void)
{
	// 情報の取得
	CParticle3D::Info info = CParticle3D::GetInfo();

	// パーティクルの粒子の数分回す
	for (int nCnt = 0; nCnt < info.nNumParticle; nCnt++)
	{
		// 最大の範囲の設定
		int nRangeMax = m_nRandomPosRange * 2;

		// ランダムな位置の選出
		float fRandomPosX = static_cast<float>(rand() % nRangeMax) - m_nRandomPosRange;
		float fRandomPosY = static_cast<float>(rand() % nRangeMax) - m_nRandomPosRange;
		float fRandomPosZ = static_cast<float>(rand() % nRangeMax) - m_nRandomPosRange;

		// 位置の設定
		D3DXVECTOR3 pos = info.pos + D3DXVECTOR3(fRandomPosX, fRandomPosY, fRandomPosZ);

		// 色の取得
		D3DXCOLOR col = info.col;

		// 方向ベクトルの取得
		D3DXVECTOR3 dir = GetVector(m_DestPos, pos);

		// 一度int型に変換する,最大の範囲の設定
		int nMaxSpeed = static_cast<int>(info.fSpeed) * 2;

		// ランダムな速さ
		float fRandomSpeed = static_cast<float>(rand() % nMaxSpeed) + info.fSpeed;

		// ランダムな速さを設定
		float fSpeed = fRandomSpeed;

		// 移動量の設定
		D3DXVECTOR3 move = dir * fSpeed;

		// 一度int型に変換する,最大の範囲の設定
		int nMaxRadius = static_cast<int>(info.fRadius) * 2;

		// ランダムな半径
		float fRandomRadius = static_cast<float>(rand() % nMaxRadius) + info.fRadius;

		// 最大の寿命の取得
		int nLifeMax = CParticle3D::GetMaxLife();

		// 最大の範囲の設定
		int nMaxLife = nLifeMax * 2;

		// ランダムな寿命の設定
		int nRandomLife = rand() % nMaxLife - nLifeMax;

		if (info.nTime > 0)
		{
			// エフェクトの生成
			auto pEffect = CEffect3D::Create(pos, fRandomRadius, col);
			pEffect->Set(nRandomLife, move);
			pEffect->SetTextureID("data/TEXTURE/effect/effect000.jpg");
		}
	}

	// 継続時間,寿命の減少
	info.nTime--;

	// 0になったら消す
	if (info.nTime <= 0)
	{
		Uninit();
		return;
	}
	else
	{
		// 情報の設定
		SetInfo(info);
	}
}

//===================================================
// 設定処理
//===================================================
void CZoneParticle3D::SetZone(D3DXVECTOR3 DestPos, const int nRandomPosRange)
{
	m_DestPos = DestPos;
	m_nRandomPosRange = nRandomPosRange;
}

