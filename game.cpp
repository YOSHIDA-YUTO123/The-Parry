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

using namespace Const; // 名前空間Constを使用

//***************************************************
// 静的メンバ変数宣言
//***************************************************
CMeshField* CGame::m_pMeshField = nullptr;		// メッシュフィールドへのポインタ
CPlayer* CGame::m_pPlayer = nullptr;			// プレイヤーへのポインタ
CMeshCylinder* CGame::m_pCylinder = nullptr;	// メッシュシリンダーへのポインタ

//===================================================
// コンストラクタ
//===================================================
CGame::CGame() : CScene(MODE_GAME)
{
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
	m_pPlayer = CPlayer::Create(D3DXVECTOR3(1.0f, 0.0f, -1500.0f));

	CObjectX::Create(VEC3_NULL, "data/MODEL/field/arena.x");

	CEnemy::Create(D3DXVECTOR3(0.0f, 0.0f, 1500.0f));

	// 面の設定
	int face = CCollisionAABB::FACE_LEFT;

	// スパイクトラップ
	CSpikeTrap::Create(D3DXVECTOR3(1840.0f, 0.0f, 0.0f), face);

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CGame::Uninit(void)
{

}

//===================================================
// 更新処理
//===================================================
void CGame::Update(void)
{
	// キーボードの取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	if (pKeyboard->GetTrigger(DIK_F9))
	{
		// 新しいモードの設定
		CManager::SetMode(new CResult);
	}
}

//===================================================
// 描画処理
//===================================================
void CGame::Draw(void)
{

}
