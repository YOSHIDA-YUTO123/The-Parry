//===================================================
//
// リザルトメニュー [ResultMenu.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _RESULTMENU_H_
#define _RESULTMENU_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"object2D.h"

//***************************************************
// リザルトのメニュークラスの定義
//***************************************************
class CResultMenu : public CObject2D
{
public:

	// メニューの種類
	enum MENU
	{
		MENU_RETRY = 0,
		MENU_QUIT,
		MENU_MAX
	};

	CResultMenu(const MENU menu);
	virtual ~CResultMenu();

	static CResultMenu* Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const MENU menu);
	virtual HRESULT Init(void) override;
	virtual void Uninit(void) override;
	virtual void Update(void) override;
	virtual void Draw(void) override;
protected:
	MENU GetMenu(void) const { return m_Menu; }
private:
	MENU m_Menu; // メニューの種類
};

//***************************************************
// リザルトメニュー(リトライ)のクラスの定義
//***************************************************
class CResultMenuRetry : public CResultMenu
{
public:

	CResultMenuRetry();
	~CResultMenuRetry();

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
};

//***************************************************
// リザルトメニュー(終了)のクラスの定義
//***************************************************
class CResultMenuQuit : public CResultMenu
{
public:

	CResultMenuQuit();
	~CResultMenuQuit();

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
};

//***************************************************
// リザルトメニューのマネージャークラスの定義
//***************************************************
class CResultMenuManager : CObject
{
public:
	~CResultMenuManager();

	static void Create(void);
	static CResultMenuManager* GetInstance(void) { return m_pInstance; }
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	CResultMenu::MENU GetMenu(void) const { return m_Menu; }
private:
	CResultMenuManager();
	static CResultMenuManager* m_pInstance; // 自分自身の破棄
	CResultMenu::MENU m_Menu;		 // 選択中のメニュー
};
#endif
