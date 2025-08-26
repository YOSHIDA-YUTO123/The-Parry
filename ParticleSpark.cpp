//===================================================
//
// 火花パーティクル [ParticleSpark.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "ParticleSpark.h"
#include"manager.h"
#include"slow.h"
#include"EffectSpark.h"

using namespace Const; // 名前空間Constを使用

//===================================================
// コンストラクタ
//===================================================
CParticleSpark::CParticleSpark()
{
}

//===================================================
// デストラクタ
//===================================================
CParticleSpark::~CParticleSpark()
{
}

//===================================================
// 生成処理
//===================================================
CParticleSpark* CParticleSpark::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const D3DXCOLOR col)
{
	// パーティクルの生成
	auto pParticle = new CParticleSpark;

	// 情報の設定
	pParticle->Init();
	pParticle->m_Size = Size;

	// 情報の取得
	Info info = pParticle->GetInfo();
	info.pos = pos;
	info.fRadius = Size.y;
	
	info.col = col;

	// 情報の設定処理
	pParticle->SetInfo(info);

	return pParticle;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CParticleSpark::Init(void)
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
void CParticleSpark::Uninit(void)
{
	// 終了処理
	CParticle3D::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CParticleSpark::Update(void)
{
	// 情報の取得
	Info info = GetInfo();

	for (int nCnt = 0; nCnt < info.nNumParticle; nCnt++)
	{
		// 位置の取得
		D3DXVECTOR3 pos = info.pos;

		// 色の取得
		D3DXCOLOR col = info.col;

		int nAngleMax = (info.nAngle * 2) + 1;

		// 角度の選出
		float fAngleX = (float)(rand() % nAngleMax - info.nAngle) * 0.01f;
		float fAngleY = (float)(rand() % nAngleMax - info.nAngle) * 0.01f;

		// 速さをint型に変換
		int speed = (int)info.fSpeed;

		// 移動量
		float fMove = (float)(rand() % speed + (speed * 0.5f));

		D3DXVECTOR3 moveWk = VEC3_NULL;

		// 球状に放出する
		moveWk.x = sinf(fAngleX) * sinf(fAngleY) * fMove;
		moveWk.y = cosf(fAngleX) * fMove;
		moveWk.z = sinf(fAngleX) * cosf(fAngleY) * fMove;

		// 半径をint型に変換
		int height = (int)m_Size.y;

		// 半径を選出
		float fHeight = (float)(rand() % height + (height * 0.5f));

		// 寿命の選出
		int nLife = rand() % info.nLife + (int)(info.nLife * 0.5f);

		// スローモーションのレベルを掛ける
		nLife *= (int)CManager::GetSlow()->GetLevel(true);

		if (info.nTime > 0)
		{
			// エフェクトの生成
			auto pEffect = CEffectSpark::Create(pos, D3DXVECTOR2(m_Size.x, fHeight), col);

			// エフェクトの設定処理
			pEffect->SetParam(nLife, moveWk,0.1f);
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
// 描画処理
//===================================================
void CParticleSpark::Draw(void)
{
	// 描画処理
	CParticle3D::Draw();
}
