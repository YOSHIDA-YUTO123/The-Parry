//================================================
//
// オーバーレイ [overlay.h]
// Author: YUTO YOSHIDA
//
//================================================

//*************************************************
// 多重インクルード防止
//*************************************************
#ifndef _OVERLAY_H_
#define _OVERLAY_H_

//************************************************
// インクルードファイル
//************************************************
#include "object2D.h"

//************************************************
// オーバーレイクラスの定義
//************************************************
class COverlay : public CObject2D
{
public:
	COverlay();
	~COverlay();

	static COverlay* Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const int nShowTime);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
	D3DXCOLOR m_col; // 色
	int m_nShowTime; // 表示する時間
};

#endif
