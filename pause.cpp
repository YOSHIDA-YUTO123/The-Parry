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
#include "background.h"
#include"opening.h"
#include "sound.h"

using namespace Const; // 名前空間Constを使用
using namespace std; // 名前空間stdを使用
using namespace math; // 名前空間mathを使用

constexpr float UI_MAG = 1.2f;	// 拡大率
constexpr float COUNTER = 1.0f / 30.0f; // カウンター

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
	m_BaseSize = VEC2_NULL;
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
	pPause->m_BaseSize = Size;

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

	// 大きさの取得
	D3DXVECTOR2 Size = CObject2D::GetSize();

	// 基準の大きさの取得
	D3DXVECTOR2 BaseSize = CPause::GetBaseSize();

	// 選択中の種類と自分の種類が一致していたら
	if (select == CPause::GetType())
	{
		// 目的の大きさ
		D3DXVECTOR2 DestSize = BaseSize * UI_MAG;

		// 目的の大きさに近づける
		Size += (DestSize - Size) * 0.1f;

		// 大きさの設定
		CObject2D::SetSize(Size.x, Size.y);

		// 色
		CObject2D::SetColor(D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f));

		// ENTERキーが押されたら || Aボタンが押されたら
		if (pKeyboard->GetTrigger(DIK_RETURN) || pJoyPad->GetTrigger(pJoyPad->JOYKEY_A))
		{
			// ポーズをオフ
			pPauseManager->EnablePause(false);
		}
	}
	else
	{
		// 目的の大きさに近づける
		Size += (BaseSize - Size) * 0.5f;

		// 大きさの設定
		CObject2D::SetSize(Size.x, Size.y);

		// 色の設定
		CObject2D::SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
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

	// 大きさの取得
	D3DXVECTOR2 Size = CObject2D::GetSize();

	// 基準の大きさの取得
	D3DXVECTOR2 BaseSize = CPause::GetBaseSize();

	// 選択中の種類と自分の種類が一致していたら
	if (select == CPause::GetType())
	{
		// 目的の大きさ
		D3DXVECTOR2 DestSize = BaseSize * UI_MAG;

		// 目的の大きさに近づける
		Size += (DestSize - Size) * 0.1f;

		// 大きさの設定
		CObject2D::SetSize(Size.x, Size.y);

		// 色
		CObject2D::SetColor(D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f));

		// ENTERキーが押されたら
		if (pKeyboard->GetTrigger(DIK_RETURN) || pJoyPad->GetTrigger(pJoyPad->JOYKEY_A))
		{
			// フェードの取得
			CFade* pFade = CManager::GetFade();

			pFade->SetFade(make_unique<CGame>());
		}
	}
	else
	{
		// 目的の大きさに近づける
		Size += (BaseSize - Size) * 0.5f;

		// 大きさの設定
		CObject2D::SetSize(Size.x, Size.y);

		// 色の設定
		CObject2D::SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
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

	// 大きさの取得
	D3DXVECTOR2 Size = CObject2D::GetSize();

	// 基準の大きさの取得
	D3DXVECTOR2 BaseSize = CPause::GetBaseSize();

	// 選択中の種類と自分の種類が一致していたら
	if (select == CPause::GetType())
	{
		// 目的の大きさ
		D3DXVECTOR2 DestSize = BaseSize * UI_MAG;

		// 目的の大きさに近づける
		Size += (DestSize - Size) * 0.1f;

		// 大きさの設定
		CObject2D::SetSize(Size.x, Size.y);

		// 色
		CObject2D::SetColor(D3DXCOLOR(1.0f, 1.0f, 0.4f, 1.0f));

		// ENTERキーが押されたら
		if (pKeyboard->GetTrigger(DIK_RETURN) || pJoyPad->GetTrigger(pJoyPad->JOYKEY_A))
		{
			// 音の取得
			CSound* pSound = CManager::GetSound();

			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_ENTER);
			}

			// フェードの取得
			CFade* pFade = CManager::GetFade();

			pFade->SetFade(make_unique<CTitle>());
		}
	}
	else
	{
		// 目的の大きさに近づける
		Size += (BaseSize - Size) * 0.5f;

		// 大きさの設定
		CObject2D::SetSize(Size.x, Size.y);

		// 色の設定
		CObject2D::SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
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

		// 背景の生成
		CBackGround::Create(D3DXVECTOR3(640.0f, 360.0f, 0.0f), D3DXVECTOR2(640.0f, 360.0f),D3DXCOLOR(1.0f,1.0f,1.0f,0.3f),CBackGround::TYPE_PAUSE);

		// ポーズ中
		CPauseNow::Create(D3DXVECTOR3(80.0f,660.0f, 0.0f), D3DXVECTOR2(60.0f, 50.0f));

		// ポーズの選択メニュー分
		for (int nCnt = CPause::TYPE_CONTINUE; nCnt <= CPause::TYPE_QUIT; nCnt++)
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

	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pKeyboard != nullptr && pJoyPad != nullptr)
	{
		// 上が押されたら
		if (pKeyboard->GetTrigger(DIK_UP) || pJoyPad->GetTrigger(pJoyPad->JOYKEY_UP))
		{
			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_MENU);
			}

			// 次の項目へ
			m_SelectMenu = static_cast<CPause::TYPE>(m_SelectMenu - 1);
		}
		// 下が押されたら
		else if (pKeyboard->GetTrigger(DIK_DOWN) || pJoyPad->GetTrigger(pJoyPad->JOYKEY_DOWN))
		{
			if (pSound != nullptr)
			{
				// 音の再生
				pSound->Play(CSound::SOUND_LABEL_MENU);
			}

			// 前の項目へ
			m_SelectMenu = static_cast<CPause::TYPE>(m_SelectMenu + 1);
		}
	}

	// 範囲内をループする
	m_SelectMenu = Wrap(m_SelectMenu, CPause::TYPE_CONTINUE, CPause::TYPE_QUIT);
}

//================================================
// コンストラクタ(ポーズ中)
//================================================
CPauseNow::CPauseNow()
{
	m_BaseSize = VEC2_NULL;
	m_fCounter = NULL;
}

//================================================
// デストラクタ(ポーズ中)
//================================================
CPauseNow::~CPauseNow()
{
}

//================================================
// 生成処理
//================================================
CPauseNow* CPauseNow::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size)
{
	// ポーズ中の生成
	CPauseNow* pPause = new CPauseNow;

	pPause->Init();
	pPause->SetPosition(pos);
	pPause->SetSize(Size.x, Size.y);
	pPause->SetVtx(WHITE);
	pPause->m_BaseSize = Size;

	return pPause;
}

//================================================
// 初期化処理(ポーズ中)
//================================================
HRESULT CPauseNow::Init(void)
{
	// 初期化処理
	if (FAILED(CObject2D::Init()))
	{
		return E_FAIL;
	}

	// 種類の設定
	CObject::SetType(TYPE_PAUSE);

	// テクスチャのIDの設定
	CObject2D::SetTextureID("data/TEXTURE/pause/pausenow.png");

	return S_OK;
}

//================================================
// 終了処理(ポーズ中)
//================================================
void CPauseNow::Uninit(void)
{
	// 終了処理
	CObject2D::Uninit();
}

//================================================
// 更新処理(ポーズ中)
//================================================
void CPauseNow::Update(void)
{
	m_fCounter += COUNTER;

	D3DXVECTOR2 Size = VEC2_NULL;

	// 大きさを求める
	Size.x = fabsf(sinf(m_fCounter) * (m_BaseSize.x * 0.2f)) + m_BaseSize.x;
	Size.y = fabsf(sinf(m_fCounter) * (m_BaseSize.y * 0.2f)) + m_BaseSize.x;

	// 大きさを計算
	CObject2D::SetSize(Size.x, Size.y);
}

//================================================
// 描画処理(ポーズ中)
//================================================
void CPauseNow::Draw(void)
{
	// 描画処理
	CObject2D::Draw();
}
