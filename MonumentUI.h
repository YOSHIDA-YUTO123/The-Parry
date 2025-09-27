//===================================================
//
// 石碑のUI [MonumentUI.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _MONUMENTUI_H_
#define _MONUMENTUI_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "object2D.h"

//***************************************************
// 石碑のUIのクラスの定義
//***************************************************
class CMonumentUI : public CObject2D
{
public:
	CMonumentUI();
	~CMonumentUI();

	static CMonumentUI* Create(const D3DXVECTOR3 pos);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Enable(const bool bShow) { m_bShow = bShow; }
private:
	D3DXCOLOR m_col;	// 色
	float m_fFlashTime;	// 点滅時間
	bool m_bShow;		// 表示するかどうか
};
#endif