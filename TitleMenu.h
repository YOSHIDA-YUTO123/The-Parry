//===================================================
//
// タイトルメニュー [TitleMenu.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _TITLELMENU_H_
#define _TITLELMENU_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "object2D.h"

//***************************************************
// タイトルメニューのクラスの定義
//***************************************************
class CTitleMenu : public CObject2D
{
public:

	// メニューの種類
	enum MENU
	{
		MENU_START = 0,
		MENU_TUTORIAL,
		MENU_QUIT,
		MENU_MAX
	};

	CTitleMenu(const MENU menu);
	virtual ~CTitleMenu();

	static CTitleMenu* Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size,const MENU menu);

	virtual HRESULT Init(void) override;
	virtual void Uninit(void) override;
	virtual void Update(void) override;
	virtual void Draw(void) override;
protected:
	MENU GetMenu(void) const { return m_Menu; }
	D3DXVECTOR2 GetBaseSize(void) const { return m_BaseSize; }
private:
	MENU m_Menu; // タイトルのメニュー
	D3DXVECTOR2 m_BaseSize; // 基準の大きさ
	D3DXCOLOR m_col;		// 色
	float m_fCounter;		// 色のカウンター
};

//***************************************************
// タイトルメニュー(スタート)のクラスの定義
//***************************************************
class CTitleStart : public CTitleMenu
{
public:
	CTitleStart();
	~CTitleStart();

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
};

//***************************************************
// タイトルメニュー(チュートリアル)のクラスの定義
//***************************************************
class CTitleTutorial : public CTitleMenu
{
public:
	CTitleTutorial();
	~CTitleTutorial();

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
};

//***************************************************
// タイトルメニュー(やめる)のクラスの定義
//***************************************************
class CTitleQuit : public CTitleMenu
{
public:
	CTitleQuit();
	~CTitleQuit();

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
};

//***************************************************
// タイトルメニューのマネージャークラスの定義
//***************************************************
class CTitleMenuManager : public CObject2D
{
public:
	~CTitleMenuManager();

	static void Create(void);
	static CTitleMenuManager* GetInstance(void) { return m_pInstance; }

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	CTitleMenu::MENU GetMenu(void) const { return m_Menu; }
	bool CheckStart(void) const { return m_bStart; }
	void SetStart(bool bStart) { m_bStart = bStart; }
private:
	CTitleMenuManager();
	static CTitleMenuManager* m_pInstance; // 自分のインスタンス
	bool m_bStart; // スタートを押したかどうか
	CTitleMenu::MENU m_Menu; // タイトルのメニュー
};

#endif