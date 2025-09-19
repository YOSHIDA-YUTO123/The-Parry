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
#include "math.h"
#include"meshfield.h"
#include "manager.h"
#include"slow.h"

using namespace Const;							// 名前空間Constを使用する
using namespace std;							// 名前空間stdを使用する

constexpr int MIN_PARTICLE = 1;	// パーティクルの最低数

//===================================================
// コンストラクタ
//===================================================
CParticle3D::CParticle3D()
{
	// 0にする
	ZeroMemory(&m_Info, sizeof(m_Info));
	
	m_nMaxLife = NULL;
}

//===================================================
// デストラクタ
//===================================================
CParticle3D::~CParticle3D()
{
}

//===================================================
// 初期化処理
//===================================================
HRESULT CParticle3D::Init(void)
{
	m_Info.nAngle = 314;
	m_Info.col = WHITE;
	m_Info.nNumParticle = MIN_PARTICLE;
	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CParticle3D::Uninit(void)
{	
	// 自分自身の破棄
	Release();
}

////===================================================
//// 更新処理
////===================================================
//void CParticle3D::Update(void)
//{
//}

//===================================================
// 描画処理
//===================================================
void CParticle3D::Draw(void)
{

}

//===================================================
// パーティクルの設定処理
//===================================================
void CParticle3D::SetParticle(const float fSpeed, const int nLife, const int nNumParticle, const int nTime, const int nAngle)
{
	// 情報の取得
	Info info = GetInfo();

	// 設定処理
	info.fSpeed = fSpeed;
	m_nMaxLife = nLife;
	info.nLife = nLife;
	info.nNumParticle = nNumParticle;
	info.nTime = nTime;
	info.nAngle = nAngle;
	// 情報の設定
	SetInfo(info);
}

//===================================================
// コンストラクタ
//===================================================
CParticle3DNormal::CParticle3DNormal()
{
	m_nPosRange = NULL;
	m_fInertia = 0.01f;
	m_type = CEffect3D::TYPE::TYPE_NORAML;
}

//===================================================
// デストラクタ
//===================================================
CParticle3DNormal::~CParticle3DNormal()
{
}

//===================================================
// 生成処理
//===================================================
CParticle3DNormal* CParticle3DNormal::Create(const D3DXVECTOR3 pos, const float fRadius, const D3DXCOLOR col)
{
	// パーティクルの生成
	auto pParticle = new CParticle3DNormal;

	// 情報の設定
	pParticle->Init();

	// 情報の取得
	Info info = pParticle->GetInfo();
	info.pos = pos;
	info.fRadius = fRadius;
	info.col = col;

	pParticle->SetInfo(info);

	return pParticle;
}

//===================================================
// パーティクルの設定処理
//===================================================
void CParticle3DNormal::SetParam(CEffect3D::TYPE type, const int nPosRange, const float fInertia)
{
	m_nPosRange = nPosRange;
	m_type = type;
	m_fInertia = fInertia;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CParticle3DNormal::Init(void)
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
void CParticle3DNormal::Uninit(void)
{
	// 終了処理
	CParticle3D::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CParticle3DNormal::Update(void)
{
	// 情報の取得
	Info info = GetInfo();

	for (int nCnt = 0; nCnt < info.nNumParticle; nCnt++)
	{
		// 位置の取得
		D3DXVECTOR3 pos = info.pos;
		
		if (m_nPosRange != 0)
		{
			// ランダムな位置を求める
			pos.x = info.pos.x + static_cast<float>(rand() % m_nPosRange) - m_nPosRange / 2;
			pos.y = info.pos.y + static_cast<float>(rand() % m_nPosRange) - m_nPosRange / 2;
			pos.z = info.pos.z + static_cast<float>(rand() % m_nPosRange) - m_nPosRange / 2;
		}

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
		int radius = (int)info.fRadius;

		// 半径を選出
		float fRadius = (float)(rand() % radius + (radius * 0.5f));

		// 寿命の選出
		int nLife = rand() % info.nLife - (int)(info.nLife * 0.5f);

		// スローモーションのレベルを掛ける
		nLife *= (int)CManager::GetSlow()->GetLevel(true);

		if (info.nTime > 0)
		{
			// エフェクトの生成
			auto pEffect = CEffect3D::Create(pos,fRadius,col,m_type);

			// エフェクトの設定処理
			pEffect->Set(nLife, moveWk,m_fInertia);
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