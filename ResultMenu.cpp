//===================================================
//
// リザルトメニュー [ResultMenu.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "ResultMenu.h"
#include "manager.h"
#include "input.h"
#include "math.h"
#include "background.h"
#include "fade.h"
#include "title.h"
#include "game.h"
#include "sound.h"

using namespace Const; // 名前空間Constを使用
using namespace std; // 名前空間stdを使用

//***************************************************
// 静的メンバ変数宣言
//***************************************************
CResultMenuManager* CResultMenuManager::m_pInstance = nullptr; // 自分のインスタンス

//===================================================
// コンストラクタ
//===================================================
CResultMenu::CResultMenu(const MENU menu)
{
	m_Menu = menu;
}

//===================================================
// デストラクタ
//===================================================
CResultMenu::~CResultMenu()
{
}

//===================================================
// 生成処理
//===================================================
CResultMenu* CResultMenu::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const MENU menu)
{
	CResultMenu* pMenu = nullptr;

	switch (menu)
	{
	case MENU_RETRY:
		pMenu = new CResultMenuRetry;
		break;
	case MENU_QUIT:
		pMenu = new CResultMenuQuit;
		break;
	default:
		return nullptr;
	}

	pMenu->Init();
	pMenu->SetPosition(pos);
	pMenu->SetSize(Size.x, Size.y);
	pMenu->SetVtx(WHITE);

	return pMenu;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CResultMenu::Init(void)
{
	// 初期化処理
	if (FAILED(CObject2D::Init()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CResultMenu::Uninit(void)
{
	// 終了処理
	CObject2D::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CResultMenu::Update(void)
{
	// リザルトマネージャーの取得
	auto pResultMenuManager = CResultMenuManager::GetInstance();

	// 取得できなかったら処理しない
	if (pResultMenuManager == nullptr) return;

	// 自分のメニューの取得
	MENU myMenu = GetMenu();

	// 選択中のメニューの取得
	MENU selectMenu = pResultMenuManager->GetMenu();

	// 自分のメニューと選択中のメニューが同じだったら
	if (myMenu == selectMenu)
	{
		// 色の設定
		CObject2D::SetColor(D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f));
	}
	else
	{
		// 色の設定
		CObject2D::SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.4f));
	}
}

//===================================================
// 描画処理
//===================================================
void CResultMenu::Draw(void)
{
	// 描画処理
	CObject2D::Draw();
}

//===================================================
// コンストラクタ
//===================================================
CResultMenuRetry::CResultMenuRetry() : CResultMenu(MENU_RETRY)
{

}

//===================================================
// デストラクタ
//===================================================
CResultMenuRetry::~CResultMenuRetry()
{
}

//===================================================
// 初期化処理
//===================================================
HRESULT CResultMenuRetry::Init(void)
{
	// 初期化処理
	if (FAILED(CResultMenu::Init()))
	{
		return E_FAIL;
	}

	// テクスチャのIDの登録
	CResultMenu::SetTextureID("data/TEXTURE/pause/pause_retry.png");

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CResultMenuRetry::Uninit(void)
{
	// 終了処理
	CResultMenu::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CResultMenuRetry::Update(void)
{
	// 更新処理
	CResultMenu::Update();

	// リザルトマネージャーの取得
	auto pResultMenuManager = CResultMenuManager::GetInstance();

	// 取得できなかったら処理しない
	if (pResultMenuManager == nullptr) return;

	// 自分のメニューの取得
	MENU myMenu = GetMenu();

	// 選択中のメニューの取得
	MENU selectMenu = pResultMenuManager->GetMenu();

	// 自分のメニューと選択中のメニューが同じだったら
	if (myMenu == selectMenu)
	{
		// パッドの取得
		CInputJoypad* pJoypad = CManager::GetInputJoypad();

		// キーボードの取得
		CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

		if (pKeyboard->GetTrigger(DIK_RETURN) || pJoypad->GetTrigger(pJoypad->JOYKEY_A))
		{
			// 音の取得
			CSound* pSound = CManager::GetSound();

			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_ENTER);
			}

			CFade* pFade = CManager::GetFade();

			// 新しいモードの設定
			pFade->SetFade(make_unique<CGame>());
		}
	}
}

//===================================================
// 描画処理
//===================================================
void CResultMenuRetry::Draw(void)
{
	// 描画処理
	CResultMenu::Draw();
}

//===================================================
// コンストラクタ
//===================================================
CResultMenuQuit::CResultMenuQuit() : CResultMenu(MENU_QUIT)
{

}

//===================================================
// デストラクタ
//===================================================
CResultMenuQuit::~CResultMenuQuit()
{
}

//===================================================
// 初期化処理
//===================================================
HRESULT CResultMenuQuit::Init(void)
{
	// 初期化処理
	if (FAILED(CResultMenu::Init()))
	{
		return E_FAIL;
	}

	// テクスチャのIDの登録
	CResultMenu::SetTextureID("data/TEXTURE/pause/pause_quit.png");

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CResultMenuQuit::Uninit(void)
{
	// 終了処理
	CResultMenu::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CResultMenuQuit::Update(void)
{
	// 更新処理
	CResultMenu::Update();

	// リザルトマネージャーの取得
	auto pResultMenuManager = CResultMenuManager::GetInstance();

	// 取得できなかったら処理しない
	if (pResultMenuManager == nullptr) return;

	// 自分のメニューの取得
	MENU myMenu = GetMenu();

	// 選択中のメニューの取得
	MENU selectMenu = pResultMenuManager->GetMenu();

	// 自分のメニューと選択中のメニューが同じだったら
	if (myMenu == selectMenu)
	{
		// パッドの取得
		CInputJoypad* pJoypad = CManager::GetInputJoypad();

		// キーボードの取得
		CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

		if (pKeyboard->GetTrigger(DIK_RETURN) || pJoypad->GetTrigger(pJoypad->JOYKEY_A))
		{
			// 音の取得
			CSound* pSound = CManager::GetSound();

			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_ENTER);
			}
			CFade* pFade = CManager::GetFade();

			// 新しいモードの設定
			pFade->SetFade(make_unique<CTitle>());
		}
	}
}

//===================================================
// 描画処理
//===================================================
void CResultMenuQuit::Draw(void)
{
	// 描画処理
	CResultMenu::Draw();
}

//===================================================
// コンストラクタ
//===================================================
CResultMenuManager::CResultMenuManager()
{
	m_Menu = CResultMenu::MENU_RETRY;
}

//===================================================
// デストラクタ
//===================================================
CResultMenuManager::~CResultMenuManager()
{
}

//===================================================
// 生成処理
//===================================================
void CResultMenuManager::Create(void)
{
	// リザルトのメニュー
	using MENU = CResultMenu::MENU;

	// 自分が生成されていなかったら
	if (m_pInstance == nullptr)
	{
		// 自分を生成
		m_pInstance = new CResultMenuManager;
		m_pInstance->Init();


		//// メニューの生成
		//for (int nCnt = MENU::MENU_RETRY; nCnt <= MENU::MENU_QUIT; nCnt++)
		//{
		//	// メニュー型にキャスト
		//	MENU menu = static_cast<MENU>(nCnt);

		// 2Dオブジェクトの生成
		CObject2D::Create(300.0f, 100.0f, D3DXVECTOR3(640.0f, 100.0f, 0.0f))->SetTextureID("data/TEXTURE/result/result_gameOver.png");

		//	// メニューの生成
		//	CResultMenu::Create(D3DXVECTOR3(300.0f + nCnt * 200.0f, 500.0f, 0.0f), D3DXVECTOR2(150.0f, 100.0f), menu);
		//}
		 
		// メニューの生成
		CResultMenu::Create(D3DXVECTOR3(400.0f, 500.0f, 0.0f), D3DXVECTOR2(150.0f, 100.0f), MENU::MENU_RETRY);
		CResultMenu::Create(D3DXVECTOR3(900.0f, 500.0f, 0.0f), D3DXVECTOR2(150.0f, 100.0f), MENU::MENU_QUIT);
	}
}

//===================================================
// 初期化処理
//===================================================
HRESULT CResultMenuManager::Init(void)
{
	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CResultMenuManager::Uninit(void)
{
	// 自分自身の破棄
	CObject::Release();
	m_pInstance = nullptr;
}

//===================================================
// 更新処理
//===================================================
void CResultMenuManager::Update(void)
{
	// キーボードの取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	// パッドの取得
	CInputJoypad* pJoypad = CManager::GetInputJoypad();

	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pKeyboard->GetTrigger(DIK_RIGHT) || pJoypad->GetTrigger(pJoypad->JOYKEY_RIGHT))
	{
		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_MENU);
		}
		// 次のメニューへ
		m_Menu = static_cast<CResultMenu::MENU>(m_Menu + 1);
	}
	else if (pKeyboard->GetTrigger(DIK_LEFT) || pJoypad->GetTrigger(pJoypad->JOYKEY_LEFT))
	{
		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_MENU);
		}

		// 次のメニューへ
		m_Menu = static_cast<CResultMenu::MENU>(m_Menu - 1);
	}

	// 範囲の制限
	m_Menu = Wrap(m_Menu, CResultMenu::MENU_RETRY, CResultMenu::MENU_QUIT);
}

//===================================================
// 描画処理
//===================================================
void CResultMenuManager::Draw(void)
{
}
