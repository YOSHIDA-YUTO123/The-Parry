//===================================================
//
// チュートリアル [tutorial.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "tutorial.h"
#include "meshfield.h"
#include"GameCamera.h"
#include"manager.h"
#include"light.h"
#include"player.h"
#include"block.h"
#include"BlockManager.h"

using namespace Const; // 名前空間Const

//***************************************************
// 静的メンバ変数宣言
//***************************************************
CGameCamera* CTutorial::m_pCamera = nullptr;	// カメラクラスへのポインタ
CMeshField* CTutorial::m_pMeshField = nullptr;	// メッシュフィールドへのポインタ

//===================================================
// コンストラクタ
//===================================================
CTutorial::CTutorial() : CScene(MODE_TUTORIAL)
{

}

//===================================================
// デストラクタ
//===================================================
CTutorial::~CTutorial()
{
}

//==================================================
// 初期化処理
//===================================================
HRESULT CTutorial::Init(void)
{
	m_pCamera = new CGameCamera;
	m_pCamera->Init();

	// ライトの取得
	CLight* pLight = CManager::GetLight();
	pLight->Init();

	// ライトの設定処理
	pLight->SetDirectional(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.25f, -0.56f, 0.74f), D3DXVECTOR3(0.0f, 100.0f, 0.0f));
	pLight->SetDirectional(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, -0.56f, -0.74f), D3DXVECTOR3(0.0f, 100.0f, 0.0f));
	//pLight->SetDirectional(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.5f, -0.56f, 0.0f), D3DXVECTOR3(0.0f, 100.0f, 0.0f));
	pLight->SetDirectional(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR3(-0.75f, -0.96f, 0.25f), D3DXVECTOR3(0.0f, 100.0f, 0.0f));

	// フィールドの生成
	m_pMeshField = CMeshField::Create(VEC3_NULL, 32, 32, D3DXVECTOR2(2500.0f, 2500.0f));

	// プレイヤーの生成
	CPlayer::Create(VEC3_NULL);

	// ブロックマネージャーの取得
	auto pBlockManager = CBlockManager::GetInstance();

	// 取得できたら
	if (pBlockManager != nullptr)
	{
		// ロード
		pBlockManager->Load();

		//// ブロックの生成
		//CBlock *pBlock = CBlock::Create(D3DXVECTOR3(500.0f, 0.0f, 0.0f), "wall000.x");
		//pBlockManager->SetBlock(pBlock);
	}


	return S_OK;
}

//==================================================
// 終了処理
//===================================================
void CTutorial::Uninit(void)
{
	m_pMeshField = nullptr;

	// カメラの破棄
	if (m_pCamera != nullptr)
	{
		m_pCamera->Uninit();
		delete m_pCamera;
		m_pCamera = nullptr;
	}
}

//==================================================
// 更新処理
//===================================================
void CTutorial::Update(void)
{
	// カメラの更新
	if (m_pCamera != nullptr)
	{
		m_pCamera->Update();
	}
}

//==================================================
// 描画処理
//===================================================
void CTutorial::Draw(void)
{
	// カメラの設定
	if (m_pCamera != nullptr)
	{
		m_pCamera->SetCamera();
	}
}
