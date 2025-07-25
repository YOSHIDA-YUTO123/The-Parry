//================================================
//
// ゲージ [gage.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _GAGE_H_
#define _GAGE_H_

//************************************************
// インクルードファイル
//************************************************
#include "object2D.h"
#include<memory>

//************************************************
// 前方宣言
//************************************************
template <class T>
class CObserver;

class CGageFrame;
class CTemporaryGage;

//************************************************
// ゲージクラスの定義
//************************************************
class CGage : public CObject2D
{
public:
	CGage(int nPriority = 6);
	~CGage();

	virtual HRESULT Init(void) override;
	virtual void Uninit(void) override;
	virtual void Update(void) override = 0; // 派生クラスで実装
	virtual void Draw(void) override;
private:
};

//************************************************
// HPゲージクラスの定義
//************************************************
class CHpGage : public CGage
{
public:
	CHpGage();
	~CHpGage();

	static CHpGage* Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const D3DXCOLOR col,const D3DXCOLOR temporaryColor, const int nLife, const bool bDecRightToLeft);
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void SetHp(const int nLife) { m_nLife = nLife; }
private:
	CTemporaryGage* m_pTemporary;	// 一時的なゲージ
	int m_nMaxLife;					// 最大のHP
	int m_nLife;					// HP
	bool m_bDecRightToLeft;			// 右から左に減るか判定
};

//************************************************
// ダメージ時の一時的なゲージの生成
//************************************************
class CTemporaryGage : public CGage
{
public:
	CTemporaryGage(int nPriority = 5);
	~CTemporaryGage();

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void SetLength(const float fLength) { m_fDestLength = fLength; }
	void SetGage(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const D3DXCOLOR col, const bool bDecRightToLeft);
private:
	float m_fDestLength;	 // 目的の長さ
	float m_fLength;		 // 長さ
	bool m_bDecRightToLeft;  // 右から左に減るか判定
};

//************************************************
// スタミナゲージクラスの定義
//************************************************
class CStaminaGage : public CGage
{
public:
	CStaminaGage();
	~CStaminaGage();

	static CStaminaGage* Create(const D3DXVECTOR3 pos, D3DXVECTOR2 Size, const D3DXCOLOR col, const float fStamina);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Set(const float fStamina) { m_fStamina = fStamina; } // スタミナゲージの設定処理
private:
	float m_fMaxStamina;	// 最大のスタミナ
	float m_fStamina;		// 現在のスタミナ
};
#endif
