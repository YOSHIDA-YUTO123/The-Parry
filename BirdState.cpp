//===================================================
//
// 鳥の状態 [BirdState.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "BirdState.h"
#include"bird.h"
#include"motion.h"
#include"game.h"
#include "sound.h"
#include "manager.h"

//===================================================
// コンストラクタ
//===================================================
CBirdState::CBirdState()
{
	m_ID = ID_BASE;
	m_pBird = nullptr;
}

//===================================================
// コンストラクタ(オーバーロード)
//===================================================
CBirdState::CBirdState(ID Id)
{
	m_ID = Id;
	m_pBird = nullptr;
}

//===================================================
// デストラクタ
//===================================================
CBirdState::~CBirdState()
{
}

//===================================================
// コンストラクタ
//===================================================
CBirdIdle::CBirdIdle() : CBirdState(ID_IDLE)
{
}

//===================================================
// デストラクタ
//===================================================
CBirdIdle::~CBirdIdle()
{
}

//===================================================
// 初期化処理
//===================================================
void CBirdIdle::Init(void)
{
	// 鳥の取得
	CBird* pBird = CBirdState::GetBird();

	// 取得できなかったら処理しない
	if (pBird == nullptr) return;

	// モーションの取得
	CMotion* pMotion = pBird->GetMotion();

	// 取得できなかったら処理しない
	if (pMotion == nullptr) return;

	// ランダムな値の選出
	int nRandom = rand() % 2;

	// モーションの種類の設定
	int motionType = (nRandom == 0) ? CBird::MOTIONTYPE_NEUTRAL000 : CBird::MOTIONTYPE_NEUTRAL001;

	// モーションの設定
	pMotion->SetMotion(motionType, true, 10);
}

//===================================================
// 更新処理
//===================================================
void CBirdIdle::Update(void)
{

}

//===================================================
// コンストラクタ
//===================================================
CBirdFly::CBirdFly() : CBirdState(ID_FLY)
{
}

//===================================================
// デストラクタ
//===================================================
CBirdFly::~CBirdFly()
{
}

//===================================================
// 初期化処理
//===================================================
void CBirdFly::Init(void)
{
	// 鳥の取得
	CBird* pBird = CBirdState::GetBird();

	// 取得できなかったら処理しない
	if (pBird == nullptr) return;

	// モーションの取得
	CMotion* pMotion = pBird->GetMotion();

	// 取得できなかったら処理しない
	if (pMotion == nullptr) return;

	// 音の取得
	CSound* pSound = CManager::GetSound();

	if (pSound != nullptr)
	{
		// 鳥の鳴き声を停止
		pSound->StopSound(CSound::SOUND_LABEL_BIRDSOUND);

		// 鳥が羽ばたく
		pSound->Play(CSound::SOUND_LABEL_BIRDFLY);
	}
	// モーションの設定
	pMotion->SetMotion(CBird::MOTIONTYPE_FLY, true, 10);
}

//===================================================
// 更新処理
//===================================================
void CBirdFly::Update(void)
{
	// 鳥の取得
	CBird* pBird = CBirdState::GetBird();

	// 取得できなかったら処理しない
	if (pBird == nullptr) return;

	// モーションの取得
	CMotion* pMotion = pBird->GetMotion();

	// 取得できなかったら処理しない
	if (pMotion == nullptr) return;

	// 飛行状態の設定
	pBird->SetFly(D3DXVECTOR3(5.0f, 5.0f, 5.0f));
}
