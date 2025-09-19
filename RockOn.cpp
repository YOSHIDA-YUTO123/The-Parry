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
#include "enemy.h"
#include "TrainingEnemy.h"
#include "manager.h"
#include "renderer.h"

using namespace Const;

//================================================
// コンストラクタ
//================================================
CRockOn::CRockOn()
{
	m_DestSize = VEC2_NULL;
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
CRockOn* CRockOn::Create(void)
{
	CRockOn* pRockOn = new CRockOn;

	pRockOn->m_DestSize = pRockOn->SIZE;

	pRockOn->SetSize(pRockOn->SIZE * 10.0f);

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
	// 現在のモードの取得
	CScene::MODE mode = CManager::GetMode();

	// 敵の取得
	CEnemy* pEnemy = CGame::GetEnemy();

	// 練習用の敵の取得
	CTrainingEnemy* pTrainingEnemy = CTutorial::GetTrainingEnemy();

	// ゲームのカメラの取得
	CGameCamera* pCamera = nullptr;

	// ロックオンする位置
	D3DXVECTOR3 RockOnPos = VEC3_NULL;

	if (mode == CScene::MODE_TUTORIAL)
	{
		if (pTrainingEnemy != nullptr)
		{
			// 位置の取得
			RockOnPos = pTrainingEnemy->GetModelPos(CTrainingEnemy::MODEL_CHEST);
		}
		// カメラの取得
		pCamera = CTutorial::GetCamera();
	}
	else if (mode == CScene::MODE_GAME)
	{
		// カメラの取得
		pCamera = CGame::GetCamera();

		if (pEnemy != nullptr)
		{
			// 胸の位置
			RockOnPos = pEnemy->GetModelPos(CEnemy::MODEL_CHEST);
		}
	}

	// 大きさの取得
	D3DXVECTOR2 Size = CObjectBillboard::GetSize();

	// 目的の大きさに近づける
	Size += (m_DestSize - Size) * 0.2f;

	// 大きさの設定
	CObjectBillboard::SetSize(Size);

	// 位置の設定
	CObjectBillboard::UpdateVertexPos(RockOnPos);

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
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ゼットテスト
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATEREQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// 描画処理
	CObjectBillboard::Draw();

	// ゼットテスト
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}
