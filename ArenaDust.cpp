//===================================================
//
// ステージの瓦礫 [ArenaDust.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "ArenaDust.h"
#include "meshfield.h"
#include "game.h"

using namespace Const; // 名前空間Const;

//===================================================
// コンストラクタ
//===================================================
CArenaDust::CArenaDust()
{
	m_move = VEC3_NULL;
}

//===================================================
// デストラクタ
//===================================================
CArenaDust::~CArenaDust()
{
}

//===================================================
// 生成処理
//===================================================
CArenaDust* CArenaDust::Create(const D3DXVECTOR3 pos, const float fAngle)
{
	CArenaDust* pArenaDust = new CArenaDust;

	pArenaDust->SetPosition(pos);
	
	// 初期化処理
	if (FAILED(pArenaDust->Init()))
	{
		pArenaDust->Uninit();
		pArenaDust = nullptr;
		return nullptr;
	}

	pArenaDust->SetRotation(D3DXVECTOR3(0.0f, fAngle, 0.0f));

	return pArenaDust;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CArenaDust::Init(void)
{
	// 初期化処理
	if (FAILED(CObjectX::Init()))
	{
		return E_FAIL;
	}

	// モデルのロード
	CObjectX::LoadModel("data/MODEL/obj/dust.x");

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CArenaDust::Uninit(void)
{
	// 終了処理
	CObjectX::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CArenaDust::Update(void)
{
	// メッシュフィールドの取得
	CMeshField* pMeshField = CGame::GetField();

	// 位置の取得
	D3DXVECTOR3 pos = CObjectX::GetPosition();

	pos += m_move;

	if (pMeshField != nullptr)
	{
		float fHeight = 0.0f;

		// 地面に乗ったら
		if (pMeshField->Collision(pos, &fHeight))
		{
			pos.y = fHeight;
		}
	}

	// 重力の設定
	m_move.y -= MAX_GRABITY;

	// 位置の設定
	CObjectX::SetPosition(pos);
}

//===================================================
// 描画処理
//===================================================
void CArenaDust::Draw(void)
{
	// 描画処理
	CObjectX::Draw();
}
