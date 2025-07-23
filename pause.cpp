//================================================
//
// ポーズ [pause.cpp]
// Author: YUTO YOSHIDA
//
//================================================

//************************************************
// インクルードファイル
//************************************************
#include "pause.h"
#include"manager.h"
#include"renderer.h"
#include"textureManager.h"
#include"input.h"
#include"math.h"
#include"fade.h"
#include "game.h"
#include "title.h"

using namespace Const; // 名前空間Constを使用
using namespace std; // 名前空間stdを使用
using namespace math; // 名前空間mathを使用

//************************************************
// 静的メンバ変数宣言
//************************************************
bool CPauseManager::m_bPause = false; // ポーズ状態かどうか
unique_ptr<CPauseManager> CPauseManager::m_pInstance = nullptr; // 自分のインスタンス

//================================================
// コンストラクタ
//================================================
CPause::CPause(const TYPE type)
{
	m_Type = type;
}

//================================================
// デストラクタ
//================================================
CPause::~CPause()
{
}

//================================================
// 生成処理
//================================================
CPause* CPause::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const TYPE type)
{
	// ポーズクラスへのポインタ
	CPause* pPause = nullptr;

	// 種類の遷移
	switch (type)
	{
	case TYPE_CONTINUE:
		pPause = new CPauseContinue;
		break;
	case TYPE_RETRY:
		pPause = new CPauseRetry;
		break;
	case TYPE_QUIT:
		pPause = new CPauseQuit;
		break;
	default:
		return nullptr;
	}
	
	// 初期化処理
	pPause->Init();

	// 位置の設定処理
	pPause->SetPosition(pos);
	pPause->SetSize(Size.x,Size.y);
	pPause->SetVtx(WHITE);

	return pPause;
}

//================================================
// 初期化処理
//================================================
HRESULT CPause::Init(void)
{
	// 初期化処理
	if (FAILED(CObject2D::Init()))
	{
		return E_FAIL;
	}

	// 種類の設定
	CObject::SetType(CObject::TYPE_PAUSE);

	return S_OK;
}

//================================================
// 終了処理
//================================================
void CPause::Uninit(void)
{
	// 終了処理
	CObject2D::Uninit();
}

//================================================
// 描画処理
//================================================
void CPause::Draw(void)
{
	// 描画処理
	CObject2D::Draw();
}

//================================================
// コンストラクタ(Continue)
//================================================
CPauseContinue::CPauseContinue() : CPause(TYPE_CONTINUE)
{
}

//================================================
// デストラクタ(Continue)
//================================================
CPauseContinue::~CPauseContinue()
{
}

//================================================
// 初期化処理(Continue)
//================================================
HRESULT CPauseContinue::Init(void)
{
	// 初期化処理
	if (FAILED(CPause::Init()))
	{
		return E_FAIL;
	}

	// テクスチャの設定
	SetTextureID("data/TEXTURE/pause/pause_continue.png");

	return S_OK;
}

//================================================
// 終了処理(Continue)
//================================================
void CPauseContinue::Uninit(void)
{
	// 終了処理
	CPause::Uninit();
}

//================================================
// 更新処理(Continue)
//================================================
void CPauseContinue::Update(void)
{
	// ポーズマネージャーの取得
	CPauseManager* pPauseManager = CPauseManager::GetInstance();

	// ポーズ状態の取得
	bool bPause = CPauseManager::GetPause();

	// ポーズのマネージャーがnullだったらまたはポーズ中じゃないなら処理しない
	if (pPauseManager == nullptr || bPause == false) return;

	// 選択中の種類
	TYPE select = pPauseManager->GetSelectMenu();

	// キーボードの取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	// パッドの取得
	CInputJoypad* pJoyPad = CManager::GetInputJoypad();

	// 選択中の種類と自分の種類が一致していたら
	if (select == CPause::GetType())
	{
		// 色
		SetColor(D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f));

		// ENTERキーが押されたら
		if (pKeyboard->GetTrigger(DIK_RETURN))
		{
			// ポーズをオフ
			pPauseManager->EnablePause(false);
		}

		// Aボタンが押されたら
		if (pJoyPad->GetTrigger(pJoyPad->JOYKEY_A))
		{
			// ポーズをオフ
			pPauseManager->EnablePause(false);
		}
	}
	else
	{
		SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}
}

//================================================
// 描画処理(Continue)
//================================================
void CPauseContinue::Draw(void)
{
	// 描画処理
	CPause::Draw();
}


//================================================
// コンストラクタ(リトライ)
//================================================
CPauseRetry::CPauseRetry() : CPause(TYPE_RETRY)
{

}

//================================================
// デストラクタ(リトライ)
//================================================
CPauseRetry::~CPauseRetry()
{
}

//================================================
// 初期化処理(リトライ)
//================================================
HRESULT CPauseRetry::Init(void)
{
	// 初期化処理
	if (FAILED(CPause::Init()))
	{
		return E_FAIL;
	}

	// テクスチャの設定
	SetTextureID("data/TEXTURE/pause/pause_retry.png");

	return S_OK;
}

//================================================
// 終了処理(リトライ)
//================================================
void CPauseRetry::Uninit(void)
{
	// 終了処理
	CPause::Uninit();
}

//================================================
// 更新処理(リトライ)
//================================================
void CPauseRetry::Update(void)
{
	// ポーズマネージャーの取得
	CPauseManager* pPauseManager = CPauseManager::GetInstance();

	// ポーズ状態の取得
	bool bPause = CPauseManager::GetPause();

	// ポーズのマネージャーがnullだったらまたはポーズ中じゃないなら処理しない
	if (pPauseManager == nullptr || bPause == false) return;

	// キーボードの取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	// パッドの取得
	CInputJoypad* pJoyPad = CManager::GetInputJoypad();

	// 選択中の種類
	TYPE select = pPauseManager->GetSelectMenu();

	// 選択中の種類と自分の種類が一致していたら
	if (select == CPause::GetType())
	{
		SetColor(D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f));

		// ENTERキーが押されたら
		if (pKeyboard->GetTrigger(DIK_RETURN))
		{
			// フェードの取得
			CFade* pFade = CManager::GetFade();

			pFade->SetFade(make_unique<CGame>());
		}

		// Aボタンが押されたら
		if (pJoyPad->GetTrigger(pJoyPad->JOYKEY_A))
		{
			// フェードの取得
			CFade* pFade = CManager::GetFade();

			pFade->SetFade(make_unique<CGame>());
		}
	}
	else
	{
		SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}
}

//================================================
// 描画処理(リトライ)
//================================================
void CPauseRetry::Draw(void)
{
	// 描画処理
	CPause::Draw();
}

//================================================
// コンストラクタ(やめる)
//================================================
CPauseQuit::CPauseQuit() : CPause(TYPE_QUIT)
{

}

//================================================
// デストラクタ(やめる)
//================================================
CPauseQuit::~CPauseQuit()
{
}

//================================================
// 初期化処理(やめる)
//================================================
HRESULT CPauseQuit::Init(void)
{
	// 初期化処理
	if (FAILED(CPause::Init()))
	{
		return E_FAIL;
	}

	// テクスチャの設定
	SetTextureID("data/TEXTURE/pause/pause_quit.png");

	return S_OK;
}

//================================================
// 終了処理(やめる)
//================================================
void CPauseQuit::Uninit(void)
{
	// 終了処理
	CPause::Uninit();
}

//================================================
// 更新処理(やめる)
//================================================
void CPauseQuit::Update(void)
{
	// ポーズマネージャーの取得
	CPauseManager* pPauseManager = CPauseManager::GetInstance();

	// ポーズ状態の取得
	bool bPause = CPauseManager::GetPause();

	// ポーズのマネージャーがnullだったらまたはポーズ中じゃないなら処理しない
	if (pPauseManager == nullptr || bPause == false) return;

	// キーボードの取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	// パッドの取得
	CInputJoypad* pJoyPad = CManager::GetInputJoypad();

	// 選択中の種類
	TYPE select = pPauseManager->GetSelectMenu();

	// 選択中の種類と自分の種類が一致していたら
	if (select == CPause::GetType())
	{
		// 色の設定
		SetColor(D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f));

		// ENTERキーが押されたら
		if (pKeyboard->GetTrigger(DIK_RETURN))
		{
			// フェードの取得
			CFade* pFade = CManager::GetFade();

			pFade->SetFade(make_unique<CTitle>());
		}

		// Aボタンが押されたら
		if (pJoyPad->GetTrigger(pJoyPad->JOYKEY_A))
		{
			// フェードの取得
			CFade* pFade = CManager::GetFade();

			pFade->SetFade(make_unique<CTitle>());
		}
	}
	else
	{
		SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}
}

//================================================
// 描画処理(やめる)
//================================================
void CPauseQuit::Draw(void)
{
	// 描画処理
	CPause::Draw();
}

//================================================
// コンストラクタ(マネージャー)
//================================================
CPauseManager::CPauseManager()
{
	m_SelectMenu = CPause::TYPE_CONTINUE;
}

//================================================
// デストラクタ(マネージャー)
//================================================
CPauseManager::~CPauseManager()
{
	m_bPause = false;
}

//================================================
// 生成処理
//================================================
void CPauseManager::Create(void)
{
	// 自分が生成されていなかったら
	if (m_pInstance == nullptr)
	{
		// 自分の生成
		m_pInstance.reset(new CPauseManager);

		// ポーズの種類分回す
		for (int nCnt = 0; nCnt < CPause::TYPE_MAX; nCnt++)
		{
			// キャストする
			CPause::TYPE type = static_cast<CPause::TYPE>(nCnt);

			// ポーズの生成処理
			CPause::Create(D3DXVECTOR3(640.0f, 360.0f + (nCnt * 100.0f), 0.0f), D3DXVECTOR2(100.0f, 50.0f), type);
		}
	}
}

//================================================
// 終了処理
//================================================
void CPauseManager::Uninit(void)
{
	// 破棄
	m_pInstance.reset();
	m_pInstance = nullptr;
}

//================================================
// ポーズの切り替え処理(マネージャー)
//================================================
void CPauseManager::EnablePause(void)
{
	// キーボードの取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	// パッドの取得
	CInputJoypad* pJoyPad = CManager::GetInputJoypad();

	if (pKeyboard != nullptr)
	{
		// pキーが押された
		if (pKeyboard->GetTrigger(DIK_P))
		{
			// ポーズの切り替え処理
			m_bPause = m_bPause ? false : true;
		}
	}

	if (pJoyPad != nullptr)
	{
		// STARTボタンが押されたら
		if (pJoyPad->GetTrigger(pJoyPad->JOYKEY_START))
		{
			// ポーズの切り替え処理
			m_bPause = m_bPause ? false : true;
		}
	}
}

//================================================
// メニューの選択処理
//================================================
void CPauseManager::SelectMenu(void)
{
	// キーボードの取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	// パッドの取得
	CInputJoypad* pJoyPad = CManager::GetInputJoypad();

	if (pKeyboard != nullptr)
	{
		// 上が押されたら
		if (pKeyboard->GetTrigger(DIK_UP))
		{
			// 次の項目へ
			m_SelectMenu = static_cast<CPause::TYPE>(m_SelectMenu - 1);
		}
		// 下が押されたら
		else if (pKeyboard->GetTrigger(DIK_DOWN))
		{
			// 前の項目へ
			m_SelectMenu = static_cast<CPause::TYPE>(m_SelectMenu + 1);
		}
	}

	if (pJoyPad != nullptr)
	{
		// 上が押されたら
		if (pJoyPad->GetTrigger(pJoyPad->JOYKEY_UP))
		{
			// 次の項目へ
			m_SelectMenu = static_cast<CPause::TYPE>(m_SelectMenu - 1);
		}
		// 下が押されたら
		else if (pJoyPad->GetTrigger(pJoyPad->JOYKEY_DOWN))
		{
			// 前の項目へ
			m_SelectMenu = static_cast<CPause::TYPE>(m_SelectMenu + 1);
		}
	}

	// 範囲内をループする
	m_SelectMenu = Wrap(m_SelectMenu, CPause::TYPE_CONTINUE, CPause::TYPE_QUIT);
}