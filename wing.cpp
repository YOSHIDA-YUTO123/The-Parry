//===================================================
//
// 羽 [Wing.cpp]
// Author:YUTO YOSHIDA
//
// << 鳥から羽を出すクラス >>
// 
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "wing.h"
#include "velocity.h"

//***************************************************
// 定数宣言
//***************************************************
namespace
{
	constexpr float SCAL_RANGE = 4.0f;		// 大きさの範囲
	constexpr float GRABITY = 0.08f;		// 重力
	constexpr float MOVE_RANGE = 15.0f;		// 移動量の範囲
	constexpr float SPEED = 0.06f;			// 往復運動の速さ
	constexpr int LIFE_RANGE = 360;			// 寿命の範囲
	constexpr int ANGLE_RANGE = 314;		// 羽の向きの範囲
};

//===================================================
// コンストラクタ
//===================================================
CWing::CWing()
{
	m_move = Const::VEC3_NULL;
	m_fCounter = NULL;
	m_nLife = NULL;
	m_nMaxLife = NULL;
	m_pMove = nullptr;
	m_Scal = Const::VEC3_NULL;
}

//===================================================
// デストラクタ
//===================================================
CWing::~CWing()
{
}

//===================================================
// 生成処理
//===================================================
CWing* CWing::Create(const D3DXVECTOR3 pos)
{
	CWing* pWing = new CWing;

	// 位置の設定
	pWing->SetPosition(pos);

	// 初期化処理
	if (FAILED(pWing->Init()))
	{
		pWing->Uninit();
		pWing = nullptr;
		return nullptr;
	}

	return pWing;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CWing::Init(void)
{
	// 初期化処理
	if (FAILED(CObjectX::Init()))
	{
		return E_FAIL;
	}

	// 羽の読み込み
	CObjectX::LoadModel("data/MODEL/effect/wing.x");

	// 移動量の生成
	m_pMove = std::make_unique<CVelocity>();

	// 寿命を選出
	m_nLife = rand() % LIFE_RANGE + (LIFE_RANGE / 2);
	m_nMaxLife = m_nLife;

	// 移動量を選出
	float fMove = static_cast<float>(rand() % static_cast<int>(MOVE_RANGE)) + MOVE_RANGE * 0.5f;

	// 角度を求める
	float fAngle = (static_cast<float>(rand() % (ANGLE_RANGE * 2) - ANGLE_RANGE)) * 0.01f;

	// 角度を設定
	CObjectX::SetRotation(D3DXVECTOR3(fAngle, fAngle, fAngle));

	// 移動量の設定
	m_move.x = sinf(fAngle) * fMove;
	m_move.z = cosf(fAngle) * fMove;

	// 大きさ
	float fScal = static_cast<float>(rand() % static_cast<int>(SCAL_RANGE)) + 1.0f;

	// スケールの設定
	m_Scal = { fScal,fScal,fScal };

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CWing::Uninit(void)
{
	m_pMove.reset();

	// 終了処理
	CObjectX::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CWing::Update(void)
{
	// 位置の取得
	D3DXVECTOR3 pos = CObjectX::GetPosition();

	// nullだったら処理しない
	if (m_pMove == nullptr)
	{
		Uninit();

		return;
	}

	// 位置の更新
	pos += m_pMove->Get();

	// 重力
	m_pMove->Gravity(-GRABITY);

	m_fCounter += SPEED;

	// 移動量の取得
	D3DXVECTOR3 moveWk = m_pMove->Get();

	// 移動量の設定
	moveWk.x = sinf(m_fCounter) * m_move.x;
	moveWk.z = cosf(m_fCounter) * m_move.z;

	// 移動量の設定
	m_pMove->Set(moveWk);

	// 位置の設定
	CObjectX::SetPosition(pos);

	m_nLife--;

	// 寿命が尽きたら
	if (m_nLife <= 0)
	{
		Uninit();
	}
}

//===================================================
// 描画処理
//===================================================
void CWing::Draw(void)
{
	// 割合を求める
	float fRate = m_nLife / static_cast<float>(m_nMaxLife);

	// 大きさの設定
	CObjectX::SetUpMatrix(m_Scal);

	// 描画処理(透明度設定)
	CObjectX::SetUpDraw(fRate);
}
