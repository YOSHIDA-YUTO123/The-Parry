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
		MENU_NONE = 0,
		MENU_START,
		MENU_QUIT,
		MENU_MAX
	};

	CTitleMenu();
	~CTitleMenu();

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

private:
};

#endif