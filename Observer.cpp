//===================================================
//
// オブザーバー [Observer.cpp]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// インクルードファイル
//***************************************************
#include "Observer.h"
#include"Gage.h"
#include"RevengeGage.h"

//===================================================
// コンストラクタ
//===================================================
CHpObserver::CHpObserver(CHpGage* pHpGage)
{
	m_pHpGage = pHpGage;
}

//===================================================
// デストラクタ
//===================================================
CHpObserver::~CHpObserver()
{
}

//===================================================
// 通知処理
//===================================================
void CHpObserver::OnNotify(const int nValue)
{
	if (m_pHpGage != nullptr)
	{
		// HPの設定処理
		m_pHpGage->SetHp(nValue);
	}
}

//===================================================
// スタミナゲージのコンストラクタ
//===================================================
CStaminaObserver::CStaminaObserver(CStaminaGage* pStaminaGage)
{
	m_pStaminaGage = pStaminaGage;
}

//===================================================
// スタミナゲージのデストラクタ
//===================================================
CStaminaObserver::~CStaminaObserver()
{
}

//===================================================
// スタミナゲージの通知処理
//===================================================
void CStaminaObserver::OnNotify(const float fValue)
{
	if (m_pStaminaGage != nullptr)
	{
		// スタミナの設定
		m_pStaminaGage->Set(fValue);
	}
}

//===================================================
// コンストラクタ
//===================================================
CRevengeUIObserver::CRevengeUIObserver(CRevengeUI* pRevenge)
{
	m_pRevengeValue = pRevenge;
}

//===================================================
// デストラクタ
//===================================================
CRevengeUIObserver::~CRevengeUIObserver()
{
}

//===================================================
// 通知処理
//===================================================
void CRevengeUIObserver::OnNotify(const float fValue)
{
	if (m_pRevengeValue != nullptr)
	{
		// スタミナの設定
		m_pRevengeValue->Set(fValue);
	}
}
