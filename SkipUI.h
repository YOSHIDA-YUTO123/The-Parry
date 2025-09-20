//===================================================
//
// オープニングのスキップUIを表示する [SkipUI.h]
// Author:YUTO YOSHIDA
//
//===================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _SKIPUI_H_
#define _SKIPUI_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "FadeInObject2D.h"

//***************************************************
// スキップUIクラスの定義
//***************************************************
class CSkipUI : public CFadeInObject2D
{
public:
	CSkipUI();
	~CSkipUI();

	static CSkipUI* Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const int nTime);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

private:
	float m_fFlashCounter; // 点滅のカウンター
};

#endif
