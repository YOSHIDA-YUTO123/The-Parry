//===================================================
//
// タイトルシーン [title.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "title.h"
#include "meshfield.h"
#include "manager.h"
#include "input.h"
#include "game.h"
#include "fade.h"
#include "objectX.h"
#include "player.h"
#include "TitleCamera.h"
#include "TitlePlayer.h"
#include "light.h"
#include "effect.h"
#include "particle.h"
#include "debugproc.h"
#include "TitleLogo.h"
#include "TitleMenu.h"
#include "edit.h"
#include "tutorial.h"
#include "opening.h"
#include "ranking.h"
#include "sound.h"

using namespace Const; // 名前空間Constを使用
using namespace std;   // 名前空間stdを使用

//***************************************************
// 静的メンバ変数宣言
//***************************************************
CTitleCamera* CTitle::m_pCamera = nullptr; // タイトルのカメラのクラスへのポインタ

//===================================================
// コンストラクタ
//===================================================
CTitle::CTitle() : CScene(MODE_TITLE)
{

}

//===================================================
// デストラクタ
//===================================================
CTitle::~CTitle()
{

}

//===================================================
// 初期化処理
//===================================================
HRESULT CTitle::Init(void)
{
	// カメラの生成
	m_pCamera = new CTitleCamera;
	m_pCamera->Init();

	// ライトの取得
	CLight* pLight = CManager::GetLight();
	pLight->Init();

	// タイトルロゴ
	CTitleLogo::Create(D3DXVECTOR3(350.0f,100.0f,0.0f),D3DXVECTOR2(300.0f,100.0f));

	// タイトルメニューの生成
	CTitleMenuManager::Create();

	// ポイントライトの設定処理
	pLight->SetPoint(D3DXVECTOR3(-200.0f, 300.0f, 0.0f),1000.0f, D3DCOLOR_RGBA(255, 255, 255,255), D3DCOLOR_RGBA(255, 255, 255, 255));

	// フィールドの設定
	CMeshField::Create(VEC3_NULL, 48, 48, D3DXVECTOR2(5500.0f, 5500.0f));

	// アリーナの生成
	CObjectX::Create(VEC3_NULL, "data/MODEL/field/TitleArena.x",D3DXVECTOR3(0.0f,0.0f,0.0f));

	// トーチの生成
	CObjectX::Create(D3DXVECTOR3(250.0f,150.0f,165.0f), "data/MODEL/obj/torch.x", D3DXVECTOR3(-D3DX_PI * 0.15f, 0.0f, 0.0f));

	// トーチの生成
	CObjectX::Create(D3DXVECTOR3(250.0f, 150.0f, -165.0f), "data/MODEL/obj/torch.x", D3DXVECTOR3(D3DX_PI * 0.15f, 0.0f, 0.0f));

	// プレイヤーの生成
	CTitlePlayer::Create(VEC3_NULL,D3DXVECTOR3(0.0f,-D3DX_PI * 0.5f,0.0f));

	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pSound != nullptr)
	{
		// bgmの再生
		pSound->Play(CSound::SOUND_LABEL_TITLE_BGM, 0.5f);
	}
	//auto pObj2D = CObject2DMT::Create(D3DXVECTOR3(640.0f, 360.0f, 0.0f), D3DXVECTOR2(350.0f, 150.0f));
	//pObj2D->SetTextureID("data/TEXTURE/GageFrame/playerHpFrame.png", "data/TEXTURE/gradation/wave000.png");

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CTitle::Uninit(void)
{
	// ライトの取得
	CLight* pLight = CManager::GetLight();
	
	if (pLight != nullptr)
	{
		// 破棄
		pLight->Uninit();
	}

	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pSound != nullptr)
	{
		// 音の再生の停止
		pSound->StopSound();
	}
	// カメラの破棄
	if (m_pCamera != nullptr)
	{
		m_pCamera->Uninit();
		delete m_pCamera;
		m_pCamera = nullptr;
	}
}

//===================================================
// 更新処理
//===================================================
void CTitle::Update(void)
{
	// パーティクルの生成
	auto pParticle = CParticle3DNormal::Create(D3DXVECTOR3(250.0f, 175.0f, 150.0f), 10.0f, D3DCOLOR_RGBA(240, 122, 27, 255));
	pParticle->SetParticle(2.0f, 60, 10, 1, 40);
	pParticle->SetParam(CEffect3D::TYPE_FIRE,0);

	// パーティクルの生成
	pParticle = CParticle3DNormal::Create(D3DXVECTOR3(250.0f, 175.0f, -150.0f), 10.0f, D3DCOLOR_RGBA(240, 122, 27, 255));
	pParticle->SetParticle(2.0f, 60, 10, 1, 40);
	pParticle->SetParam(CEffect3D::TYPE_FIRE,0);

	// カメラの更新処理
	if (m_pCamera != nullptr)
	{
		m_pCamera->Update();
	}
#ifdef _DEBUG

	if (CManager::GetInputKeyboard()->GetTrigger(DIK_F9))
	{
		CManager::GetFade()->SetFade(make_unique<CRanking>());
	}
#endif // _DEBUG

}

//===================================================
// 描画処理
//===================================================
void CTitle::Draw(void)
{
#ifdef _DEBUG
	// カメラの位置
	D3DXVECTOR3 cameraposV = m_pCamera->GetPosV();
	D3DXVECTOR3 cameraposR = m_pCamera->GetPosR();
	D3DXVECTOR3 camerarot = m_pCamera->GetRotaition();

	// デバッグ情報
	CDebugProc::Print("視点の座標 : [ %.2f ] [ %.2f ] [ %.2f ] \n", cameraposV.x, cameraposV.y, cameraposV.z);
	CDebugProc::Print("注視点の座標 : [ %.2f ] [ %.2f ] [ %.2f ] \n", cameraposR.x, cameraposR.y, cameraposR.z);
	CDebugProc::Print("カメラの向き : [ %.2f ] [ %.2f ] [ %.2f ] \n", camerarot.x, camerarot.y, camerarot.z);
#endif // _DEBUG

	// カメラの設定処理
	if (m_pCamera != nullptr)
	{
		m_pCamera->SetCamera();
	}
}
