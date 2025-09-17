//================================================
//
// ロックオン時にビルボードを描画する [RockOn.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "RockOn.h"
#include "game.h"
#include "GameCamera.h"
#include "manager.h"
#include "tutorial.h"

using namespace Const;

//================================================
// コンストラクタ
//================================================
CRockOn::CRockOn()
{
	m_DestSize = VEC2_NULL;
	m_col = WHITE;
	m_fCounter = NULL;
}

//================================================
// デストラクタ
//================================================
CRockOn::~CRockOn()
{
}

//================================================
// 生成処理
//================================================
CRockOn* CRockOn::Create(const D3DXVECTOR2 Size)
{
	CRockOn* pRockOn = new CRockOn;

	pRockOn->m_DestSize = Size;

	pRockOn->SetSize(Size * 10.0f);

	// 初期化処理
	if (FAILED(pRockOn->Init()))
	{
		pRockOn->Uninit();
		pRockOn = nullptr;
		return nullptr;
	}

	return pRockOn;
}

//================================================
// 初期化処理
//================================================
HRESULT CRockOn::Init(void)
{
	// 初期化処理
	if (FAILED(CObjectBillboard::Init()))
	{
		return E_FAIL;
	}

	// テクスチャのIDの設定
	CObjectBillboard::SetTextureID("data/TEXTURE/UI/rockon.png");

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CRockOn::Uninit(void)
{
	// 終了処理
	CObjectBillboard::Uninit();
}

//================================================
// 更新処理
//================================================
void CRockOn::Update(void)
{
	// カメラの取得
	CGameCamera* pCamera = nullptr;

	// 現在のモードの取得
	CScene::MODE mode = CManager::GetMode();

	if (mode == CScene::MODE_TUTORIAL)
	{
		// カメラの取得
		pCamera = CTutorial::GetCamera();
	}
	else if (mode == CScene::MODE_GAME)
	{
		// カメラの取得
		pCamera = CGame::GetCamera();
	}

	// 取得できなかったら処理しない
	if (pCamera == nullptr) return;

	// 大きさの取得
	D3DXVECTOR2 Size = CObjectBillboard::GetSize();

	// 目的の大きさに近づける
	Size += (m_DestSize - Size) * 0.2f;

	// 大きさの設定
	CObjectBillboard::SetSize(Size);

	// 現在の注視点の取得
	D3DXVECTOR3 pos = pCamera->GetPosR();

	// 位置の設定
	CObjectBillboard::UpdateVertexPos(pos);

	m_fCounter += 0.01f;

	// 透明度を設定
	m_col.a = 1.0f - fabsf(sinf(m_fCounter));

	// 色の設定
	CObjectBillboard::SetColor(m_col);

	// ロックオン状態じゃないなら
	if (pCamera->GetState() != pCamera->STATE_ROCKON)
	{
		// 破棄
		Uninit();
	}
}

//================================================
// 描画処理
//================================================
void CRockOn::Draw(void)
{
	// 描画処理
	CObjectBillboard::Draw();
}
