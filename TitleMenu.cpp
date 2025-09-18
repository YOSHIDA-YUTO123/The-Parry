//===================================================
//
// タイトルメニュー [TitleMenu.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "TitleMenu.h"
#include "manager.h"
#include"input.h"
#include"math.h"
#include"fade.h"
#include "game.h"
#include<memory>
//#include"game.h"
#include"opening.h"
#include "tutorial.h"
#include "sound.h"

using namespace math;  // 名前空間mathを使用
using namespace Const; // 名前空間Constを使用
using namespace std; // 名前空間stdを使用

//***************************************************
// 静的メンバ変数宣言
//***************************************************
CTitleMenuManager* CTitleMenuManager::m_pInstance = nullptr; // 自分のインスタンス

//===================================================
// コンストラクタ
//===================================================
CTitleMenu::CTitleMenu(const MENU menu)
{
	m_col = WHITE;
	m_fCounter = NULL;
	m_Menu = menu;
}

//===================================================
// デストラクタ
//===================================================
CTitleMenu::~CTitleMenu()
{
}

//===================================================
// 生成処理
//===================================================
CTitleMenu* CTitleMenu::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size,const MENU menu)
{
	// メニューの生成
	CTitleMenu* pMenu = nullptr;

	switch (menu)
	{
	case MENU_START:
		pMenu = new CTitleStart;
		break;
	case MENU_TUTORIAL:
		pMenu = new CTitleTutorial;
		break;
	case MENU_QUIT:
		pMenu = new CTitleQuit;
		break;
	default:
		return nullptr;
	}

	// 初期化に失敗したら
	if (FAILED(pMenu->Init()))
	{
		pMenu->Uninit();
		pMenu = nullptr;
		return nullptr;
	}

	pMenu->SetPosition(pos);
	pMenu->SetSize(Size.x, Size.y);
	pMenu->SetVtx(WHITE);
	pMenu->m_BaseSize = Size;

	return pMenu;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CTitleMenu::Init(void)
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
void CTitleMenu::Uninit(void)
{
	// 終了処理
	CObject2D::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CTitleMenu::Update(void)
{
	// タイトルマネージャーの取得
	auto pTitleManager = CTitleMenuManager::GetInstance();

	// 取得できなかったら処理しない
	if (pTitleManager == nullptr) return;

	// 大きさの取得
	D3DXVECTOR2 Size = CObject2D::GetSize();

	// 位置の取得
	D3DXVECTOR3 pos = CObject2D::GetPosition();

	// カウンターを加算
	m_fCounter += 0.03f;

	// 種類が同じだったら
	if (m_Menu == pTitleManager->GetMenu())
	{
		D3DXVECTOR2 destSize = m_BaseSize * 1.3f; // 拡大する

		Size += (destSize - Size) * 0.1f;

		// 大きさの設定
		CObject2D::SetSize(Size);

		// 透明度の変更
		m_col.a = 1.0f - fabsf(sinf(m_fCounter) * 0.5f);

		// 位置の設定
		pTitleManager->SetPosition(D3DXVECTOR3(pos.x - Size.x * 1.3f, pos.y, pos.z));
	}
	else
	{
		// リセット
		m_col.a = 1.0f;

		m_fCounter = NULL;

		Size += (m_BaseSize - Size) * 0.1f;

		// 大きさの設定
		CObject2D::SetSize(Size);
	}

	// 色の設定
	CObject2D::SetColor(m_col);

	// 頂点座標の更新
	pTitleManager->UpdateVertex(); 
	CObject2D::UpdateVertex();
}

//===================================================
// 描画処理
//===================================================
void CTitleMenu::Draw(void)
{
	// 描画処理
	CObject2D::Draw();
}

//===================================================
// コンストラクタ
//===================================================
CTitleStart::CTitleStart() : CTitleMenu(MENU_START)
{
}

//===================================================
// デストラクタ
//===================================================
CTitleStart::~CTitleStart()
{
}

//===================================================
// 初期化処理
//===================================================
HRESULT CTitleStart::Init(void)
{
	// 初期化処理
	if (FAILED(CTitleMenu::Init()))
	{
		return E_FAIL;
	}

	// テクスチャのIDの設定
	SetTextureID("data/TEXTURE/title/title_menu_start.png");

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CTitleStart::Uninit(void)
{
	// 終了処理
	CTitleMenu::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CTitleStart::Update(void)
{
	// 更新処理
	CTitleMenu::Update();

	// キーボードの取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	// パッドの取得
	CInputJoypad* pJoypad = CManager::GetInputJoypad();

	// タイトルマネージャーの取得
	auto pTitleManager = CTitleMenuManager::GetInstance();

	// 取得できなかったら処理しない
	if (pTitleManager == nullptr) return;

	// 自分のメニュー
	MENU myMenu = GetMenu();

	// 選択中のメニュー
	MENU selectMenu = pTitleManager->GetMenu();

	// フェードの取得
	CFade* pFade = CManager::GetFade();

	// 選択されているメニューと自分のメニューが同じだったら
	if (myMenu == selectMenu && pFade != nullptr && pFade->GetState() == CFade::FADE_NONE)
	{
		if (pKeyboard->GetTrigger(DIK_RETURN) || pJoypad->GetTrigger(pJoypad->JOYKEY_A))
		{
			// 音の取得
			CSound* pSound = CManager::GetSound();

			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_ENTER);
			}

			// スタートを押した
			pTitleManager->SetStart(true);

			// 新しいモードの設定
			pFade->SetFade(make_unique<COpening>());
		}
	}
}

//===================================================
// 描画処理
//===================================================
void CTitleStart::Draw(void)
{
	// 描画処理
	CTitleMenu::Draw();
}

//===================================================
// コンストラクタ
//===================================================
CTitleQuit::CTitleQuit() : CTitleMenu(MENU_QUIT)
{
}

//===================================================
// デストラクタ
//===================================================
CTitleQuit::~CTitleQuit()
{
}

//===================================================
// 初期化処理
//===================================================
HRESULT CTitleQuit::Init(void)
{
	// 初期化処理
	if (FAILED(CTitleMenu::Init()))
	{
		return E_FAIL;
	}

	// テクスチャのIDの設定
	SetTextureID("data/TEXTURE/title/title_menu_quit.png");

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CTitleQuit::Uninit(void)
{
	// 終了処理
	CTitleMenu::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CTitleQuit::Update(void)
{
	// 更新処理
	CTitleMenu::Update();

	// キーボードの取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	// パッドの取得
	CInputJoypad* pJoypad = CManager::GetInputJoypad();

	// タイトルマネージャーの取得
	auto pTitleManager = CTitleMenuManager::GetInstance();

	// 取得できなかったら処理しない
	if (pTitleManager == nullptr) return;

	// 自分のメニュー
	MENU myMenu = GetMenu();

	// 選択中のメニュー
	MENU selectMenu = pTitleManager->GetMenu();

	// 選択されているメニューと自分のメニューが同じだったら
	if (myMenu == selectMenu)
	{
		if (pKeyboard->GetTrigger(DIK_RETURN) || pJoypad->GetTrigger(pJoypad->JOYKEY_A))
		{
			// 終了
			PostQuitMessage(0);
		}
	}
}

//===================================================
// 描画処理
//===================================================
void CTitleQuit::Draw(void)
{
	// 描画処理
	CTitleMenu::Draw();
}

//===================================================
// コンストラクタ
//===================================================
CTitleMenuManager::CTitleMenuManager()
{
	m_Menu = CTitleMenu::MENU_START;
	m_bStart = false;
}

//===================================================
// デストラクタ
//===================================================
CTitleMenuManager::~CTitleMenuManager()
{
}

//===================================================
// 生成処理
//===================================================
void CTitleMenuManager::Create(void)
{
	// 自分がnullだったら
	if (m_pInstance == nullptr)
	{
		// 自分自身の生成
		m_pInstance = new CTitleMenuManager;
		m_pInstance->Init();
		m_pInstance->SetPosition(D3DXVECTOR3(150.0f, 400.0f, 0.0f));
		m_pInstance->SetSize(40.0f, 30.0f);
		m_pInstance->SetVtx(WHITE);

		// タイトルの選択メニュー分
		for (int nCnt = CTitleMenu::MENU_START; nCnt <= CTitleMenu::MENU_QUIT; nCnt++)
		{
			// メニュー型にキャストする
			CTitleMenu::MENU menu = static_cast<CTitleMenu::MENU>(nCnt);

			// タイトルのメニューの生成
			CTitleMenu::Create(D3DXVECTOR3(300.0f, 300.0f + nCnt * 140.0f, 0.0f), D3DXVECTOR2(150.0f, 50.0f), menu);
		}
	}
}

//===================================================
// 初期化処理
//===================================================
HRESULT CTitleMenuManager::Init(void)
{
	// 初期化処理
	if (FAILED(CObject2D::Init()))
	{
		return E_FAIL;
	}

	// テクスチャのIDの登録
	SetTextureID("data/TEXTURE/title/title_select.png");

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CTitleMenuManager::Uninit(void)
{
	// 終了処理
	CObject2D::Uninit();

	m_pInstance = nullptr;
}

//===================================================
// 更新処理
//===================================================
void CTitleMenuManager::Update(void)
{
	// キーボードの取得
	CInputKeyboard *pKeyboard = CManager::GetInputKeyboard();

	// パッドの取得
	CInputJoypad* pJoypad = CManager::GetInputJoypad();

	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pKeyboard->GetTrigger(DIK_DOWN) || pKeyboard->GetTrigger(DIK_S) || pJoypad->GetTrigger(pJoypad->JOYKEY_DOWN))
	{
		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_MENU);
		}

		// 次のメニューへ
		m_Menu = static_cast<CTitleMenu::MENU>(m_Menu + 1);
	}
	else if (pKeyboard->GetTrigger(DIK_UP) || pKeyboard->GetTrigger(DIK_W) || pJoypad->GetTrigger(pJoypad->JOYKEY_UP))
	{
		if (pSound != nullptr)
		{
			// 音の再生
			pSound->Play(CSound::SOUND_LABEL_MENU);
		}

		// 次のメニューへ
		m_Menu = static_cast<CTitleMenu::MENU>(m_Menu - 1);
	}

	// 範囲の制限
	m_Menu = Wrap(m_Menu, CTitleMenu::MENU_START, CTitleMenu::MENU_QUIT);
}

//===================================================
// 描画処理
//===================================================
void CTitleMenuManager::Draw(void)
{
	// 描画処理
	CObject2D::Draw();
}

//===================================================
// コンストラクタ(チュートリアル)
//===================================================
CTitleTutorial::CTitleTutorial() : CTitleMenu(MENU_TUTORIAL)
{
}

//===================================================
// デストラクタ(チュートリアル)
//===================================================
CTitleTutorial::~CTitleTutorial()
{
}

//===================================================
// 初期化処理(チュートリアル)
//===================================================
HRESULT CTitleTutorial::Init(void)
{
	// 初期化処理
	if (FAILED(CTitleMenu::Init()))
	{
		return E_FAIL;
	}

	// テクスチャのIDの設定
	SetTextureID("data/TEXTURE/title/title_menu_tutorial.png");

	return S_OK;
}

//===================================================
// 終了処理(チュートリアル)
//===================================================
void CTitleTutorial::Uninit(void)
{
	// 終了処理
	CTitleMenu::Uninit();
}

//===================================================
// 更新処理(チュートリアル)
//===================================================
void CTitleTutorial::Update(void)
{
	// 更新処理
	CTitleMenu::Update();

	// キーボードの取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	// パッドの取得
	CInputJoypad* pJoypad = CManager::GetInputJoypad();

	// タイトルマネージャーの取得
	auto pTitleManager = CTitleMenuManager::GetInstance();

	// 取得できなかったら処理しない
	if (pTitleManager == nullptr) return;

	// 自分のメニュー
	MENU myMenu = GetMenu();

	// 選択中のメニュー
	MENU selectMenu = pTitleManager->GetMenu();

	// フェードの取得
	CFade* pFade = CManager::GetFade();

	// 選択されているメニューと自分のメニューが同じだったら
	if (myMenu == selectMenu && pFade != nullptr && pFade->GetState() == CFade::FADE_NONE)
	{
		if (pKeyboard->GetTrigger(DIK_RETURN) || pJoypad->GetTrigger(pJoypad->JOYKEY_A))
		{
			// 音の取得
			CSound* pSound = CManager::GetSound();

			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_ENTER);
			}
			// スタートを押した
			pTitleManager->SetStart(true);

			// 新しいモードの設定
			pFade->SetFade(make_unique<CTutorial>(), D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
		}
	}
}

//===================================================
// 描画処理(チュートリアル)
//===================================================
void CTitleTutorial::Draw(void)
{
	// 描画処理
	CTitleMenu::Draw();
}
