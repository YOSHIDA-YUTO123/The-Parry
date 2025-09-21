//===================================================
//
// ランキングシーン [ranking.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "ranking.h"
#include "ResultCamera.h"
#include "input.h"
#include "manager.h"
#include "fade.h"
#include "title.h"
#include "meshfield.h"
#include "objectX.h"
#include "background.h"
#include "RankingScore.h"
#include "light.h"
#include "RankingManager.h"
#include "sound.h"

using namespace std;	// 名前空間stdの使用
using namespace Const;	// 名前空間Constの使用

//===================================================
// コンストラクタ
//===================================================
CRanking::CRanking() : CScene(MODE_RANKING)
{
	m_pCamera = nullptr;
}

//===================================================
// デストラクタ
//===================================================
CRanking::~CRanking()
{
}

//===================================================
// 初期化処理
//===================================================
HRESULT CRanking::Init(void)
{
	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pSound != nullptr)
	{
		// 音の再生
		pSound->Play(CSound::SOUND_LABEL_BGM_RANKING,0.3f);
	}
	// カメラの生成
	m_pCamera = make_unique<CResultCamera>();
	m_pCamera->Init();

	// ライトの取得
	CLight* pLight = CManager::GetLight();
	pLight->Init();

	// ライトの設定処理
	pLight->SetDirectional(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, -0.56f, 0.74f), D3DXVECTOR3(0.0f, 100.0f, 0.0f));
	pLight->SetDirectional(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, -0.56f, -0.74f), D3DXVECTOR3(0.0f, 100.0f, 0.0f));

	// アレーナの生成
	CObjectX::Create(VEC3_NULL, "data/MODEL/field/arena.x", VEC3_NULL);

	// フィールドの生成
	CMeshField::Create(VEC3_NULL, 48, 48, D3DXVECTOR2(5000.0f, 5000.0f));

	// 背景の生成
	CBackGround::Create(D3DXVECTOR3(640.0f, 360.0f, 0.0f), D3DXVECTOR2(500.0f, 300.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.4f), CBackGround::TYPE_RESULT);

	// 生成処理
	CRankingManager::Create();

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CRanking::Uninit(void)
{
	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pSound != nullptr)
	{
		pSound->StopSound();
	}

	// カメラの破棄
	if (m_pCamera != nullptr)
	{
		m_pCamera->Uninit();
		m_pCamera.reset();
	}
}

//===================================================
// 更新処理
//===================================================
void CRanking::Update(void)
{
	// キーボードの取得
	auto pKeyboard = CManager::GetInputKeyboard();

	// ジョイパッドの取得
	auto pJoypad = CManager::GetInputJoypad();

	// カメラの更新
	if (m_pCamera != nullptr)
	{
		m_pCamera->Update();
	}

	if (pKeyboard->GetTrigger(DIK_RETURN) || pJoypad->GetTrigger(pJoypad->JOYKEY_A))
	{
		// フェードの取得
		CFade* pFade = CManager::GetFade();

		if (pFade != nullptr)
		{
			// シーンの遷移
			pFade->SetFade(make_unique<CTitle>());
		}
	}
}

//===================================================
// 描画処理
//===================================================
void CRanking::Draw(void)
{
	// カメラの設定
	if (m_pCamera != nullptr)
	{
		m_pCamera->SetCamera();
	}
}
