//===================================================
//
// ゲームシーン [game.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "game.h"
#include "meshfield.h"
#include "dome.h"
#include"cylinder.h"
#include"player.h"
#include"objectX.h"
#include"enemy.h"
#include"Obstacle.h"
#include "manager.h"
#include"input.h"
#include"result.h"
#include"fade.h"
#include "debugproc.h"
#include "Gage.h"
#include "Observer.h"
#include"GageFrame.h"

using namespace Const; // 名前空間Constを使用
using namespace std; // 名前空間stdを使用

//***************************************************
// 静的メンバ変数宣言
//***************************************************
CMeshField* CGame::m_pMeshField = nullptr;		// メッシュフィールドへのポインタ
CPlayerGame* CGame::m_pPlayer = nullptr;			// プレイヤーへのポインタ
CMeshCylinder* CGame::m_pCylinder = nullptr;	// メッシュシリンダーへのポインタ
CGame::STATE CGame::m_state = STATE_NORMAL;     // ゲームの状態

//===================================================
// コンストラクタ
//===================================================
CGame::CGame() : CScene(MODE_GAME)
{
	m_nCounterState = NULL;
}

//===================================================
// デストラクタ
//===================================================
CGame::~CGame()
{

}

//===================================================
// 初期化処理
//===================================================
HRESULT CGame::Init(void)
{
	// フィールドの設定
	m_pMeshField = CMeshField::Create(VEC3_NULL, 48, 48, D3DXVECTOR2(5500.0f, 5500.0f));

	// ドームの生成
	CMeshDome::Create(VEC3_NULL, 10, 10, 60000.0f, 20000.0f);

	// ドームの生成
	CMeshDome::Create(VEC3_NULL, 10, 10, 60000.0f, -20000.0f);

	// シリンダーの生成
	m_pCylinder = CMeshCylinder::Create(D3DXVECTOR3(0.0f, 0.0f, 0.0f), 32, 1, 1900.0f, 1900.0f);

	// プレイヤーの生成
	m_pPlayer = CPlayerGame::Create(D3DXVECTOR3(1.0f, 0.0f, -1500.0f));

	// ゲージのフレームの生成
	CGageFrame::Create(D3DXVECTOR3(220.0f, 36.0f, 0.0f), D3DXVECTOR2(200.0f, 25.0f),CGageFrame::TYPE_HP_PLAYER);

	// HPゲージの生成
	auto pGage = CHpGage::Create(D3DXVECTOR3(103.0f, 36.0f, 0.0f), D3DXVECTOR2(310.0f, 17.0f), D3DXCOLOR(0.0f,1.0f,0.0f,1.0f), CPlayerGame::MAX_LIFE,true);

	// Hpゲージのオブザーバーの設定
	CHpObserver *observer = new CHpObserver(pGage);

	// オブザーバーの設定
	m_pPlayer->SetObserver(observer);

	// オブザーバーの設定
	pGage->SetObserver(observer);

	// アリーナの生成
	CObjectX::Create(VEC3_NULL, "data/MODEL/field/arena.x");

	// 敵の生成
	auto pEnemy = CEnemy::Create(D3DXVECTOR3(0.0f, 0.0f, 1500.0f));

	// ゲージのフレームの生成
	CGageFrame::Create(D3DXVECTOR3(1000.0f, 36.0f, 0.0f), D3DXVECTOR2(200.0f, 25.0f), CGageFrame::TYPE_HP_ENEMY);

	// HPゲージの生成
	pGage = CHpGage::Create(D3DXVECTOR3(1115.0f, 36.0f, 0.0f), D3DXVECTOR2(308.0f, 17.0f), D3DXCOLOR(1.0f, 0.2f, 0.0f, 1.0f), CEnemy::MAX_LIFE, false);

	// Hpゲージのオブザーバーの設定
	observer = new CHpObserver(pGage);

	// オブザーバーの設定
	pEnemy->SetObserver(observer);

	// オブザーバーの設定
	pGage->SetObserver(observer);

	// 面の設定
	int face = CCollisionAABB::FACE_LEFT;

	// スパイクトラップ
	CSpikeTrap::Create(D3DXVECTOR3(1840.0f, 0.0f, 0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f) ,face);

	// 面の設定
	face = CCollisionAABB::FACE_RIGHT;

	// スパイクトラップ
	CSpikeTrap::Create(D3DXVECTOR3(-1840.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f,D3DX_PI, 0.0f), face);

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CGame::Uninit(void)
{
	m_pMeshField = nullptr;
	m_pCylinder = nullptr;
	m_pPlayer = nullptr;
}

//===================================================
// 更新処理
//===================================================
void CGame::Update(void)
{
	// フェードの取得
	CFade* pFade = CManager::GetFade();

	switch (m_state)
	{
	case STATE_NORMAL:
		break;
	case STATE_END:
		m_nCounterState++;

		if (m_nCounterState >= 60 && pFade != nullptr)
		{
			// 新しいモードの設定
			pFade->SetFade(make_unique<CResult>());

			m_state = STATE_NORMAL;
		}
		break;
	default:
		break;
	}

	// キーボードの取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	if (pKeyboard->GetTrigger(DIK_F9))
	{
		SetState(STATE_END);
	}
}

//===================================================
// 描画処理
//===================================================
void CGame::Draw(void)
{
	CDebugProc::Print("デバッグ 非表示      : [ F2 ]\n");

	CDebugProc::Print("影の表示 : [ 2 ]\n");

	for (int nCntPriority = 0; nCntPriority < NUM_PRIORITY; nCntPriority++)
	{
		int nNumAll = CObject::GetNumObject(nCntPriority);

		// 文字の表示
		CDebugProc::Print("[ Priority = %d : Numobj = %d]\n", nCntPriority, nNumAll);
	}

	CDebugProc::Print("ワイヤーフレーム : [ F6 ]\n");
}
