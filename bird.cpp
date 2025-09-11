//===================================================
//
// 鳥 [bird.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "bird.h"
#include"BirdState.h"
#include"motion.h"
#include "CharacterManager.h"

using namespace std; // 名前空間stdの使用

//***************************************************
// 静的メンバ変数宣言
//***************************************************
bool CBird::m_bLoad = false;  // ロードしたかどうか

//===================================================
// コンストラクタ
//===================================================
CBird::CBird() : CCharacter3D(TYPE_BIRD)
{
	m_pMachine = nullptr;
}

//===================================================
// デストラクタ
//===================================================
CBird::~CBird()
{
}

//===================================================
// 生成処理
//===================================================
CBird* CBird::Create(const D3DXVECTOR3 pos)
{
	CBird* pBird = new CBird;

	// 位置の設定
	pBird->SetPosition(pos);

	// 初期化処理
	if (FAILED(pBird->Init()))
	{
		pBird->Uninit();
		pBird = nullptr;
		return nullptr;
	}

	return pBird;
}

//===================================================
// 初期化処理
//===================================================
HRESULT CBird::Init(void)
{
	// 初期化処理
	if (FAILED(CCharacter3D::Init()))
	{
		return E_FAIL;
	}

	if (!m_bLoad)
	{
		// モーションのロード
		CCharacter3D::LoadMotion("motionBird.txt", MOTIONTYPE_MAX);
		m_bLoad = true;
	}
	else
	{
		// キャラクターのマネージャーの取得
		auto pCharacterManager =  CCharacterManager::GetInstance();

		CCharacter3D::CreateMotion();

		// キャラクター情報の取得
		pCharacterManager->GetCharacter(this);
	}

	// 状態マシーンの生成
	m_pMachine = make_unique<CStateMachine>();

	// 状態の設定
	ChangeState(make_shared<CBirdIdle>());

	// キャラクターの設定処理
	CCharacter3D::SetCharacter(10, 6.0f, D3DXVECTOR3(2.0f, 1.0f, 2.0f), D3DXVECTOR3(50.0f, 100.0f, 50.0f));

	return S_OK;
}

//===================================================
// 終了処理
//===================================================
void CBird::Uninit(void)
{
	m_pMachine.reset();

	// 終了処理
	CCharacter3D::Uninit();
}

//===================================================
// 更新処理
//===================================================
void CBird::Update(void)
{
	// モーション更新処理
	CCharacter3D::UpdateMotion();

	// 更新処理
	CCharacter3D::Update();
}

//===================================================
// 描画処理
//===================================================
void CBird::Draw(void)
{
	// 描画処理
	CCharacter3D::Draw();
}

//===================================================
// 状態の変更
//===================================================
void CBird::ChangeState(std::shared_ptr<CBirdState> pNewState)
{
	if (pNewState != nullptr)
	{
		// オーナーの設定
		pNewState->SetOwner(this);
	}

	if (m_pMachine != nullptr)
	{
		// 状態の設定
		m_pMachine->Change(pNewState);
	}
}
