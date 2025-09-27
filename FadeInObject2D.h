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
	CFadeInObject2D(const int nPriority = 7);
	virtual ~CFadeInObject2D();

	static CFadeInObject2D* Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 Size, const int nTime);

	virtual HRESULT Init(void) override;
	virtual void Uninit(void) override;
	virtual void Update(void) override;
	virtual void Draw(void) override;
	
	// ゲッター
	D3DXCOLOR GetColor(void) const { return m_col; }

	// セッター
	void SetColor(const D3DXCOLOR col) { m_col = col; }
	void Reset(void);
protected:
	void SetAddAvl(const int nTime) { m_fAddAlv = 1.0f / nTime; }
	bool GetFinish(void) const { return m_bFinish; }
private:
	D3DXCOLOR m_col; // 色
	float m_fAddAlv; // アルファ値の加算値
	bool m_bFinish;	 // フェードが終わったかどうか
};

#endif
