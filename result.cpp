//===================================================
//
// リザルトシーン [result.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "result.h"
#include "meshfield.h"
#include "manager.h"
#include "title.h"
#include "input.h"
#include "fade.h"
#include "objectX.h"
#include "ResultCamera.h"
#include "ResultMenu.h"
#include "game.h"
#include "timer.h"
#include "light.h"
#include "player.h"
#include "ResultPlayer.h"
#include "bird.h"
#include "WinResultManager.h"
#include "background.h"
#include "ranking.h"
#include "sound.h"

using namespace Const; // 名前空間Constを使用
using namespace std; // 名前空間stdを使用

//===================================================
// コンストラクタ
//===================================================
CResultWin::CResultWin() : CScene(MODE_RESULT)
{
	m_pCamera = nullptr;
}

//===================================================
// デストラクタ
//===================================================
CResultWin::~CResultWin()
{
}

//===================================================
// 初期化処理
//===================================================
HRESULT CResultWin::Init(void)
{
	// 勝利時のリザルトのマネージャーの生成
	CWinResultManager::Create();

	// リザルトのカメラの生成
	m_pCamera = make_unique<CResultCamera>();
	m_pCamera->Init();

	// アレーナの生成
	CObjectX::Create(VEC3_NULL, "data/MODEL/field/arena.x", VEC3_NULL);

	// フィールドの生成
	CMeshField::Create(VEC3_NULL, 48, 48, D3DXVECTOR2(5000.0f, 5000.0f));

	// 背景の生成
	CBackGround::Create(D3DXVECTOR3(640.0f, 360.0f, 0.0f), D3DXVECTOR2(500.0f, 300.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.4f), CBackGround::TYPE_RESULT);

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CResultWin::Uninit(void)
{
	// カメラの破棄
	if (m_pCamera != nullptr)
	{
		m_pCamera->Uninit();
		m_pCamera.reset();
	}

	// ゲームマネージャーの取得
	auto pGameManager = CGameManager::GetInstance();

	if (pGameManager != nullptr)
	{
		// 破棄
		pGameManager->Uninit();
	}
}

//===================================================
// 更新処理
//===================================================
void CResultWin::Update(void)
{
	if (m_pCamera != nullptr)
	{
		// 更新処理
		m_pCamera->Update();
	}
}

//===================================================
// 描画処理
//===================================================
void CResultWin::Draw(void)
{
	if (m_pCamera != nullptr)
	{
		// 更新処理
		m_pCamera->SetCamera();
	}
}

//===================================================
// コンストラクタ
//===================================================
CResultLose::CResultLose() : CScene(MODE_RESULT)
{

}

//===================================================
// デストラクタ
//===================================================
CResultLose::~CResultLose()
{
}

//===================================================
// 初期化処理
//===================================================
HRESULT CResultLose::Init(void)
{
	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pSound != nullptr)
	{
		// 音の再生
		pSound->Play(CSound::SOUND_LABEL_RESULT_LOSE);
	}
	// リザルトマネージャーの生成
	CResultMenuManager::Create();

	// カメラクラスの定義
	m_pCamera = make_unique<CCamera>();
	m_pCamera->Init();
	m_pCamera->SetCamera(D3DXVECTOR3(0.0f, 200.0f, -400.0f), VEC3_NULL, D3DXVECTOR3(D3DX_PI * 0.65f, 0.0f, 0.0f));

	// ライトの取得
	CLight* pLight = CManager::GetLight();
	pLight->Init();

	// ポイントライトの設定処理
	pLight->SetPoint(D3DXVECTOR3(0.0f, 300.0f, 0.0f), 400.0f, D3DCOLOR_RGBA(255, 255, 255, 255), D3DCOLOR_RGBA(255, 255, 255, 255));

	// フィールドの生成
	CMeshField::Create(VEC3_NULL, 48, 48, D3DXVECTOR2(5000.0f, 5000.0f));

	// プレイヤーの生成
	CResultPlayer::Create(VEC3_NULL, VEC3_NULL);

	// 鳥の生成
	CBird::Create(D3DXVECTOR3(100.0f, 0.0f, 0.0f),CBird::TYPE_NORMAL,false);

	// 鳥の生成
	CBird::Create(D3DXVECTOR3(-100.0f, 0.0f, 0.0f), CBird::TYPE_NORMAL, false);

	// 鳥の生成
	CBird::Create(D3DXVECTOR3(-100.0f, 0.0f, -80.0f), CBird::TYPE_NORMAL, false);

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CResultLose::Uninit(void)
{
	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pSound != nullptr)
	{
		// 音の停止
		pSound->StopSound();
	}

	// ゲームマネージャーの取得
	auto pGameManager = CGameManager::GetInstance();

	if (pGameManager != nullptr)
	{
		// 破棄
		pGameManager->Uninit();
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
void CResultLose::Update(void)
{

}

//===================================================
// 描画処理
//===================================================
void CResultLose::Draw(void)
{
	// カメラの破棄
	if (m_pCamera != nullptr)
	{
		// カメラの設定処理
		m_pCamera->SetCamera();
	}
}
