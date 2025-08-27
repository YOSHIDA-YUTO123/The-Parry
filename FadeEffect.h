//===================================================
//
// フェードエフェクト [FadeEffect.h]
// Author:YUTO YOSHIDA
//
//===================================================

//***************************************************
// 多重インクルード防止
//***************************************************
#ifndef _FADEEFFECT_H_
#define _FADEEFFECT_H_

//***************************************************
// インクルードファイル
//***************************************************
#include "object2D.h"

//***************************************************
// フェードエフェクトクラスの定義
//***************************************************
class CFadeEffect : public CObject2D
{
public:

	typedef enum
	{
		FADE_NONE = 0,			// フェードなしの状態
		FADE_IN,				// フェードインの状態
		FADE_OUT,				// フェードアウトの状態
		FAFE_MAX
	}FADE;

	CFadeEffect();
	~CFadeEffect();

	static CFadeEffect* Create(const FADE fade);

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void SetState(FADE fade) { m_Fade = fade; }
	FADE GetState(void) const { return m_Fade; }
private:
	FADE m_Fade;		// フェードの状態
	D3DXCOLOR m_col;	// 色
};

#endif