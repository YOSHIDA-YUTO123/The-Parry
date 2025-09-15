//===================================================
//
// ゲートのUI [GateUI.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _GATEUI_H_
#define _GATEUI_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "object2D.h"

//***************************************************
// ゲートのUIのクラスの定義
//***************************************************
class CGateUI : public CObject2D
{
public:
	CGateUI();
	~CGateUI();

	static CGateUI* Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const D3DXVECTOR3 UsePos);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
	D3DXVECTOR3 m_UsePos;	// 表示される位置
	D3DXCOLOR m_col;		// 色
	float m_fALvCounter;	// アルファ値のレベルのカウンター		
	bool m_bUse;			// 表示されているかどうか
};

#endif