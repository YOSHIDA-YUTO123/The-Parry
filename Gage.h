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
class CObserver;

//************************************************
// ゲージクラスの定義
//************************************************
class CGage : public CObject2D
{
public:
	CGage();
	~CGage();

	virtual HRESULT Init(void) override;
	virtual void Uninit(void) override;
	virtual void Update(void) override = 0; // 派生クラスで実装
	virtual void Draw(void) override;

	void SetObserver(CObserver* pObserver) { m_Observer = pObserver; }
protected:
	CObserver* m_Observer; // オブザーバークラスへのポインタ
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

	static CHpGage* Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const D3DXCOLOR col,const int nLife);
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void SetHp(const int nLife) { m_nLife = nLife; }
private:
	int m_nMaxLife;
	int m_nLife;
};
#endif
