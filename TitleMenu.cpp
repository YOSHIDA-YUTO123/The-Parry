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

using namespace math;  // 名前空間mathを使用
using namespace Const; // 名前空間Constを使用

//***************************************************
// 静的メンバ変数宣言
//***************************************************
CTitleMenuManager* CTitleMenuManager::m_pInstance = nullptr; // 自分のインスタンス

//===================================================
// コンストラクタ
//===================================================
CTitleMenu::CTitleMenu(const MENU menu)
{
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

	// 種類が同じだったら
	if (m_Menu == pTitleManager->GetMenu())
	{
		// 位置の取得
		D3DXVECTOR3 pos = CObject2D::GetPosition();

		// 大きさの取得
		D3DXVECTOR2 Size = CObject2D::GetSize();

		// 位置の設定
		pTitleManager->SetPosition(D3DXVECTOR3(pos.x - Size.x, pos.y, pos.z));
	}
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
		m_pInstance->SetSize(50.0f, 40.0f);
		m_pInstance->SetVtx(WHITE);

		// タイトルの選択メニュー分
		for (int nCnt = CTitleMenu::MENU_START; nCnt <= CTitleMenu::MENU_QUIT; nCnt++)
		{
			// メニュー型にキャストする
			CTitleMenu::MENU menu = static_cast<CTitleMenu::MENU>(nCnt);

			// タイトルのメニューの生成
			CTitleMenu::Create(D3DXVECTOR3(300.0f, 400.0f + nCnt * 160.0f, 0.0f), D3DXVECTOR2(100.0f, 80.0f), menu);
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
}

//===================================================
// 更新処理
//===================================================
void CTitleMenuManager::Update(void)
{
	// キーボードの取得
	CInputKeyboard *pKeyboard = CManager::GetInputKeyboard();

	if (pKeyboard->GetTrigger(DIK_DOWN))
	{
		// 次のメニューへ
		m_Menu = static_cast<CTitleMenu::MENU>(m_Menu + 1);
	}
	else if (pKeyboard->GetTrigger(DIK_UP))
	{
		// 次のメニューへ
		m_Menu = static_cast<CTitleMenu::MENU>(m_Menu - 1);
	}

	m_Menu = Clamp(m_Menu, CTitleMenu::MENU_START, CTitleMenu::MENU_MAX);
}

//===================================================
// 描画処理
//===================================================
void CTitleMenuManager::Draw(void)
{
	// 描画処理
	CObject2D::Draw();
}
