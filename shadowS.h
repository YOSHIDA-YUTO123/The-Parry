//===================================================
//
// ステンシルシャドウ [shadowS.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _SHADOWS_H_
#define _SHADOWS_H_

//***************************************************
// インクルードファイル
//***************************************************
#include"main.h"
#include "objectX.h"

//***************************************************
// 影(ステンシル)のクラスの定義
//***************************************************
class CShadowS : public CObjectX
{
public:
	CShadowS(int nPriority = 3);
	~CShadowS();
	static CShadowS* Create(const D3DXVECTOR3 pos,D3DXVECTOR3 *pScal);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void UpdatePos(const D3DXVECTOR3 pos) { SetPosition(pos); }
private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer; // 頂点バッファへのポインタ
	D3DXVECTOR3 *m_pScal;				  // 大きさ
	bool m_bShowShadow;					  // *************影のモデルを見せるかどうか(Release時に消去)**************
};

#endif