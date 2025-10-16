//===================================================
//
// 石碑ブロックの処理 [BlockMonumet.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "BlockMonument.h"
#include "tutorial.h"
#include "player.h"
#include "math.h"
#include "MonumentUI.h"
#include "input.h"
#include "manager.h"
#include "FadeInObject2D.h"
#include "MonumentMessageUI.h"

//***************************************************
// 名前空間
//***************************************************
using namespace math;  // 名前空間mathの使用
using namespace Const; // 名前空間Constの使用

//***************************************************
// 定数宣言
//***************************************************
constexpr float SHOW_UI_DISTANCE = 200.0f; // UIが見える距離

//===================================================
// コンストラクタ
//===================================================
CBlockMonument::CBlockMonument()
{
	m_pMessageUI = nullptr;
	m_pUI = nullptr;
	m_bShowMessage = false;
}

//===================================================
// モニュメントのメッセージの表示
//===================================================
void CBlockMonument::SetMonumentMessage(void)
{
	// キーボードの取得
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	// ジョイパッドの取得
	CInputJoypad* pJoyPad = CManager::GetInputJoypad();

	// ボタンを押したら
	if(pJoyPad != nullptr && pKeyboard != nullptr &&
		(pKeyboard->GetTrigger(DIK_RETURN) || pJoyPad->GetTrigger(pJoyPad->JOYKEY_A)))
	{
		// 切り替え
		m_bShowMessage = m_bShowMessage ? false : true;

		if (m_pMessageUI)
		{
			m_pMessageUI->Reset();
		}
	}
}

//===================================================
// デストラクタ
//===================================================
CBlockMonument::~CBlockMonument()
{
}

//===================================================
// 生成処理
//===================================================
CBlockMonument* CBlockMonument::Create(const D3DXVECTOR3 pos, const TYPE type, const D3DXVECTOR3 rot)
{
	CBlockMonument* pBlock = new CBlockMonument;

	pBlock->SetPosition(pos);
	pBlock->SetType(type);

	// 初期化処理
	if (FAILED(pBlock->Init()))
	{
		pBlock->Uninit();
		pBlock = nullptr;
		return nullptr;
	}

	pBlock->SetRotation(rot);

	return pBlock;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CBlockMonument::Init(void)
{
	// 初期化処理
	if (FAILED(CBlock::Init()))
	{
		return E_FAIL;
	}

	// UIの生成
	m_pUI = CMonumentUI::Create(D3DXVECTOR3(1100.0f, 650.0f, 0.0f));
	
	// ブロックの種類の取得
	TYPE type = CBlock::GetType();

	switch (type)
	{
	case TYPE_MONUMENT_000:
		m_pMessageUI = CMonumentMessageUI::Create(CENTER_POS_2D, CMonumentMessageUI::TYPE_ONE);
		break;
	case TYPE_MONUMENT_001:
		m_pMessageUI = CMonumentMessageUI::Create(CENTER_POS_2D, CMonumentMessageUI::TYPE_TWO);
		break;
	default:
		break;
	}
	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CBlockMonument::Uninit(void)
{
	// 終了処理
	CBlock::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CBlockMonument::Update(void)
{
	// プレイヤーの取得
	CPlayer *pPlayer = CTutorial::GetPlayer();

	// プレイヤーが取得できなかったら処理しない
	if (pPlayer == nullptr) return;

	// 位置の取得
	D3DXVECTOR3 pos = CBlock::GetPosition();

	// プレイヤーの位置の取得
	D3DXVECTOR3 playerPos = pPlayer->GetPosition();

	// 距離を求める
	float fDistance = GetDistance(playerPos - pos);

	if (m_pUI != nullptr)
	{
		// 表示距離内に入ったら
		if (fDistance <= SHOW_UI_DISTANCE)
		{
			// モニュメントのメッセージUIの生成
			SetMonumentMessage();

			// 表示
			m_pUI->Enable(true);
		}
		else
		{
			m_bShowMessage = false;

			// 非表示
			m_pUI->Enable(false);	
		}
	}

	if (m_pMessageUI != nullptr)
	{
		m_pMessageUI->Enable(m_bShowMessage);
	}
	// 更新処理
	CBlock::Update();
}

//===================================================
// 描画処理
//===================================================
void CBlockMonument::Draw(void)
{
	// 描画処理
	CBlock::Draw();
}
