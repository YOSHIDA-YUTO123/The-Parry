//===================================================
//
// オブザーバー [Observer.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _OBSERVER_H_
#define _OBSERVER_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"

//***************************************************
// 前方宣言
//***************************************************
class CHpGage;
class CStaminaGage;

//***************************************************
// オブザーバークラスの定義
//***************************************************
template <class T>  class CObserver
{
public:
	CObserver() {};
	virtual ~CObserver() {};
	virtual void OnNotify(const T nValue) = 0; // 値の変化を通知
private:
};

//***************************************************
// HPオブザーバークラスの定義
//***************************************************
class CHpObserver : public CObserver<int>
{
public:
	CHpObserver(CHpGage *pHpGage);
	~CHpObserver();
	void OnNotify(const int nValue) override;
private:
	CHpGage* m_pHpGage;
};

//***************************************************
// スタミナオブザーバークラスの定義
//***************************************************
class CStaminaObserver : public CObserver<float>
{
public:
	CStaminaObserver(CStaminaGage* pStaminaGage);
	~CStaminaObserver();
	void OnNotify(const float nValue) override;
private:
	CStaminaGage* m_pStaminaGage;
};

#endif