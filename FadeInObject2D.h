//===================================================
//
// フェードインする2Dオブジェクト [FadeInObject2D.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _FADEINOBJECT2D_H_
#define _FADEINOBJECT2D_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "object2D.h"

//***************************************************
// フェードインする2Dオブジェクトのクラスの定義
//***************************************************
class CFadeInObject2D :public CObject2D
{
public:
	CFadeInObject2D();
	~CFadeInObject2D();

	static CFadeInObject2D* Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const int nTime);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
private:
	D3DXCOLOR m_col; // 色
	float m_fAddAlv; // アルファ値の加算値
};

#endif
